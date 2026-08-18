#!/usr/bin/env python3
"""
HSE Bridge Client - sends commands to the HSE live bridge via named pipe.

Usage:
    python bridge_client.py              # interactive mode
    python bridge_client.py demo         # run demo sequence
    python bridge_client.py test         # run test sequence
    python bridge_client.py health       # health check
"""

import sys
import time
import json

PIPE_NAME = r'\\.\pipe\hse_bridge'


def connect():
    """Connect to the HSE bridge named pipe."""
    import win32file
    import win32pipe

    while True:
        try:
            handle = win32file.CreateFile(
                PIPE_NAME,
                win32file.GENERIC_READ | win32file.GENERIC_WRITE,
                0, None,
                win32file.OPEN_EXISTING, 0, None
            )
            print(f"Connected to {PIPE_NAME}")
            return handle
        except Exception:
            print("Waiting for HSE bridge...")
            time.sleep(1)


def send_command(handle, cmd_dict):
    """Send a JSON command and read the response."""
    import win32file

    data = json.dumps(cmd_dict) + '\n'
    win32file.WriteFile(handle, data.encode('utf-8'))

    response = b''
    while True:
        hr, chunk = win32file.ReadFile(handle, 1)
        if hr != 0:
            break
        if chunk == b'\n':
            break
        response += chunk

    return json.loads(response.decode('utf-8'))


def cmd_add_primitive(prim_type="Quad", px=0, py=0, pz=0, rx=0, ry=0, rz=0, sx=1, sy=1, sz=1, cr=1, cg=1, cb=1, name=""):
    return {"cmd": "add_primitive", "type": prim_type,
            "px": px, "py": py, "pz": pz,
            "rx": rx, "ry": ry, "rz": rz,
            "sx": sx, "sy": sy, "sz": sz,
            "cr": cr, "cg": cg, "cb": cb,
            "name": name}


def cmd_remove_primitive(index):
    return {"cmd": "remove_primitive", "index": index}


def cmd_set_position(index, px, py, pz):
    return {"cmd": "set_position", "index": index, "px": px, "py": py, "pz": pz}


def cmd_set_rotation(index, rx, ry, rz):
    return {"cmd": "set_rotation", "index": index, "rx": rx, "ry": ry, "rz": rz}


def cmd_set_scale(index, sx, sy, sz):
    return {"cmd": "set_scale", "index": index, "sx": sx, "sy": sy, "sz": sz}


def cmd_set_color(index, cr, cg, cb):
    return {"cmd": "set_color", "index": index, "cr": cr, "cg": cg, "cb": cb}


def cmd_set_camera(px, py, pz, rx=0, ry=0, rz=0, fov=60):
    return {"cmd": "set_camera", "px": px, "py": py, "pz": pz,
            "rx": rx, "ry": ry, "rz": rz, "fov": fov}


def cmd_set_clear_color(r, g, b):
    return {"cmd": "set_clear_color", "r": r, "g": g, "b": b}


def cmd_scene_state():
    return {"cmd": "scene_state"}


def cmd_health():
    return {"cmd": "health"}


def demo(handle):
    """Run a demo sequence: create a colorful room."""
    print("\n=== Demo: Building a colorful room ===\n")

    print("Setting clear color...")
    resp = send_command(handle, cmd_set_clear_color(0.02, 0.02, 0.05))
    print(f"  {resp}")
    time.sleep(0.1)

    print("Setting camera...")
    resp = send_command(handle, cmd_set_camera(6, 4, 6, 0, 0, 0, 60))
    print(f"  {resp}")
    time.sleep(0.1)

    objects = [
        ("Floor",    "Quad", 0, -1.5, 0, -90, 0, 0, 8, 8, 1, 0.3, 0.25, 0.2),
        ("Ceiling",  "Quad", 0, 1.5, 0, 90, 0, 0, 8, 8, 1, 0.4, 0.35, 0.3),
        ("Wall B",   "Quad", 0, 0, -4, 0, 0, 0, 8, 3, 1, 0.5, 0.5, 0.55),
        ("Wall L",   "Quad", -4, 0, 0, 0, 90, 0, 8, 3, 1, 0.55, 0.5, 0.5),
        ("Wall R",   "Quad", 4, 0, 0, 0, -90, 0, 8, 3, 1, 0.55, 0.5, 0.5),
        ("Pillar 1", "Cube", -3, -0.5, -3, 0, 0, 0, 0.4, 2, 0.4, 0.7, 0.3, 0.3),
        ("Pillar 2", "Cube", 3, -0.5, -3, 0, 0, 0, 0.4, 2, 0.4, 0.3, 0.7, 0.3),
        ("Box",      "Cube", 0, -0.75, -1, 0, 30, 0, 1, 1.5, 1, 0.3, 0.3, 0.8),
    ]

    for name, ptype, px, py, pz, rx, ry, rz, sx, sy, sz, cr, cg, cb in objects:
        print(f"Adding {name}...")
        resp = send_command(handle, cmd_add_primitive(ptype, px, py, pz, rx, ry, rz, sx, sy, sz, cr, cg, cb, name))
        print(f"  {resp}")
        time.sleep(0.05)

    print("\nQuerying scene state...")
    resp = send_command(handle, cmd_scene_state())
    print(json.dumps(resp, indent=2))

    print("\n=== Demo complete! ===")


