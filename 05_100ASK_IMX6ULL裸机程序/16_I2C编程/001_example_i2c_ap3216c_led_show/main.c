/***************************************************************
版本	   : V1.0
描述	   : I2C testapp
其他	   : 
***************************************************************/
#include "i2c.h"
#include "type.h"
#include "ap3216c.h"
#include "led.h"
void delay(volatile unsigned int d)
{
	while(d--);
}

int  main()
{
	int ret = 0;
	uint16_t ir;
	uint16_t ps;
	uint16_t als;

	led_init();

	ret = ap3216c_init();
	if (ret)
	{	
		while(1)
		{
			/*初始化失败的话绿灯会狂闪*/
			led_ctl(1);
			delay(100000);
			led_ctl(0);
			delay(100000);
		}

	}
	else
	{
		/*初始化成功的话，绿灯闪一下，后面开关取决于传感器*/
		led_ctl(1);
		delay(100000);
		led_ctl(0);
	}

	while(!ret)
	{	
		/*环境光强度(ALS)、接近距离(PS)和红外线强度(IR)*/
		ap3216c_read_data(&ir,&ps,&als);
		/*调整光强和接近距离可以控制LED亮灭*/
		if (als>100 || ps <200)
		{
			led_ctl(1);
		}
		else
		{
			led_ctl(0);
		}
	}
					
	return 0;
}


