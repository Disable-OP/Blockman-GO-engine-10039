# Build

How to build the **Android client** and the **Linux game server** from this repository.

This is a CMake + Android NDK + Gradle project. There is no root `CMakeLists.txt` yet (see
[`ROADMAP.md`](ROADMAP.md) M1); for now the `engine-main/cmakeall` driver script invokes
each module's `CMakeLists.txt` in dependency order.

## 1. Toolchain

### 1.1 Common (both client and server)

| Tool         | Minimum version | Notes                                                        |
|--------------|-----------------|--------------------------------------------------------------|
| CMake        | 3.10            | The legacy `cmake_minimum_required(VERSION 2.8)` lines will be bumped in M1. |
| Git          | 2.20+           | For cloning and pulling.                                     |
| Python       | 3.8+            | Some codegen scripts under `engine-main/logic/Src/` use it.  |

### 1.2 Server (Linux x86_64)

| Tool                | Minimum version       |
|---------------------|-----------------------|
| GCC                 | 9                     |
| Clang               | 10 (alternative)      |
| CMake               | 3.10                  |
| MySQL client libs   | 5.7+ (dev headers)    |
| Redis (hiredis)     | shipped in `server/dependencies/linux/redis/` |
| raknet              | shipped in `server/dependencies/` (verify path) |

Tested on Ubuntu 20.04 LTS and Debian 11. Other modern distros should work but are not
officially supported.

### 1.3 Client (Android)

| Tool                       | Minimum version | Notes                                                            |
|----------------------------|-----------------|------------------------------------------------------------------|
| Android Studio             | Hedgehog (2023.1)| Recommended IDE.                                                 |
| Android SDK Platform       | API 30+         | `platform-30`, `build-tools 30.0.3`.                             |
| Android NDK                | r21e or r25c    | r21e matches the legacy project files; r25c is the M1 target.    |
| Gradle                     | 8.x             | Wrapper shipped in `client/Shells/Android/Blockmango/gradlew`.   |
| JDK                        | 17              | Required by AGP 8.x.                                             |

ABIs built: `arm64-v8a` (primary), `armeabi-v7a` (legacy devices), `x86_64` (emulator).

## 2. Building the server (Linux)

```bash
# 1. Install host deps (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install -y \
  build-essential cmake git python3 \
  libmysqlclient-dev libssl-dev zlib1g-dev

# 2. From the repo root
cd engine-main

# 3. Clean build (recommended for first build)
./cmakeall_with_clean

#    — or, incremental —
./cmakeall

# 4. The server binary lands at (verify; depends on CMAKE_RUNTIME_OUTPUT_DIRECTORY):
#    engine-main/server/Build/Server   (or server/Bin/Server)
```

### 2.1 Running the server

```bash
cd engine-main/server
./Build/Server --config path/to/server.cfg
```

The default `main.cpp` ships a `getTestRGConfig()` that hard-codes a debug config
(`gameId = "g100211978"`, port `19130`, monitor `120.92.133.131:9912`, ...). This is
test-only and will be replaced by a real config loader in M2. For now, edit
`server/src/main.cpp` to point at your own MySQL / Redis / monitor addresses, or wait for
the config-loader task.

### 2.2 Server dependencies note

`server/dependencies/linux/` ships prebuilt hiredis and a few other libs. If your distro's
glibc is significantly newer than what they were built against, you may need to rebuild them
from source — that path is documented in [`ROADMAP.md`](ROADMAP.md) M1.

## 3. Building the client (Android)

### 3.1 First-time setup

```bash
# 1. Make sure ANDROID_SDK_ROOT and ANDROID_NDK_HOME are set
export ANDROID_SDK_ROOT=/opt/android-sdk
export ANDROID_NDK_HOME=/opt/android-ndk-r21e

# 2. Build the native dependencies (one-time, slow)
cd engine-main/engine/NDKBuild
./DepBuild/build.sh        # builds FreeImage, FreeType, LuaJIT, zlib, ... for NDK

# 3. Build the engine + logic + clothes native libs
cd engine-main
./cmakeall                 # detects LORD_BUILD_PLATFORM_ANDROID from toolchain file
```

### 3.2 Building the APK

```bash
cd engine-main/client/Shells/Android/Blockmango

# Debug APK
./gradlew assembleDebug
# Output: app/build/outputs/apk/debug/app-debug.apk

# Release APK (needs signing config in app/build.gradle)
./gradlew assembleRelease
```

### 3.3 Installing on a device / emulator

```bash
adb install -r app/build/outputs/apk/debug/app-debug.apk
```

### 3.4 Native libs packaging

The Gradle `app/build.gradle` is configured to pull `.so` files from
`app/src/main/jniLibs/<abi>/`. The `copyBin.bat` (and a Linux equivalent to be added in M1)
copies the freshly built `.so` outputs from `engine-main/client/Build/` into `jniLibs/`.
After a native rebuild, run `copyBin` before `./gradlew assembleDebug`.

## 4. Common build issues

| Symptom                                                | Fix                                                              |
|--------------------------------------------------------|------------------------------------------------------------------|
| `error: 'LORD_PLATFORM_ANDROID' was not declared`      | Toolchain file not picked up; pass `-DCMAKE_TOOLCHAIN_FILE=...`. |
| `lua.h: No such file or directory`                     | Run `engine/NDKBuild/DepBuild/build.sh` first.                   |
| Gradle: `NDK not configured`                           | Set `ANDROID_NDK_HOME` and add `ndk.dir=` to `local.properties`. |
| `hiredis/hiredis.h: No such file or directory` (server)| Install `libhiredis-dev` or point at `server/dependencies/linux/redis/include/hiredis/`. |
| Link error: duplicate symbol in `logic` + `client`     | `logic` must be built as a static lib and linked once per binary. |
| `cmakeall` claims success but no binary                | Check `Build/` dirs were actually populated; some modules silently skip on missing deps. |

## 5. Build artifacts (where things land)

```
engine-main/
├── libraries/Build/       libblockmango_libraries.a
├── engine/Build/          libLordEngine.a
├── logic/Build/           libblockmango_logic.a
├── clothes/Build/         libblockmango_clothes.a
├── client/Build/          libblockmango.so  (Android)
├── server/Build/          Server            (Linux executable)
└── client/Shells/Android/Blockmango/app/src/main/jniLibs/<abi>/
                           libblockmango.so  (copied for APK packaging)
```

## 6. Clean

```bash
cd engine-main
./clean                    # removes all Build/ directories
# or per-module:
rm -rf engine/Build logic/Build client/Build server/Build
```

## 7. CI (future)

A GitHub Actions workflow will be added in M1 to:

1. Build the server on `ubuntu-22.04`.
2. Build the Android client `.so` libs with NDK r25c.
3. Assemble the debug APK.
4. Upload artifacts on tag pushes.

Until then, builds are local-only.
