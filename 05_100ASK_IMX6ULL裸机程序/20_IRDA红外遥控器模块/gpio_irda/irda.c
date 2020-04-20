
#include "irda.h"

unsigned char result[4] = { 0x00, 0x00, 0x00, 0x00 };
		  			 		  						  					  				 	   		  	  	 	  
//设置引脚输出电平
static void set_gpio_value(int value)
{
	/* IRDA GPIO4_IO19 */
	GPIO4->GDIR |= (1<<19);

    if (1 == value)
        GPIO4->DR |= (1<<19);
    else 
        GPIO4->DR &= ~(1<<19);
}

//读取引脚电平
static int get_gpio_value(void)
{
	GPIO4->GDIR &= ~(1<<19);
						
	if((GPIO4->DR >> 19 ) & 0x1)
		return 1;
	else
		return 0;								
}


//接收判断开始信号
static int irda_start(void)
{
    int i;
    int start_flag = 1;
    
    while(get_gpio_value());//一直等到有低电平
    
    for(i=0; i<9; i++)  //循环十次，目的是检测每个800微秒内(共9ms内)如果出现高电平就退出解码程序  
    {  
        delay_us(800);  
        if (get_gpio_value() == 1)  //出现高电平，错误
        {  
            start_flag = 0;  
            break;  
        }  
    }
   
    return start_flag;  
}
		  			 		  						  					  				 	   		  	  	 	  
//判断开始的是引导码还是连发码
static int irda_mode(void)
{
    int mode_flag = 1;
    
    while(0 == get_gpio_value());//一直等到前面的9ms完
    
    delay_ms(2);//2.5ms 
    delay_us(500);
	
    if(get_gpio_value())
        mode_flag = 1; //是引导码 
    else
        mode_flag = 0; //是连发码
    
    while(1 == get_gpio_value()); //一直待整个判断周期结束
    
    return mode_flag;
}
		  			 		  						  					  				 	   		  	  	 	  
//读1bit
static int read_byte(void)
{
    while(0 == get_gpio_value()); //一直等到高电平，即过掉0.56ms的共同低电平  
    delay_us(800); 
    
    if(get_gpio_value())
    {
		while(1 == get_gpio_value());
        return 1;
    }
    else
	{
		while(1 == get_gpio_value());
        return 0;
	}
}

//接收显示
void value_show(void)
{
    int start_flag, mode_flag, i, j;
    unsigned char data;
    
    set_gpio_value(1);
    
    while(1)
    {
        start_flag = irda_start();     //判断是否有开始信号
        
        if (start_flag)
            mode_flag = irda_mode();   //再判断是何种码

        if (start_flag && mode_flag)
        {
            for(i=0; i<4; i++)         //连续接收四个字节
            {      
                for(j=0; j<8; j++)
                {
                    data >>= 1;   
          
                    if (read_byte())    
                        data |= 0x80;   
                }
                
                result[i] = data;
            } 
            
            printf("addr code: %d \n\r", result[0]);
            printf("addr anti-code: %d \n\r", result[1]);
            printf("data code: %d \n\r", result[2]);
            printf("data anti-code: %d \n\r", result[3]);
            
            printf("\n\r");
        }
        else if ((start_flag && !mode_flag))
        {
            printf("addr code: %d \n\r", result[0]);
            printf("addr anti-code: %d \n\r", result[1]);
            printf("data code: %d \n\r", result[2]);
            printf("data anti-code: %d \n\r", result[3]);
            
            printf("\n\r");
        }
    }
}
