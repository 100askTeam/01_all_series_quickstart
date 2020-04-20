
#include "irda.h"

unsigned char result[4] = { 0x00, 0x00, 0x00, 0x00 };
		  			 		  						  					  				 	   		  	  	 	  
//�������������ƽ
static void set_gpio_value(int value)
{
	/* IRDA GPIO4_IO19 */
	GPIO4->GDIR |= (1<<19);

    if (1 == value)
        GPIO4->DR |= (1<<19);
    else 
        GPIO4->DR &= ~(1<<19);
}

//��ȡ���ŵ�ƽ
static int get_gpio_value(void)
{
	GPIO4->GDIR &= ~(1<<19);
						
	if((GPIO4->DR >> 19 ) & 0x1)
		return 1;
	else
		return 0;								
}


//�����жϿ�ʼ�ź�
static int irda_start(void)
{
    int i;
    int start_flag = 1;
    
    while(get_gpio_value());//һֱ�ȵ��е͵�ƽ
    
    for(i=0; i<9; i++)  //ѭ��ʮ�Σ�Ŀ���Ǽ��ÿ��800΢����(��9ms��)������ָߵ�ƽ���˳��������  
    {  
        delay_us(800);  
        if (get_gpio_value() == 1)  //���ָߵ�ƽ������
        {  
            start_flag = 0;  
            break;  
        }  
    }
   
    return start_flag;  
}
		  			 		  						  					  				 	   		  	  	 	  
//�жϿ�ʼ���������뻹��������
static int irda_mode(void)
{
    int mode_flag = 1;
    
    while(0 == get_gpio_value());//һֱ�ȵ�ǰ���9ms��
    
    delay_ms(2);//2.5ms 
    delay_us(500);
	
    if(get_gpio_value())
        mode_flag = 1; //�������� 
    else
        mode_flag = 0; //��������
    
    while(1 == get_gpio_value()); //һֱ�������ж����ڽ���
    
    return mode_flag;
}
		  			 		  						  					  				 	   		  	  	 	  
//��1bit
static int read_byte(void)
{
    while(0 == get_gpio_value()); //һֱ�ȵ��ߵ�ƽ��������0.56ms�Ĺ�ͬ�͵�ƽ  
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

//������ʾ
void value_show(void)
{
    int start_flag, mode_flag, i, j;
    unsigned char data;
    
    set_gpio_value(1);
    
    while(1)
    {
        start_flag = irda_start();     //�ж��Ƿ��п�ʼ�ź�
        
        if (start_flag)
            mode_flag = irda_mode();   //���ж��Ǻ�����

        if (start_flag && mode_flag)
        {
            for(i=0; i<4; i++)         //���������ĸ��ֽ�
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
