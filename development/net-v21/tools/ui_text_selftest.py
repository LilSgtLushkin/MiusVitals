#!/usr/bin/env python3
"""Prevent the lobby from advertising the superseded guest-turn model."""
from pathlib import Path

ui = (Path(__file__).resolve().parents[1] / "src/ui.cpp").read_text(encoding="utf-8")
required = (
    "The host alone controls turns. The guest follows snapshots; local orders are not sent.",
    "Press READY; the host can then open the operation.",
    "Ready. Waiting for the host to open the operation...",
)
assert all(message in ui for message in required), "spectator operational lobby wording missing"
assert "then guest" not in ui and "Your moves go to the host" not in ui, "stale guest-turn wording"
print("operation lobby text: host-only order and READY hints OK")
