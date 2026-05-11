# Native-Port Hardware Seams

The PS1 matching build remains the oracle. These seams describe future host
interfaces only; they do not change current reconstruction code.

## MDEC / libpress

Source references:

- `src/TITLE/TITLE.PRG/libpress/LIBPRESS.c`
- `src/TITLE/TITLE.PRG/22C.c`
- `include/psx/libpress.h`

The native runtime needs a decode-session surface for `DecDCTReset`,
`DecDCTvlcBuild`, `DecDCTvlc2`, `DecDCTin`, `DecDCTout`, sync polling, and input
/ output callbacks. Keep CD ring-buffer streaming separate from the MDEC decode
API; title movies currently combine both concerns.

## GTE / fixed-point math

Source references:

- `include/psx/libgte.h`
- `include/psx/inline_c.h`
- `src/BATTLE/BATTLE.PRG/58578.c`
- `src/BATTLE/BATTLE.PRG/4A0A8.c`

Native math should mirror `MATRIX`, `VECTOR`, and `SVECTOR` layout and preserve
12-bit fixed-point semantics where gameplay depends on exact results. Rendering
and scratchpad ownership should stay outside the math API.

## SPU / audio

Source references:

- `src/SLUS_010.40/25AC.c`
- `include/psx/libspu.h`

The current runtime mixes libspu calls with direct register-style behavior. Split
future host audio into lifecycle, transfer, voice-register, and stream-command
surfaces so capture tests can compare timing before adding higher-level mixing.

## CD / XA / sectors

Source references:

- `src/SLUS_010.40/main.c`
- `src/TITLE/TITLE.PRG/22C.c`
- `include/psx/libcd.h`
- `include/psx/libds.h`

Native CD services should expose sector reads, XA stream control, seek/status
polling, and asset-root resolution. They should not share an API with BIOS-style
file or memory-card operations.

## BIOS file I/O / memory card

Source references:

- `src/TITLE/TITLE.PRG/22C.c`
- `src/MENU/MENU7.PRG/260.c`
- `include/psx/libapi.h`

Save data needs BIOS-like `open`, `read`, `write`, `close`, `lseek`, `firstfile`,
`nextfile`, and `erase` semantics mapped onto host files. Keep this separate from
CD sectors so save/load tests can run without disc streaming.

## Entry criteria for native code

Before adding a `make native` target, require:

- core loop, file I/O, and platform-service call sites named well enough to wrap;
- a host-only test harness that compiles without touching PS1 include paths;
- clear ownership for fixed-address memory and scratchpad replacements;
- capture tests for at least one title/menu path.
