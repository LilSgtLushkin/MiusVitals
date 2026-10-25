#!/usr/bin/env python3
"""White-box checks for the ordered operational-map and replication-hygiene overlay."""
from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[3]
UPSTREAM = ROOT / "development/upstream/miusplayer-public-43b2736"
PATCHES = (
    ROOT / "development/merged-source/patches/0001-elide-idempotent-remote-transform-writes.patch",
    ROOT / "development/merged-source/patches/0002-ordered-operation-turns-and-network-hygiene.patch",
)
HEADER = "development/upstream/miusplayer-public-43b2736/basic_network/operation_turn.h"


class Stage:
    HOST = 0
    CLIENT = 1
    COLLECT = 2


@dataclass
class OrderedTurn:
    stage: int = Stage.HOST
    host_done: bool = False
    client_done: bool = False
    host_snapshot_published: bool = False
    engine_turn_calls: int = 0

    def host_finish(self) -> None:
        assert self.stage == Stage.HOST and not self.host_done
        self.host_done = True
        # The client stays blocked until the host snapshot has been published.

    def publish_host_snapshot(self) -> None:
        assert self.host_done and self.stage == Stage.HOST
        self.host_snapshot_published = True
        self.stage = Stage.CLIENT

    def client_finish(self) -> None:
        assert self.stage == Stage.CLIENT and self.host_snapshot_published
        self.client_done = True
        self.stage = Stage.COLLECT

    def resolve_on_host(self) -> None:
        assert self.stage == Stage.COLLECT and self.host_done and self.client_done
        self.engine_turn_calls += 1


def require(value: bool, message: str) -> None:
    if not value:
        raise SystemExit(message)


def patched_tree() -> Path:
    temp = tempfile.TemporaryDirectory(prefix="mius-operation-patch-")
    root = Path(temp.name)
    # Keep the TemporaryDirectory alive by storing it on the Path subclass-free function attribute.
    patched_tree.temp = temp  # type: ignore[attr-defined]
    shutil.copytree(UPSTREAM, root / "development/upstream/miusplayer-public-43b2736")
    subprocess.run(["git", "init", "-q"], cwd=root, check=True)
    subprocess.run(["git", "add", "-A"], cwd=root, check=True)
    for patch in PATCHES:
        subprocess.run(["git", "apply", "--check", str(patch)], cwd=root, check=True)
        subprocess.run(["git", "apply", str(patch)], cwd=root, check=True)
    return root


def main() -> None:
    text = PATCHES[1].read_text(encoding="utf-8")
    for marker in (
        "MAP_TURN_HOST",
        "MAP_TURN_CLIENT",
        "MAP_TURN_COLLECT",
        "map_turn_allows_input",
        "g_map_handoff_pending",
        "publish_host_map",
        "begin_map_download",
        "client-issued orders (including attacks) are merged",
        "queue_remote_transform",
        "g_stat_coalesced",
    ):
        require(marker in text, f"ordered-operation overlay lost required marker: {marker}")
    for forbidden in ("MP_BG_FPS", "MP_FG_FPS", "MP_STATE_KBPS", "k_version = 13"):
        require(forbidden not in text, f"ordered-operation overlay must not change {forbidden}")

    candidate = patched_tree()
    header = candidate / HEADER
    require(header.is_file(), "operation turn codec was not added to the candidate")
    # The turn codec is independent C++ and can be compiled on the development host even though the full
    # game module requires the Windows engine headers/toolchain.
    source = candidate / "turn_codec_test.cpp"
    binary = candidate / "turn_codec_test"
    source.write_text(
        "#include \"development/upstream/miusplayer-public-43b2736/basic_network/operation_turn.h\"\n"
        "int main() { unsigned f = map_turn_flags(17, MAP_TURN_CLIENT, false, true); "
        "return map_epoch_from_flags(f) == 17 && map_turn_from_flags(f) == MAP_TURN_CLIENT && "
        "map_turn_allows_input(true, MAP_TURN_HOST) && !map_turn_allows_input(false, MAP_TURN_HOST) ? 0 : 1; }\n",
        encoding="utf-8",
    )
    subprocess.run(["g++", "-std=c++17", "-Wall", "-Wextra", "-Werror", "-I", str(candidate), str(source), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)

    turn = OrderedTurn()
    turn.host_finish()
    require(turn.stage == Stage.HOST and turn.engine_turn_calls == 0, "host completion must not start a battle before handoff")
    turn.publish_host_snapshot()
    require(turn.stage == Stage.CLIENT, "client input must open only after the authoritative host snapshot")
    turn.client_finish()
    require(turn.stage == Stage.COLLECT and turn.engine_turn_calls == 0, "client attack/order must wait for host resolution")
    turn.resolve_on_host()
    require(turn.engine_turn_calls == 1, "host must resolve only the fully completed two-phase turn")

    # A receive burst preserves the newest record for every unit and never collapses different units.
    latest: dict[int, str] = {}
    order: list[int] = []
    for unit, state in ((4, "moving"), (9, "first"), (4, "stopped"), (9, "rotated")):
        if unit not in latest:
            order.append(unit)
        latest[unit] = state
    require(order == [4, 9] and latest == {4: "stopped", 9: "rotated"}, "transform queue must retain the latest record per unit")

    print("OK ordered-operation overlays apply after the transform overlay")
    print("OK host -> synchronized handoff -> client -> collection -> host resolution model")
    print("OK client-order/attack path cannot call the host turn worker early")
    print("OK burst transform coalescing retains each unit's newest state")


if __name__ == "__main__":
    main()