def test(handle):
    """Run a test sequence: create, modify, remove objects."""
    print("\n=== Test: Create, modify, remove ===\n")

    resp = send_command(handle, cmd_health())
    print(f"Health: {resp}")

    print("\n1. Add triangle...")
    resp = send_command(handle, cmd_add_primitive("Triangle", 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0))
    print(f"  {resp}")
    idx = resp.get("index", -1)

    time.sleep(0.2)
    print("\n2. Set color to blue...")
    resp = send_command(handle, cmd_set_color(idx, 0, 0.5, 1))
    print(f"  {resp}")

    print("\n3. Set position to (2, 1, 0)...")
    resp = send_command(handle, cmd_set_position(idx, 2, 1, 0))
    print(f"  {resp}")

    print("\n4. Set scale to 2x...")
    resp = send_command(handle, cmd_set_scale(idx, 2, 2, 2))
    print(f"  {resp}")

    time.sleep(0.5)
    print("\n5. Remove triangle...")
    resp = send_command(handle, cmd_remove_primitive(idx))
    print(f"  {resp}")

    print("\n6. Final scene state...")
    resp = send_command(handle, cmd_scene_state())
    print(json.dumps(resp, indent=2))

    print("\n=== Test complete! ===")


def interactive(handle):
    """Interactive command mode."""
    print("\n=== Interactive Bridge Client ===")
    print("Commands:")
    print("  add <type> <x> <y> <z> <r> <g> <b>   - add primitive (Triangle/Quad/Cube)")
    print("  remove <index>                         - remove primitive")
    print("  pos <index> <x> <y> <z>               - set position")
    print("  rot <index> <x> <y> <z>               - set rotation")
    print("  scale <index> <x> <y> <z>             - set scale")
    print("  color <index> <r> <g> <b>             - set color (0-1)")
    print("  camera <x> <y> <z> [fov]              - set camera")
    print("  clear <r> <g> <b>                      - set clear color")
    print("  state                                  - query scene state")
    print("  health                                 - health check")
    print("  quit                                   - exit")
    print()

    while True:
        try:
            line = input("hse> ").strip()
        except (EOFError, KeyboardInterrupt):
            break

        if not line:
            continue
        if line == "quit":
            break

        parts = line.split()
        cmd = parts[0].lower()

        try:
            if cmd == "add":
                resp = send_command(handle, cmd_add_primitive(
                    parts[1] if len(parts) > 1 else "Quad",
                    *[float(x) for x in parts[2:8]] if len(parts) >= 8 else [0,0,0,0,0,0],
                    *[float(x) for x in parts[8:11]] if len(parts) >= 11 else [1,1,1],
                    *[float(x) for x in parts[11:14]] if len(parts) >= 14 else [1,1,1]
                ))
            elif cmd == "remove":
                resp = send_command(handle, cmd_remove_primitive(int(parts[1])))
            elif cmd == "pos":
                resp = send_command(handle, cmd_set_position(int(parts[1]), float(parts[2]), float(parts[3]), float(parts[4])))
            elif cmd == "rot":
                resp = send_command(handle, cmd_set_rotation(int(parts[1]), float(parts[2]), float(parts[3]), float(parts[4])))
            elif cmd == "scale":
                resp = send_command(handle, cmd_set_scale(int(parts[1]), float(parts[2]), float(parts[3]), float(parts[4])))
            elif cmd == "color":
                resp = send_command(handle, cmd_set_color(int(parts[1]), float(parts[2]), float(parts[3]), float(parts[4])))
            elif cmd == "camera":
                resp = send_command(handle, cmd_set_camera(
                    float(parts[1]), float(parts[2]), float(parts[3]),
                    0, 0, 0, float(parts[4]) if len(parts) > 4 else 60))
            elif cmd == "clear":
                resp = send_command(handle, cmd_set_clear_color(float(parts[1]), float(parts[2]), float(parts[3])))
            elif cmd == "state":
                resp = send_command(handle, cmd_scene_state())
            elif cmd == "health":
                resp = send_command(handle, cmd_health())
            else:
                print("Unknown command")
                continue

            print(json.dumps(resp, indent=2))

        except Exception as e:
            print(f"Error: {e}")


def main():
    handle = connect()

    if len(sys.argv) > 1:
        mode = sys.argv[1].lower()
        if mode == "demo":
            demo(handle)
        elif mode == "test":
            test(handle)
        elif mode == "health":
            resp = send_command(handle, cmd_health())
            print(json.dumps(resp, indent=2))
        else:
            print(f"Unknown mode: {mode}")
            print("Usage: python bridge_client.py [demo|test|health]")
    else:
        interactive(handle)


if __name__ == "__main__":
    main()
