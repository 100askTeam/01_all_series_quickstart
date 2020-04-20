	
.text
.global  _start, _vector_table
_start:
_vector_table:
	ldr 	pc, =Reset_Handler			 /* Reset				   */
	

Reset_Handler:
	/* Reset SCTlr Settings */
	mrc 	p15, 0, r0, c1, c0, 0	  /* read SCTRL, Read CP15 System Control register		*/
	bic 	r0,  r0, #(0x1 << 13)	  /* Clear V bit 13 to use normal exception vectors  	*/
	bic 	r0,  r0, #(0x1 << 12)	  /* Clear I bit 12 to disable I Cache					*/
	bic 	r0,  r0, #(0x1 <<  2)	  /* Clear C bit  2 to disable D Cache					*/
	bic 	r0,  r0, #(0x1 << 2)	  /* Clear A bit  1 to disable strict alignment 		*/
	bic 	r0,  r0, #(0x1 << 11)	  /* Clear Z bit 11 to disable branch prediction		*/
	bic 	r0,  r0, #0x1			  /* Clear M bit  0 to disable MMU						*/
	mcr 	p15, 0, r0, c1, c0, 0	  /* write SCTRL, Write to CP15 System Control register	*/
	bl clean_bss
	
	bl system_init
/*	cpsie	i					 /* Unmask interrupts			  */
	bl main

halt:
	b  halt


clean_bss:
	/* 清除BSS段 */
	ldr r1, =__bss_start
	ldr r2, =__bss_end
	mov r3, #0
clean:
	cmp r1, r2
	strlt r3, [r1]
	add r1, r1, #4
	blt clean
	
	mov pc, lr


