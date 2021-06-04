; this is a test program
	xor r0,r0
	xor r0,r0
	call mem      ; jsr test1
	xor r0,r0
	xor r0, %$0100
	inc %$FF

mem:
	;xor r1, r1
	;inc %$FF, r0
	ret