#include "motor.h"

int  main()
{
	gpio_init();
	
	while(1)
	{
		motor_sw(10,2000);  //顺时针 转2000步 速度 10          速度范围：1-10
		motor_aw(7,2000);   //逆时针 转2000步 速度 7	  
	}
   
	return 0;
}


