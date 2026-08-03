#!/usr/bin/env bash
# ============================================================
# run.sh — autonomous build + emulator + install + screenshot.
#
# Builds the project for x86 (emulator), launches the emulator,
# installs the APK, takes screenshots, and uses VLM to analyze
# the screen for interactive UI elements (permission dialogs, etc).
#
# Usage:
#   bash .devcontainer/run.sh           # full pipeline
#   bash .devcontainer/run.sh --build   # build only
#   bash .devcontainer/run.sh --run     # emulator + install only
# ============================================================
set -ex

cd "$(dirname "$0")/.."

# --- Config ---
ABI="x86"
BUILD_TYPE="Debug"
AVD_NAME="blockman_avd"
SCREENSHOT_DIR="$HOME/screenshots"
SDK_DIR="$HOME/android-sdk"
NDK_DIR="$HOME/android-ndk-r17c"
GRADLE_DIR="$HOME/gradle-4.10.2"

export ANDROID_HOME="$SDK_DIR"
export ANDROID_SDK_ROOT="$SDK_DIR"
export ANDROID_NDK_HOME="$NDK_DIR"
export ANDROID_NDK_ROOT="$NDK_DIR"
export NDK_ROOT="$NDK_DIR"
export JAVA_HOME="/usr/lib/jvm/java-8-openjdk-amd64"
export GRADLE_OPTS="-Dhttps.protocols=TLSv1.2"
export PATH="$SDK_DIR/cmdline-tools/latest/bin:$SDK_DIR/platform-tools:$SDK_DIR/emulator:$GRADLE_DIR/bin:$NDK_DIR:$PATH"
export CCACHE_DIR="$HOME/.ccache"
export CCACHE_MAXSIZE=2G
export NDK_CCACHE=/usr/bin/ccache
export DISPLAY=:0

mkdir -p "$SCREENSHOT_DIR"

