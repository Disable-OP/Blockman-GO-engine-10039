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

## 2026-08-29 — M3 stability + rendering/lighting fix + real world population
Branch: main
What: Fixed the three blockers reported from on-device testing:
1. **Client rendering + lighting (root cause found)**: `S2CChunkPacketHandles` decoded network chunks with the base `ChunkWithMeta` binding, constructing base `Chunk` objects containing base `Section`s. Base `Section`'s light accessors are no-op (return 0 → pitch-black world), it has no visibility set, and `LightTransferThread`/`SectionRenderer` `static_pointer_cast<ChunkClient>` on it is UB → the random mid-game crashes. Now decodes with `ChunkClientWithMeta` (same binding the Anvil disk path uses) → `ChunkClient` + `SectionClient` with real light NibbleArrays. Also: after `injectChunk`, the whole 16×256×16 chunk column is now flagged for re-render via `World::markBlockRangeForRenderUpdate` (previously nothing ever marked injected sections dirty, so the chunk never appeared).
2. **Connection instability / crashes**: (a) `ServerJni.cpp` got a proper startup state machine (IDLE→STARTING→RUNNING) — `nativeServerStart` is now idempotent; previously the "already running" guard only flipped after multi-second init, so the Reset button / re-init double-`new Server()` tripped `LordAssert(!ms_pSingleton)` → SIGABRT. `nativeServerIsRunning` now returns true only once the RakNet socket is bound. (b) Java side replaced the blind `Thread.sleep(500)` with a background readiness poll (up to 30 s) that only enters the game once the server reports RUNNING — no more racing the server's first-time world generation.
3. **Actual world generation**: wired the missing population step — `ServerWorld::populateChunk(x,z)` decorates chunks (trees, ores, flowers, mushrooms, cacti...) via `BiomeDecorator` with the vanilla per-chunk seed mix, guarded by the persisted `TerrainPopulated` Anvil flag, requiring the +x/+z/+x+z neighbours (vanilla semantics). Called from the chunk-request handlers BEFORE serialization so the client always receives fully-populated chunks. Spawn selection now scans a pre-generated 3×3 chunk area for a grass/sand/dirt surface (any world type, previously gated on the dead custom-world marker and only probed chunk (0,0)). Periodic `saveAllChunks` flush every 600 ticks (~30 s) persists edits + decoration to Anvil region files. `ServerJni` default worldType is now `TERRAIN_TYPE_DEFAULT` (vanilla overworld) instead of the stale custom marker 100.
Why: User reported broken client rendering/lighting, intermittent connect-then-crash, and that the server had no real world generation. All three root causes were traced end-to-end through the code; fixes verified by host-GCC syntax checks of every changed TU (server: ServerWorld.cpp, Server.cpp, C2SChunkPacketHandles.cpp; client: S2CChunkPacketHandles.cpp) — full validation via the manual CI APK build + on-device test.
Open: Run the "Android Build (Manual)" workflow, install the APK, verify: spawn on grass, world is lit, trees/ores generate, no crash on Reset.

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

---

## 2026-08-04 — M2 custom world generation + full client/server chunk pipeline
Branch: main
What: Wrote the first end-to-end "spawn in a new world with custom world generation and connect to gameserver" pipeline. The previous M1 commit wired the packet infrastructure but the worldgen was never actually executed at runtime — `ServerWorld::createChunkService()` used `useReadableStorage<ChunkReadableStorageFileServer>`, which only reads chunks from disk. The client never sent `C2SRequestChunk` and had no `S2CPacketChunkData` handler, so even with the packets defined, the runtime was empty.

Concrete changes:

1. **New chunk generator: `ChunkProviderCustom`** (`logic/Src/Chunk/ChunkProviderCustom.{h,cpp}`). Self-contained "sky islands" world type — uses 4-octave `NoiseGeneratorOctaves` for density + 2-octave for column height variation. Produces floating blobs of stone/grass/dirt centred on y=64, no ocean, no bedrock floor. Visually distinct from the default overworld so the user can immediately tell it's a new world. No dependency on `BiomeGen`/`MapGenerate`/`StructureStart` (so it works without the full biome pipeline).

2. **Config fields: `RoomGameConfig::worldSeed` + `worldType`** (`Network/RoomClient.h`). Two new fields propagate the Java-supplied seed and the world-type selector through to `ServerWorld::createWorld`. `worldType == 100` (TERRAIN_TYPE_CUSTOM) is the marker that selects `ChunkProviderCustom`.

