#!/usr/bin/env python3
"""
screenshot_helper.py — runs INSIDE the codespace.
Takes screenshots from the emulator and saves them for download.
Does NOT do VLM analysis — that happens in the parent session
via the VLM skill.

Usage (inside codespace):
  python3 .devcontainer/screenshot_helper.py take 01_initial
  python3 .devcontainer/screenshot_helper.py tap 540 1200
  python3 .devcontainer/screenshot_helper.py logcat
  python3 .devcontainer/screenshot_helper.py wait 10
"""
import sys
import os
import subprocess
import time

SCREENSHOT_DIR = os.path.expanduser("~/screenshots")
os.makedirs(SCREENSHOT_DIR, exist_ok=True)


def take_screenshot(name):
    """Take a screenshot from the emulator."""
    filepath = os.path.join(SCREENSHOT_DIR, f"{name}.png")
    result = subprocess.run(
        ["adb", "exec-out", "screencap", "-p"],
        capture_output=True, timeout=15
    )
    with open(filepath, "wb") as f:
        f.write(result.stdout)
    size = os.path.getsize(filepath)
    print(f"screenshot:{filepath}:{size}")
    return filepath


def tap(x, y):
    """Tap the screen at (x, y)."""
    subprocess.run(["adb", "shell", "input", "tap", str(x), str(y)], check=True)
    print(f"tapped:{x},{y}")


def swipe(x1, y1, x2, y2, duration=300):
    """Swipe from (x1,y1) to (x2,y2)."""
    subprocess.run(["adb", "shell", "input", "swipe", str(x1), str(y1), str(x2), str(y2), str(duration)], check=True)
    print(f"swiped:{x1},{y1}->{x2},{y2}")


def dump_logcat():
    """Dump relevant logcat output."""
    result = subprocess.run(
        ["adb", "logcat", "-d", "-s",
         "AndroidRuntime:E",
         "BlockmanServer:I",
         "ActivityManager:W",
         "System.err:W"],
        capture_output=True, text=True, timeout=15
    )
    print(result.stdout)


def check_boot():
    """Check if emulator is booted."""
    result = subprocess.run(
        ["adb", "shell", "getprop", "sys.boot_completed"],
        capture_output=True, text=True, timeout=10
    )
    booted = result.stdout.strip()
    print(f"booted:{booted}")
    return booted == "1"


def wait(seconds):
    """Wait for the given number of seconds."""
    time.sleep(int(seconds))
    print(f"waited:{seconds}")


def main():
    if len(sys.argv) < 2:
        print("Usage: screenshot_helper.py <take|tap|swipe|logcat|boot|wait> [args]")
        sys.exit(1)

    cmd = sys.argv[1]

    if cmd == "take":
        name = sys.argv[2] if len(sys.argv) > 2 else "screenshot"
        take_screenshot(name)
    elif cmd == "tap":
        tap(int(sys.argv[2]), int(sys.argv[3]))
    elif cmd == "swipe":
        swipe(int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5]))
    elif cmd == "logcat":
        dump_logcat()
    elif cmd == "boot":
        check_boot()
    elif cmd == "wait":
        wait(sys.argv[2])
    else:
        print(f"Unknown command: {cmd}")
        sys.exit(1)


if __name__ == "__main__":
    main()
