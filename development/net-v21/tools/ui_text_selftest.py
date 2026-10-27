#!/usr/bin/env python3
"""Prevent the lobby from advertising the superseded simultaneous M5 turn order."""
from pathlib import Path

ui = (Path(__file__).resolve().parents[1] / "src/ui.cpp").read_text(encoding="utf-8")
required = (
    "Orders: host first, then guest; the host resolves the turn after both have finished.",
    "Press READY; the host can then open the operation.",
    "Ready. Waiting for the host to open the operation...",
)
assert all(message in ui for message in required), "sequential operational lobby wording missing"
assert "both players move their own forces at the same time" not in ui, "stale simultaneous-turn wording"
print("operation lobby text: sequential order and READY hints OK")
