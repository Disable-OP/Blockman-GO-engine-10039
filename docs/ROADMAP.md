# Roadmap

Phased milestones for the Blockman-GO-engine-10039 fork. Each milestone is "done" when its
acceptance criteria pass and the work is committed + pushed to `main`.

Legend:
- ✅ done
- 🚧 in progress
- ⬜ not started

---

## M0 — Project bootstrap  ✅

**Goal:** Stand up the repo, docs, and contribution discipline.

- [x] Clone repo, inspect extracted engine.
- [x] Write `README.md`, `docs/ARCHITECTURE.md`, `docs/PROJECT_STRUCTURE.md`,
      `docs/BUILD.md`, `docs/WORLDGEN.md`, this `ROADMAP.md`.
- [x] Write `CONTRIBUTING.md`, `DEVLOG.md`, `NEXT_PROMPT.md`.
- [x] Add root `.gitignore`.
- [x] Commit + push the bootstrap to `main`.

**Acceptance:** `main` builds the same way it did before the bootstrap (no source files
touched under `engine-main/` or `engine-res-main/`).

---

## M1 — In-process server on Android + worldgen wiring  ✅

**Goal:** The phone runs BOTH the client (`libBlockMan.so`) AND the server
(`libGameServer.so`) in one APK. The server runs actual world generation. The
client connects to `127.0.0.1:19130` directly — no PC, no external tools.

- [x] Wire `ServerWorldProvider::createChunkGenerator` to return real
      `ChunkProviderGenerate` / `ChunkProviderHell` / `ChunkProviderEnd`
      instead of throwing `std::logic_error`. (Was a one-liner — the base
      class already does the right thing.)
- [x] Add `C2SRequestChunk` + `C2SRequestChunkBulk` packet types.
- [x] Add `S2CChunkData`, `S2CWorldInfo`, `S2CBlockChange`, `S2CMultiBlockChange`
      packet types.
- [x] Add server-side handler `C2SChunkPacketHandles` that serves chunk requests
      by encoding the chunk via Anvil's NBT+zlib encoder and streaming it back.
- [x] Add `LORD_BUILD_PLATFORM_ANDROID` option to `server/CMakeLists.txt` +
      `server/src/CMakeLists.txt`. When ON, the server builds as a shared
      library (`libGameServer.so`) instead of an executable; `main.cpp` is
      excluded; the JNI entry point in `ServerJni.cpp` is included.
- [x] Write `ServerJni.cpp` — JNI entry (`nativeServerStart` / `nativeServerStop`
      / `nativeServerIsRunning`) that boots `Server` with a standalone config:
      `DISABLE_ROOM=1`, all HTTP endpoints stubbed to `127.0.0.1:1`.
- [x] Write `ServerService.java` — Android `Service` that wraps the JNI calls
      and is registered in `AndroidManifest.xml`.
- [x] Patch `EchoesGLSurfaceView.initGame()` to add a `LOCAL_MODE` flag (default
      `true`) that skips `EnterMiniGameTask` (matchmaking) and instead starts
      the in-process server + connects directly to `127.0.0.1:19130`.
- [x] Patch `EchoesActivity.onDestroy()` to stop the in-process server.
- [x] Keep the Win32 shell ("WinShell-Blockman") as a parallel build target —
      do NOT quarantine it.
- [x] Add `.github/workflows/android-build.yml` with **`workflow_dispatch`
      only** (no push trigger) to conserve the 0.5 GB Actions storage quota.
