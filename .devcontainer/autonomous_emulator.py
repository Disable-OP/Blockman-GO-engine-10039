#!/usr/bin/env python3
"""
autonomous_emulator.py — fully autonomous emulator interaction.

Takes screenshots of the running emulator, analyzes them with VLM
(vision language model), and taps UI elements (permission dialogs,
buttons) without human intervention.

Usage:
  python3 .devcontainer/autonomous_emulator.py

Prerequisites:
  - Emulator running (bash .devcontainer/run.sh --run)
  - z-ai-web-dev-sdk installed (npm install z-ai-web-dev-sdk)
  - adb in PATH
"""
import os
import sys
import time
import base64
import subprocess
import json
import glob

SCREENSHOT_DIR = os.path.expanduser("~/screenshots")
os.makedirs(SCREENSHOT_DIR, exist_ok=True)

MAX_ITERATIONS = 20
SLEEP_BETWEEN = 5  # seconds


def adb(*args):
    """Run an adb command, return stdout."""
    result = subprocess.run(["adb", *args], capture_output=True, text=True, timeout=30)
    return result.stdout.strip()


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
    if size < 1000:
        print(f"  ⚠️  Screenshot too small ({size} bytes) — emulator may not be ready")
        return None

    print(f"  📸 {filepath} ({size // 1024} KB)")
    return filepath


def analyze_with_vlm(image_path):
    """Analyze a screenshot using VLM via z-ai-web-dev-sdk."""
    with open(image_path, "rb") as f:
        img_b64 = base64.b64encode(f.read()).decode()

    prompt = (
        "This is a screenshot of an Android emulator running a Blockman game app.\n"
        "Analyze the screen and respond in JSON format:\n"
        "{\n"
        '  "description": "what is visible on screen",\n'
        '  "has_permission_dialog": true/false,\n'
        '  "has_crash": true/false,\n'
        '  "buttons": [{"text": "button text", "x": 540, "y": 1200}],\n'
        '  "action": "tap|wait|swipe|done",\n'
        '  "action_params": {"x": 540, "y": 1200}\n'
        "}\n\n"
        "If there's a permission dialog (like 'Allow access to photos'), "
        "set has_permission_dialog=true and provide the ALLOW button coordinates.\n"
        "If the screen is black or shows an error, set has_crash=true.\n"
        "If the game is loading, set action='wait'.\n"
        "If the game is running normally, set action='done'.\n"
        "Screen resolution is 1080x2340 (Pixel 4a)."
    )

    try:
        # Write the image to a temp file and use the VLM CLI
        result = subprocess.run(
            ["npx", "z-ai-web-dev-sdk", "vlm",
             "--image-base64", img_b64,
             "--prompt", prompt],
            capture_output=True, text=True, timeout=45
        )

        if result.returncode == 0:
            # Try to parse JSON from the response
            output = result.stdout.strip()
            # Find JSON in the output
            json_start = output.find('{')
            json_end = output.rfind('}') + 1
            if json_start >= 0 and json_end > json_start:
                try:
                    return json.loads(output[json_start:json_end])
                except json.JSONDecodeError:
                    pass
            return {"description": output, "action": "wait"}
        else:
            print(f"  ⚠️  VLM error: {result.stderr[:200]}")
            return {"description": "VLM call failed", "action": "wait"}

    except subprocess.TimeoutExpired:
        print("  ⚠️  VLM call timed out")
        return {"description": "timeout", "action": "wait"}
    except Exception as e:
        print(f"  ⚠️  VLM exception: {e}")
        return {"description": str(e), "action": "wait"}


def tap(x, y):
    """Tap the screen at (x, y)."""
    print(f"  👆 Tapping ({x}, {y})")
    adb("shell", "input", "tap", str(x), str(y))


def swipe(x1, y1, x2, y2, duration=300):
    """Swipe from (x1,y1) to (x2,y2)."""
    print(f"  👆 Swiping ({x1},{y1}) → ({x2},{y2})")
    adb("shell", "input", "swipe", str(x1), str(y1), str(x2), str(y2), str(duration))


def check_crash():
    """Check logcat for crash messages."""
    logs = adb("logcat", "-d", "-s", "AndroidRuntime:E")
    if "FATAL EXCEPTION" in logs or "CRASH" in logs:
        print("  💥 CRASH DETECTED!")
        # Print the last 20 lines of crash log
        lines = logs.split('\n')
        for line in lines[-20:]:
            print(f"    {line}")
        return True
    return False


def main():
    print("🤖 Autonomous Emulator Interaction")
    print(f"   Max iterations: {MAX_ITERATIONS}")
    print(f"   Sleep between: {SLEEP_BETWEEN}s")
    print(f"   Screenshots: {SCREENSHOT_DIR}")
    print()

    # Check emulator is running
    devices = adb("devices")
    if "emulator" not in devices:
        print("❌ No emulator detected! Run 'bash .devcontainer/run.sh --run' first.")
        sys.exit(1)

    for i in range(MAX_ITERATIONS):
        print(f"\n--- Iteration {i + 1}/{MAX_ITERATIONS} ---")

        # Take screenshot
        screenshot = take_screenshot(f"auto_{i+1:02d}")
        if not screenshot:
            print("  Waiting for emulator to stabilize...")
            time.sleep(SLEEP_BETWEEN)
            continue

        # Check for crashes
        if check_crash():
            print("\n💥 App crashed! Check logcat for details.")
            print("   Run: adb logcat -d -s AndroidRuntime:E")
            break

        # Analyze with VLM
        print("  🔍 Analyzing with VLM...")
        analysis = analyze_with_vlm(screenshot)

        desc = analysis.get("description", "unknown")
        action = analysis.get("action", "wait")
        has_crash = analysis.get("has_crash", False)
        has_perm = analysis.get("has_permission_dialog", False)
        buttons = analysis.get("buttons", [])

        print(f"  📋 Description: {desc[:150]}")
        print(f"  🔘 Permission dialog: {has_perm}")
        print(f"  💥 Crash detected: {has_crash}")
        print(f"  🎯 Action: {action}")

        if has_crash:
            print("\n💥 VLM detected crash! Stopping.")
            break

        if action == "done":
            print("\n✅ App is running normally! Done.")
            break

        if action == "tap":
            params = analysis.get("action_params", {})
            x = params.get("x", 0)
            y = params.get("y", 0)
            if x and y:
                tap(x, y)
                time.sleep(2)
        elif action == "wait":
            pass

        # If VLM found buttons, tap the first one
        if buttons and action != "tap":
            btn = buttons[0]
            x = btn.get("x", 0)
            y = btn.get("y", 0)
            if x and y:
                print(f"  🔘 Found button: '{btn.get('text', '?')}' at ({x}, {y})")
                tap(x, y)
                time.sleep(2)

        time.sleep(SLEEP_BETWEEN)

    # Final screenshot
    print("\n📸 Taking final screenshot...")
    take_screenshot("auto_final")

    # Dump logcat
    print("\n📋 Dumping relevant logcat...")
    logs = adb("logcat", "-d", "-s",
               "AndroidRuntime:E",
               "BlockmanServer:I",
               "ActivityManager:W",
               "System.err:W")
    log_file = os.path.join(SCREENSHOT_DIR, "logcat.txt")
    with open(log_file, "w") as f:
        f.write(logs)
    print(f"   Logs saved to: {log_file}")

    print(f"\n✅ Autonomous session complete! Check {SCREENSHOT_DIR}/")


if __name__ == "__main__":
    main()
