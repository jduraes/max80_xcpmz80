; CLOCK.COM for RomWBW: print HBIOS time as YY/MM/DD HH:MM:SS.
	org	100h
	ld	hl,timebuf
	ld	b,020h
	call	0FFF0h
	or	a
	jr	nz,error
	ld	hl,timebuf
	call	field
	ld	a,'/'
	call	out
	call	field
	ld	a,'/'
	call	out
	call	field
	ld	a,' '
	call	out
	call	field
	ld	a,':'
	call	out
	call	field
	ld	a,':'
	call	out
	call	field
	ld	de,crlf
	ld	c,9
	call	5
	ret

field:	ld	a,(hl)
	inc	hl
	ld	(pointer),hl
	call	putbcd
	ld	hl,(pointer)
	ret
putbcd:	push	af
	rrca
	rrca
	rrca
	rrca
	call	digit
	pop	af
digit:	and	0Fh
	add	a,'0'
out:	push	hl
	ld	e,a
	ld	c,2
	call	5
	pop	hl
	ret
error:	ld	de,errmsg
	ld	c,9
	call	5
	ret
crlf:	db	13,10,'$'
errmsg:	db	"No clock",13,10,'$'
pointer: ds	2
timebuf: ds	6