- [x] **Custom world generation**: wrote `ChunkProviderCustom` (sky islands)
      and wired it as the default world type for LOCAL_MODE. See
      [`docs/WORLDGEN.md` §9](WORLDGEN.md#9-custom-world-type--terrain_type_custom-sky-islands).
- [x] **Full client ↔ server chunk pipeline**: client sends `C2SRequestChunk`
      on cache miss; server generates via `ChunkProviderCustom`; client decodes
      `S2CChunkData` and injects into cache. No more dead code paths.
- [x] **Per-launch random seed**: each app launch produces a brand-new world
      (configurable via `LOCAL_WORLD_RANDOM_SEED` in `EchoesGLSurfaceView.java`).
- [x] **Safe spawn computation**: server force-generates chunk (0,0) and probes
      for a solid block to stand on, instead of using a hardcoded (4, 60, -17).
- [x] **`S2CPacketWorldInfo` sent on login**: client receives spawn pos + world
      type + dimension + seed hash (raw seed never leaves the server).
- [ ] First green CI run on `workflow_dispatch` — produces a signed debug APK.
- [ ] Verify on a real device: install APK, launch, see terrain render.

**Acceptance:**
- `cmake -B build -S engine-main/server -DLORD_BUILD_PLATFORM_ANDROID=ON
  -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake
  -DANDROID_ABI=armeabi-v7a && cmake --build build` produces `libGameServer.so`.
- The APK installs on a phone, launches, and the client connects to the
  in-process server. World terrain renders (proving the chunk pipeline works).
- `grep -rn "throw std::logic_error" engine-main/server/src/Blockman/World/`
  returns no matches (the worldgen stub is gone).
- Each app launch shows DIFFERENT terrain (random seed → different sky islands).

---

## M2 — Build system unification  ⬜

**Goal:** One CMake entrypoint builds both binaries; PC/Win32 paths stay
first-class (not quarantined).

- [ ] Add a root `engine-main/CMakeLists.txt` that does `add_subdirectory()`
      for `libraries`, `engine`, `logic`, `clothes`, `client`, `server` in
      dependency order.
- [ ] Bump every `cmake_minimum_required(VERSION 2.8)` to `3.10`.
- [ ] Add a `LORD_BUILD_SERVER_ONLY` option for headless Linux server builds.
- [ ] Migrate Android build from `ndk-build` + `gnustl_static` to CMake +
      `c++_shared` so modern NDK (r25c+) can be used. This unblocks arm64-v8a.
- [ ] Pre-build all third-party deps (FreeImage, FreeType, LuaJIT, zlib, RakNet,
      curl, cpr, g3log) for Android + Linux + Win32 and check them into the
      repo (or fetch them via a CMake `FetchContent` / vcpkg graph).

**Acceptance:**
- `cmake -B build -S engine-main && cmake --build build` produces both
  `libBlockMan.so` + `libGameServer.so` for Android in one invocation.
- `cmake -B build -S engine-main -DLORD_BUILD_SERVER_ONLY=ON && cmake --build build`
  produces the Linux `Server` binary.

---

## M3 — Client chunk cache + rendering pipeline  ⬜

**Goal:** The Android client renders streamed chunks at a stable 30+ FPS on
mid-range phones.

- [ ] Implement the client chunk cache (LRU, configurable size; default 256 chunks).
- [ ] Implement the chunk mesher: convert a `Chunk`'s sections into a GLES2
      vertex buffer with face culling against neighbours.
- [ ] Implement the chunk request policy: based on player position + render
      distance, request chunks ahead of movement and evict behind.
- [ ] Implement optimistic block placement (client predicts, reverts on reject).
- [ ] Implement `S2CBlockChange` / `S2CMultiBlockChange` application to the
      cache + remesh.
- [ ] Strip the client's local-disk chunk read path (the original code reads
      `.mca` files from `<mapPath>/<mapId>/`); the client must only get chunks
      from the server now.

**Acceptance:**
- Walking in a straight line on a populated server sustains ≥30 FPS on a
  Pixel 4a-class device at render distance 6.
- Placing/breaking a block feels responsive (<150 ms to server confirmation).

---

## M4 — Persistence + multi-room  ⬜

**Goal:** Multiple worlds per server; edits survive restarts.

- [ ] `RoomManager` supports N concurrent `Room`s, each with its own seed + world dir.
- [ ] Room routing: `ClientPeer` joins a specific room by id; packets are scoped to that room.
- [ ] Anvil region files written under `worlds/<room_id>/region/`.
- [ ] Player profile (last room, last position, inventory) persisted to local JSON
      (no MySQL — the in-process server has no DB).
- [ ] Graceful shutdown: on app exit, flush all dirty chunks to Anvil.

**Acceptance:**
- Two players in two different rooms on the same server never see each other's chunks.
- Kill -9 the app, relaunch, reconnect: chunks + player positions are intact.

---

## M5 — Lua scripting surface  ⬜

**Goal:** Game modes are written in Lua, not C++.

- [ ] Audit `server/src/LuaRegister/` and document the existing C++ → Lua bindings.
- [ ] Expose a stable worldgen API to Lua.
- [ ] Ship one example game mode (`survival.lua`) that uses the API.
- [ ] Hot-reload: editing a `.lua` file reloads it without restarting the room.

---

## M6 — Polish + release readiness  ⬜

**Goal:** The engine is shippable as a v0.1 to a small closed beta.

- [ ] Performance pass: profile + optimise the chunk mesher and the S2C packet encoder.
- [ ] Memory pass: cap client chunk cache by bytes, not by count; add OOM handling.
- [ ] Netcode pass: implement delta-compressed chunk updates, RakNet reliability tuning.
- [ ] Anti-cheat pass: server validates every `C2S` action (placement reach, movement
      speed, inventory sanity).
- [ ] Crash reporting: integrate a minimal crash dumper on both sides; write tombstones
      to `<filesDir>/crashes/`.
- [ ] Sign the release APK; produce a versioned `libGameServer.so` with version stamping.
- [ ] Write `docs/OPERATIONS.md` (how to back up worlds, rotate logs).

**Acceptance:**
- 8 concurrent players on a single Room for 1 hour without crash or visible desync.
- Closed-beta build artefacts published under a `v0.1.0-beta` git tag.

---

## Backlog (unscheduled)

- Cross-dimensional travel (Nether / End) — needs `ChunkProviderHell` / `ChunkProviderEnd`
  server-side wiring + dimension-transition packets.
- LAN co-op: expose the in-process server's UDP port on the LAN so other phones on the
  same Wi-Fi can join (turn the single-player server into a listen server).
- Replay system: record `C2S` packets from a session, replay against a fresh `Room`.
- Migrate `engine/Dependencies/` to a vendored Conan or vcpkg graph for easier updates.
- Android foreground service notification (so the server keeps running when the app is
  backgrounded — currently it stops on `onDestroy`).
