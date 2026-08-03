# Blockman-GO-engine-10039

A **mobile-only** Blockman-style voxel game engine — a single codebase that ships both an
**Android client** and a **dedicated game server**, where the **server is the authoritative
source of world generation**. No PC client, no console targets, no third-party matchmaking
servers — just the phone in your pocket and the server it talks to.

> **Status:** Bootstrapping. The extracted engine source lives under `engine-main/` and
> `engine-res-main/`. The first milestone is wiring the existing `WorldGenerator` into the
> server's authoritative chunk pipeline and stripping the PC/Win32 build paths so the
> project is mobile-only end to end. See [`docs/ROADMAP.md`](docs/ROADMAP.md).

---

## What this project is

| Layer            | What it does                                                                 |
|------------------|------------------------------------------------------------------------------|
| **Client**       | Android (NDK + GLES2) app. Renders the world, sends player input, plays audio. |
| **Game Server**  | Linux process. Owns world state, runs `WorldGenerator`, ticks entities, persists chunks to Anvil, validates every player action. |
| **Shared Logic** | One C++ codebase (`engine-main/logic`) linked into both client and server: blocks, chunks, entities, NBT, world-gen algorithms, packet protocol. |
| **Engine Core**  | `engine-main/engine` ("Lord"): renderer, scene graph, audio, resource manager, math, animation. |
| **Resources**    | `engine-res-main/`: `Media/`, `recipe/`, `resource.cfg` — textures, models, audio, recipes. |

The defining rule of this engine: **the server generates the world, the client only renders it.**
A phone never decides what blocks exist — it asks the server, the server runs the noise +
biome + structure pipeline, and ships back chunks. This is what makes cheating hard and what
makes the world consistent across every player on a server.

---

## Repository layout

```
Blockman-GO-engine-10039/
├── README.md                  ← you are here
├── NEXT_PROMPT.md             ← the next prompt to hand to the dev agent
├── DEVLOG.md                  ← running log of milestones (append-only)
├── CONTRIBUTING.md            ← branch / commit / review rules
├── .gitignore
├── docs/
│   ├── ARCHITECTURE.md        ← system diagram + module responsibilities
│   ├── PROJECT_STRUCTURE.md   ← full directory map with per-folder purpose
│   ├── BUILD.md               ← how to build client + server on Linux/Android
│   ├── WORLDGEN.md            ← server-authoritative world generation spec
│   └── ROADMAP.md             ← phased milestones, M1 → M6
├── engine-main/               ← extracted engine source (DO NOT reorganise yet)
│   ├── client/                ← Android + (legacy) Win32 shells + client C++
│   ├── server/                ← Linux game server C++
│   ├── engine/                ← "Lord" core engine (GLES2 renderer, audio, scene)
│   ├── logic/                 ← shared game logic (blocks, chunks, world-gen, NBT, net)
│   ├── libraries/             ← shared static libs
│   ├── clothes/               ← avatar / cosmetic system
│   ├── cmakeall               ← top-level build driver
│   └── clean
└── engine-res-main/           ← runtime resources (Media, recipes, resource.cfg)
```

Full per-folder breakdown: [`docs/PROJECT_STRUCTURE.md`](docs/PROJECT_STRUCTURE.md).

---

## Mobile-only constraint

This repository intentionally drops every non-mobile target. Concretely:

- **Client target:** Android (`arm64-v8a`, `armeabi-v7a`, `x86_64` for emulators). No iOS, no
  Windows desktop, no macOS app.
- **Server target:** Linux x86_64. The server runs on a host (cloud, dedicated box, or a
  rooted Android device for local testing) — never on the player's phone as a peer.
- **No PC client shell.** The legacy `client/Shells/Win32/` directory is preserved only as a
  reference snapshot; it is **not** in the build graph for new milestones and will be deleted
  once the Android shell is the only one referenced by `cmakeall`.

If you find yourself adding `#ifdef LORD_PLATFORM_WIN32` to a new feature, stop — that path is
deprecated for this fork.

---

## Server-authoritative world generation

The shared `engine-main/logic/Src/WorldGenerator/` already contains a complete pipeline
(Perlin/Octaves noise, biome layers, biome decorator, Anvil chunk storage, structure
pieces, nether/end providers). The server already has `AnvilManagerServer` and
`ChunkProviderGenerate` hooks. What is missing is making that pipeline **the only** source
of chunks — the client must never run `WorldGenerator` itself, only request chunks from the
server and cache what it receives.

Full design, packet contract, and seed-handling rules: [`docs/WORLDGEN.md`](docs/WORLDGEN.md).

---

## Build (quick reference)

Detailed instructions: [`docs/BUILD.md`](docs/BUILD.md).

```bash
# Server (Linux x86_64)
cd engine-main
./cmakeall            # configures + builds all modules, server included

# Client (Android, requires Android NDK + SDK)
cd engine-main/client/Shells/Android/Blockmango
./gradlew assembleDebug
```

Toolchain requirements:
- **CMake** ≥ 3.10 (the legacy `cmake_minimum_required(VERSION 2.8)` lines in some modules
  will be bumped as part of M1).
- **Android NDK** r21+ for the client; **Android SDK** with `platform-30`+ for the Gradle shell.
- **Linux toolchain** GCC 9+ / Clang 10+ for the server.
- `engine-main/engine/Dependencies/` ships prebuilt deps for FreeImage, FreeType, Lua/LuaJIT,
  RapidXML, RapidJSON, zlib, g3log, libxl, nedmalloc, opengles_v2, pfs.

---

## Branching & backup discipline

This repo is the **primary backup** for the project. Rules:

1. `main` is always buildable. Never push a broken commit to `main`.
2. Work on `feat/<short-name>` or `fix/<short-name>` branches. Open a PR (even solo) to merge
   into `main` so the diff is recorded.
3. **Commit after every successful build, every major feature, every milestone.** Do not
   leave meaningful work only in the local workspace — push it.
4. Descriptive commit messages, imperative mood:
   `Add server-side ChunkProviderGenerate wiring in AnvilManagerServer`.
5. **Never force-push to `main`.** Force-push is allowed only on your own feature branch and
   only if no one else has fetched it.
6. If the session becomes unstable (tool timeouts, IDE crashes), `git add -A && git commit
   -m "wip: snapshot before session recovery" && git push` **before** doing anything else.

---

## License

TBD. The engine source was extracted from an existing codebase; before publishing or
distributing any binary, the licensing status of `engine-main/` and `engine-res-main/` must
be resolved and recorded in a `LICENSE` file at the repo root. Until then, treat this
repository as **private and non-distributable**.

---

## Where to start

Read these in order:

1. [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) — understand the module boundaries.
2. [`docs/PROJECT_STRUCTURE.md`](docs/PROJECT_STRUCTURE.md) — know where everything lives.
3. [`docs/WORLDGEN.md`](docs/WORLDGEN.md) — the central design contract for this fork.
4. [`docs/ROADMAP.md`](docs/ROADMAP.md) — what milestone we are on and what is next.
5. [`NEXT_PROMPT.md`](NEXT_PROMPT.md) — the literal next prompt to hand to the dev agent.
