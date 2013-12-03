! diesel_glue.s
! assembly code for some DIESEL run-time support
! (this should be included by d.out)

	.section ".bss"
	.align	8
	.global	display
display:
	.skip	32
	.type	display,#object
	.size	display,32

	.seg	"text"
	.align	4
	.global	main
main:				! this is where the process starts
	save	%sp,-72,%sp
	set	display,%o1
	st	%g1,[%o1+4]
	std	%g2,[%o1+8]
	std	%g4,[%o1+16]
	std	%g6,[%o1+24]
	call	L3,0		! L3 is the DIESEL main program label
	nop
	ret
	restore
	.type	main,#function
	.size	main,(.-main)

swap_display:
	set	display,%o1
	ld	[%o1+4],%o2
	st	%g1,[%o1+4]
	mov	%o2,%g1
	ldd	[%o1+8],%o2
	std	%g2,[%o1+8]
	mov	%o2,%g2
	mov	%o3,%g3
	ldd	[%o1+16],%o2
	std	%g4,[%o1+16]
	mov	%o2,%g4
	mov	%o3,%g5
	ldd	[%o1+24],%o2
	std	%g6,[%o1+24]
	mov	%o2,%g6
	retl
	mov	%o3,%g7
	.type	swap_display,#function
	.size	swap_display,(.-swap_display)

L0:			! read
	save	%sp,-64,%sp
	call	swap_display,0
	nop
	call	getchar,0
	nop
	call	swap_display,0
	nop
	ret
	restore	%g0,%o0,%o0
	.type	L0,#function
	.size	L0,(.-L0)

L1:			! write
	save	%sp,-96,%sp
	call	swap_display,0
	nop
	call	myputchar,1	! in diesel_rts.o
	mov	%i0,%o0
	call	swap_display,0
	nop
	ret
	restore
	.type	L1,#function
	.size	L1,(.-L1)

L2:			! trunc
	save	%sp,-72,%sp
	st	%i0,[%sp+64]
	ld	[%sp+64],%f0
	fstoi	%f0,%f1
	st	%f1,[%sp+64]
	ld	[%sp+64],%i0
	ret
	restore
	.type	L2,#function
	.size	L2,(.-L2)

Mul:
	save	%sp,-96,%sp
	call	swap_display,0
	mov	%i0,%o0
	call	.mul,2
	mov	%i1,%o1
	call	swap_display,0
	nop
	ret
	restore	%g0,%o0,%o0
	.type	Mul,#function
	.size	Mul,(.-Mul)

Div:
	save	%sp,-96,%sp
	call	swap_display,0
	mov	%i0,%o0
	call	.div,2
	mov	%i1,%o1
	call	swap_display,0
	nop
	ret
	restore	%g0,%o0,%o0
	.type	Div,#function
	.size	Div,(.-Div)

Rem:
	save	%sp,-96,%sp
	call	swap_display,0
	mov	%i0,%o0
	call	.rem,2
	mov	%i1,%o1
	call	swap_display,0
	nop
	ret
	restore	%g0,%o0,%o0
	.type	Rem,#function
	.size	Rem,(.-Rem)

#ifdef	TRACE
Prologue:
	save	%sp,-96,%sp
	call	swap_display,0
	nop
	call	prologue,1	! in diesel_trace.o
	mov	%i0,%o0
	call	swap_display,0
	nop
	ret
	restore	%g0,%o0,%o0
	.type	Prologue,#function
	.size	Prologue,(.-Prologue)

Epilogue:
	save	%sp,-96,%sp
	call	swap_display,0
	nop
	call	epilogue,1	! in diesel_trace.o
	mov	%i0,%o0
	call	swap_display,0
	nop
	ret
	restore	%g0,%o0,%o0
	.type	Epilogue,#function
	.size	Epilogue,(.-Epilogue)

Quad:
	save	%sp,-96,%sp
	call	swap_display,0
	nop
	mov	%i4,%o4
	mov	%i3,%o3
	mov	%i2,%o2
	mov	%i1,%o1
	call	quad,5		! in diesel_trace.o
	mov	%i0,%o0
	call	swap_display,0
	nop
	ret
	restore	%g0,%o0,%o0
	.type	Quad,#function
	.size	Quad,(.-Quad)

#define PROLOGUE(X)\
	set	X,%o0 ;\
	call	Prologue,1 ;\
	nop

#define EPILOGUE(X)\
	set	X,%o0 ;\
	call	Epilogue,1 ;\
	nop

#define QUAD(A,B,C,D,E)\
	set	E,%o4 ;\
	set	D,%o3 ;\
	set	C,%o2 ;\
	set	A,%o0 ;\
	call	Quad,5 ;\
	or	%g0,B,%o1

#endif	/*TRACE*/
