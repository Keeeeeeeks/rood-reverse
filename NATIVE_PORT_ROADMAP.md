# Native Port Roadmap

## Purpose

This document describes the work needed to move Rood Reverse from a PlayStation
1 decompilation/reconstruction project to a native, emulator-free port of
Vagrant Story.

It is intentionally separate from `DECOMP_MILESTONES.md`:

- `DECOMP_MILESTONES.md` tracks short-horizon, falsifiable decompilation work:
  which original PS1 functions still need matching or guarded drafts, and which
  Docker commands prove the PS1 rebuild still matches.
- This roadmap tracks the longer native-port path: what has to be understood,
  isolated, replaced, and tested before the game can run as a normal host
  application without a PS1 emulator.

The current repository still builds PS1 binaries with `mipsel-linux-gnu-*`,
links them with PS1 linker scripts, and verifies byte-for-byte matching output
against the original disc-derived targets. That matching build remains the
oracle until a native runtime has its own behavioral test suite.

## North-star acceptance target

A native port is achieved when a user can provide legally obtained Vagrant Story
disc data, run a host-platform executable, and play the game without invoking a
PS1 emulator or executing the original MIPS binary.

The port should preserve original gameplay semantics first. Widescreen,
high-resolution rendering, bug fixes, modding, and quality-of-life changes are
secondary goals and should remain behind compatibility-preserving switches until
the baseline native runtime is trustworthy.

## Current state

- Overall matched code: `586696 / 1064508` bytes (`55.11%`) from
  `progress.json`.
- The repo can rebuild and verify the PS1 outputs with `make` / `commit-check`
  in the Docker toolchain.
- The codebase still contains hundreds of `INCLUDE_ASM` sites. Those are valid
  for PS1 reconstruction, but they cannot be compiled into a native host binary.
- Core platform libraries are partially decompiled under `src/SLUS_010.40`,
  including CD, GPU, SPU, controller, memory-card, interrupt, and GTE-adjacent
  code. These areas are exactly the seams a native port must replace or emulate
  at the API level.
- Current decomp work is tracking `SCREFF2.PRG::func_800F986C` as an active
  generated-assembly target while leaving the asm fallback in normal builds.

## Strategy

Do not try to turn every matched C file directly into a native binary in one
step. The PS1 executable model leaks through the entire program: fixed memory
addresses, overlays, scratchpad pointers, ordering tables, GPU packets, SPU
voices, BIOS calls, CD sectors, memory cards, and frame-timed logic.

The safer strategy is a dual-runtime architecture:

1. Keep the PS1 matching build as the regression oracle.
2. Gradually introduce host-buildable source variants and adapters behind clear
   platform boundaries.
3. Preserve original data formats and behavior while replacing PS1 services with
   native services.
4. Add native integration tests and capture-based gameplay tests before making
   compatibility-changing improvements.

## Phase 0: Decompilation oracle stays green

Goal: preserve confidence while the codebase is still mostly PS1-shaped.

Required work:

- Continue removing `INCLUDE_ASM` from high-leverage systems: main loop,
  overlay loading, rendering submission, input, audio, CD/file IO, memory card,
  battle, events, and menus.
- Keep every normal PS1 rebuild passing `✔ All files match`.
- Prefer guarded drafts under `OBJDIFF` / `PERMUTER` for nonmatching work so
  normal builds remain byte-matching.
- Document every frontier selection with reproducible counts and commands.

Exit criteria:

- The remaining assembly is either fully removed or isolated to subsystems that
  will be replaced wholesale in the native runtime.
- Core game-state structures, scheduler/state-machine flow, asset loading, and
  render/audio/input boundaries are named well enough to support adapter design.

## Phase 1: Host build skeleton

Goal: compile a native executable that can initialize, load extracted assets, and
enter a controlled test loop without running original MIPS code.

Required work:

- Add a host build target separate from the PS1 matching target.
- Define platform-selection macros that never affect matching PS1 builds.
- Create a minimal native entry point that owns process startup, window/audio
  initialization, and shutdown.
- Add an asset-root resolver that reads files extracted from the user-owned disc
  instead of relying on PS1 LBAs.
- Add a compatibility layer header for PS1 services used by decompiled code.

Candidate host backends:

- Window/input/audio: SDL is the lowest-friction first backend.
- Rendering: start with a software or packet-capture renderer, then move to
  OpenGL/Vulkan/Metal once GPU packet behavior is understood.
- Audio: start with a simple mixer that can play decoded/streamed assets before
  attempting SPU-accurate voice behavior.

Exit criteria:

- `make native` or an equivalent command builds a host executable.
- The executable can start, open a window, resolve the extracted asset root, run
  a deterministic tick loop, and exit cleanly.

### Current Phase 1 preparation

The `native-port/` directory is a documentation-only work area for this phase.
It contains seam descriptions and header-only contracts for future host runtime
services, but it is deliberately not referenced by `src/`, `config/`, or the
root `Makefile`. This keeps native-port design work from perturbing the PS1
matching build while the decompilation oracle remains the source of truth.

## Phase 2: Platform service replacement

