; SETDEF.COM: display or set XCPM's four-entry drive search path.
SCB_PATH	equ	04Ch
	org	0100h
start:	ld	hl,0080h
	ld	b,(hl)
	inc	hl
	ld	a,b
	or	a
	jp	z,show
	ld	ix,path
	xor	a
	ld	(count),a
parse:	call	separators
	jr	z,finish
	cp	'?'
	jp	z,usage
	cp	'*'
	jr	z,current
	sub	'A'
	cp	16
	jp	nc,usage
	inc	a
	jr	store
current:xor	a
store:	ld	c,a
	ld	a,(count)
	cp	4
	jp	nc,usage
	ld	(ix),c
	inc	ix
	inc	a
	ld	(count),a
	inc	hl
	dec	b
	jr	parse
finish:	ld	a,(count)
	or	a
	jp	z,usage
	ld	a,0FFh
	ld	(ix),a
	call	setpath
show:	call	getpath
	ld	de,msgpath
	call	print
	ld	ix,path
	ld	b,0
show1:	ld	a,(ix)
	cp	0FFh
	jp	z,crlf
	ld	c,a
	push	bc
	ld	a,b
	or	a
	jr	z,show2
	ld	a,','
	call	out
	ld	a,' '
	call	out
show2:	pop	bc
	push	bc
	ld	a,c
	or	a
	jr	z,showcur
	add	a,'A'-1
	call	out
	ld	a,':'
	call	out
	jr	show3
showcur:ld	de,msgcur
	call	print
show3:	pop	bc
	inc	ix
	inc	b
	jr	show1

separators:
	ld	a,b
	or	a
	ret	z
	ld	a,(hl)
	cp	' '
	jr	z,sep1
	cp	','
	jr	z,sep1
	cp	':'
	ret	nz
sep1:	inc	hl
	dec	b
	jr	separators

getpath:ld	ix,path
	ld	b,0
get1:	ld	a,b
	add	a,SCB_PATH
	ld	(scbpb),a
	xor	a
	ld	(scbpb+1),a
	push	bc
	ld	de,scbpb
	ld	c,49
	call	5
	pop	bc
	ld	(ix),a
	cp	0FFh
	ret	z
	inc	ix
	inc	b
	ld	a,b
	cp	4
	jr	nz,get1
	ret

setpath:ld	ix,path
	ld	b,0
set1:	ld	a,(ix)
	ld	(scbpb+2),a
	ld	a,b
	add	a,SCB_PATH
	ld	(scbpb),a
	ld	a,0FFh
	ld	(scbpb+1),a
	push	bc
	ld	de,scbpb
	ld	c,49
	call	5
	pop	bc
	ld	a,(ix)
	cp	0FFh
	ret	z
	inc	ix
	inc	b
	jr	set1

usage:	ld	de,msguse
	jp	print
crlf:	ld	de,newline
print:	ld	c,9
	jp	5
out:	ld	e,a
	ld	c,2
	jp	5

msgpath:db	"Search path: $"
msgcur:	db	"current drive$"
msguse:	db	"Usage: SETDEF [A:] [B:] [C:] [D:]  (* = current drive)",13,10,'$'
newline:db	13,10,'$'
scbpb:	ds	4
path:	ds	5
count:	ds	1
