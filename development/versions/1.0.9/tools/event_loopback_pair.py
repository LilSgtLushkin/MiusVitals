#!/usr/bin/env python3
"""Two processes with production session/shot/hit/anim paths and a fake engine.

Runs both loopback UDP and the UDP-based Steam stand-in. The first event packet and the
first nonzero ACK are dropped; a later event packet is duplicated. Neither mode runs the
real game, real Steam, or real engine callbacks.
"""
import argparse
import os
import socket
import subprocess
import sys


def ports():
    sockets = []
    try:
        for _ in range(2):
            s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            s.bind(("127.0.0.1", 0))
            sockets.append(s)
        return [s.getsockname()[1] for s in sockets]
    finally:
        for s in sockets:
            s.close()


def run(peer, fake_steam):
    pair = ports()
    env = os.environ.copy()
    if fake_steam:
        env["EVENT_FAKE_STEAM"] = "1"
    else:
        env.pop("EVENT_FAKE_STEAM", None)
    proc = [subprocess.Popen([peer, "host", str(pair[0]), str(pair[1])], env=env,
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True),
            subprocess.Popen([peer, "join", str(pair[1]), str(pair[0])], env=env,
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)]
    try:
        output = [p.communicate(timeout=25)[0] for p in proc]
    except subprocess.TimeoutExpired:
        for p in proc:
            p.kill()
        output = [p.communicate()[0] for p in proc]
        print("\n".join(output))
        print("event loopback: TIMEOUT", file=sys.stderr)
        return False
    tag = "EVENT_STEAM_RESULT" if fake_steam else "EVENT_IP_RESULT"
    ok = True
    for name, p, text in zip(("host", "join"), proc, output):
        lines = [line for line in text.splitlines() if line.startswith(tag + " ")]
        if p.returncode or len(lines) != 1 or f"{tag} {name} ok=1" not in lines[0]:
            print(f"--- {tag} {name} failed ({p.returncode}) ---\n{text}")
            ok = False
        else:
            print(lines[0])
    print(f"{tag} two-process loopback: {'PASS' if ok else 'FAIL'}")
    return ok


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--peer", required=True)
    args = parser.parse_args()
    ip = run(args.peer, False)
    fake_steam = run(args.peer, True)
    return 0 if ip and fake_steam else 1


if __name__ == "__main__":
    sys.exit(main())
