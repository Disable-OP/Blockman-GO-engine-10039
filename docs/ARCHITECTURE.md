# Architecture

This document describes the runtime architecture of the Blockman-GO-engine-10039 fork after
the mobile-only + server-authoritative-worldgen refactor lands. It is the target state, not
necessarily the current state — gaps between this document and the code are tracked as
issues / roadmap items in [`ROADMAP.md`](ROADMAP.md).

## 1. High-level topology

```
┌─────────────────────────────┐         ┌─────────────────────────────────────┐
│   Android Client (phone)    │         │        Game Server (Linux)          │
│  ─────────────────────────  │         │  ─────────────────────────────────  │
│  • GLES2 renderer (Lord)    │  TCP/   │  • RakNet listener (19130)          │
│  • Scene graph + audio      │  UDP    │  • RoomManager (one room per world) │
│  • Input / touch / camera   │ ◀─────▶ │  • ChunkProviderGenerate            │
│  • Chunk cache (LRU)        │ RakNet  │  • AnvilManagerServer (persistence) │
│  • C2S packet sender        │ packets │  • Entity tick loop (server-author.)│
│  • S2C packet handler       │         │  • Lua scripting (LuaRegister)      │
│  • NO world generation      │         │  • MySQL / Redis (players, rooms)   │
│  • NO block authority       │         │  • HTTP API (login, props, ranks)   │
└─────────────────────────────┘         └─────────────────────────────────────┘
              │                                          │
              │                                          │
              └──────────── shared logic (linked into both) ──────────
                              engine-main/logic/Src
                  WorldGenerator · Block · Chunk · Entity · NBT · Network/protocol
```

The single most important rule encoded in that diagram: **the client never decides what a
block is.** Every block the client renders arrived in a `S2C` chunk packet from the server.
If the client tries to place a block, it sends a `C2S` request; the server decides whether
the placement is legal, applies it, and broadcasts the result.

## 2. Module responsibilities

### 2.1 `engine-main/engine` — "Lord" core engine

A platform-abstract engine layer. On Android it is built with `LORD_BUILD_PLATFORM_ANDROID=TRUE`
and renders through the `GLES2Render` module. Responsibilities:

- **Core** (`Src/Core/`): Actor, Animation, Audio, Effects, Math, Memory, Model, Navigation,
  Object system, Resource manager, Scene graph, Stream/IO, Tessellator, Logging.
- **GLES2Render** (`Src/GLES2Render/`): Frame buffers, GPU buffers, render targets, render
  states, depth/stencil — the thin GLES2 backend used by Core's render abstraction.

The engine has no game logic. It does not know what a "block" is. The game layer sits on top.

### 2.2 `engine-main/logic` — shared game logic

Linked into **both** client and server. This is the contract that keeps them in sync.

- **Block** — block definitions, materials, archive, loader, manager, rail logic.
- **Chunk** — `Chunk`, `ChunkCache`, `ChunkProviderGenerate`, `ChunkProviderFlat`,
  `ChunkProviderEnd`, `ChunkProviderHell`, `ChunkService`, `ChunkReadableStorageFile`.
- **World** — `World`, `WorldProvider`, `WorldInfo`, `WorldSettings`, `WorldChunkManager`,
  `WorldEffectManager`, `Section`, `NibbleArray`, `SchematicModel`.
- **WorldGenerator** — `WorldGenerator`, `Anvil`/`AnvilManager`, `BiomeGen`, `BiomeCache`,
  `BiomeDecorator`, `GenLayer`, `NoiseGeneratorPerlin`, `NoiseGeneratorOctaves`,
  `MapGenerate`, `StructureStart`/`StructurePieces`/`StructureComponent`,
  `FlatGeneratorInfo`, `WeightedRandomItem`.
- **Entity / AI** — entities, AI goals, enchantments.
- **Item / Inventory / TileEntity / Trigger** — items, inventories, block entities, triggers.
- **NBT** — NBT serialization (chunk save format).
- **Network/protocol** — `C2SPackets`, `S2CPackets`, `DataPacket`, `DataPacketFactory`,
  `NetDataStream`, `Serializer`, `PacketHandlerRegistry`, `RaknetInstance`,
  `AutoRegisterPacketC2S/S2C.h` (auto-generated packet registration).
- **Script** — Lua glue for game-mode scripts.

Anything in `logic/` MUST compile and link cleanly into both client and server. Code that
only makes sense on one side belongs in `client/Src/` or `server/src/`, not here.

### 2.3 `engine-main/client` — Android client

