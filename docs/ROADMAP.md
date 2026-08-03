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

## M1 — Build system unification + mobile-only enforcement  ⬜

**Goal:** One CMake entrypoint builds both binaries; PC/Win32 paths are quarantined.

- [ ] Add a root `engine-main/CMakeLists.txt` that does `add_subdirectory()` for
      `libraries`, `engine`, `logic`, `clothes`, `client`, `server` in dependency order.
- [ ] Bump every `cmake_minimum_required(VERSION 2.8)` to `3.10`.
- [ ] Add a `LORD_SERVER` CMake option. When `ON`, only `libraries + logic + server` build
      (used by the Linux server build). When `OFF`, the full Android client build runs.
- [ ] Quarantine `client/Shells/Win32/` and `server/WinServer.sln` under a
      `legacy/` folder (or delete if git history is enough). Remove from the build graph.
- [ ] Add a GitHub Actions workflow that builds the server on `ubuntu-22.04` and the
      client `.so` libs with NDK r25c on every push to `main`.
- [ ] Verify `engine-main/cmakeall` still works as a thin wrapper around the new root
      CMake (so existing muscle memory doesn't break).

**Acceptance:**
- `cmake -B build -S engine-main -DLORD_SERVER=ON && cmake --build build` produces a
  working `Server` binary on Linux.
- `cmake -B build -S engine-main -DLORD_BUILD_PLATFORM_ANDROID=ON -DCMAKE_TOOLCHAIN_FILE=...`
  produces `libblockmango.so` for all three ABIs.
- No Win32 / `.sln` / `.vcproj` file is referenced by any active CMake target.

---

## M2 — Server-authoritative world generation wiring  ⬜

**Goal:** The server generates chunks; the client only requests and renders them.

- [ ] Audit `server/src/Blockman/World/AnvilManagerServer.{h,cpp}` and confirm it owns the
      `ChunkProviderGenerate` instance for its `Room`.
- [ ] Wire `Room` → `WorldProvider` → `ChunkProviderGenerate` → `WorldGenerator` end to end.
- [ ] Implement `C2SRequestChunk` / `C2SRequestChunkBulk` handlers in
      `server/src/Network/C2SPacketHandles/`.
- [ ] Implement `S2CChunkData`, `S2CWorldInfo`, `S2CBlockChange`, `S2CMultiBlockChange`
      serializers in `logic/Src/Network/protocol/`.
- [ ] Add a per-`ClientPeer` chunk-request queue with a configurable per-tick budget.
- [ ] Add Anvil lazy-write (chunk persists only after N ticks unmodified).
- [ ] Strip `WorldGenerator` references from `client/Src/` (the client must not call it).
- [ ] Gate `logic/Src/WorldGenerator/` behind `LORD_SERVER` so the client build excludes it.

**Acceptance:**
- Connect a mock `ClientPeer` to a `Room` with a known seed, request chunks (0,0)…(3,3),
  and verify the streamed `S2CChunkData` matches a golden snapshot.
- `grep -r "WorldGenerator" engine-main/client/Src/` returns no matches (excluding comments).
- A second connect to the same `Room` after restart loads chunks from Anvil without
  regenerating them.

---

## M3 — Client chunk cache + rendering pipeline  ⬜

**Goal:** The Android client renders streamed chunks at a stable 30+ FPS on mid-range phones.

- [ ] Implement the client chunk cache (LRU, configurable size; default 256 chunks).
- [ ] Implement the chunk mesher: convert a `Chunk`'s sections into a GLES2 vertex buffer
      with face culling against neighbours.
- [ ] Implement the chunk request policy: based on player position + render distance,
      request chunks ahead of movement and evict behind.
- [ ] Implement optimistic block placement (client predicts, reverts on server reject).
- [ ] Implement `S2CBlockChange` / `S2CMultiBlockChange` application to the cache + remesh.

**Acceptance:**
- Walking in a straight line on a populated server sustains ≥30 FPS on a Pixel 4a-class
  device at render distance 6.
- Placing/breaking a block feels responsive (<150 ms to server confirmation).

---

## M4 — Persistence + multi-room  ⬜

**Goal:** Multiple worlds per server; edits survive restarts.

- [ ] `RoomManager` supports N concurrent `Room`s, each with its own seed + world dir.
- [ ] Room routing: `ClientPeer` joins a specific room by id; packets are scoped to that room.
- [ ] Anvil region files written under `worlds/<room_id>/region/`.
- [ ] Player profile (last room, last position, inventory) persisted to MySQL.
- [ ] Graceful shutdown: on SIGTERM, flush all dirty chunks to Anvil before exiting.

**Acceptance:**
- Two players in two different rooms on the same server never see each other's chunks.
- Kill -9 the server, restart, reconnect: chunks + player positions are intact.

---

## M5 — Lua scripting surface  ⬜

**Goal:** Server-side game modes are written in Lua, not C++.

- [ ] Audit `server/src/LuaRegister/` and document the existing C++ → Lua bindings.
- [ ] Expose a stable worldgen API to Lua (`worldgen.set_seed`, `worldgen.get_biome`,
      `worldgen.register_decorator`, ...).
- [ ] Ship one example game mode (`survival.lua`) that uses the API.
- [ ] Hot-reload: editing a `.lua` file in `worlds/<room_id>/scripts/` reloads it without
      restarting the room.

**Acceptance:**
- A Lua script can register a custom biome decorator that places a new ore, and that ore
  appears in newly generated chunks without a server restart.

---

## M6 — Polish + release readiness  ⬜

**Goal:** The engine is shippable as a v0.1 to a small closed beta.

- [ ] Performance pass: profile + optimise the chunk mesher and the S2C packet encoder.
- [ ] Memory pass: cap client chunk cache by bytes, not by count; add OOM handling.
- [ ] Netcode pass: implement delta-compressed chunk updates, RakNet reliability tuning.
- [ ] Anti-cheat pass: server validates every `C2S` action (placement reach, movement
      speed, inventory sanity).
- [ ] Crash reporting: integrate a minimal crash dumper on both sides; upload to a configurable endpoint.
- [ ] Sign the release APK; produce signed `Server` binary with version stamping.
- [ ] Write `docs/OPERATIONS.md` (how to run a server, back up worlds, rotate logs).

**Acceptance:**
- 8 concurrent players on a single Room for 1 hour without crash or visible desync.
- Closed-beta build artefacts published under a `v0.1.0-beta` git tag.

---

## Backlog (unscheduled)

- Cross-dimensional travel (Nether / End) — needs `ChunkProviderHell` / `ChunkProviderEnd`
  server-side wiring + dimension-transition packets.
- Mobile-friendly server admin panel (HTTP UI in `server/src/HttpRequest`).
- Replay system: record `C2S` packets from a session, replay against a fresh `Room` for
  debugging / regression testing.
- Deterministic worldgen snapshot tests across platforms (Linux server, Android local
  test server).
- Consider migrating `engine/Dependencies/` to a vendored Conan or vcpkg graph for easier
  updates.
