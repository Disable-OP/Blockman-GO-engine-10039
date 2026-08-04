# NEXT_PROMPT.md

This file contains the literal next prompt to hand to the dev agent to begin **M1 — Build
system unification + mobile-only enforcement**. Paste it verbatim (or tweak the
config-specific bits) as the next user message.

---

## Next prompt

> I am continuing the Blockman-GO-engine-10039 project (mobile-only Blockman-style engine:
> Android client + Linux game server, server-authoritative world generation). The repo is
> already cloned at `/home/z/my-project/workspace/Blockman-GO-engine-10039` and the M0
> bootstrap (README, docs/, CONTRIBUTING, DEVLOG, .gitignore) is committed to `main`.
>
> Read these in order before writing any code:
> 1. `README.md`
> 2. `docs/ARCHITECTURE.md`
> 3. `docs/PROJECT_STRUCTURE.md`
> 4. `docs/WORLDGEN.md`
> 5. `docs/ROADMAP.md` — we are starting **M1**.
>
> Repository rules (from `CONTRIBUTING.md`):
> - Pull before starting: `git -C /home/z/my-project/workspace/Blockman-GO-engine-10039 pull --ff-only`.
> - Work on a branch: `feat/m1-build-system-unification`.
> - Commit + push after every successful build and after each M1 sub-task. Do not leave
>   work only in the local workspace.
> - Never print, expose, or commit the GitHub PAT. Use the remote URL already configured.
> - If anything times out or fails repeatedly, commit + push a `wip:` snapshot before
>   continuing.
>
> **M1 goal:** One CMake entrypoint builds both binaries (server on Linux, client on
> Android). PC / Win32 paths are quarantined and removed from the build graph. A
> `LORD_SERVER` CMake option selects server-only vs full-client builds.
>
> **M1 sub-tasks (do them in order, commit after each):**
>
> 1. Create `engine-main/CMakeLists.txt` (the root CMake) that:
>    - sets `cmake_minimum_required(VERSION 3.10)`,
>    - defines option `LORD_SERVER` (default `OFF`) and `LORD_BUILD_PLATFORM_ANDROID`
>      (default `OFF`, auto-on when an Android toolchain file is in use),
>    - `add_subdirectory(libraries)`, `add_subdirectory(engine)`,
>      `add_subdirectory(logic)`,
>    - `add_subdirectory(clothes)` and `add_subdirectory(client)` only when
>      `NOT LORD_SERVER`,
>    - `add_subdirectory(server)` only when `LORD_SERVER` (or always, gated by the option
>      inside the subdirectory).
>
> 2. Bump every `cmake_minimum_required(VERSION 2.8)` in
>    `engine-main/{engine,logic,client,server,clothes,libraries}/CMakeLists.txt` to
>    `VERSION 3.10`. Do not change any other CMake logic in this step.
>
> 3. Verify `engine-main/cmakeall` still works as a thin wrapper around the new root CMake
>    (so `./cmakeall` invokes `cmake -B Build -S .` with the right options per module).
>    Update `cmakeall` if needed; preserve its existing CLI behaviour.
>
> 4. Quarantine legacy PC paths:
>    - Move `engine-main/client/Shells/Win32/` → `engine-main/legacy/client-shells-win32/`.
>    - Move `engine-main/server/WinServer.sln` → `engine-main/legacy/server-win-sln/`.
>    - Move `engine-main/server/dependencies/win32/` →
>      `engine-main/legacy/server-deps-win32/`.
>    - Move `engine-main/client/curl/Windows/` → `engine-main/legacy/client-curl-windows/`.
>    - Add a one-paragraph `engine-main/legacy/README.md` explaining these are
>      reference-only and not in the build graph.
>    - Make sure nothing under `engine-main/legacy/` is referenced by any active
>      `CMakeLists.txt` or `cmakeall` step.
>
> 5. Add `.github/workflows/build.yml` that on every push to `main` and on PRs:
>    - Builds the server on `ubuntu-22.04` with
>      `cmake -B build -S engine-main -DLORD_SERVER=ON && cmake --build build`.
>    - Builds the Android client native libs on `ubuntu-22.04` using NDK r25c with
>      `cmake -B build -S engine-main -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake
>      -DANDROID_ABI=arm64-v8a -DLORD_BUILD_PLATFORM_ANDROID=ON && cmake --build build`.
>    - Uploads the `Server` binary and `libblockmango.so` as workflow artifacts.
>    - Does not fail the workflow on the APK assemble step yet (Gradle wiring is M3).
>
> 6. Update `DEVLOG.md` with one entry per sub-task above. Update `docs/ROADMAP.md` to mark
>    each M1 box ✅ as it lands.
>
> **Acceptance for M1:**
> - `cmake -B build -S engine-main -DLORD_SERVER=ON && cmake --build build` produces a
>   working `Server` binary on Linux.
> - `cmake -B build -S engine-main -DLORD_BUILD_PLATFORM_ANDROID=ON
>   -DCMAKE_TOOLCHAIN_FILE=...` produces `libblockmango.so` for `arm64-v8a`.
> - `grep -rn "Win32\|win32\|LORD_PLATFORM_WIN32" engine-main/{client,server,engine,logic,clothes,libraries}/CMakeLists.txt`
>   returns no matches (the legacy folder is exempt).
> - GitHub Actions CI is green on `main`.
>
> Do not start M2 work in this prompt. When M1 is done, commit, push, and report back with
> the list of commits you pushed and any acceptance criteria that did not pass (with the
> reason). Then I will give you the M2 prompt.

---

## How to use this file

- The block above is the **literal** next prompt. Copy it as-is into the next session.
- If you need to customise (e.g. different NDK version, different CI runner), edit the
  relevant line in the copy you paste — do **not** edit this file to match your local
  environment, because the canonical version should stay repo-portable.
- After M1 lands, replace the prompt block above with the **M2 prompt** (server-authoritative
  worldgen wiring). Keep the old M1 prompt below a `---` separator under a `## Archive — M1`
  heading so the history of "what was the next prompt" is preserved.
