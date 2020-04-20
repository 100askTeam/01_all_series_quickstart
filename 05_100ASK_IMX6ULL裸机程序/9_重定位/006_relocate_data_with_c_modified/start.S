	
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
