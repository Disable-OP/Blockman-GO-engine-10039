#!/usr/bin/env bash
set -x

echo "=== Auto-build started ==="

# Source the env vars set by setup.sh
export ANDROID_HOME="$HOME/android-sdk"
export ANDROID_SDK_ROOT="$HOME/android-sdk"
export ANDROID_NDK_HOME="$HOME/android-ndk-r17c"
export ANDROID_NDK_ROOT="$HOME/android-ndk-r17c"
export NDK_ROOT="$HOME/android-ndk-r17c"
export JAVA_HOME="/usr/lib/jvm/java-8-openjdk-amd64"
export GRADLE_OPTS="-Dhttps.protocols=TLSv1.2"
export PATH="$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$ANDROID_HOME/emulator:$HOME/gradle-4.10.2/bin:$NDK_ROOT:$PATH"
export CCACHE_DIR="$HOME/.ccache"
export CCACHE_MAXSIZE=2G
export NDK_CCACHE=/usr/bin/ccache
export DISPLAY=:0

cd /workspaces/Blockman-GO-engine-10039

# Build for armeabi-v7a (native bridge will run it on x86 emulator)
echo "=== Building for armeabi-v7a ==="
bash .devcontainer/run.sh --build 2>&1 | tail -20

# Start emulator
echo "=== Starting emulator ==="
nohup emulator -avd blockman_avd -no-window -no-audio -no-boot-anim -gpu swiftshader_indirect -partition-size 2048 -qemu -m 2048 > /tmp/emulator.log 2>&1 &
adb start-server
adb wait-for-device
echo "Waiting for boot..."
for i in $(seq 1 24); do
  BOOTED=$(adb shell getprop sys.boot_completed 2>/dev/null | tr -d '\r')
  if [ "$BOOTED" = "1" ]; then echo "Booted!"; break; fi
  sleep 5
done

# Install + launch
echo "=== Installing APK ==="
APK=$(find . -name "*.apk" -type f | head -1)
echo "APK: $APK"
adb install -r "$APK"
adb shell am start -n com.sandboxol.blockmango/.EchoesActivity
sleep 10

# Take screenshots
mkdir -p ~/screenshots
adb exec-out screencap -p > ~/screenshots/01_initial.png
sleep 5
adb exec-out screencap -p > ~/screenshots/02_after_5s.png
sleep 5
adb exec-out screencap -p > ~/screenshots/03_after_10s.png

# Dump logcat
adb logcat -d -s AndroidRuntime:E BlockmanServer:I ActivityManager:W > ~/screenshots/logcat.txt

# Push screenshots to a branch for download
cd /workspaces/Blockman-GO-engine-10039
git checkout -b screenshots
cp ~/screenshots/*.png ~/screenshots/*.txt .devcontainer/
git add .devcontainer/*.png .devcontainer/*.txt
git -c user.name="Codespace" -c user.email="codespace@users.noreply.github.com" commit -m "Add emulator screenshots"
git push origin screenshots

echo "=== Auto-build complete! ==="
echo "Screenshots pushed to 'screenshots' branch"
