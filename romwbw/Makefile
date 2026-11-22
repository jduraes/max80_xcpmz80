UPSTREAM ?= ..
SJASM ?= tools/sjasmplus
EMU ?= ../romwbw_emu/src/romwbw_emu
PORT_SRC = $(wildcard port/*.asm port/*.inc $(UPSTREAM)/src/*.asm $(UPSTREAM)/src/*.inc)

all: build/xcpm-rc2014.rom build/xcpm-emu.rom build/xcpm.img

build/xcpm-rc2014.rom: build/hw/loader.bin build/hw/combined.bin build/hw/xcpm.sym base/RC2014_std.rom tools/build_rom.py
	python3 tools/build_rom.py base/RC2014_std.rom build/hw/combined.bin build/hw/xcpm.sym build/hw/loader.bin $@

build/xcpm-emu.rom: build/emu/loader.bin build/emu/combined.bin build/emu/xcpm.sym base/emu_avw.rom tools/build_rom.py
	python3 tools/build_rom.py base/emu_avw.rom build/emu/combined.bin build/emu/xcpm.sym build/emu/loader.bin $@

build/hw/combined.bin: $(PORT_SRC)
	mkdir -p build/hw
	cd port && ../$(SJASM) --nologo --msg=war --sym=../build/hw/xcpm.sym --lst=../build/hw/xcpm.lst --raw=../$@ xcpmz80.asm

build/emu/combined.bin: $(PORT_SRC)
	mkdir -p build/emu
	cd port && ../$(SJASM) --nologo --msg=war -DEMU --sym=../build/emu/xcpm.sym --lst=../build/emu/xcpm.lst --raw=../$@ xcpmz80.asm

build/hw/xcpm.sym: build/hw/combined.bin
	@test -f $@

build/emu/xcpm.sym: build/emu/combined.bin
	@test -f $@

build/hw/layout.inc: build/hw/combined.bin build/hw/xcpm.sym
	python3 tools/build_rom.py base/RC2014_std.rom build/hw/combined.bin build/hw/xcpm.sym

build/emu/layout.inc: build/emu/combined.bin build/emu/xcpm.sym
	python3 tools/build_rom.py base/emu_avw.rom build/emu/combined.bin build/emu/xcpm.sym

build/hw/loader.bin: port/loader.asm port/hw.inc build/hw/layout.inc
	cd port && ../$(SJASM) --nologo --msg=war -I../build/hw --raw=../$@ loader.asm

build/emu/loader.bin: port/loader.asm port/hw.inc build/emu/layout.inc
	cd port && ../$(SJASM) --nologo --msg=war -DEMU -I../build/emu --raw=../$@ loader.asm

$(UPSTREAM)/build/xcpmz80.sys: $(wildcard $(UPSTREAM)/src/*.asm $(UPSTREAM)/src/*.inc $(UPSTREAM)/Makefile)
	$(MAKE) -C $(UPSTREAM) SJASM=$(CURDIR)/$(SJASM) build/xcpmz80.sys

$(UPSTREAM)/build/MORE.COM $(UPSTREAM)/build/BENCH.COM &: $(UPSTREAM)/build/xcpmz80.sys
	$(MAKE) -C $(UPSTREAM) SJASM=$(CURDIR)/$(SJASM) build/MORE.COM build/BENCH.COM

build/CLOCK.COM: port/clock.asm
	mkdir -p build
	$(SJASM) --nologo --msg=war --raw=$@ $<

build/xcpm.img: $(UPSTREAM)/build/xcpmz80.sys $(UPSTREAM)/build/MORE.COM $(UPSTREAM)/build/BENCH.COM build/CLOCK.COM
	cd $(UPSTREAM) && python3 tools/mkdisk.py build/xcpmz80.sys --hd 8 $(CURDIR)/$@ \
		build/MORE.COM $(CURDIR)/build/CLOCK.COM build/BENCH.COM

run: all
	$(EMU) --no-config --romwbw=build/xcpm-emu.rom --romldr=build/xcpm-emu.rom \
		--disk0=build/xcpm.img --boot=Z

test: all
	python3 tools/test_emu.py

clean:
	rm -rf build $(UPSTREAM)/build

.PHONY: all run test clean
