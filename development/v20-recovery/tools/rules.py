"""Shared rules for classifying a v20 function: what it is, and which milestone owns it.

Kept in one place because the backlog, the curated dump and the docs quote the same numbers; if the
rules lived twice they would drift and the reports would stop agreeing with each other.
"""
from __future__ import annotations

import re

# Names proven by earlier byte-level work (docs/NETWORK_LIMITATIONS.md, docs/V20_*.md) or by the
# decompilation itself.  Everything else in the CSV's name column is a structural hint: v20 and v12
# were built by different compilers, so a 0.2-0.5 match can point at the wrong function.
VERIFIED = {
    0x16230: "on_packet",
    0x19F88: "session_frame",
    0x1C544: "session_sim_step",
    0x19A98: "send_peer_packet",
    0x19AC8: "send_datagram",
    0x17CF0: "owns_object",
    0x27DC: "mp_log",
    0x3F75C: "steam_init",
    0x3E8BC: "fake_steam_open",
    0x3EB34: "steam_bind_interfaces",
    0x402B0: "security_cookie_check",
}

PROJECT_NAMESPACES = ("@opsave@", "@Joiner@", "@Sidecar@", "lambda_")

# Instantiations of standard containers and the helpers MSVC emits around them.  Their names carry
# the project type as a template argument (`vector<opsave::Tagged>`), which is why the project
# namespace test above is not enough: none of this is code somebody wrote, it is code the compiler
# generates, and it comes back by itself when the source is rebuilt.
COMPILER_OUTPUT = re.compile(
    r"\$\$?_(Emplace_reallocate|Reallocate|Reallocation|Change_array|Tidy|Growmap|Reset_map|"
    r"Tree_temp_node|Insert_counted_range|Erase_tree|Unchecked_erase|Try_emplace)|"
    r"\$vector@|\$deque@|\$list@|\$basic_string@|\$_Hash@|\$list_node@|\$_Tree_node@",
)

MILESTONES = [
    # Order matters: a lobby message that mentions an operation belongs to the lobby, so the
    # narrower stages come first and the broad "net:"/"session" ones last.
    ("M2", "Steam", re.compile(
        r"steam|MP_STEAM|rich presence|gtmf_join|P2P|friends", re.I)),
    ("M4", "Lobby and forces", re.compile(
        r"lobby|MULTIPLAYER|force|cfgpack|battle setup|MP_BATTLE_SETUP|skirmish|resign|MP_AUTOJOIN",
        re.I)),
    ("M5", "Operation map", re.compile(
        r"map:|opgame|opsave|sidecar|_known|MP_MAP|operation|adv_opmap|formation", re.I)),
    ("M3", "Battle synchronization", re.compile(
        r"ceasefire|MP_SYNC|MP_END|hit:|crater|trench|shot|shots|clock|anim|"
        r"MP_HIT|MP_CRATER|MP_TRENCH|MP_SHOTS|MP_APPLY_DELAY|MP_TEST_TRACE|"
        r"net:|session|battle|transform|shellphys|sync|stamp|MP_ROLE|MP_PEER|MP_VERSUS", re.I)),
    ("M6", "Tests and tooling", re.compile(
        r"MP_TEST|mp_cmd_|mp_join|MP_LOG", re.I)),
]


def is_compiler_output(row: dict[str, str]) -> bool:
    """True for code the compiler writes, i.e. nothing to port."""
    name = row["v12_name"]
    if row["status"] != "NEW":
        return False
    if name.startswith("??__"):
        return True
    if COMPILER_OUTPUT.search(name):
        return True
    return "@std@@" in name and not any(p in name for p in PROJECT_NAMESPACES)


def kind(row: dict[str, str]) -> str:
    if row["status"] == "SAME":
        return "SAME"
    if row["status"] == "CHANGED":
        return "CHANGED"
    if not row["v12_name"].strip():
        return "NEW:unknown"
    return "NEW:stl" if is_compiler_output(row) else "NEW:logic"


def milestone(row: dict[str, str]) -> tuple[str, str]:
    """File by what the function prints; fall back to the symbol when it prints nothing."""
    for tag, label, pattern in MILESTONES:
        if pattern.search(row["strings"]):
            return tag, label
    if not row["strings"].strip():
        for tag, label, pattern in MILESTONES:
            if pattern.search(row["v12_name"]):
                return tag, label
        if int(row["size"]) < 128:
            return "M8", "Small deltas (<128 bytes, no log lines): one-liners to diff, not to design"
    return "M7", "Unsorted"
