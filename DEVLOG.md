# Devlog

Append-only milestone log for Blockman-GO-engine-10039. One entry per merged PR (or per
significant commit when working solo). Newest entries at the top.

Format:

```
## YYYY-MM-DD — <title>
Branch: <branch-name>
What: <one-line summary of the change>
Why: <milestone / issue reference, or short rationale>
```

---

## 2026-08-03 — M0 project bootstrap
Branch: main
What: Stood up the repo with README, docs (ARCHITECTURE, PROJECT_STRUCTURE, BUILD, WORLDGEN, ROADMAP), CONTRIBUTING, DEVLOG, NEXT_PROMPT, and a root .gitignore. No source under engine-main/ or engine-res-main/ was modified.
Why: M0 — establish project structure, mobile-only contract, and server-authoritative worldgen design before any code changes. Provides the baseline for M1 (build system unification).

---

## 2026-08-03 — M1 in-process server on Android + worldgen wiring
Branch: main
What: Major pivot from "mobile-only, quarantine Win32, dedicated Linux server" to "phone runs both client + server in one APK, keep Win32 as parallel target, no external tools". Concrete changes:
  - Server worldgen: ServerWorldProvider::{Surface,Hell,End}::createChunkGenerator now returns real ChunkProviderGenerate/Hell/End instead of throwing std::logic_error.
  - Protocol: added C2SRequestChunk, C2SRequestChunkBulk, S2CWorldInfo, S2CChunkData, S2CBlockChange, S2CMultiBlockChange packet types + struct declarations. Auto-registration via the existing template metaprogramming walks them automatically.
  - Server handler: C2SChunkPacketHandles serves chunk requests by encoding the chunk via Anvil's NBT+zlib encoder and streaming S2CChunkData back. Registered in ServerNetworkRecver.
  - Android server target: server/CMakeLists.txt + server/src/CMakeLists.txt now branch on LORD_BUILD_PLATFORM_ANDROID — when ON, the server builds as libGameServer.so (shared lib) instead of an executable; main.cpp is excluded; the JNI entry in ServerJni.cpp is included. Android include paths, link paths, and system libs (log, android) added.
  - JNI entry: ServerJni.cpp exposes nativeServerStart/Stop/IsRunning. The standalone config sets DISABLE_ROOM=1 (empty monitorAddr) and stubs all HTTP endpoints to 127.0.0.1:1 so the server runs without any external services.
  - Java side: ServerService.java wraps the JNI calls; registered in AndroidManifest.xml as a non-exported Service. EchoesGLSurfaceView.initGame() gained a LOCAL_MODE flag (default true) that skips EnterMiniGameTask (matchmaking) and instead starts the in-process server + connects directly to 127.0.0.1:19130. EchoesActivity.onDestroy() stops the server.
  - CI: .github/workflows/android-build.yml with workflow_dispatch ONLY (no push trigger, to conserve the 0.5 GB Actions storage quota). Sets up NDK r17c (last version with gnustl_static), builds deps + engine + client + server, assembles the APK, uploads artifacts with 30-day retention.
  - Docs: README, ROADMAP rewritten to reflect the new direction. Win32 is kept, not quarantined.
Why: User explicitly asked for "build GameServer for MOBILE and build client for MOBILE and make the server actually work and client connects to it without using any external tools, and then build the final apk, use GitHub Actions, but do NOT make it trigger build for each push". This commit implements all of those requirements at the code + config level; the first CI run will surface remaining build issues (missing prebuilt deps, gnustl_static migration).
Open: First green CI run on workflow_dispatch. Verify on a real device. Migrate gnustl_static → c++_shared to unblock modern NDK + arm64-v8a (tracked in M2).
