	
.text
.global  _start

_start: 

	/* 设置栈 */
	ldr  sp,=0x80200000

	/* 重定位data段 */
	bl copy_data

	/* 清除bss段 */
	bl clean_bss

	/* 跳转到主函数 */
	bl main

halt:
	b  halt 

copy_data:
	/* 重定位data段 */
	ldr r1, =data_load_addr  /* data段的加载地址 (0x8010....) */
	ldr r2, =data_start 	 /* data段重定位地址, 0x900000 */
	ldr r3, =data_end 	     /* data段结束地址(重定位后地址 0x90....) */
cpy:
	ldr r4, [r1] 			 /* 从r1读到r4 */
	str r4, [r2] 			 /* r4存放到r2 */
	add r1, r1, #4 			 /* r1+1 */
	add r2, r2, #4			 /* r2+1 */
	cmp r2, r3 				 /* r2 r3比较 */
	bne cpy 				 /* 如果不等则继续拷贝 */

	mov pc, lr

clean_bss:
	/* 清除bss段 */
	ldr r1, =__bss_start
	ldr r2, =__bss_end
	mov r3, #0
clean:
	strb r3, [r1]
	add r1, r1, #1
	cmp r1, r2
	bne clean

	mov pc, lr
