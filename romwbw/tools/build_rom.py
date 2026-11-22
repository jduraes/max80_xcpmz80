#!/usr/bin/env python3
import argparse
import re
import struct
from pathlib import Path

BANK = 0x8000
ZSYS_OFFSET = BANK + 0x2000
KERNEL_BANK_OFFSET = 2 * BANK


def symbols(path):
    result = {}
    for line in path.read_text().splitlines():
        match = re.match(r"([A-Za-z_][A-Za-z0-9_]*):\s*EQU\s+0x([0-9A-Fa-f]+)", line)
        if match:
            result[match.group(1)] = int(match.group(2), 16)
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("base_rom", type=Path)
    parser.add_argument("combined", type=Path)
    parser.add_argument("symbols", type=Path)
    parser.add_argument("loader", type=Path, nargs="?")
    parser.add_argument("output", type=Path, nargs="?")
    args = parser.parse_args()

    sym = symbols(args.symbols)
    combined = args.combined.read_bytes()
    low_size = sym["KERNEL_SIZE"]
    common_size = sym["COMMON_LEN"]
    kernel_size = sym["FONT_LOAD"] - 0x1000
    assert len(combined) >= kernel_size + common_size
    common = combined[kernel_size:kernel_size + common_size]
    cold = combined[kernel_size + common_size:]
    kernel = combined[:kernel_size] + bytes(common_size) + cold
    assert len(kernel) == low_size
    assert len(common) == common_size
    assert low_size <= BANK
    assert len(common) <= 0xEA00 - 0xDF00

    outdir = args.output.parent if args.output else args.combined.parent
    outdir.mkdir(parents=True, exist_ok=True)
    (outdir / "kernel.bin").write_bytes(kernel)
    (outdir / "common.bin").write_bytes(common)
    (outdir / "layout.inc").write_text(
        "KERNEL_SIZE\tequ\t0%04Xh\ncold\tequ\t0%04Xh\n" % (low_size, sym["cold"])
    )
    if args.loader is None or args.output is None:
        return

    rom = bytearray(args.base_rom.read_bytes())
    assert len(rom) == 16 * BANK
    z_tail = bytes([0x00, 0x00, 0x20, 0x00, 0xD0, 0x00, 0x2E, 0x00, 0xE6])
    matches = [i for i in range(BANK, BANK + 0x1000)
               if rom[i:i + 2] == b"\0Z" and rom[i + 3] in (0x00, 0xFF)
               and rom[i + 4:i + 12] == z_tail[1:]]
    assert len(matches) == 2, matches
    for i in matches:
        rom[i + 10:i + 12] = struct.pack("<H", 0xEA00)
    os_image = bytearray(args.loader.read_bytes())
    assert len(os_image) <= 0x2E00
    os_image[0x0F00:0x0F00 + len(common)] = common
    rom[ZSYS_OFFSET:ZSYS_OFFSET + 0x3000] = os_image.ljust(0x3000, b"\0")
    rom[KERNEL_BANK_OFFSET:KERNEL_BANK_OFFSET + BANK] = kernel.ljust(BANK, b"\0")
    args.output.write_bytes(rom)


if __name__ == "__main__":
    main()
