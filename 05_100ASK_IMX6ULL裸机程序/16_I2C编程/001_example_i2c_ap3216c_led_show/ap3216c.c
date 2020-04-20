#include "ap3216c.h"
#include "i2c.h"
#include "type.h"
static void delay(volatile unsigned int d)
{
	while(d--);
}
static void ap3216c_init_io()
{
	/*I2C1 复用UART4  SCL-TXD SDA-RXD*/
	IOMUXC_SetPinMux(IOMUXC_UART4_TX_DATA_I2C1_SCL, 1);
	IOMUXC_SetPinMux(IOMUXC_UART4_RX_DATA_I2C1_SDA, 1);
	IOMUXC_SetPinConfig(IOMUXC_UART4_TX_DATA_I2C1_SCL, 0x70B0);
	IOMUXC_SetPinConfig(IOMUXC_UART4_RX_DATA_I2C1_SDA, 0X70B0);
}
static uint8_t ap3216c_read_system_status()
{
	unsigned char data = 0;
	data = i2c_read_one_byte(AP3216C_ADDR, AP3216C_SYSTEMCONFIG, I2C1);	        

	if(data != 0 || data != 0xff)
	{
		return 0;	/* AP3216C正常	*/
	}
	else 
	{
		return 1;	/* AP3216C失败	*/
	}
	
}
uint8_t ap3216c_init(void)
{

	int ret = 0;

	/*初始化IO*/
	ap3216c_init_io();
	i2c_init(I2C1); 	

	/*初始化/复位AP3216C		*/
	i2c_write_one_byte(AP3216C_ADDR, AP3216C_SYSTEMCONFIG, AP3216C_INIT, I2C1);
	/*手册上写至少等待10ms*/		
	delay(10000);		
	/*开始转换*/								        
	i2c_write_one_byte(AP3216C_ADDR, AP3216C_SYSTEMCONFIG, AP3216C_START_ALL, I2C1);	

	return ap3216c_read_system_status();
}

void ap3216c_read_ir(uint16_t *ir)
{
    uint8_t ucIrLow;
    uint8_t ucIrHigh;	
    uint8_t i;

	ucIrLow = i2c_read_one_byte(AP3216C_ADDR, AP3216C_IRDATALOW, I2C1);	
	ucIrHigh = i2c_read_one_byte(AP3216C_ADDR, AP3216C_IRDATAHIGH, I2C1);	

    if(ucIrLow & 0x80)      
	{
		*ir = 0;
		return ;			
	}
	else 				  
	{
		*ir = ((uint16_t)ucIrHigh << 2) | (ucIrLow & 0X03); 	
	}
	return;
}

void ap3216c_read_als(uint16_t *als)
{
    uint8_t ucAlsLow;
    uint8_t ucAlsHigh;	
    uint8_t i;

	ucAlsLow = i2c_read_one_byte(AP3216C_ADDR, AP3216C_ALSDATALOW, I2C1);	
	ucAlsHigh = i2c_read_one_byte(AP3216C_ADDR, AP3216C_ALSDATAHIGH, I2C1);	

	*als = ((uint16_t)ucAlsHigh << 8) | ucAlsLow;

	return;
}
void ap3216c_read_ps(uint16_t *ps)
{
    uint8_t ucPsLow;
    uint8_t ucPsHigh;	
    uint8_t i;

	ucPsLow = i2c_read_one_byte(AP3216C_ADDR, AP3216C_PSDATALOW, I2C1);	
	ucPsHigh = i2c_read_one_byte(AP3216C_ADDR, AP3216C_PSDATAHIGH, I2C1);	

    if(ucPsLow & 0x40)      
	{
		*ps = 0;
		return ;			
	}
	else 				   
	{
		*ps = ((uint16_t)(ucPsHigh & 0X3F) << 4) | (ucPsLow & 0x0F); 	
	}
	return;
}


void ap3216c_read_data(uint16_t *ir, uint16_t *ps, uint16_t *als)
{
	ap3216c_read_ir(ir);
	ap3216c_read_als(als);	
	ap3216c_read_ps(ps);
	return;
}
