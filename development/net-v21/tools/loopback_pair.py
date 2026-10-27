#!/usr/bin/env python3
"""Run two simulated peers in separate processes on loopback; no game or Steam needed.

This exercises the actual protocol structs and production reliable inbox with packet loss,
reordering, retransmission, duplicate delivery and delayed application. It is not a game test.
"""
from __future__ import annotations
import argparse
import socket
import subprocess
import sys
from pathlib import Path


def free_ports() -> tuple[int, int]:
    # Hold both sockets until both ephemeral ports have been chosen, so they cannot be identical.
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as a, socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as b:
        a.bind(("127.0.0.1", 0))
        b.bind(("127.0.0.1", 0))
        return a.getsockname()[1], b.getsockname()[1]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--peer", required=True, type=Path, help="compiled loopback_peer executable")
    parser.add_argument("--rounds", type=int, default=3)
    args = parser.parse_args()
    exe = str(args.peer.resolve())
    if not args.peer.is_file() or not 1 <= args.rounds <= 20:
        parser.error("executable must exist and rounds must be in 1..20")
    for round_no in range(1, args.rounds + 1):
        a, b = free_ports()
        peers = [subprocess.Popen([exe, "1", str(a), str(b)], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True),
                 subprocess.Popen([exe, "2", str(b), str(a)], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)]
        try:
            results = [p.communicate(timeout=22)[0] for p in peers]
        except subprocess.TimeoutExpired:
            for p in peers:
                p.kill()
            for p in peers:
                p.communicate()
            print(f"FAIL round {round_no}: peers did not finish", file=sys.stderr)
            return 1
        print(f"round {round_no}:")
        for text in results:
            print(text.strip())
        if any(p.returncode != 0 for p in peers):
            print(f"FAIL round {round_no}", file=sys.stderr)
            return 1
    print(f"loopback pair: {args.rounds} rounds passed (two processes; no game)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
