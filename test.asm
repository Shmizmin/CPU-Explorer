; this is a test program
	xor r0,r0
	xor r0,r0
	call mem      ; jsr test1
	xor r0,r0
	inc %$FF

mem:
	xor r0,r0 ; xor test
	ret