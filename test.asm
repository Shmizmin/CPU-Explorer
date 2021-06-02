mem:
	inc %$0043
	dec %$FF0D

test:
	xor r1, r1
	inc r1

; this is a test program
	call mem      ; jsr test
	call test
	inc r0        ; register literal test
	add r1, #$100 ; immediate value test