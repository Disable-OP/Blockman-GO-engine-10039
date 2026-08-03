# Project Structure

Full directory map of the repository as it exists today, with the purpose of each folder.
Folders marked **(legacy)** are preserved for reference and will be removed or quarantined
as the mobile-only refactor progresses — do not add new code to them.

```
Blockman-GO-engine-10039/
│
├── README.md                      Project overview, mobile-only contract, build quick-ref
├── NEXT_PROMPT.md                 The literal next prompt to hand to the dev agent
├── DEVLOG.md                      Append-only milestone log (one block per merged feature)
├── CONTRIBUTING.md                Branch / commit / push / review rules
├── .gitignore                     Root-level ignores (build dirs, IDE files, secrets)
│
├── docs/
│   ├── ARCHITECTURE.md            Topology, module responsibilities, process model
│   ├── PROJECT_STRUCTURE.md       ← this file
│   ├── BUILD.md                   Toolchain + step-by-step build instructions
│   ├── WORLDGEN.md                Server-authoritative world generation spec
│   └── ROADMAP.md                 Phased milestones M1 → M6
│
├── engine-main/                   Extracted engine source (the working tree)
│   ├── CMakeLists.txt             (per-module; no root CMakeLists yet — see ROADMAP M1)
│   ├── cmakeall                   Bash driver: configures + builds all modules in order
│   ├── cmakeall.bat               Windows wrapper (legacy)
│   ├── cmakeall_with_clean        Wipes build/ dirs first, then runs cmakeall
│   ├── cmakeall_with_clean.bat    Windows wrapper (legacy)
│   ├── clean                      Removes build artifacts
│   ├── clean.bat                  Windows wrapper (legacy)
│   ├── .gitignore                 Module-level ignores (Build/, *.a, *.so, *.pdb, ...)
│   │
│   ├── client/                    Android client
│   │   ├── CMakeLists.txt
│   │   ├── Src/                   Client C++ (game.cpp, ShellInterface, Blockman/, Network/, ...)
│   │   ├── Shells/
│   │   │   ├── Android/Blockmango/  Gradle APK shell (the active target)
│   │   │   │   ├── app/             Android app module (Java/Kotlin activity, jniLibs/)
│   │   │   │   ├── build.gradle
│   │   │   │   ├── gradle/          Gradle wrapper
│   │   │   │   ├── gradlew / gradlew.bat
│   │   │   │   ├── settings.gradle
│   │   │   │   ├── buildAndroid.bat
│   │   │   │   └── copyBin.bat      Copies native .so into jniLibs/
│   │   │   └── Win32/               (legacy, reference-only — do not extend)
│   │   └── curl/                    Prebuilt libcurl (Android/ + Windows/)
│   │
│   ├── server/                    Linux game server
│   │   ├── CMakeLists.txt
│   │   ├── WinServer.sln           (legacy, reference-only)
│   │   ├── src/
│   │   │   ├── main.cpp             Process entry
│   │   │   ├── Server.cpp/.h        Server lifecycle, config, room boot
│   │   │   ├── RoomManager.cpp/.h   One Room per running world
│   │   │   ├── CommodityManager.*   In-game economy
│   │   │   ├── ConsoleCmd.*         Admin console
│   │   │   ├── Global.h
│   │   │   ├── Network/             RakNet listener, packet dispatcher, ClientPeer, ...
│   │   │   ├── Blockman/World/      ★ AnvilManagerServer, AnvilServer — server worldgen wiring
│   │   │   ├── DB/                  MySQL / Redis access
│   │   │   ├── Script/              Server-side Lua scripting
│   │   │   ├── LuaRegister/         C++ → Lua bindings (incl. WorldGenerator_Register.cpp)
│   │   │   └── Behaviac/            Behaviour-tree integration (optional)
│   │   └── dependencies/
│   │       ├── linux/               Redis (hiredis), MySQL client, ...
│   │       └── win32/               (legacy, reference-only)
│   │
│   ├── engine/                    "Lord" core engine
│   │   ├── CMakeLists.txt
│   │   ├── ApkBuild/               Android packaging helpers
│   │   ├── NDKBuild/
│   │   │   ├── DepBuild/           Builds third-party deps for NDK
│   │   │   ├── build.sh
│   │   │   └── jni/                Android.mk / Application.mk
│   │   ├── Dependencies/           Prebuilt third-party libs:
│   │   │   ├── FreeImage/
│   │   │   ├── FreeType/
│   │   │   ├── Lua/
│   │   │   ├── luajit/
│   │   │   ├── RapidXml/
│   │   │   ├── rapidjson/
│   │   │   ├── Zlib/
│   │   │   ├── g3log/
│   │   │   ├── libxl/
│   │   │   ├── nedmalloc/
│   │   │   ├── opengles_v2/
│   │   │   ├── pfs/
│   │   │   └── dirent/
│   │   └── Src/
│   │       ├── Core/               Actor, Anim, Audio, Effect, Math, Memory, Model,
│   │       │                       Navigation, Object, Render, Resource, Scene, Stream,
│   │       │                       Tessellator, Log, Std, Functional, Geom, Image
│   │       └── GLES2Render/        GLES2 backend (framebuffer, GPU buffer, render state, ...)
│   │
│   ├── logic/                     Shared game logic (linked into client AND server)
│   │   ├── CMakeLists.txt
│   │   ├── ApkBuild/
│   │   ├── dependencies/
│   │   └── Src/
│   │       ├── AI/                 AI goals / behaviours
│   │       ├── Any/                Type-erased helpers
│   │       ├── Block/              Block, BlockArchive, BlockConfig, BlockLoader,
│   │       │                       BlockManager, Blocks, CustomBlock, BM_Material, BlockRailLogic
│   │       ├── Chat/               Chat protocol + handlers
│   │       ├── Chunk/              ★ Chunk, ChunkCache, ChunkProviderGenerate/Flat/End/Hell,
│   │       │                       ChunkService, ChunkReadableStorageFile, IChunkProvider
│   │       ├── Entity/             Entities, enchantments
│   │       ├── Enums/
│   │       ├── Inventory/          Player inventories
│   │       ├── Item/               Item definitions
│   │       ├── Misc/
│   │       ├── Nbt/                NBT serialization (chunk save format)
│   │       ├── Network/            ★ Shared net layer
│   │       │   ├── CommonPacketSender.h
│   │       │   ├── NetworkDefine.h
│   │       │   ├── PacketHandlerRegistry.h
│   │       │   ├── RaknetInstance.cpp/.h
│   │       │   └── protocol/       C2SPackets.h, S2CPackets.h, DataPacket, Serializer,
│   │       │                       NetDataStream, AutoRegisterPacket{C2S,S2C}.h
│   │       ├── Script/             Lua glue for shared logic
│   │       ├── Setting/
│   │       ├── Stats/
│   │       ├── TileEntity/         Block entities (chests, furnaces, ...)
│   │       ├── Trigger/           Map triggers
│   │       ├── TypeTraits/
│   │       ├── Util/
│   │       ├── World/              ★ World, WorldProvider, WorldInfo, WorldSettings,
│   │       │                       WorldChunkManager, WorldEffectManager, Section,
│   │       │                       NibbleArray, SchematicModel, IBlockAccess
│   │       ├── WorldGenerator/     ★★★ Anvil, AnvilManager, BiomeGen, BiomeCache,
│   │       │                       BiomeDecorator, GenLayer, NoiseGeneratorPerlin,
│   │       │                       NoiseGeneratorOctaves, MapGenerate, StructureStart,
│   │       │                       StructurePieces, StructureComponent, FlatGeneratorInfo
│   │       ├── BM_Container_def.h
│   │       ├── BM_TypeDef.h
│   │       ├── Common.cpp/.h
│   │       ├── GameVersionDefine.cpp/.h
│   │       ├── LanguageDef.h
│   │       ├── RegConfig.ini       Auto-registration config (drives codegen)
│   │       └── RTTI.h
│   │
│   ├── libraries/                 Cross-module static helpers
│   │   ├── CMakeLists.txt
│   │   ├── ApkBuild/
│   │   ├── include/
│   │   └── src/android/
│   │
│   └── clothes/                   Avatar / cosmetic system
│       ├── CMakeLists.txt
│       ├── ApkBuild/
│       ├── Shells/
│       └── Src/
│
└── engine-res-main/               Runtime resources (mounted by resource manager)
    ├── Media/                     Textures, models, audio, UI assets (PNGs, TGAs, .mesh,
    │                              .skin, .anim, .actor, .effect, .mp3, ...)
    ├── recipe/                    Crafting recipes (.csv / .json)
    └── resource.cfg               Master resource manifest
```

## Legend

- ★ — modules directly involved in the server-authoritative world generation work (see
  [`WORLDGEN.md`](WORLDGEN.md)).
- ★★★ — the heart of the worldgen pipeline.
- **(legacy)** — preserved for reference only, not in the mobile-only build graph.

## Conventions

- **No root `CMakeLists.txt` yet.** Today the build is driven by `cmakeall` invoking each
  module's `CMakeLists.txt` in dependency order. A unified root `CMakeLists.txt` that calls
  `add_subdirectory()` for every module is on the M1 list (see [`ROADMAP.md`](ROADMAP.md)).
- **ApkBuild/** folders inside each module hold Android-specific packaging scripts; they are
  consumed by the Gradle shell, not by `cmakeall` directly.
- **Auto-generated files** live next to their hand-written siblings and are marked with a
  header comment. Do not edit them — edit the `.in` template or the codegen script instead.
  Examples: `AutoRegisterPacketC2S.h`, `AutoRegisterPacketS2C.h`, `LordConfig.h` (from
  `LordConfig.h.in`).
