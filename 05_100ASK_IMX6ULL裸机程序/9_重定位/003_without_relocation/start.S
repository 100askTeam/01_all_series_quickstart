	
.text
.global  _start

_start: 

	/* 设置栈 */
	ldr  sp,=0x80200000

	/* 清除bss段 */
	bl clean_bss
	
	/* 跳转到主函数 */
	bl main

halt:
	b  halt 

clean_bss:
	ldr r1, =__bss_start	//将链接脚本变量__bss_start变量保存于r1
	ldr r2, =__bss_end		//将链接脚本变量__bss_end变量保存于r2
	mov r3, #0
clean:
	strb r3, [r1]			//将当前地址下的数据清零
	add r1, r1, #1			//将r1内存储的地址+1
	cmp r1, r2				//相等：清零操作结束；否则继续执行clean函数清零bss段
	bne clean
	
	mov pc, lr
