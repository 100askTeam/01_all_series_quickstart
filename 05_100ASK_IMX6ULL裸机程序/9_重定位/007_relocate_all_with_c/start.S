	
.text
.global  _start

_start: 

	/* 设置栈 */
	ldr  sp,=0x80200000

	/* 重定位text, rodata, data段 */
	bl copy_data

	/* 清除bss段 */
	bl clean_bss

	/* 跳转到主函数 */
	// bl main		/* 相对跳转，程序仍在DDR3内存中执行 */
	ldr pc, =main 	/* 绝对跳转，程序在片内RAM中执行 */

halt:
	b  halt 
