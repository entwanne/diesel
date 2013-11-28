program prog;
var
    a : integer;
    b : integer;
    c : integer;
    
    procedure p1(d : integer);
    var
    	b : integer;
	c : integer;

        procedure p2;
        var
            c : integer;
        begin
            c := b + a;
        end;

    begin
        c := b + a + d;
    end;

begin
    a := b;
    p1(c);
end.

{ hypothetical translation to 68000:

	.text
	jra	L0			;jump to main program

L2:	link	a6,#-12			;space for c, $1 and displaysave.
	movl	a2,a6@(-4)
	movl	a6,a2
	movl	a1@(-12), d0		;d0 = b
	addl	a0@(-12), d0		;do = b + a
	movl	d0, a2@(-16)		;$1 = b + a
	movl	a2@(-16), a2@(-12)	;c = b + a
L3:	movl	a6@(-4), a0	
	unlk	a6
	rts

L1:	link	a6,#-20                 ;space for b, c, $2, $3 and displaysave
	movl	a1,a6@(-4)		;save display
	movl	a6,a1			;set up new display for level 1
	movl	a1@(-8), d0		;d0 = b
	addl	a0@(-8), d0		;d0 = d0 + a
	movl	d0, a1@(-16)		;$2 = b + a
	movl	a1@(-16), d0		;d0 = $2
	addl	a1@(16), d0		;d0 = d0 + d
	movl	d0, a1@(-20)		;$3 = b + a + d
	movl	a1@(-20), a1@(-12)	;c := b + a + d
L4:	movl	a6@(-4), a0		;restore display
	unlk	a6
	rts
	
L0:	link	a6,#-16 		;space for a, b, c and previous display
	movl	a0,a6@(-4)		;save old display for level 0
	movl	a6,a0			;set up new display for level 0
	movl	a0@(-12), a0@(-8)	;a := b
	movl	a0@(-16), -(sp)		;push parameter c
	jbsr	L1			;call p1
	lea	sp@(4), sp		;pop parameter c
L5:	movl	a6@(-4), a0		;restore display
	unlk	a6
	rts
}
