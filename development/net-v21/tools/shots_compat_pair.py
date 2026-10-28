#!/usr/bin/env python3
"""Run real 1.0.7/current shot endpoints in separate synthetic processes, not a game."""
from __future__ import annotations

import argparse
from pathlib import Path
import subprocess
import tempfile
import time


def case(sender: Path, receiver: Path, mode: str) -> None:
    with tempfile.TemporaryDirectory(prefix="mius-shots-compat-") as tmp:
        packet, ack = Path(tmp) / "packet.bin", Path(tmp) / "ack.bin"
        process = subprocess.Popen(
            [str(sender), "send", str(packet), str(ack), mode],
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
        )
        try:
            deadline = time.monotonic() + 10
            while not packet.exists():
                if process.poll() is not None:
                    out, _ = process.communicate()
                    raise RuntimeError(f"sender exited before packet ({process.returncode}):\n{out}")
                if time.monotonic() > deadline:
                    raise RuntimeError("sender did not produce a packet")
                time.sleep(0.01)
            other = subprocess.run(
                [str(receiver), "receive", str(packet), str(ack), mode],
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, timeout=15,
            )
            if other.returncode:
                raise RuntimeError(f"receiver failed ({other.returncode}):\n{other.stdout}")
            out, _ = process.communicate(timeout=15)
            if process.returncode:
                raise RuntimeError(f"sender failed ({process.returncode}):\n{out}")
            print(f"{sender.name} -> {receiver.name}, {mode}: {other.stdout.strip()}")
        finally:
            if process.poll() is None:
                process.kill()
                process.communicate()


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--legacy", required=True, type=Path)
    parser.add_argument("--current", required=True, type=Path)
    args = parser.parse_args()
    legacy, current = args.legacy.resolve(strict=True), args.current.resolve(strict=True)
    for sender in (legacy, current):
        for receiver in (legacy, current):
            for mode in ("mapped", "marked", "unknown"):
                case(sender, receiver, mode)
    for receiver in (legacy, current):
        case(current, receiver, "named")
    print("14 no-game process pairs passed; legacy fallback remains unsafe for unknown IDs")


if __name__ == "__main__":
    main()
