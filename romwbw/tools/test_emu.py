#!/usr/bin/env python3
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parent.parent
UPSTREAM = ROOT.parent
BUILD = ROOT / "build" / "test"
EMU = UPSTREAM.parent / "romwbw_emu" / "src" / "romwbw_emu"


def clean(data):
    return data.decode("ascii", "replace").replace("\x1b", "<ESC>")


def main():
    BUILD.mkdir(parents=True, exist_ok=True)
    (BUILD / "HELLO.TXT").write_bytes(b"Hello from RomWBW XCPM.\r\nSecond line.\r\n\x1a")
    (BUILD / "AUTOEXEC.BAT").write_bytes(
        b"ver\r\ndir\r\nclock\r\ntype hello.txt\r\n"
        b"copy hello.txt copy.txt\r\nmd test\r\n"
        b"copy hello.txt test\\h.txt\r\ncd test\r\nren g.txt=h.txt\r\n"
        b"type g.txt\r\ncd \\\r\nera copy.txt\r\ndir\r\ndir test\\\r\nfree\r\n"
    )
    image = BUILD / "xcpm-test.img"
    subprocess.run([
        "python3", "tools/mkdisk.py", "build/xcpmz80.sys", "--hd", "8",
        str(image), "build/MORE.COM", str(ROOT / "build/CLOCK.COM"), "build/BENCH.COM",
        str(BUILD / "HELLO.TXT"), str(BUILD / "AUTOEXEC.BAT"),
    ], cwd=UPSTREAM, check=True, stdout=subprocess.DEVNULL)
    run = subprocess.run([
        str(EMU), "--no-config", f"--romwbw={ROOT / 'build/xcpm-emu.rom'}",
        f"--romldr={ROOT / 'build/xcpm-emu.rom'}", f"--disk0={image}",
        "--boot=Z", "--max-instructions=20000000",
    ], stdin=subprocess.DEVNULL, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
       timeout=45)
    output = clean(run.stdout)
    (BUILD / "session.txt").write_text(output)
    assert run.returncode == 0, output
    expected = [
        "XCP/M-Z80 v0.2 for RomWBW on RC2014, 128K, 55K TPA",
        "A: MORE     COM : CLOCK    COM : BENCH    COM",
        "Hello from RomWBW XCPM.",
        "Second line.",
        "HELLO    TXT",
        "A: G        TXT",
        "K free",
    ]
    for text in expected:
        assert text in output, f"missing {text!r}\n{output}"
    assert re.search(r"\d\d/\d\d/\d\d \d\d:\d\d:\d\d", output), output
    subprocess.run(["python3", "tools/fatcheck.py", str(image)],
                   cwd=UPSTREAM, check=True, stdout=subprocess.DEVNULL)
    print("PASS: boot, shell, FAT reads/writes, folders, rename, erase, free space, RTC")


if __name__ == "__main__":
    main()