- **Src/** — game-side client code: `game.cpp` entry, `ShellInterface`, `BattleSummary`,
  `iniFileObj`, plus `Blockman/` (client world, chunk provider, render entities), `Network/`,
  `Setting/`, `Util/`, `External/`.
- **Shells/Android/Blockmango/** — Gradle project (the APK shell). Native libs built by
  CMake/NDK are packaged into `app/src/main/jniLibs/` by `copyBin.bat` / Gradle tasks.
- **Shells/Win32/** — **legacy, do not extend.** Kept only as a reference snapshot; will be
  removed once the Android shell is the sole target.
- **curl/** — prebuilt libcurl for Android and Windows (used by the client for HTTP).

### 2.4 `engine-main/server` — Linux game server

- **src/Server.cpp / Server.h / main.cpp** — process entry, config load, room boot.
- **src/Network/** — `ServerNetwork`, `ServerNetworkCore`, `ServerNetworkRecver`,
  `ServerPacketSender`, `ClientPeer`, `RoomClient`, `AsyncClient`, `GameCmdMgr`,
  `C2SPacketDispatcher`, `C2SPacketHandles/`, `HttpRequest`, `ManorRequest`.
- **src/RoomManager** — one room = one running world instance with its own seed and chunk store.
- **src/Blockman/World/** — `AnvilManagerServer`, `AnvilServer` — server-side chunk
  persistence + generation entry points. **This is where the server-authoritative worldgen
  wiring lives.**
- **src/DB/** — MySQL/Redis access for player profiles, rooms, commodities.
- **src/Script/** + **src/LuaRegister/** — server-side Lua scripting; `WorldGenerator_Register.cpp`
  exposes the worldgen API to Lua game scripts.
- **src/CommodityManager** — in-game economy.
- **src/ConsoleCmd** — admin console.
- **src/Behaviac** — behaviour-tree integration (optional; gated behind a build flag).
- **dependencies/linux/** — Redis (hiredis), MySQL client, etc.

### 2.5 `engine-main/clothes` — avatar / cosmetic system

Standalone module that builds character meshes, clothes, and skins. Consumed by the client
for rendering and by the server (data only) for persistence.

### 2.6 `engine-main/libraries` — shared static libraries

Cross-module helpers (currently Android-only under `src/android/`). Linked into both
engine and game code as needed.

### 2.7 `engine-res-main` — runtime resources

- **Media/** — textures, models, audio, UI assets. Mounted by the resource manager at startup.
- **recipe/** — crafting recipes.
- **resource.cfg** — master resource manifest (which packs to mount, search order).

## 3. Process model

### Client process (Android)
1. Native shared library `libblockmango.so` loaded by the Java activity in
   `client/Shells/Android/Blockmango/app/`.
2. `game.cpp` boots the Lord engine, mounts `engine-res-main`, connects to the configured
   server address, and enters the main render loop.
3. Touch / sensor input → C2S packets → server.
4. S2C packets → chunk cache + entity state → render.

### Server process (Linux)
1. `main.cpp` parses config, connects to MySQL/Redis, opens the RakNet listener on `19130`.
2. `RoomManager` creates one `Room` per active world. Each `Room` owns:
   - a `World` instance with a fixed seed,
   - an `AnvilManagerServer` (chunk generation + persistence),
   - a `ChunkProviderGenerate` wired to that world's `WorldGenerator`,
   - the entity tick loop,
   - the set of connected `ClientPeer`s.
3. On `ClientPeer` connect: server sends world info, seed hash, and the player's last known
   position. From then on the client requests chunks by coordinate; the server generates
   (or loads from Anvil) and streams them back.

## 4. Network protocol

Packets are defined declaratively in `logic/Src/Network/protocol/`:
- `C2SPackets.h` / `S2CPackets.h` — packet id + payload struct per message.
- `AutoRegisterPacketC2S.h` / `AutoRegisterPacketS2C.h` — auto-generated handler
  registration (do not hand-edit; regenerated by the build).
- `DataPacket` / `NetDataStream` / `Serializer` — wire format.

**World-gen-relevant packets** (target contract, see [`WORLDGEN.md`](WORLDGEN.md) for full
spec):

| Direction | Packet                          | Purpose                                              |
|-----------|---------------------------------|------------------------------------------------------|
| C → S     | `C2SRequestChunk`               | "Send me chunk (cx, cz)."                            |
| C → S     | `C2SRequestChunkBulk`           | "Send me these N chunks in one shot."                |
| S → C     | `S2CChunkData`                  | Full chunk payload (blocks, biomes, light, heightmap). |
| S → C     | `S2CBlockChange`                | A single block changed (player edit or server event). |
| S → C     | `S2CMultiBlockChange`           | A batch of block changes in one chunk.               |
| S → C     | `S2CWorldInfo`                  | On connect: seed hash, spawn point, game type, rules.|

## 5. Build graph

```
                        ┌─────────────┐
                        │  libraries  │ ← shared static lib
                        └──────┬──────┘
                               │
                ┌──────────────┼──────────────┐
                ▼              ▼              ▼
          ┌─────────┐    ┌─────────┐   ┌──────────┐
          │  engine │    │  logic  │   │  clothes │
          └────┬────┘    └────┬────┘   └────┬─────┘
               │              │              │
               └──────┬───────┴──────────────┘
                      ▼
              ┌───────────────┐
              │     client    │  (Android NDK → .so → APK)
              └───────────────┘
              ┌───────────────┐
              │     server    │  (Linux native executable)
              └───────────────┘
```

`engine-main/cmakeall` orchestrates the order: `libraries` → `engine` → `logic` →
`clothes` → `client` + `server`. The `cmakeall_with_clean` variant wipes build dirs first.

## 6. What is intentionally out of scope

- No PC client build. The Win32 shell is reference-only.
- No iOS client.
- No web client.
- No peer-to-peer / listen-server mode. The server is always a dedicated process.
- No client-side world generation, ever. Even single-player must run a local server
  process (long-term goal — not required for M1).