# ============================================================
# PHASE 1: Build for x86 (emulator ABI)
# ============================================================
build() {
  echo "🔨 Building for $ABI..."

  # Fix libtinfo if needed
  LIBTINFO6=$(find /usr/lib /lib -name 'libtinfo.so.6*' 2>/dev/null | head -1)
  if [ -n "$LIBTINFO6" ] && [ ! -f "${LIBTINFO6%/*}/libtinfo.so.5" ]; then
    sudo ln -sf "$LIBTINFO6" "${LIBTINFO6%/*}/libtinfo.so.5"
    sudo ldconfig
  fi

  # Step 1: Third-party deps
  echo "  [1/6] Third-party deps..."
  cd engine-main/engine/NDKBuild/DepBuild
  $NDK_ROOT/ndk-build NDK_DEBUG=0 APP_ABI=$ABI APP_PLATFORM=android-14 APP_STL=gnustl_static

  # Step 2: Engine core
  echo "  [2/6] Engine core..."
  cd ../../engine/NDKBuild
  $NDK_ROOT/ndk-build NDK_DEBUG=0 APP_ABI=$ABI APP_PLATFORM=android-14 APP_STL=gnustl_static

  # Step 3: Shared libraries (RakNet, curl, cpr, g3log)
  echo "  [3/6] Shared libraries..."
  cd ../../libraries/ApkBuild
  $NDK_ROOT/ndk-build NDK_DEBUG=0 APP_ABI=$ABI APP_PLATFORM=android-14 APP_STL=gnustl_static

  # Step 4: Logic
  echo "  [4/6] Logic..."
  cd ../../logic/ApkBuild
  $NDK_ROOT/ndk-build NDK_DEBUG=0 APP_ABI=$ABI APP_PLATFORM=android-14 APP_STL=gnustl_static

  # Step 5: Client
  echo "  [5/6] Client..."
  cd ../../client/ApkBuild
  $NDK_ROOT/ndk-build NDK_DEBUG=0 APP_ABI=$ABI APP_PLATFORM=android-14 APP_STL=gnustl_static

  # Step 6: Server (libGameServer.so)
  echo "  [6/6] Server (libGameServer.so)..."
  cd ../../server
  mkdir -p build-android-$ABI
  cd build-android-$ABI
  cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=$NDK_DIR/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=$ABI \
    -DANDROID_PLATFORM=android-14 \
    -DANDROID_STL=gnustl_static \
    -DLORD_BUILD_PLATFORM_ANDROID=ON \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
  cmake --build . -j$(nproc)

  # Stage native libs + assets
  echo "📦 Staging native libs + assets..."
  cd ../../client/Shells/Android/Blockmango/app
  mkdir -p libs/$ABI
  cp -v ../../../../client/Bin/$ABI/libBlockMan.so libs/$ABI/
  cp -v ../../../../server/build-android-$ABI/libGameServer.so libs/$ABI/
  cp -v $NDK_DIR/sources/cxx-stl/gnu-libstdc++/4.9/libs/$ABI/libgnustl_shared.so libs/$ABI/

  mkdir -p src/main/assets/resources
  cp -r ../../../../../../engine-res-main/* src/main/assets/resources/

  # Build APK
  echo "📦 Building APK..."
  cd ..
  gradle assembleDebug --no-daemon --stacktrace

  echo "✅ Build complete!"
  find . -name "*.apk" -type f
}

# ============================================================
# PHASE 2: Emulator
# ============================================================
start_emulator() {
  echo "🖥️  Starting emulator..."
  # Kill any existing emulator
  adb kill-server 2>/dev/null || true
  pkill -f "emulator.*$AVD_NAME" 2>/dev/null || true
  sleep 2

  # Start emulator in headless mode with KVM acceleration
  nohup emulator -avd "$AVD_NAME" \
    -no-window \
    -no-audio \
    -no-boot-anim \
    -gpu swiftshader_indirect \
    -partition-size 2048 \
    -qemu -m 2048 \
    > "$SCREENSHOT_DIR/emulator.log" 2>&1 &

  echo "  Waiting for emulator to boot..."
  adb start-server

  # Wait for device to come online
  adb wait-for-device
  echo "  Device detected, waiting for boot..."

  # Wait for boot completion (up to 120 seconds)
  BOOT_TIMEOUT=120
  BOOT_START=$SECONDS
  while true; do
    BOOTED=$(adb shell getprop sys.boot_completed 2>/dev/null | tr -d '\r')
    if [ "$BOOTED" = "1" ]; then
      echo "  ✅ Emulator booted!"
      break
    fi
    ELAPSED=$((SECONDS - BOOT_START))
    if [ $ELAPSED -ge $BOOT_TIMEOUT ]; then
      echo "  ⚠️  Boot timeout (${BOOT_TIMEOUT}s), proceeding anyway..."
      break
    fi
    echo "  Waiting for boot... (${ELAPSED}s / ${BOOT_TIMEOUT}s)"
    sleep 5
  done
  sleep 3
}

# ============================================================
# PHASE 3: Install + Launch + Screenshot
# ============================================================
install_and_launch() {
  echo "📱 Installing APK..."
  APK_PATH=$(find engine-main/client/Shells/Android/Blockmango -name "*.apk" -type f | head -1)
  if [ -z "$APK_PATH" ]; then
    echo "❌ No APK found! Run with --build first."
    exit 1
  fi
  echo "  APK: $APK_PATH"
  adb install -r "$APK_PATH"

  echo "🚀 Launching app..."
  adb shell am start -n com.sandboxol.blockmango/.EchoesActivity

  # Wait for the app to start
  echo "  Waiting for app to initialize..."
  sleep 10

  # Take initial screenshot
  echo "📸 Taking screenshots..."
  take_screenshot "01_initial"

  # Wait more for the permission dialog or loading screen
  sleep 5
  take_screenshot "02_after_5s"

  sleep 5
  take_screenshot "03_after_10s"

  sleep 10
  take_screenshot "04_after_20s"

  # Check logcat for crashes
  echo "📋 Checking for crashes..."
  adb logcat -d -s AndroidRuntime:E BlockmanServer:I ActivityManager:W | tail -30 || true

  echo ""
  echo "=== Screenshots saved to $SCREENSHOT_DIR ==="
  ls -la "$SCREENSHOT_DIR/"
}

take_screenshot() {
  local name="$1"
  local filepath="$SCREENSHOT_DIR/${name}.png"
  adb exec-out screencap -p > "$filepath" 2>/dev/null
  if [ -s "$filepath" ]; then
    echo "  📸 $filepath ($(du -h "$filepath" | cut -f1))"
  else
    echo "  ⚠️  Screenshot failed: $filepath"
  fi
}

# ============================================================
# PHASE 4: VLM Analysis (optional — requires z-ai-web-dev-sdk)
# ============================================================
analyze_screenshots() {
  echo "🔍 Analyzing screenshots with VLM..."
  local script="$HOME/analyze_screenshots.py"
  cat > "$script" << 'PYEOF'
#!/usr/bin/env python3
"""Analyze emulator screenshots using VLM to find UI elements to interact with."""
import os
import sys
import glob
import base64
import subprocess

SCREENSHOT_DIR = os.path.expanduser("~/screenshots")

def analyze_image(image_path):
    """Use the z-ai-web-dev-sdk CLI to analyze a screenshot."""
    print(f"\n=== Analyzing {os.path.basename(image_path)} ===")

    # Read the image and base64-encode it
    with open(image_path, "rb") as f:
        img_b64 = base64.b64encode(f.read()).decode()

    # Use the VLM skill to analyze the screenshot
    prompt = (
        "This is a screenshot of an Android emulator running a game called Blockman. "
        "Analyze the screen and describe:\n"
        "1. What UI elements are visible (buttons, dialogs, loading screens)\n"
        "2. If there's a permission dialog, describe where the 'Allow'/'Confirm' button is\n"
        "3. If the app has crashed (black screen, error dialog)\n"
        "4. What action should be taken next (tap a button, wait, etc.)\n"
        "Be concise — 3-4 sentences max."
    )

    # Call the VLM via z-ai-web-dev-sdk
    try:
        result = subprocess.run(
            ["npx", "z-ai-web-dev-sdk", "vlm", "--image-base64", img_b64, "--prompt", prompt],
            capture_output=True, text=True, timeout=30
        )
        if result.returncode == 0:
            print(result.stdout)
            return result.stdout
        else:
            print(f"VLM error: {result.stderr}")
            return None
    except Exception as e:
        print(f"VLM call failed: {e}")
        return None

def tap_screen(x, y):
    """Tap the screen at the given coordinates via adb."""
    print(f"👆 Tapping ({x}, {y})")
    subprocess.run(["adb", "shell", "input", "tap", str(x), str(y)], check=True)

def main():
    screenshots = sorted(glob.glob(os.path.join(SCREENSHOT_DIR, "*.png")))

    if not screenshots:
        print("No screenshots found!")
        return

    for screenshot in screenshots:
        analysis = analyze_image(screenshot)
        if analysis:
            # Look for button coordinates in the VLM response
            # (VLM might say something like "tap at (540, 1200)")
            # For now, just print the analysis
            print(f"\n→ Next action based on analysis: {analysis[:200]}")

    # Take a final screenshot after any interactions
    final_path = os.path.join(SCREENSHOT_DIR, "05_final.png")
    subprocess.run(["adb", "exec-out", "screencap", "-p"], stdout=open(final_path, "wb"))
    print(f"\n📸 Final screenshot: {final_path}")

if __name__ == "__main__":
    main()
PYEOF
  python3 "$script" || echo "VLM analysis failed (may need npm install)"
}

# ============================================================
# Main
# ============================================================
case "${1:-all}" in
  --build) build ;;
  --run)   start_emulator; install_and_launch ;;
  --analyze) analyze_screenshots ;;
  all)     build; start_emulator; install_and_launch; analyze_screenshots ;;
  *)       echo "Usage: $0 [--build|--run|--analyze|all]"; exit 1 ;;
esac

echo ""
echo "✅ Done! Check screenshots in ~/screenshots/"
