; HBIOS block I/O. The FAT layer still supplies 24-bit LBAs and the
; original four-sector write-back cache remains above this driver.

div16x8: xor	a
	ld	b,16
dv1:	add	hl,hl
	rla
	jr	c,dv2
	cp	c
	jr	c,dv3
dv2:	sub	c
	inc	l
dv3:	djnz	dv1
	ret

hbrw:	push	bc
	push	de
	push	hl
	ld	(fdlast),hl
	call	ptptr
	ld	a,(hl)
	add	a,e
	ld	(sblk),a
	inc	hl
	ld	a,(hl)
	adc	a,d
	ld	(sblk+1),a
	inc	hl
	ld	a,(lbax)
	ld	c,a
	ld	a,(hl)
	adc	a,c
	ld	(sblk+2),a
	cp	20h
	jr	nc,hb_io
	ld	c,(ix+D_UNIT)
	ld	b,BF_DIOSEEK
	ld	d,80h
	ld	e,a
	ld	hl,(sblk)
	call	HB_INVOKE
	or	a
	jr	nz,hb_error
	ld	c,(ix+D_UNIT)
	ld	d,BANK_SYS
	ld	e,1
	ld	hl,(fdlast)
	ld	a,(rwflag)
	add	a,BF_DIOREAD
	ld	b,a
	call	HB_INVOKE
	or	a
	jr	nz,hb_error
hb_ok:	pop	hl
	pop	de
	pop	bc
	xor	a
	ret
hb_error:
	cp	ERR_READONLY
	jr	z,hb_ro
	cp	ERR_NOMEDIA
	jr	z,hb_nr
	cp	ERR_NOTRDY
	jr	z,hb_nr
hb_io:	ld	a,(rwflag)
	or	a
	ld	a,E_READ
	jr	z,hb_fail
	ld	a,E_WRITE
	jr	hb_fail
hb_ro:	ld	a,E_WRPROT
	jr	hb_fail
hb_nr:	ld	a,E_NOTRDY
hb_fail:or	a
	pop	hl
	pop	de
	pop	bc
	ret

; HL = drive IX's entry in ptab. Keeps BC and DE.
ptptr:	push	ix
	pop	hl
	ld	a,l
	sub	low drvtab
	rrca
	rrca
	and	38h
	add	a,low ptab
	ld	l,a
	ld	a,high ptab
	adc	a,0
	ld	h,a
	ret

; Discover a whole-disk FAT volume or this drive's MBR partition.
pfind:	call	ptptr
	ld	b,3
	xor	a
pf_0:	ld	(hl),a
	inc	hl
	djnz	pf_0
	inc	hl
	ld	(hl),0FFh
	inc	hl
	ld	(hl),0FFh
	inc	hl
	ld	(hl),a
	inc	hl
	ld	(hl),a
	call	pfsel
	ld	de,0
	xor	a
	ld	(lbax),a
	call	getsec
	ret	nz
	ex	de,hl
	call	ptptr
	push	hl
	pop	iy
	ld	a,(de)
	cp	18h
	jr	z,pf_b
	cp	0EBh
	jr	z,pf_b
	cp	0E9h
	jr	nz,pf_t
pf_b:	ld	hl,0Bh
	add	hl,de
	ld	a,(hl)
	or	a
	jr	nz,pf_t
	inc	hl
	ld	a,(hl)
	cp	2
	jr	nz,pf_t
	ld	a,(iy+3)
	or	a
	jp	nz,pf_no
	ld	(iy+6),1Fh
pf_ok:	ld	(iy+7),1
	xor	a
	ret
pf_t:	ld	hl,1FEh
	add	hl,de
	ld	a,(hl)
	cp	55h
	jr	nz,pf_x
	inc	hl
	ld	a,(hl)
	cp	0AAh
	jr	nz,pf_x
	ld	a,(iy+3)
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	add	a,0BEh
	ld	l,a
	ld	a,1
	adc	a,0
	ld	h,a
	add	hl,de
	ld	bc,4
	add	hl,bc
	ld	a,(hl)
	cp	01h
	jr	z,pf_f
	cp	04h
	jr	z,pf_f
	cp	06h
	jr	z,pf_f
	cp	0Bh
	jr	z,pf_f
	cp	0Ch
	jr	z,pf_f
	cp	0Eh
	jr	nz,pf_no
pf_f:	add	hl,bc
	ld	a,(hl)
	ld	(iy+0),a
	inc	hl
	ld	a,(hl)
	ld	(iy+1),a
	inc	hl
	ld	a,(hl)
	ld	(iy+2),a
	cp	1Fh
	jr	nc,pf_no
	inc	hl
	ld	a,(hl)
	or	a
	jr	nz,pf_no
	inc	hl
	ld	a,(hl)
	ld	(iy+4),a
	inc	hl
	ld	a,(hl)
	ld	(iy+5),a
	inc	hl
	ld	a,(hl)
	ld	(iy+6),a
	inc	hl
	ld	a,(hl)
	or	a
	jr	z,pf_s
	ld	(iy+4),0FFh
	ld	(iy+5),0FFh
	ld	(iy+6),0FFh
pf_s:	ld	a,(iy+0)
	or	(iy+1)
	or	(iy+2)
	jr	z,pf_no
	call	pfsel
	jp	pf_ok
pf_x:	ld	a,(iy+3)
	or	a
	ret	z
pf_no:	ld	a,E_BADDRV
	or	a
	ret
pfsel:	ld	a,(drvnum)
	call	bforget
	ld	a,(drvnum)
	jp	seldrv
