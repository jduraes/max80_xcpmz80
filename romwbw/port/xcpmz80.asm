; Combined XCPM kernel and common-memory image for the RomWBW port.
	include	"hw.inc"
	include	"../../src/kernel.asm"
	db	0
FONT_LOAD:
	include	"../../src/common.asm"
IMAGE_END:
	include	"../../src/vars.inc"
KERNEL_SIZE	equ	IMAGE_END-1000h
	assert	FONT_LOAD <= 8000h
