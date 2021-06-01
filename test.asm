	
	call mem     ;  	jsr test
	inc r0       ;  	register literal test
	add r1, #100 ;  	immediate value test

mem:
	inc %50
	return