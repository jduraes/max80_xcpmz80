#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p base tools
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT HUP INT TERM

if [ ! -x tools/sjasmplus ]; then
    version=1.24.0
    curl -fsSL "https://github.com/z00m128/sjasmplus/releases/download/v$version/sjasmplus-$version-src.tar.xz" \
        -o "$work/sjasmplus.tar.xz"
    tar -xJf "$work/sjasmplus.tar.xz" -C "$work"
    make -C "$work/sjasmplus-$version"
    cp "$work/sjasmplus-$version/sjasmplus" tools/
fi

if [ ! -f base/RC2014_std.rom ]; then
    version=3.6.0
    curl -fsSL "https://github.com/wwarthen/RomWBW/releases/download/v$version/RomWBW-v$version-Package.zip" \
        -o "$work/romwbw.zip"
    unzip -o -j "$work/romwbw.zip" Binary/RC2014_std.rom -d base
fi

if [ ! -f base/emu_avw.rom ]; then
    curl -fsSL "https://github.com/avwohl/romwbw_disks/releases/download/v0-romwbw-3.6.0/emu_avw-v0-3.6.0.rom" \
        -o base/emu_avw.rom
fi
