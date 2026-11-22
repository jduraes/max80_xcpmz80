; ROM OS image entered by RomWBW's Z-System menu entry. The stock
; Z-System image is replaced by this loader and XCPM's common part.
	include	"hw.inc"
	include	"layout.inc"
KERNEL_ROM_BANK	equ	2
	org	0D000h
	block	0EA00h-$,0
xstart:	di
	ld	sp,STKTOP
	ld	b,BF_SYSSETCPY
	ld	d,BANK_SYS
	ld	e,KERNEL_ROM_BANK
	ld	hl,KERNEL_SIZE
	call	HB_INVOKE
	ld	hl,0
	ld	de,1000h
	ld	b,BF_SYSBNKCPY
	call	HB_INVOKE
	ld	a,BANK_SYS
	BANKSET
	jp	cold
	assert	$ <= 0FE00h
