; Soroc IQ-120 console on RomWBW's current HBIOS console. Cursor and
; editing sequences are translated to ANSI for a serial terminal.

crtout:	push	af
	push	bc
	push	de
	push	hl
	ld	a,c
	and	7Fh
	ld	c,a
	ld	a,(ESCST)
	or	a
	jp	nz,co_esc
	ld	a,c
	cp	1Bh
	jr	z,co_start
	cp	20h
	jr	c,co_ctl
co_send:ld	a,c
	call	hbout
co_done:pop	hl
	pop	de
	pop	bc
	pop	af
	ret
co_start:
	ld	a,1
co_state:
	ld	(ESCST),a
	jp	co_done
co_ctl:	cp	0Dh
	jr	z,co_send
	cp	0Ah
	jr	z,co_send
	cp	08h
	jr	z,co_send
	cp	09h
	jr	z,co_send
	cp	07h
	jr	z,co_send
	cp	0Bh
	jr	z,co_up
	cp	0Ch
	jr	z,co_right
	cp	1Ah
	jr	z,co_clear
	cp	1Eh
	jr	z,co_home
	jp	co_done
co_up:	ld	hl,ansi_up
	jp	co_str
co_right:ld	hl,ansi_right
	jp	co_str
co_home:ld	hl,ansi_home
	jp	co_str
co_clear:ld	hl,ansi_clear
co_str:	call	hbstr
	jp	co_done

co_esc:	cp	2
	jr	z,co_row
	jr	nc,co_col
	xor	a
	ld	(ESCST),a
	ld	a,c
	cp	'='
	jr	z,co_eq
	cp	'*'
	jr	z,co_clear
	cp	'Y'
	jr	z,co_eos
	cp	'T'
	jr	z,co_eol
	cp	'E'
	jr	z,co_ins
	cp	'R'
	jr	z,co_del
	cp	')'
	jr	z,co_inv
	cp	'('
	jr	z,co_nrm
	cp	'0'
	jr	z,co_hide
	cp	'1'
	jr	z,co_show
	jp	co_done
co_eq:	ld	a,2
	jp	co_state
co_row:	ld	a,c
	ld	(ESCROW),a
	ld	a,3
	jp	co_state
co_col:	xor	a
	ld	(ESCST),a
	ld	a,(ESCROW)
	sub	20h
	jp	c,co_done
	cp	ROWS
	jp	nc,co_done
	inc	a
	push	af
	ld	a,c
	sub	20h
	jp	c,co_done_pop
	cp	COLS
	jp	nc,co_done_pop
	inc	a
	push	af
	ld	hl,ansi_pos
	call	hbstr
	pop	af
	push	af
	call	hbnum
	ld	a,';'
	call	hbout
	pop	af
	call	hbnum
	ld	a,'H'
	call	hbout
	pop	af
	jp	co_done
co_done_pop:
	pop	af
	jp	co_done
co_eos:	ld	hl,ansi_eos
	jp	co_str
co_eol:	ld	hl,ansi_eol
	jp	co_str
co_ins:	ld	hl,ansi_ins
	jp	co_str
co_del:	ld	hl,ansi_del
	jp	co_str
co_inv:	ld	hl,ansi_inv
	jp	co_str
co_nrm:	ld	hl,ansi_nrm
	jp	co_str
co_hide:ld	hl,ansi_hide
	jp	co_str
co_show:ld	hl,ansi_show
	jp	co_str

hbout:	ld	e,a
	ld	b,BF_CIOOUT
	ld	c,CIO_CONSOLE
	jp	HB_INVOKE
hbstr:	ld	a,(hl)
	or	a
	ret	z
	push	hl
	call	hbout
	pop	hl
	inc	hl
	jr	hbstr
hbnum:	ld	b,0
hbn_1:	inc	b
	sub	10
	jr	nc,hbn_1
	add	a,10
	dec	b
	push	af
	ld	a,b
	or	a
	jr	z,hbn_2
	add	a,'0'
	call	hbout
hbn_2:	pop	af
	add	a,'0'
	jp	hbout

ansi_up:	db	1Bh,"[A",0
ansi_right:	db	1Bh,"[C",0
ansi_home:	db	1Bh,"[H",0
ansi_clear:	db	1Bh,"[2J",1Bh,"[H",0
ansi_pos:	db	1Bh,"[",0
ansi_eos:	db	1Bh,"[0J",0
ansi_eol:	db	1Bh,"[0K",0
ansi_ins:	db	1Bh,"[L",0
ansi_del:	db	1Bh,"[M",0
ansi_inv:	db	1Bh,"[7m",0
ansi_nrm:	db	1Bh,"[27m",0
ansi_hide:	db	1Bh,"[?25l",0
ansi_show:	db	1Bh,"[?25h",0

; HBIOS input is polled into the same small ring the MAX-80 keyboard ISR
; filled, so the fast BDOS console paths continue to work unchanged.
kbdst:	call	kpoll
	ld	a,(KTAKE)
	ld	b,a
	ld	a,(KFILL)
	sub	b
	ret	z
	ld	a,0FFh
	ret
kpoll:	push	af
	push	bc
	push	de
	push	hl
	ld	b,BF_CIOIST
	ld	c,CIO_CONSOLE
	call	HB_INVOKE
	or	a
	jr	z,kp_done
	bit	7,a
	jr	nz,kp_done
	ld	b,BF_CIOIN
	ld	c,CIO_CONSOLE
	call	HB_INVOKE
	ld	a,(KFILL)
	inc	a
	and	7Fh
	ld	b,a
	ld	a,(KTAKE)
	cp	b
	jr	z,kp_done
	ld	a,(KFILL)
	ld	l,a
	ld	h,high KRING
	ld	(hl),e
	ld	a,b
	ld	(KFILL),a
kp_done:pop	hl
	pop	de
	pop	bc
	pop	af
	ret
kpeek:	push	hl
	ld	a,(KTAKE)
	ld	l,a
	ld	a,(KFILL)
	cp	l
	ld	a,0
	jr	z,kk_1
	ld	h,high KRING
	ld	a,(hl)
kk_1:	pop	hl
	ret
kbdin:	push	bc
	push	hl
ci1:	call	kbdst
	jr	z,ci1
	ld	a,(KTAKE)
	ld	l,a
	ld	h,high KRING
	ld	c,(hl)
	inc	a
	and	7Fh
	ld	(KTAKE),a
	ld	a,c
	pop	hl
	pop	bc
	ret
