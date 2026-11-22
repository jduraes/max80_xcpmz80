; HBIOS returns year, month, day, hour, minute and second as packed BCD.
clkget:	ld	hl,clk
	ld	b,BF_RTCGETTIM
	call	HB_INVOKE
	or	a
	jp	nz,cg_bad
	ld	a,(clk)
	call	hbcd
	cp	78
	jr	nc,cg_1
	add	a,100
cg_1:	add	a,low 1900
	ld	(cyear),a
	ld	a,high 1900
	adc	a,0
	ld	(cyear+1),a
	ld	a,(clk+1)
	call	hbcd
	ld	(cmon),a
	dec	a
	cp	12
	jr	nc,cg_bad
	ld	a,(clk+2)
	call	hbcd
	ld	(cday),a
	dec	a
	cp	31
	jr	nc,cg_bad
	ld	a,(clk+3)
	call	hbcd
	ld	(chour),a
	cp	24
	jr	nc,cg_bad
	ld	a,(clk+4)
	call	hbcd
	ld	(cmin),a
	cp	60
	jr	nc,cg_bad
	ld	a,(clk+5)
	call	hbcd
	ld	(csec),a
	cp	60
	ret	c
cg_bad:	ld	hl,cg_def
	ld	de,cyear
	ld	bc,7
	ldir
	ret
cg_def:	dw	2026
	db	1,1,0,0,0

hbcd:	push	bc
	ld	b,a
	and	0F0h
	rrca
	ld	c,a
	rrca
	rrca
	add	a,c
	ld	c,a
	ld	a,b
	and	0Fh
	add	a,c
	pop	bc
	ret
