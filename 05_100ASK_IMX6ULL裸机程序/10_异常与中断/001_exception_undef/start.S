	
.text
.global  _start, _vector_table
_start:
_vector_table:
	ldr 	pc, =Reset_Handler			 /* Reset				   */
	ldr 	pc, =Undefined_Handler		 /* Undefined instructions */
	//b Reset_Handler
	//b Undefined_Handler
	b halt//b SVC_Handler//ldr 	pc, =SVC_Handler			 /* Supervisor Call 	   */
	b halt//ldr 	pc, =PrefAbort_Handler		 /* Prefetch abort		   */
	b halt//ldr 	pc, =DataAbort_Handler		 /* Data abort			   */
	.word	0							 /* RESERVED			   */
	b halt//ldr 	pc, =IRQ_Handler			 /* IRQ interrupt		   */
	b halt//ldr 	pc, =FIQ_Handler			 /* FIQ interrupt		   */

.align 2
Undefined_Handler:
	/* 执行到这里之前:
	 * 1. lr_und保存有被中断模式中的下一条即将执行的指令的地址
	 * 2. SPSR_und保存有被中断模式的CPSR
	 * 3. CPSR中的M4-M0被设置为11011, 进入到und模式
	 * 4. 跳到0x4的地方执行程序 
	 */

	/* 在und异常处理函数中有可能会修改r0-r12, 所以先保存 */
	/* lr是异常处理完后的返回地址, 也要保存 */
	stmdb sp!, {r0-r12, lr}  
	
	/* 保存现场 */
	/* 处理und异常 */
	mrs r0, cpsr
	ldr r1, =und_string
	bl printException
	
	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^  /* ^会把spsr的值恢复到cpsr里 */
	
und_string:
	.string "undefined instruction exception"

.align 2
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

    cps     #0x1B                /* Enter undef mode                */
    ldr     sp, =0x80300000     /* Set up undef mode stack      */

    cps     #0x13                /* Enter Supervisor mode         */
    ldr     sp, =0x80200000     /* Set up Supervisor Mode stack  */

	ldr r0, =_vector_table
	mcr p15, 0, r0, c12, c0, 0  /* set VBAR, Vector Base Address Register*/
	//mrc p15, 0, r0, c12, c0, 0  //read VBAR

	bl clean_bss
	
	bl system_init

und_code:
	.word 0xeeadc0de  /* undefine instruction */
	//.word 0xFFFFFFFF

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


