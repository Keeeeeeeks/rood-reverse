# Ghidra companion workflow

Ghidra is an optional analysis companion for `rood-reverse`. It is not part of
the matching build path: `objdiff`, `decomp.me`, the permuter, and the MIPS
toolchain remain the source of truth for byte matching.

## Import repository symbols

1. Build or split the project so `data/` and `build/` artifacts exist.
2. Open the target binary in Ghidra at the same VRAM base used by the relevant
   `config/*/splat.yaml` segment.
3. Add this directory to Ghidra's Script Manager search path.
4. Run `ImportRoodSymbols.py` with one or more `symbol_addrs.txt` files.

For `SCREFF2.PRG`, the relevant config declares `vram: 0x800F9800` and pulls
symbols from both the overlay and main executable symbol files:

```text
config/GIM/SCREFF2.PRG/symbol_addrs.txt
config/SLUS_010.40/symbol_addrs.txt
```

The script accepts paths as script arguments. Add `--functions` if you want it
to create functions for non-data/non-jump-table symbols that do not already have
a function at the imported address.

Accepted symbol lines use this format:

```text
symbol_name = 0x80012345; // optional comment
```

Use `--functions` only after checking the listing. It can create incorrect or
overlapping functions if Ghidra has not disassembled the region cleanly.

Example arguments from the repository root:

```text
--functions config/GIM/SCREFF2.PRG/symbol_addrs.txt config/SLUS_010.40/symbol_addrs.txt
```

## How this helps matching

Use Ghidra for cross-references, type exploration, and control-flow reading.
Do not treat Ghidra decompiler output as matching-ready C. After using Ghidra to
understand a function, continue to verify the actual decompilation with:

```sh
make -W src/GIM/SCREFF2.PRG/0.c OBJDIFF=/usr/bin/true CPP="mipsel-linux-gnu-cpp -DOBJDIFF" build/src/GIM/SCREFF2.PRG/0.o
make -W src/GIM/SCREFF2.PRG/0.c CPP=mipsel-linux-gnu-cpp build/src/GIM/SCREFF2.PRG/0.o
make CPP=mipsel-linux-gnu-cpp -j
```
