# Server-Authoritative World Generation

This is the central design contract of the Blockman-GO-engine-10039 fork. Everything else in
the project bends around this document.

## 1. Principle

**The server is the only entity that decides what blocks exist.** The client never runs
`WorldGenerator`, never calls `ChunkProviderGenerate`, never produces a `Chunk` from noise.
The client only:

1. connects to a server,
2. receives `S2CWorldInfo` (seed hash, spawn point, game rules),
3. requests chunks by coordinate as the player moves,
4. caches received chunks in an LRU,
5. renders them,
6. sends player actions (place / break / interact) as C2S requests,
7. applies block changes only after the server confirms them with an S2C packet.

If the client ever finds itself generating a chunk locally, that is a bug — the contract is
broken.

## 2. Why

- **Anti-cheat.** A client that cannot generate the world cannot fabricate ores, structures,
  or terrain to its advantage.
- **Consistency.** Every player on a server sees the same world because there is exactly one
  source of truth.
- **Persistence.** Server-side Anvil storage means player edits survive disconnects and
  server restarts. Client-side generation would lose edits on every reconnect.
- **Moddability.** World generation logic lives in one place; Lua scripts on the server can
  change terrain rules without shipping a client patch.

## 3. Pipeline (server side)

```
  Player connects → RoomManager spawns a Room with (seed, gameType, worldDir)
                          │
                          ▼
                   WorldProvider initialised
                          │
                          ▼
            AnvilManagerServer  ◀──── loads existing chunks from disk (Anvil .mca)
                          │
                          ▼
            ChunkProviderGenerate
                          │
                          ▼
            ┌─────────────────────────────┐
            │      WorldGenerator          │
            │  (logic/Src/WorldGenerator/) │
            │                             │
            │  1. GenLayer (biome map)     │
            │  2. NoiseGeneratorOctaves    │  ← heightmap, density, temperature, humidity
            │  3. BiomeGen + BiomeDecorator│  ← surface blocks, flora, ores
            │  4. MapGenerate              │  ← caves, ravines
            │  5. StructureStart / Pieces  │  ← villages, nether fortresses, etc.
            │  6. Chunk populated          │
            └──────────────┬──────────────┘
                           │
                           ▼
                  Chunk written to Anvil (cache + persist)
                           │
                           ▼
                  S2CChunkData packet → ClientPeer → client chunk cache → render
```

The shared classes doing the heavy lifting are already in
`engine-main/logic/Src/WorldGenerator/`:

| Class                          | Role                                                            |
|--------------------------------|----------------------------------------------------------------|
| `WorldGenerator`               | Orchestrator: runs the full pipeline for one chunk.            |
| `Anvil` / `AnvilManager`       | Region-file chunk storage (.mca). `AnvilManagerServer` is the server adapter. |
| `BiomeGen`                     | Generates the 2D biome map for a chunk.                        |
| `BiomeCache`                   | Memoises biome lookups.                                        |
| `BiomeDecorator`               | Places surface blocks, trees, grass, flowers, ores.            |
| `GenLayer`                     | Layered biome-scale-noise framework (zoom, fuzzy, edge, ...).  |
| `NoiseGeneratorPerlin`         | Single-octave Perlin.                                          |
| `NoiseGeneratorOctaves`        | Stacks multiple Perlin octaves (height, density, temp, humid). |
| `MapGenerate`                  | Cave / ravine carvers.                                         |
| `StructureStart` / `StructurePieces` / `StructureComponent` | Villages, fortresses, etc. |
| `FlatGeneratorInfo` / `FlatLayerInfo` | Flat-world preset.                                       |
| `WeightedRandomItem`           | Weighted random selection for loot / structure pieces.         |
| `ChunkProviderGenerate`        | Glue: given (cx, cz), produce a populated `Chunk`.             |
| `ChunkProviderFlat` / `ChunkProviderEnd` / `ChunkProviderHell` | Variants for flat / End / Nether. |

## 4. Packet contract

