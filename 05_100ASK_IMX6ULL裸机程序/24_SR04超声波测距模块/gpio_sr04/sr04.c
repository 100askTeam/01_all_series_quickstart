
#include "sr04.h"

//设置Trig引脚输出电平
static void set_gpio_value(int value)
{
	/* IRDA GPIO4_IO19 */
	GPIO4->GDIR |= (1<<19);

    if (1 == value)
        GPIO4->DR |= (1<<19);
    else 
        GPIO4->DR &= ~(1<<19);
}
		  			 		  						  					  				 	   		  	  	 	  
void start_sr04(void)
{
	set_gpio_value(0);
	set_gpio_value(1);
	delay_us(15);
	
	gic_enable_irq(GPIO4_Combined_16_31_IRQn);
	set_gpio_value(0);
}
		  			 		  						  					  				 	   		  	  	 	  