3. **JNI wiring: `ServerJni.cpp`** now stashes the Java-supplied `seed` into `g_serverWorldSeed` (an `atomic<int64_t>`) and sets `g_serverWorldType = 100` by default. `serverThreadMain()` copies both into `cfg.worldSeed` / `cfg.worldType`. The previously-ignored `seed` arg is now honoured.

4. **`ServerWorld::createWorld(name, seed, worldType)`** — new overload that builds `WorldSettings(seed, ...)` and stashes the marker string `"custom"` in `generateOptions` when `worldType == 100`. The marker propagates: `WorldSettings::m_generateOptions` → `WorldInfo::m_generatorOptions` → `WorldProvider::generateOptions` (via `WorldProvider::registerWorld()`).

5. **`ServerWorldProviderSurface::createChunkGenerator()`** now checks `generateOptions == "custom"` and returns `ChunkProviderCustom` instead of delegating to the base class.

6. **`ServerWorld::createChunkService()`** now branches on the same marker: when custom, it wires `useChunkProvider<ChunkProviderCustom>(this, getSeed())` + `useChunkStorage<ChunkReadableStorageFileServer>(this, mapPath)` separately — so `getChunk(x, z)` always generates fresh (the generator IS the provider), while `saveChunk` still writes to disk. Result: every app launch produces a brand-new world; edits within a session are visible (cache holds them) but are never read back from disk.

7. **`Server::init()`** now reads `m_config.worldType`; when it's `100`, it force-generates chunk (0,0) and probes every column in [0,15]² for a safe spawn Y. The first column with a height value in (1, 127) becomes the spawn point — so the player actually stands on solid ground instead of falling through sky.

8. **`S2CPacketWorldInfo` sent on login**: `C2SInitPacketHandles::handlePacket(C2SPacketLogin)` now constructs an `S2CPacketWorldInfo` with the spawn pos, world type, dimension, and a FNV-1a hash of the seed (raw seed never leaves the server). Sent right after `sendGameInfo`.

9. **Client-side S2C handlers: `S2CChunkPacketHandles.{h,cpp}`**. New file under `client/Src/Network/S2CPacketHandles/`. Two handlers:
   - `handlePacket(S2CPacketWorldInfo)` — overrides the client's local hardcoded spawn with the server's authoritative one via `World::setSpawnLocation`.
   - `handlePacket(S2CPacketChunkData)` — wraps the byte blob in a `ZlibInputStream`, calls `deserialize<ChunkWithMeta>`, hands the resulting `Chunk*` to a `shared_ptr`, and calls `ChunkService::injectChunk()` so the next `getChunk(x, z)` hits the cache.

10. **`ClientNetworkRecver`** now registers `S2CChunkPacketHandles` in the dispatch.

11. **`ChunkService` gained two new methods**:
    - `injectChunk(ChunkPtr)` — public, adds to cache + calls `prepareChunk`. Used by the S2C handler.
    - `onChunkMiss(int x, int z)` — protected virtual, default no-op. Called by `getChunk()` when the local provider returns a `NonexistentChunk`. The client overrides this to fire a `C2SRequestChunk` instead of returning empty.

12. **`ChunkServiceClient::onChunkMiss(int x, int z)`** — sends `C2SPacketRequestChunk(x, z)` via `ClientPacketSender::sendRequestChunk`. Fire-and-forget: `getChunk()` returns the `NonexistentChunk` to the renderer (which sees empty/air), and when the network reply arrives, `injectChunk()` puts the real chunk in the cache. The next frame's `getChunk()` hits the cache.

13. **`ClientPacketSender::sendRequestChunk(int, int)`** — new method that constructs and sends `C2SPacketRequestChunk`.

14. **`EchoesGLSurfaceView.java`** now has `LOCAL_WORLD_RANDOM_SEED = true` — each launch generates a fresh `new java.util.Random().nextLong()` seed, so every session is a NEW world. Sleep before connecting bumped from 300 ms to 500 ms to give the server time to generate the spawn chunk.

Why: User explicitly asked "Now make it spawn in a new world with custom world generation and connect to gameserver". This commit implements every part of that: (a) custom world generator (`ChunkProviderCustom`), (b) spawn position computation from generated terrain (not hardcoded), (c) full client ↔ server chunk pipeline wired end-to-end (`C2SRequestChunk` on cache miss → server generates → `S2CChunkData` → client decodes + injects), (d) per-launch random seed so each session is genuinely a new world.

Open: First green CI run on workflow_dispatch to validate the build. Then on-device test. The `ChunkProviderCustom` terrain is intentionally simple — future iterations can add caves, ores, trees, etc. without changing the wiring.

