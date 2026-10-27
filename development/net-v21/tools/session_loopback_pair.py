#!/usr/bin/env python3
"""Two production tactical session processes, with the Steam UDP stand-in and no game.

The engine, shots and object-state code are mocked. Both peers deliberately have no separate IP
socket: this checks the actual session handshake, F9 withdrawal, end notice and pending engine
exit over the Steam seam. It is not evidence about Steamworks or real game behaviour.
"""
import argparse
import socket
import subprocess
import sys


def free_ports():
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


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--peer", required=True)
    args = parser.parse_args()
    ports = free_ports()
    processes = [
        subprocess.Popen([args.peer, "host", str(ports[0]), str(ports[1])], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True),
        subprocess.Popen([args.peer, "join", str(ports[1]), str(ports[0])], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True),
    ]
    try:
        outputs = [p.communicate(timeout=25)[0] for p in processes]
    except subprocess.TimeoutExpired:
        for p in processes:
            p.kill()
        outputs = [p.communicate()[0] for p in processes]
        for out in outputs:
            print(out)
        print("session loopback: TIMEOUT", file=sys.stderr)
        return 1
    ok = True
    for name, process, output in zip(("host", "join"), processes, outputs):
        lines = [line for line in output.splitlines() if line.startswith("SESSION_RESULT ")]
        if process.returncode != 0 or len(lines) != 1 or f"SESSION_RESULT {name} ok=1" not in lines[0]:
            ok = False
            print(f"--- {name} failed ({process.returncode}) ---\n{output}")
        else:
            print(lines[0])
    print("session loopback: PASS" if ok else "session loopback: FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
