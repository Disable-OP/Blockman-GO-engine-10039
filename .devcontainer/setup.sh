#!/usr/bin/env bash
set -x

SDK_DIR="$HOME/android-sdk"
NDK_DIR="$HOME/android-ndk-r17c"
GRADLE_DIR="$HOME/gradle-4.10.2"

# --- 1. System packages (codespace user has passwordless sudo) ---
sudo apt-get update -qq
sudo apt-get install -y -qq --no-install-recommends \
  build-essential ccache cmake ninja-build python3 \
  zlib1g-dev libssl-dev unzip wget \
  openjdk-8-jdk openjdk-17-jdk \
  libpulse0 libgl1-mesa-glx libx11-6 libx11-xcb1 \
  libxcb1 libxcomposite1 libxcursor1 libxdamage1 \
  libxext6 libxfixes3 libxi6 libxrandr2 libxrender1 \
  libxtst6 libnss3 libatk1.0-0 libatk-bridge2.0-0 \
  libgtk-3-0 libgbm1 libasound2 \
  cpu-checker qemu-kvm 2>&1 | tail -5

# KVM
if [ -e /dev/kvm ]; then
  echo "✅ /dev/kvm found"
  sudo chmod 666 /dev/kvm 2>/dev/null || true
else
  echo "⚠️ /dev/kvm not found — emulator will be slow"
fi

# --- 2. JDK 8 as default ---
export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64
echo "export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64" >> ~/.bashrc

# --- 3. Android SDK ---
if [ ! -d "$SDK_DIR/cmdline-tools/latest" ]; then
  echo "Installing Android SDK..."
  mkdir -p "$SDK_DIR"
  cd /tmp
  wget -q https://dl.google.com/android/repository/commandlinetools-linux-9477386_latest.zip
  unzip -q commandlinetools-linux-9477386_latest.zip -d "$SDK_DIR/"
  mkdir -p "$SDK_DIR/cmdline-tools/latest"
  mv "$SDK_DIR/cmdline-tools/bin" "$SDK_DIR/cmdline-tools/latest/bin" 2>/dev/null || true
  mv "$SDK_DIR/cmdline-tools/lib" "$SDK_DIR/cmdline-tools/latest/lib" 2>/dev/null || true
  rm -rf "$SDK_DIR/cmdline-tools/bin" "$SDK_DIR/cmdline-tools/lib"
  rm commandlinetools-linux-9477386_latest.zip
fi

export ANDROID_HOME="$SDK_DIR"
export PATH="$SDK_DIR/cmdline-tools/latest/bin:$SDK_DIR/platform-tools:$SDK_DIR/emulator:$PATH"
echo "export ANDROID_HOME=$SDK_DIR" >> ~/.bashrc
echo "export PATH=$SDK_DIR/cmdline-tools/latest/bin:$SDK_DIR/platform-tools:$SDK_DIR/emulator:\$PATH" >> ~/.bashrc

# Install SDK packages (Android 9 API 28 x86_64 for native bridge)
yes | sdkmanager --licenses > /dev/null 2>&1 || true
sdkmanager "platform-tools" "platforms;android-28" "build-tools;28.0.3" "emulator" "system-images;android-28;default;x86_64" 2>&1 | tail -5

# --- 4. NDK r17c ---
if [ ! -d "$NDK_DIR" ]; then
  echo "Installing NDK r17c..."
  cd /tmp
  wget -q https://dl.google.com/android/repository/android-ndk-r17c-linux-x86_64.zip
  unzip -q android-ndk-r17c-linux-x86_64.zip -d "$HOME/"
  rm android-ndk-r17c-linux-x86_64.zip
fi

# Fix libtinfo
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

# --- 6. AVD ---
AVD_NAME="blockman_avd"
if ! avdmanager list avd 2>/dev/null | grep -q "$AVD_NAME"; then
  echo "Creating AVD..."
  echo "no" | avdmanager create avd \
    --name "$AVD_NAME" \
    --package "system-images;android-28;default;x86_64" \
    --device "pixel_4a" \
    --force
  AVD_CONFIG="$HOME/.android/avd/$AVD_NAME.avd/config.ini"
  echo "hw.gpu.enabled=yes" >> "$AVD_CONFIG"
  echo "hw.gpu.mode=swiftshader_indirect" >> "$AVD_CONFIG"
  echo "hw.keyboard=yes" >> "$AVD_CONFIG"
  echo "disk.dataPartition.size=4G" >> "$AVD_CONFIG"
  echo "vm.heapSize=512" >> "$AVD_CONFIG"
fi

# --- 7. ccache ---
export CCACHE_DIR="$HOME/.ccache"
export CCACHE_MAXSIZE=2G
export NDK_CCACHE=/usr/bin/ccache
mkdir -p "$CCACHE_DIR"
ccache --set-config=max_size=2G 2>/dev/null || true
echo "export CCACHE_DIR=$CCACHE_DIR" >> ~/.bashrc
echo "export CCACHE_MAXSIZE=2G" >> ~/.bashrc
echo "export NDK_CCACHE=/usr/bin/ccache" >> ~/.bashrc

echo ""
echo "✅ Setup complete!"
echo "Run: bash .devcontainer/auto_build.sh"
