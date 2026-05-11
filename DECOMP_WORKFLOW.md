# Decompilation Workflow

Rood Reverse already uses a build-and-diff decompilation harness. It is not the
same toolchain as GameCube/Wii Zelda projects, but the workflow is the same in
spirit: provide an original game image, split the original binaries, rebuild the
reconstructed source with matching compilers and linker scripts, compare rebuilt
objects, and verify the final binary output.

## How this maps to the Zelda-style workflow

| Zelda-style term | Rood Reverse equivalent |
| --- | --- |
| Original disc/game input | `disks/SLUS-01040.bin` |
| `dtk` split of DOL/REL | `dumpsxiso` plus `splat` configs under `config/**/splat.yaml` |
| Original compiler behavior | `tools/old-gcc`, `mipsel-linux-gnu-*`, `maspsx` |
| Per-object diff/progress | `objdiff-cli`, generated through `tools/dev/objdiff_config.py` |
| Matching progress report | `progress.json` plus `tools/dev/progress.py` |
| Nonmatching source fallback | `INCLUDE_ASM(...)` in C files |
| Final output verification | `make` / `make commit-check` ending with `✔ All files match` |

The important difference is granularity. Many Zelda projects track matching at
whole original object-file boundaries. Rood Reverse can keep byte-identical PS1
outputs while individual functions in a C file still use `INCLUDE_ASM` fallbacks.
That makes function-by-function progress practical, but it also means the project
needs a clear inventory of remaining stubs.

## Standard contributor loop

Run commands from the repository root.

1. **Build/split from the original disc**

   ```sh
   make -j
   ```

   This uses `dumpsxiso` to extract data from `disks/SLUS-01040.bin`, then
   `splat` to generate split source/asm/data files under `build/`.

2. **Pick a frontier**

   ```sh
   python3 tools/dev/frontier.py --overlays-only --no-menu --limit 20 --functions
   ```

   Use the smallest reasonable file first unless there is a strategic reason to
   prioritize a native-port seam such as rendering, audio, input, file IO, saves,
   or battle/gameplay.

3. **Inspect the target assembly**

   Target assembly lives under `build/src/**/nonmatchings/`. Example:

   ```sh
   build/src/TITLE/TITLE.PRG/nonmatchings/libpress/LIBPRESS/func_8007225C.s
   ```

4. **Create a guarded draft**

   Use this pattern when the C is useful but not matching yet:

   ```c
   #if defined(PERMUTER) || defined(OBJDIFF)
   void target_function(void)
   {
       /* draft C */
   }
   #else
   INCLUDE_ASM("build/src/.../nonmatchings/...", target_function);
   #endif
   ```

   The normal build keeps using the asm fallback. The guarded build lets objdiff,
   decomp.me, and the permuter compile the candidate C.

5. **Compile the guarded object**

   ```sh
   make -W src/PATH/TO/file.c OBJDIFF=/usr/bin/true CPP="mipsel-linux-gnu-cpp -DOBJDIFF" build/src/PATH/TO/file.o
   ```

6. **Use decomp.me or the permuter when helpful**

   ```sh
   make decompme TARGET=build/src/PATH/nonmatchings/file/function.s
   make permute TARGET=build/src/PATH/nonmatchings/file/function.s
   ```

7. **Remove the fallback only when matching**

   Once the C produces matching assembly/object output, remove the guarded
   `INCLUDE_ASM` fallback and leave the C in the normal build.

8. **Run normal verification**

   If you compiled a guarded object, force the normal fallback object before
   checking the full build:

   ```sh
   make -W src/PATH/TO/file.c CPP=mipsel-linux-gnu-cpp build/src/PATH/TO/file.o
   make CPP=mipsel-linux-gnu-cpp -j
   make CPP=mipsel-linux-gnu-cpp commit-check
   ```

   `commit-check` runs formatting/linting, a clean rebuild, target verification,
   and objdiff progress generation. The objdiff step can build `objdiff-cli`
   through Cargo; make sure `protoc` is available through the
   `protobuf-compiler` package before running it in a fresh environment.

## Frontier inventory

`tools/dev/frontier.py` answers “what functions/files still need work?” by
scanning `src/**/*.c` for `INCLUDE_ASM` and `INCLUDE_RODATA`.

Useful commands:

```sh
python3 tools/dev/frontier.py --limit 30
python3 tools/dev/frontier.py --overlays-only --no-menu --limit 20 --functions
python3 tools/dev/frontier.py --json > frontier.json
```

The frontier and objdiff config helpers have lightweight fixture tests that do
not require the disc image or a split build:

```sh
python3 -m unittest discover -s tools/dev/tests
```

The table includes:

- `ASM`: number of `INCLUDE_ASM` stubs in the file.
- `RO`: number of `INCLUDE_RODATA` stubs in the file.
- native-port tags such as `render`, `audio`, `file-io`, `input`, `saves`,
  `gameplay`, and `mdec/video`.
- optional function rows with target assembly path, detected size, and whether
  the target file is marked handwritten.

## Marking complete units

Whole-unit completion is now tracked in `config/complete_units.json`. The
objdiff report generator reads that file through `tools/dev/objdiff_config.py`.

Only add a unit to `config/complete_units.json` when it is actually complete at
the object/unit level, not merely when the final linked game still matches via
asm fallbacks.

## Verification expectations

For decomp work, a good checkpoint has all of these:

- guarded candidate compiles, if guarded code was added;
- normal build uses fallbacks or matching C and reports `✔ All files match`;
- `make CPP=mipsel-linux-gnu-cpp commit-check` passes;
- generated files such as `progress.json` are restored unless the intended
  change is explicitly to update them;
- docs record why the target was selected when the frontier choice is not
  obvious.
