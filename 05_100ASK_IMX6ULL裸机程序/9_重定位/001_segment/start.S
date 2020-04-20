	
.text
.global  _start

_start: 

	/* 设置栈 */
	ldr  sp,=0x80200000
	
	/* 跳转到主函数 */
	bl main

halt:
	b  halt 
