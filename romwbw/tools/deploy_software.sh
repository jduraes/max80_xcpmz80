#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
ROMWBW=$(pwd)
SOFTWARE=$ROMWBW/build/software
CPROGRAMS=${CPROGRAMS:-../../max80_cprograms}
mkdir -p "$SOFTWARE"

if [ ! -f "$CPROGRAMS/Makefile" ]; then
	git clone https://github.com/fletto2/max80_cprograms.git "$CPROGRAMS"
fi
CPROGRAMS=$(cd "$CPROGRAMS" && pwd)

if [ -n "${SDCC_BIN:-}" ]; then
	:
elif [ -x /opt/cross/sdcc-4.6.0/bin/sdcc ]; then
	SDCC_BIN=/opt/cross/sdcc-4.6.0/bin
elif [ -x ../../tools/sdcc-4.6.0/bin/sdcc ]; then
	SDCC_BIN=../../tools/sdcc-4.6.0/bin
else
	SDCC_BIN=$SOFTWARE/sdcc-4.6.0/bin
	if [ ! -x "$SDCC_BIN/sdcc" ]; then
		curl -fsSL --retry 3 \
			https://sourceforge.net/projects/sdcc/files/sdcc-linux-amd64/4.6.0/sdcc-4.6.0-amd64-unknown-linux2.5.tar.bz2/download \
			-o "$SOFTWARE/sdcc-4.6.0.tar.bz2"
		python3 - "$SOFTWARE/sdcc-4.6.0.tar.bz2" "$SOFTWARE" <<'PY'
import sys
import tarfile
with tarfile.open(sys.argv[1], "r:bz2") as archive:
    archive.extractall(sys.argv[2])
PY
	fi
fi
SDCC_BIN=$(cd "$SDCC_BIN" && pwd)

BBC_ZIP=$SOFTWARE/BBCZ80V5.zip
if [ ! -f "$BBC_ZIP" ]; then
	curl -fsSL --retry 3 https://www.bbcbasic.co.uk/bbcbasic/BBCZ80V5.zip -o "$BBC_ZIP"
fi
if [ ! -f "$SOFTWARE/bbc/B/0/BBCBASIC.COM" ]; then
	python3 - "$BBC_ZIP" "$SOFTWARE/bbc" <<'PY'
import sys
import zipfile
with zipfile.ZipFile(sys.argv[1]) as archive:
    archive.extractall(sys.argv[2])
PY
fi
if [ ! -f "$SOFTWARE/ZORK1.DAT" ]; then
	curl -fsSL --retry 3 \
		https://raw.githubusercontent.com/skx/cpm-dist/master/G/ZORK1.DAT \
		-o "$SOFTWARE/ZORK1.DAT"
fi

make -j2 all
make -C "$CPROGRAMS" -j2 all SDCC_BIN="$SDCC_BIN"

set -- \
	build/MORE.COM "$ROMWBW/build/CLOCK.COM" build/BENCH.COM \
	"$CPROGRAMS/build/CLIB.SYS" \
	"$CPROGRAMS/build/ADVENT.COM" \
	"$CPROGRAMS/build/ADVENT1.TXT" "$CPROGRAMS/build/ADVENT2.TXT" \
	"$CPROGRAMS/build/ADVENT3.TXT" "$CPROGRAMS/build/ADVENT4.TXT" \
	"$CPROGRAMS/build/BASE64.COM" "$CPROGRAMS/build/ZORK.COM" \
	"$SOFTWARE/ZORK1.DAT" "$CPROGRAMS/build/EMACS.COM" \
	"$CPROGRAMS/build/VI.COM" "$SOFTWARE/bbc/B/0/BBCBASIC.COM"
cd ..
python3 tools/mkdisk.py build/xcpmz80.sys --hd 8 "$ROMWBW/build/xcpm.img" "$@" \
	--in BBC "$BBC_ZIP" "$SOFTWARE/bbc/README.TXT" \
	"$SOFTWARE/bbc/B/0/WHATSNEW.TXT" "$SOFTWARE"/bbc/B/0/*.BBC
python3 tools/fatcheck.py "$ROMWBW/build/xcpm.img"
