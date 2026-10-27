#!/usr/bin/env python3
"""Two-process production session/Joiner tests with the Steam UDP stand-in and no game.

The engine, shots and object-state code are mocked. Tactical peers test F9/end behaviour with no
separate UDP socket. A menu Joiner follows the real session-before-UI receive order for both fake
Steam and loopback UDP: an operation-menu host must observe READY, and a tactical host must serve
the complete synthetic image. The Steam menu case suppresses the independent UDP socket; the IP
cases suppress UPnP only in the test build. A host-started operation battle traverses the real
sessions, synthetic map/battle transfer and both real CreateProcess child environments. The
lobby UI is simulated and the game engine is not present. This is not real Steam or game validation.
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


def run_pair(peer, mode):
    ports = free_ports()
    prefix, result = {
        "tactical": ("", "SESSION_RESULT"),
        "steam-lobby": ("lobby-", "LOBBY_RESULT"),
        "ip-lobby": ("ip-lobby-", "IP_LOBBY_RESULT"),
        "steam-menu": ("steam-menu-", "STEAM_MENU_RESULT"),
        "ip-menu": ("ip-menu-", "IP_MENU_RESULT"),
        "spectator-steam": ("spectator-steam-", "SPECTATOR_STEAM_RESULT"),
        "spectator-ip": ("spectator-ip-", "SPECTATOR_IP_RESULT"),
        "battle-steam": ("battle-steam-", "BATTLE_STEAM_RESULT"),
        "battle-ip": ("battle-ip-", "BATTLE_IP_RESULT"),
    }[mode]
    processes = [
        subprocess.Popen([peer, prefix + "host", str(ports[0]), str(ports[1])], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True),
        subprocess.Popen([peer, prefix + "join", str(ports[1]), str(ports[0])], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True),
    ]
    try:
        outputs = [p.communicate(timeout=32)[0] for p in processes]
    except subprocess.TimeoutExpired:
        for p in processes:
            p.kill()
        outputs = [p.communicate()[0] for p in processes]
        for out in outputs:
            print(out)
        print(f"{prefix}session loopback: TIMEOUT", file=sys.stderr)
        return False
    ok = True
    for name, process, output in zip(("host", "join"), processes, outputs):
        lines = [line for line in output.splitlines() if line.startswith(result + " ")]
        if process.returncode != 0 or len(lines) != 1 or f"{result} {name} ok=1" not in lines[0]:
            ok = False
            print(f"--- {prefix}{name} failed ({process.returncode}) ---\n{output}")
        else:
            print(lines[0])
    print(f"{prefix}session loopback: {'PASS' if ok else 'FAIL'}")
    return ok


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--peer", required=True)
    args = parser.parse_args()
    tactical = run_pair(args.peer, "tactical")
    steam_lobby = run_pair(args.peer, "steam-lobby")
    ip_lobby = run_pair(args.peer, "ip-lobby")
    steam_menu = run_pair(args.peer, "steam-menu")
    ip_menu = run_pair(args.peer, "ip-menu")
    spectator_steam = run_pair(args.peer, "spectator-steam")
    spectator_ip = run_pair(args.peer, "spectator-ip")
    battle_steam = run_pair(args.peer, "battle-steam")
    battle_ip = run_pair(args.peer, "battle-ip")
    return 0 if all((tactical, steam_lobby, ip_lobby, steam_menu, ip_menu, spectator_steam, spectator_ip,
                     battle_steam, battle_ip)) else 1


if __name__ == "__main__":
    sys.exit(main())
