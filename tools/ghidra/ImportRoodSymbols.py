# Imports rood-reverse symbol_addrs.txt files into the current Ghidra program.
#@category Rood Reverse

import os
import re

from ghidra.program.model.symbol import SourceType


SYMBOL_RE = re.compile(r"^\s*([A-Za-z_.$][A-Za-z0-9_.$]*)\s*=\s*(0x[0-9A-Fa-f]+)\s*;?(?:\s*//\s*(.*))?$")


def parse_symbol_line(line):
    match = SYMBOL_RE.match(line)
    if match is None:
        return None

    name = match.group(1)
    address = int(match.group(2), 16)
    comment = match.group(3) or ""
    return name, address, comment


def should_create_function(name, comment, create_functions):
    if not create_functions:
        return False
    if "type:jtbl" in comment:
        return False
    if name.startswith("D_") or name.startswith("jtbl_"):
        return False
    return True


def import_symbol_file(path, create_functions):
    imported = 0
    skipped = 0
    failed = 0

    with open(path, "r") as symbol_file:
        for line_number, line in enumerate(symbol_file, 1):
            parsed = parse_symbol_line(line)
            if parsed is None:
                if line.strip() and not line.lstrip().startswith("//"):
                    skipped += 1
                continue

            name, address_value, comment = parsed
            address = toAddr(address_value)

            try:
                existing_names = set(symbol.getName() for symbol in getSymbols(address))
                if name not in existing_names:
                    createLabel(address, name, False, SourceType.USER_DEFINED)

                if should_create_function(name, comment, create_functions):
                    if getFunctionAt(address) is None:
                        createFunction(address, name)

                imported += 1
            except Exception as exc:
                failed += 1
                print("Failed to import %s:%d %s @ 0x%08X: %s" % (
                    path,
                    line_number,
                    name,
                    address_value,
                    exc,
                ))

    return imported, skipped, failed


def main():
    args = list(getScriptArgs())
    create_functions = False
    paths = []

    for arg in args:
        if arg == "--functions":
            create_functions = True
        else:
            paths.append(arg)

    if not paths:
        chosen = askFile("Choose a rood-reverse symbol_addrs.txt file", "Import")
        paths = [chosen.getAbsolutePath()]

    total_imported = 0
    total_skipped = 0
    total_failed = 0

    for path in paths:
        if not os.path.exists(path):
            print("Missing symbol file: %s" % path)
            total_failed += 1
            continue

        imported, skipped, failed = import_symbol_file(path, create_functions)
        total_imported += imported
        total_skipped += skipped
        total_failed += failed
        print("%s: imported=%d skipped=%d failed=%d" % (path, imported, skipped, failed))

    print("Rood symbol import complete: imported=%d skipped=%d failed=%d" % (
        total_imported,
        total_skipped,
        total_failed,
    ))


main()
