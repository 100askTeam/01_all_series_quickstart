	
.text
.global  _start

_start: 

	/* 设置栈 */
	ldr  sp,=0x80200000

	/* 重定位data段 */
	ldr r0, =data_load_addr  /* data段的加载地址 (0x8010....) */
	ldr r1, =data_start 	 /* data段重定位地址, 0x900000 */
	ldr r2, =data_end 	     /* data段结束地址(重定位后地址 0x90....) */
	sub r2, r2, r1			 /* r2的值为data段的长度 */	

	bl copy_data			 /* 跳转到函数copy_data并将r0,r1,r2作为函数参数传入 */

	/* 清除bss段 */
	ldr r0, =__bss_start	
	ldr r1, =__bss_end

	bl clean_bss			/* 跳转到函数clean_bss并将r0, r1作为函数参数传入*/

	/* 跳转到主函数 */
	bl main

halt:
	b  halt 
