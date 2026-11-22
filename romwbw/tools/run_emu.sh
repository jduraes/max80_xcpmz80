#!/bin/sh
set -eu
cd "$(dirname "$0")/.."

EMU=${EMU:-../../romwbw_emu/src/romwbw_emu}
if [ ! -x "$EMU" ]; then
	printf 'Emulator not found or not executable: %s\n' "$EMU" >&2
	printf 'Set EMU to the romwbw_emu executable and try again.\n' >&2
	exit 1
fi

make -j2 all
printf 'Starting XCPM. Press Ctrl+E, then enter quit, to exit the emulator.\n'
exec "$EMU" --no-config \
	--romwbw=build/xcpm-emu.rom \
	--romldr=build/xcpm-emu.rom \
	--disk0=build/xcpm.img \
	--boot=Z