These are the world-gen-relevant packets. The exact wire format lives in
`engine-main/logic/Src/Network/protocol/` (`C2SPackets.h`, `S2CPackets.h`, `Serializer.h`).
The contract below is the target shape; mismatches with the current headers are tracked in
[`ROADMAP.md`](ROADMAP.md).

### 4.1 `S2CWorldInfo` (server → client, on connect)

Sent once right after `ClientPeer` is established, before any chunk data.

| Field           | Type        | Notes                                                       |
|-----------------|-------------|-------------------------------------------------------------|
| `worldSeedHash` | `u64`       | Hash of the seed (never send the raw seed — clients must not be able to reproduce terrain). |
| `spawnX/Y/Z`    | `i32,i32,i32` | Spawn point in block coords.                              |
| `gameType`      | `u8`        | Survival / Creative / Adventure / Spectator.               |
| `worldType`     | `u8`        | Default / Flat / Large Biomes / Amplified.                 |
| `dimension`     | `u8`        | Overworld / Nether / End.                                   |
| `rules`         | `kv[]`      | Gamerule overrides (doDaylightCycle, pvp, difficulty, ...). |

### 4.2 `C2SRequestChunk` (client → server)

| Field | Type  | Notes                                            |
|-------|-------|--------------------------------------------------|
| `cx`  | `i32` | Chunk X (block x >> 4).                          |
| `cz`  | `i32` | Chunk Z (block z >> 4).                          |

Server-side behaviour: generate (or fetch from Anvil cache), enqueue an `S2CChunkData` reply.
If the chunk is already in flight for this peer, drop the duplicate.

### 4.3 `C2SRequestChunkBulk` (client → server)

Same as above but with a list of `(cx, cz)` pairs. Server may coalesce into a single
compressed bulk reply or split into multiple `S2CChunkData` packets.

### 4.4 `S2CChunkData` (server → client)

| Field          | Type        | Notes                                                          |
|----------------|-------------|----------------------------------------------------------------|
| `cx`, `cz`     | `i32,i32`   | Chunk coords.                                                  |
| `sections`     | `Section[]` | Up to 16 vertical sections (each 16×16×16). Empty sections omitted. |
| `biomes`       | `u8[256]`   | Biome id per column.                                           |
| `heightmap`    | `u8[256]`   | Per-column heightmap (for client-side occlusion / shadows).    |
| `blockEntities`| `Nbt[]`     | Tile entities in this chunk (chests, signs, ...).              |
| `entities`     | `Nbt[]`     | Mobs / items standing in this chunk.                           |

Wire format: the same NBT + palette-encoded section layout used by the Anvil region files,
so the server can ship a chunk straight from disk without re-encoding.

### 4.5 `S2CBlockChange` / `S2CMultiBlockChange`

Server → client. Informs the client that one (or many) block(s) changed. The client updates
its chunk cache and re-renders. **The client is not allowed to apply a local block change
before this packet arrives** — local placement is optimistic only (predicted) and is
reverted if the server rejects the request.

## 5. Throttling & fairness

- Each `ClientPeer` has a chunk-request queue with a per-tick budget (default 4 chunks/tick
  ≈ 80 chunks/sec at 20 TPS). Requests beyond the budget are queued, not dropped.
- The server keeps an LRU of recently generated chunks per Room so a reconnecting player
  doesn't pay regeneration cost.
- Anvil region files are written lazily: a chunk is persisted only after it has been
  unmodified for N ticks (default 600 = 30 s), to avoid thrashing on heavy edit sessions.

## 6. What the client must NOT do

These are forbidden on the client side and will be enforced by code review + (eventually)
compile-time guards:

- Instantiating `WorldGenerator`, `ChunkProviderGenerate`, `NoiseGeneratorOctaves`,
  `BiomeGen`, `GenLayer`, `MapGenerate`, `StructureStart`, or any class under
  `logic/Src/WorldGenerator/` directly.
- Reading or writing `.mca` / Anvil region files.
- Computing a chunk's block data from a seed.
- Applying a block change from a C2S request locally before the S2C confirmation.

A future task will introduce a `LORD_SERVER` compile flag that excludes the entire
`WorldGenerator/` folder from the client build, making violation a link error.

