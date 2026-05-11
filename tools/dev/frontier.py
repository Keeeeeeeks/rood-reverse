#!/usr/bin/env python3
import argparse
import json
import re
from dataclasses import dataclass
from pathlib import Path


INCLUDE_ASM_RE = re.compile(
    r'INCLUDE_ASM\("(?P<directory>[^"]+)",\s*(?P<name>[A-Za-z_$][A-Za-z0-9_$]*)\s*\)'
)
INCLUDE_RODATA_RE = re.compile(
    r'INCLUDE_RODATA\("(?P<directory>[^"]+)",\s*(?P<name>[A-Za-z_$][A-Za-z0-9_$]*)\s*\)'
)
SIZE_RE = re.compile(r"nonmatching\s+[^,]+,\s*(0x[0-9A-Fa-f]+)")

OVERLAY_ROOTS = (
    "src/TITLE/",
    "src/BATTLE/",
    "src/GIM/",
    "src/ENDING/",
    "src/MENU/",
)


@dataclass
class Stub:
    name: str
    kind: str
    asm_path: str
    size: str
    handwritten: bool


@dataclass
class FileFrontier:
    path: str
    asm_count: int
    rodata_count: int
    stubs: list[Stub]
    tags: list[str]


def parse_args():
    parser = argparse.ArgumentParser(
        description="List decompilation frontier files and INCLUDE_ASM functions."
    )
    parser.add_argument("--root", default=".", help="Repository root (default: current directory)")
    parser.add_argument("--limit", type=int, default=30, help="Maximum file rows to print")
    parser.add_argument("--functions", action="store_true", help="Print functions under each file")
    parser.add_argument("--json", action="store_true", help="Emit machine-readable JSON")
    parser.add_argument("--no-menu", action="store_true", help="Exclude src/MENU frontiers")
    parser.add_argument(
        "--overlays-only",
        action="store_true",
        help="Only include TITLE/BATTLE/GIM/ENDING/MENU overlay paths",
    )
    parser.add_argument(
        "--include-rodata",
        action="store_true",
        help="Include files that only have INCLUDE_RODATA and no INCLUDE_ASM",
    )
    return parser.parse_args()


def native_tags(path: str) -> list[str]:
    tags = []
    lowered = path.lower()
    if "libpress" in lowered:
        tags.append("mdec/video")
    if "libgpu" in lowered or "screff" in lowered:
        tags.append("render")
    if "libspu" in lowered or "libsnd" in lowered:
        tags.append("audio")
    if "libcd" in lowered or "znd" in lowered:
        tags.append("file-io")
    if "libpad" in lowered:
        tags.append("input")
    if "libcard" in lowered or "mcm" in lowered:
        tags.append("saves")
    if "/battle/" in lowered:
        tags.append("gameplay")
    if "/menu/" in lowered or "mainmenu" in lowered:
        tags.append("ui")
    if not tags:
        tags.append("unknown")
    return tags


def read_asm_metadata(repo_root: Path, directory: str, name: str) -> tuple[str, bool]:
    repo_root = repo_root.resolve()
    asm_path = (repo_root / directory / f"{name}.s").resolve()
    try:
        asm_path.relative_to(repo_root)
    except ValueError:
        return "", False

    if not asm_path.exists():
        return "", False

    text = asm_path.read_text(errors="ignore")
    size_match = SIZE_RE.search(text)
    handwritten = "Handwritten function" in text or "handwritten instruction" in text
    return size_match.group(1) if size_match else "", handwritten


def should_skip(path: str, args) -> bool:
    if args.no_menu and path.startswith("src/MENU/"):
        return True
    if args.overlays_only and not path.startswith(OVERLAY_ROOTS):
        return True
    return False


def collect_frontiers(repo_root: Path, args) -> list[FileFrontier]:
    frontiers = []
    for source_path in sorted((repo_root / "src").rglob("*.c")):
        rel_path = source_path.relative_to(repo_root).as_posix()
        if should_skip(rel_path, args):
            continue

        text = source_path.read_text(errors="ignore")
        asm_matches = list(INCLUDE_ASM_RE.finditer(text))
        rodata_matches = list(INCLUDE_RODATA_RE.finditer(text))
        if not asm_matches and not (args.include_rodata and rodata_matches):
            continue

        stubs = []
        for match in asm_matches:
            directory = match.group("directory")
            name = match.group("name")
            size, handwritten = read_asm_metadata(repo_root, directory, name)
            stubs.append(Stub(name, "asm", f"{directory}/{name}.s", size, handwritten))

        for match in rodata_matches:
            directory = match.group("directory")
            name = match.group("name")
            stubs.append(Stub(name, "rodata", f"{directory}/{name}.s", "", False))

        frontiers.append(
            FileFrontier(
                path=rel_path,
                asm_count=len(asm_matches),
                rodata_count=len(rodata_matches),
                stubs=stubs,
                tags=native_tags(rel_path),
            )
        )

    return sorted(frontiers, key=lambda item: (item.asm_count, item.rodata_count, item.path))


def print_table(frontiers: list[FileFrontier], limit: int, include_functions: bool):
    shown = frontiers[:limit]
    total_asm = sum(item.asm_count for item in frontiers)
    total_rodata = sum(item.rodata_count for item in frontiers)
    print(f"Frontier files: {len(frontiers)} | INCLUDE_ASM: {total_asm} | INCLUDE_RODATA: {total_rodata}")
    print(f"{'ASM':>4} {'RO':>3} {'Tags':<22} Path")
    print(f"{'-' * 4} {'-' * 3} {'-' * 22} {'-' * 60}")
    for item in shown:
        print(f"{item.asm_count:>4} {item.rodata_count:>3} {','.join(item.tags):<22} {item.path}")
        if include_functions:
            for stub in item.stubs:
                handwritten = " handwritten" if stub.handwritten else ""
                size = f" {stub.size}" if stub.size else ""
                print(f"     - {stub.kind}:{size}{handwritten} {stub.name} ({stub.asm_path})")


def to_json(frontiers: list[FileFrontier]):
    return json.dumps(
        [
            {
                "path": item.path,
                "asm_count": item.asm_count,
                "rodata_count": item.rodata_count,
                "tags": item.tags,
                "stubs": [stub.__dict__ for stub in item.stubs],
            }
            for item in frontiers
        ],
        indent=2,
    )


def main():
    args = parse_args()
    repo_root = Path(args.root).resolve()
    frontiers = collect_frontiers(repo_root, args)
    if args.json:
        print(to_json(frontiers))
    else:
        print_table(frontiers, args.limit, args.functions)


if __name__ == "__main__":
    main()
