#!/usr/bin/env python3
import sys
import zipfile
from pathlib import Path

archive, output = sys.argv[1:3]
with zipfile.ZipFile(archive) as z:
    turbo = bytearray(z.read("TURBO.COM"))

assert len(turbo) == 30720
assert turbo[0x24] == 3
assert turbo[0x53] == 0x0C
assert turbo[0x54:0x68] == b"Microbee VDUped VDU."
turbo[0x24] = 7
turbo[0x53] = 0x14
turbo[0x54:0x68] = b"Soroc 120/Apple CP/M"

path = Path(output)
path.parent.mkdir(parents=True, exist_ok=True)
path.write_bytes(turbo)