## 7. Open questions (track in ROADMAP)

- Should the raw `worldSeed` ever leave the server? Current answer: no, only the hash. This
  blocks client-side structure prediction but also blocks legitimate client-side minimap
  mods. Revisit if a sanctioned minimap feature is needed.
- How do we handle `ChunkProviderEnd` / `ChunkProviderHell` for cross-dimensional travel —
  same `ClientPeer`, separate chunk queues per dimension?
- Structure generation can be expensive (villages span multiple chunks). Do we generate
  structures lazily on first chunk request, or eagerly at Room start? Current proposal:
  lazy, with a `structureMap` index persisted alongside Anvil so a half-generated village
  resumes correctly when the next chunk in its footprint is requested.

## 8. Testing strategy

- **Unit tests** on the shared `WorldGenerator` classes: given a fixed seed and chunk
  coords, the output `Chunk` must be byte-identical across runs (determinism).
- **Snapshot tests**: generate a 32×32 chunk grid with a known seed, snapshot the block
  palette per chunk, fail the build if a code change shifts the snapshot.
- **Server integration test**: spin up a `Room` with a test seed, connect a mock
  `ClientPeer`, request chunks, assert the streamed `S2CChunkData` matches the snapshot.
- **Client guard test**: the client build must fail to link if any `WorldGenerator/*.cpp` is
  referenced from `client/Src/`.

## 9. Custom world type — TERRAIN_TYPE_CUSTOM (sky islands)

The fork ships a **custom world generator** (`ChunkProviderCustom`) that produces a
distinctive "sky islands" world: floating blobs of stone/grass/dirt at varying heights,
separated by air, with no ocean and no bedrock floor. This is the default world type for
the in-process LOCAL_MODE server.

### 9.1 Why a custom generator

The vanilla `ChunkProviderGenerate` (overworld) is great for the original matchmaking
game, but it expects a full biome/structure pipeline that is not yet wired on the local
server. `ChunkProviderCustom` is self-contained — it only needs `NoiseGeneratorOctaves`
(no `BiomeGen`, no `MapGenerate`, no `StructureStart`) — so it works end-to-end on the
phone today, and gives the user something visually distinct from the default overworld so
they can immediately tell "this is the new custom world".

### 9.2 Where it lives

| File | Role |
|---|---|
| `engine-main/logic/Src/Chunk/ChunkProviderCustom.{h,cpp}` | The generator itself. Subclasses `IChunkProvider`. Uses 4-octave `NoiseGeneratorOctaves` for density + 2-octave for column height variation. |
| `engine-main/server/src/Blockman/World/ServerWorldProvider.cpp` | `ServerWorldProviderSurface::createChunkGenerator()` returns `ChunkProviderCustom` when the world's `generateOptions` field is the string `"custom"`. |
| `engine-main/server/src/Blockman/World/ServerWorld.cpp` | `createChunkService()` checks the same marker and wires `ChunkProviderCustom` directly as the chunk provider (instead of the disk reader). |
| `engine-main/server/src/Server.cpp` | `Server::init()` reads `m_config.worldType`; when it's `100` (TERRAIN_TYPE_CUSTOM), it force-generates chunk (0,0) and probes for a safe spawn Y. |
| `engine-main/server/src/ServerJni.cpp` | Stashes the Java-supplied `seed` into `RoomGameConfig::worldSeed` and sets `worldType = 100` by default. |
| `engine-main/server/src/Network/RoomClient.h` | `RoomGameConfig` now has `int64_t worldSeed` and `int worldType` fields. |
| `engine-main/client/Shells/Android/Blockmango/app/src/main/java/com/sandboxol/blockmango/EchoesGLSurfaceView.java` | `LOCAL_WORLD_RANDOM_SEED = true` — each launch generates a fresh random seed via `new java.util.Random().nextLong()`, so every session is a NEW world. |

### 9.3 Configuration flow

