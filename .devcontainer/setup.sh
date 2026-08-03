#!/usr/bin/env bash
set -ex

# ============================================================
# setup.sh — runs on codespace creation.
# Installs: Android SDK, NDK r17c, Gradle 4.10.2, JDK 8+17,
#           KVM + Android emulator + x86 system image (API 30).
# ============================================================

SDK_DIR="$HOME/android-sdk"
NDK_DIR="$HOME/android-ndk-r17c"
GRADLE_DIR="$HOME/gradle-4.10.2"

# --- 1. System packages ---
sudo apt-get update
sudo apt-get install -y --no-install-recommends \
  build-essential ccache cmake ninja-build python3 \
  zlib1g-dev libssl-dev unzip wget \
  openjdk-8-jdk openjdk-17-jdk \
  libpulse0 libgl1-mesa-glx libx11-6 libx11-xcb1 \
  libxcb1 libxcomposite1 libxcursor1 libxdamage1 \
  libxext6 libxfixes3 libxi6 libxrandr2 libxrender1 \
  libxtst6 libnss3 libatk1.0-0 libatk-bridge2.0-0 \
  libgtk-3-0 libgbm1 libasound2 \
  cpu-checker qemu-kvm

# Verify KVM is available (requires --privileged + --device /dev/kvm)
if [ -e /dev/kvm ]; then
  echo "✅ /dev/kvm found — KVM acceleration available"
  sudo chmod 666 /dev/kvm
  kvm-ok || true
else
  echo "⚠️  /dev/kvm NOT found — emulator will be slow (no KVM)."
  echo "    Make sure the codespace was created with --privileged."
fi

# --- 2. JDK 8 as default (for Gradle 4.10.2 + AGP 3.2.1) ---
sudo update-java-alternatives --set java-1.8.0-openjdk-amd64 || true
export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64
echo "export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64" >> ~/.bashrc

# --- 3. Android SDK ---
if [ ! -d "$SDK_DIR/cmdline-tools" ]; then
  echo "Installing Android SDK command-line tools..."
  mkdir -p "$SDK_DIR"
  cd /tmp
  wget -q https://dl.google.com/android/repository/commandlinetools-linux-9477386_latest.zip
  unzip -q commandlinetools-linux-9477386_latest.zip -d "$SDK_DIR/"
  mv "$SDK_DIR/cmdline-tools" "$SDK_DIR/cmdline-tools-tmp"
  mkdir -p "$SDK_DIR/cmdline-tools/latest"
  mv "$SDK_DIR/cmdline-tools-tmp/"* "$SDK_DIR/cmdline-tools/latest/"
  rm -rf "$SDK_DIR/cmdline-tools-tmp"
  rm commandlinetools-linux-9477386_latest.zip
fi

export ANDROID_HOME="$SDK_DIR"
export PATH="$SDK_DIR/cmdline-tools/latest/bin:$SDK_DIR/platform-tools:$SDK_DIR/emulator:$PATH"
echo "export ANDROID_HOME=$SDK_DIR" >> ~/.bashrc
echo "export PATH=$SDK_DIR/cmdline-tools/latest/bin:$SDK_DIR/platform-tools:$SDK_DIR/emulator:\$PATH" >> ~/.bashrc

# Accept licenses + install SDK packages
yes | sdkmanager --licenses > /dev/null 2>&1 || true
# Android 9 (API 28) x86_64 — native bridge translates ARM libs to x86,
# so armeabi-v7a apps run on x86 emulator without rebuilding for x86.
sdkmanager "platform-tools" "platforms;android-28" "build-tools;28.0.3" "emulator" "system-images;android-28;default;x86_64"

# --- 4. Android NDK r17c (gnustl_static support) ---
if [ ! -d "$NDK_DIR" ]; then
  echo "Installing NDK r17c..."
  cd /tmp
  wget -q https://dl.google.com/android/repository/android-ndk-r17c-linux-x86_64.zip
  unzip -q android-ndk-r17c-linux-x86_64.zip -d "$HOME/"
  rm android-ndk-r17c-linux-x86_64.zip
fi

# Fix libtinfo.so.5 for NDK r17c clang on Ubuntu 22.04
LIBTINFO6=$(find /usr/lib /lib -name 'libtinfo.so.6*' 2>/dev/null | head -1)
if [ -n "$LIBTINFO6" ]; then
  sudo ln -sf "$LIBTINFO6" "${LIBTINFO6%/*}/libtinfo.so.5"
  sudo ldconfig
fi

echo "export ANDROID_NDK_HOME=$NDK_DIR" >> ~/.bashrc
echo "export ANDROID_NDK_ROOT=$NDK_DIR" >> ~/.bashrc
echo "export NDK_ROOT=$NDK_DIR" >> ~/.bashrc

# --- 5. Gradle 4.10.2 ---
if [ ! -d "$GRADLE_DIR" ]; then
  echo "Installing Gradle 4.10.2..."
  cd /tmp
  wget -q https://services.gradle.org/distributions/gradle-4.10.2-bin.zip
  unzip -q gradle-4.10.2-bin.zip -d "$HOME/"
  rm gradle-4.10.2-bin.zip
fi
echo "export PATH=$GRADLE_DIR/bin:\$PATH" >> ~/.bashrc

# --- 6. Create AVD (Android Virtual Device) — Android 11 x86_64 ---
AVD_NAME="blockman_avd"
if ! avdmanager list avd 2>/dev/null | grep -q "$AVD_NAME"; then
  echo "Creating AVD: $AVD_NAME..."
  echo "no" | avdmanager create avd \
    --name "$AVD_NAME" \
    --package "system-images;android-28;default;x86_64" \
    --device "pixel_4a" \
    --force
  # Enable hardware acceleration in the AVD config
  AVD_CONFIG="$HOME/.android/avd/$AVD_NAME.avd/config.ini"
  echo "hw.gpu.enabled=yes" >> "$AVD_CONFIG"
  echo "hw.gpu.mode=swiftshader_indirect" >> "$AVD_CONFIG"
  echo "hw.keyboard=yes" >> "$AVD_CONFIG"
  echo "disk.dataPartition.size=4G" >> "$AVD_CONFIG"
  echo "vm.heapSize=512" >> "$AVD_CONFIG"
fi

# --- 7. ccache setup ---
export CCACHE_DIR="$HOME/.ccache"
export CCACHE_MAXSIZE=2G
export NDK_CCACHE=/usr/bin/ccache
mkdir -p "$CCACHE_DIR"
ccache --set-config=max_size=2G || true
ccache --set-config=compress=true || true

echo "export CCACHE_DIR=$CCACHE_DIR" >> ~/.bashrc
echo "export CCACHE_MAXSIZE=2G" >> ~/.bashrc
echo "export NDK_CCACHE=/usr/bin/ccache" >> ~/.bashrc

echo ""
echo "✅ Codespace setup complete!"
echo ""
echo "To build + run the app on the emulator:"
echo "  bash .devcontainer/run.sh"
echo ""
echo "To start the emulator manually:"
echo "  emulator -avd blockman_avd -no-window -no-audio -gpu swiftshader_indirect -qemu -m 2048"
echo ""
echo "To take a screenshot:"
echo "  adb exec-out screencap -p > screenshot.png"
