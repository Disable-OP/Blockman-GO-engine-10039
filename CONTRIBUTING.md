# Contributing

This repository is the **primary backup** for the Blockman-GO-engine-10039 project. Every
meaningful change must reach `main` on GitHub before the session that produced it is
considered done.

## 1. Branching

- `main` is always buildable. Never push a broken commit to `main`.
- Work on `feat/<short-name>` or `fix/<short-name>` branches off the latest `main`.
- For larger milestones, use `milestone/<m-name>` branches and merge sub-features into them
  before the milestone merges back to `main`.
- Keep branch names lowercase, kebab-case, ≤40 chars. Examples:
  - `feat/server-chunk-request-handler`
  - `fix/client-chunk-cache-lru-eviction`
  - `milestone/m2-server-worldgen`

## 2. Commits

- **Commit after every successful build, every major feature, every milestone.** Do not let
  meaningful work sit only in your local workspace.
- One logical change per commit. If you find yourself writing "and also …" in the commit
  message, split the commit.
- Imperative mood in the subject line, ≤72 chars, no trailing period:
  - ✅ `Add server-side ChunkProviderGenerate wiring in AnvilManagerServer`
  - ✅ `Strip WorldGenerator references from client/Src`
  - ❌ `Added some changes to the server.` (vague, past tense, period)
- Body (optional, wrapped at 72 chars) explains *why*, not *what*. The diff already shows
  what.
- Reference issues / milestones in the body when relevant:
  ```
  Refs: M2 (server-authoritative worldgen)
  Closes #42
  ```
- **Never commit secrets.** No PATs, no passwords, no private keys, no `local.properties`
  with real paths. The root `.gitignore` blocks the common offenders; double-check before
  pushing.

## 3. Push discipline

- Push your feature branch at least once per working session, even if it's not ready for
  review. A pushed WIP is recoverable; a local-only WIP is not.
- WIP commits on a feature branch may use the `wip:` prefix:
  `wip: snapshot before session recovery`.
- **Never force-push to `main`.** Force-push is allowed only on your own feature branch and
  only if no one else has fetched it.
- If the session becomes unstable (tool timeouts, IDE crashes, network drops):
  ```bash
  git add -A
  git commit -m "wip: snapshot before session recovery"
  git push origin <your-branch>
  ```
  Do this **before** trying to fix anything else.

## 4. Pull requests

- Even on a solo project, open a PR for every merge into `main`. The PR description is your
  audit trail.
- PR description template:
  ```
  ## What
  <one-paragraph summary>

  ## Why
  <link to ROADMAP milestone or issue>

  ## How
  <bullet list of key changes>

  ## Verification
  <how you confirmed it works — build cmd, test run, manual steps>
  ```
- Squash-merge into `main` so the history stays linear.

## 5. Code conventions

- C++ style: match the surrounding file. The existing engine uses tabs for indentation,
  `PascalCase` for class names, `camelCase` for methods and variables, `UPPER_SNAKE` for
  macros. Don't reformat untouched code in the same PR as a behavioural change.
- Headers go next to their `.cpp` (e.g. `Foo.cpp` + `Foo.h` in the same folder). Don't
  create new `include/` trees unless the module's existing convention already has one.
- New shared game code goes in `engine-main/logic/Src/<Subsystem>/`. Client-only code goes
  in `engine-main/client/Src/`. Server-only code goes in `engine-main/server/src/`. If you
  are unsure where a class belongs, ask in the PR description before writing it.
- **No client-side world generation.** See [`docs/WORLDGEN.md`](docs/WORLDGEN.md). Any new
  reference to `WorldGenerator`, `ChunkProviderGenerate`, `BiomeGen`, `GenLayer`,
  `MapGenerate`, `StructureStart`, `NoiseGeneratorOctaves`, etc. from
  `engine-main/client/Src/` will be rejected in review.
- **No new PC / Win32 code.** See the mobile-only contract in [`README.md`](README.md).
  New `#ifdef LORD_PLATFORM_WIN32` blocks are forbidden; existing ones are being removed.

## 6. Build verification

Before opening a PR that touches C++:

- For server changes: build the server with
  `cmake -B build -S engine-main -DLORD_SERVER=ON && cmake --build build` and confirm it
  links.
- For client changes: build the native libs with `./cmakeall` (Android toolchain) and
  assemble the debug APK with `./gradlew assembleDebug`.
- For shared `logic/` changes: build **both** sides. A change that compiles on the server
  but breaks the client build (or vice versa) blocks the PR.

## 7. Devlog

Append a short entry to [`DEVLOG.md`](DEVLOG.md) for every merged PR. Format:

```
## YYYY-MM-DD — <PR title>
Branch: <branch-name>
What: <one-line summary>
Why: <milestone / issue reference>
```

The devlog is the human-readable project history; the git log is the machine-readable one.
Both must exist.

## 8. When in doubt

- Read [`README.md`](README.md) and [`docs/`](docs/) first.
- If a rule here conflicts with a rule in [`README.md`](README.md) or
  [`docs/WORLDGEN.md`](docs/WORLDGEN.md), the more specific doc wins.
- If still unsure, open a draft PR with your question in the description. Better to ask in
  a PR than to push a broken convention to `main`.