```
Java: ServerService.nativeServerStart(port, workDir, seed)
            │
            ▼
ServerJni.cpp: g_serverWorldSeed = seed
               g_serverWorldType = 100 (TERRAIN_TYPE_CUSTOM)
            │
            ▼
serverThreadMain(): cfg.worldSeed = g_serverWorldSeed.load()
                    cfg.worldType = g_serverWorldType
            │
            ▼
Server::init(cfg): m_config = cfg
                   m_serverWorld = ServerWorld::createWorld(name, cfg.worldSeed, cfg.worldType)
            │
            ▼
ServerWorld::createWorld(name, seed, 100):
    WorldSettings settings(seed, ..., TERRAIN_TYPE_DEFAULT, ...)
    settings.func_82750_a("custom")  // stash the marker
    World* w = new ServerWorld(..., settings, ...)
    // World ctor calls m_provider->registerWorld(this) which copies
    // settings.getGenerateOptions() → m_provider->generateOptions == "custom"
    w->createChunkService(loadRange)
            │
            ▼
ServerWorld::createChunkService():
    if (m_provider->generateOptions == "custom") {
        m_pChunkService->useChunkProvider<ChunkProviderCustom>(this, this->getSeed());
        m_pChunkService->useChunkStorage<ChunkReadableStorageFileServer>(this, mapPath);
    } else {
        m_pChunkService->useReadableStorage<ChunkReadableStorageFileServer>(this, mapPath);
    }
```

### 9.4 Why we use the generator directly (not the disk reader)

`ChunkService::useReadableStorage<T>()` sets `m_chunkProvider = T` AND `m_chunkStorage = T`.
For the custom world type, we want chunks to be **generated fresh on each request**, not
loaded from disk. So we split the two:
- `useChunkProvider<ChunkProviderCustom>` — `getChunk(x, z)` always generates fresh
- `useChunkStorage<ChunkReadableStorageFileServer>` — `saveChunk` still writes to disk

This means block edits made during a session ARE persisted to disk, but they are NEVER
read back (because the provider is the generator, not the disk reader). Effectively, every
app launch produces a brand-new world — exactly what the user asked for. The disk writes
are a harmless side effect (and could be turned off entirely by skipping `useChunkStorage`).

### 9.5 Runtime chunk request flow (client ↔ server)

```
Client: Blockman::generateWorld() preloads 7×7 spawn-area chunks
            │
            ▼
ChunkServiceClient::getChunk(x, z)
    cache miss → m_chunkProvider->provideChunk(x, z)
              → ChunkReadableStorageFileClient returns NonexistentChunk (no .mca file)
    onChunkMiss(x, z) → ClientPacketSender::sendRequestChunk(x, z) → C2SPacketRequestChunk
            │
            ▼
Server: C2SChunkPacketHandles::handlePacket(C2SPacketRequestChunk)
    ChunkService::getChunk(x, z) → ChunkProviderCustom::provideChunk(x, z)
    encodeChunkToBlob(chunk) → S2CPacketChunkData → sendPacket
            │
            ▼
Client: S2CChunkPacketHandles::handlePacket(S2CPacketChunkData)
    ZlibInputStream + deserialize<ChunkWithMeta> → Chunk*
    ChunkService::injectChunk(chunkPtr) → cache + prepareChunk
    Next getChunk(x, z) hits the cache
```

### 9.6 Seed propagation contract

- The Java side generates a fresh `long worldSeed` on every `initGame()` call (when
  `LOCAL_WORLD_RANDOM_SEED = true`).
- The seed is passed via JNI to `nativeServerStart(port, workDir, seed)`.
- The C++ side stashes it in `g_serverWorldSeed`, then copies it into `RoomGameConfig::worldSeed`.
- `ServerWorld::createWorld(name, seed, worldType)` builds a `WorldSettings(seed, ...)`.
- The `WorldSettings` seed flows through `WorldInfo::m_randomSeed` → `World::getSeed()`
  → `ChunkProviderCustom::ChunkProviderCustom(world, seed)`.
- The seed hash (FNV-1a) is sent to the client in `S2CPacketWorldInfo::m_worldSeedHash`
  — the raw seed never leaves the server, so the client cannot reproduce the terrain
  locally even if it wanted to.

