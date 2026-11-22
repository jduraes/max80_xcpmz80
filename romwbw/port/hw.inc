; xcpmz80 on an RC2014 running RomWBW. Three RAM banks below 8000h:
; the kernel, the program TPA, and the optional C library. Common RAM is
; always above 8000h and HBIOS occupies its proxy at FE00h.

	define	ROMWBW
BANK_SYS	equ	08Dh		; BID_AUX
BANK_TPA	equ	08Eh		; BID_USR
BANK_B2		equ	08Ch		; BID_BUF
LIBFN		equ	200
OVLFN		equ	201

HB_INVOKE	equ	0FFF0h
HB_CURBNK	equ	0FFE0h
CIO_CONSOLE	equ	080h
BF_CIOIN	equ	000h
BF_CIOOUT	equ	001h
BF_CIOIST	equ	002h
BF_CIOOST	equ	003h
BF_DIOREAD	equ	013h
BF_DIOWRITE	equ	014h
BF_DIOSEEK	equ	012h
BF_DIODEVICE	equ	017h
BF_RTCGETTIM	equ	020h
BF_SYSGET	equ	0F8h
BF_SYSGET_DIOCNT equ	010h
BF_SYSSETCPY	equ	0F4h
BF_SYSBNKCPY	equ	0F5h
DIODEV_MD	equ	0
ERR_READONLY	equ	-10
ERR_NOMEDIA	equ	-7
ERR_NOTRDY	equ	-14

; Inline bank selection: no return address or saved flags live in the bank
; being replaced. RC2014's 512K/512K memory board selects 16K pages at
; ports 78h and 79h. Callers do not rely on A or the flags afterward.
	macro	BANKSET
	ld	(HB_CURBNK),a
	ifdef	EMU
	out	(078h),a
	else
	res	7,a
	add	a,16
	rlca
	out	(078h),a
	inc	a
	out	(079h),a
	endif
	endm

CBUF		equ	0F400h
CFCB		equ	0F480h
CVARS		equ	0F500h
STKTOP		equ	0F700h
ISRSTK		equ	0F7C0h
FMTBUF		equ	8000h
FMTSZ		equ	11900
OVLORG		equ	0AF00h
		assert	FMTBUF+FMTSZ <= OVLORG
KRING		equ	0F900h
ALV		equ	0F980h
ALVSZ		equ	512
DPB		equ	0FB80h
XDPH		equ	0FBA0h
IVPAGE		equ	0FBh

SPT		equ	17
TRACKS		equ	77
RSVD		equ	4*SPT
COLS		equ	80
ROWS		equ	24
CURSOR		equ	7Fh
FD_LATCH	equ	060h
FD5_LATCH	equ	040h
BLK		equ	0FE00h
