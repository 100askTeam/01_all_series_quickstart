	
.text
.global  _start
_start: 				

//ÉèÖÃÕ»
	ldr  sp,=0x80200000

	bl clean_bss
	
	bl main

halt:
	b  halt 

clean_bss:
	/* Çå³ıBSS¶Î */
	ldr r1, =__bss_start
	ldr r2, =__bss_end
	mov r3, #0
clean:
	cmp r1, r2
	strlt r3, [r1]
	add r1, r1, #4
	blt clean
	
	mov pc, lr