Goal: replace PS1 BIOS/library assumptions with native services while preserving
game-visible semantics.

Subsystems to replace:

- CD/file IO: map LBA/sector reads and file IDs to host files or archives.
- Overlay loading: replace executable overlay loading with native module/state
  dispatch.
- Input: translate keyboard/controller events into the game’s pad state.
- Memory card: map card blocks and save slots to host save files.
- Timing/interrupts: replace VSync/interrupt callbacks with a deterministic
  frame scheduler.
- Scratchpad/fixed-address memory: replace literal PS1 addresses with owned
  native storage where possible, keeping matching-build code untouched.
- GTE/math: either preserve fixed-point math exactly or introduce tested native
  equivalents with bit-level compatibility where gameplay depends on it.

Exit criteria:

- Native runtime reaches title/menu state using native IO/input/timing services.
- Save/load can round-trip through host files.
- No original MIPS code is executed.

## Phase 3: Rendering

Goal: turn PS1 GPU commands into native frames.

Recommended order:

1. Capture and log ordering tables and GPU packets from decompiled code.
2. Implement packet decoding for primitives the title/menu paths need first.
3. Render at original resolution with original coordinate, CLUT, texture-page,
   semi-transparency, and ordering semantics.
4. Compare native frame captures against emulator/PS1-oracle captures.
5. Only after baseline accuracy, add optional higher-resolution or widescreen
   paths behind compatibility switches.

Exit criteria:

- Title screen, menus, and at least one gameplay scene render correctly at
  original resolution from native code.
- Rendering regressions can be detected with capture comparisons.

## Phase 4: Audio

Goal: replace SPU behavior with native playback that preserves timing and mix
semantics enough for gameplay and presentation.

Recommended order:

1. Identify music, voice/effect, stream, and sequence paths.
2. Build a native audio command log from decompiled SPU calls.
3. Implement sample/stream loading and basic mixing.
4. Add voice allocation, envelopes, pitch, looping, and reverb-like behavior as
   needed by captured command traces.
5. Compare native event timing and audible output against known scenes.

Exit criteria:

- Title/menu/gameplay audio plays through the host backend.
- Save/load/menu transitions do not desynchronize audio state.

## Phase 5: Gameplay completeness

Goal: make native execution playable end-to-end.

Required work:

- Port battle, event, map, entity, camera, collision, effects, UI, and file-load
  flows as host-buildable code.
- Replace or adapt remaining handwritten/GTE-heavy functions.
- Add smoke tests for boot, title, new game/load game, menu navigation, battle
  entry, battle resolution, room transitions, save/load, and ending/title flows.
- Use the PS1 matching build and emulator captures only as verification inputs,
  not as runtime dependencies.

Exit criteria:

- A user can play from boot into normal gameplay, save, reload, and continue in
  the native executable.

## Phase 6: Port-quality improvements

Goal: improve the native release without compromising the compatibility base.

Possible work:

- Higher internal resolution.
- Widescreen/hud-safe layouts.
- Configurable input mapping.
- Faster loading from host storage.
- Bug-fix toggles.
- Modding hooks for extracted assets.
- Platform packaging for macOS, Windows, and Linux.

Exit criteria:

- Enhancements are optional and can be disabled to return to baseline behavior.
- Packaging does not distribute copyrighted original assets.

## Decompilation priorities that unblock native work

These areas should be prioritized over isolated leaf functions once the project
switches from pure matching progress to native-port enablement:

1. Main loop and state scheduler.
2. Overlay loading and executable transition paths.
3. CD/file/LBA loading paths.
4. GPU packet construction and ordering-table submission.
5. SPU/audio command paths.
6. Controller and memory-card flows.
7. GTE/fixed-point math helpers used by camera, battle, and rendering.
8. Menu/title flows, because they are the shortest native boot-to-visible-output
   path.
9. Battle/gameplay flows, because they are the largest correctness surface.

## Immediate next milestones

These are the next two decomp milestones to run alongside this roadmap:

1. Finish the active `SCREFF2.PRG::func_800F986C` checkpoint: keep improving the
   guarded C draft, remove the fallback only if it matches, otherwise document
   the exact remaining mismatch and keep normal PS1 builds green.
2. Select the next smallest non-menu frontier by reproducible scan, then start
   its first generated-assembly target with the same guarded-draft discipline.
   If the next candidate is handwritten/GTE-heavy, document that blocker and
   either isolate it as a native-port-risk item or move to the next generated
   target.

## Non-goals for now

- Do not ship copyrighted game assets.
- Do not abandon the PS1 matching build; it is the correctness oracle.
- Do not make widescreen/high-resolution changes before original-resolution
  rendering is correct.
- Do not mix native-port compatibility code into PS1 matching paths without
  preprocessor guards and matching-build verification.
- Do not treat decompiled-but-still-PS1-shaped code as automatically portable.

## Verification discipline

Every native-port-enabling change should answer three questions:

1. Does the PS1 matching build still pass?
2. Does the native build or native harness pass its focused test?
3. Is the behavior compared against a captured or otherwise reproducible oracle?

Until all three answers are yes for a subsystem, that subsystem is not ported;
it is only partially understood.
