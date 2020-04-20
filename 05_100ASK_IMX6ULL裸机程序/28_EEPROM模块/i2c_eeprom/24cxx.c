#include "i2c.h"
#include "24cxx.h"
		  			 		  						  					  				 	   		  	  	 	  

/**********************************************************************
 * 函数名称： AT24CXX_Init
 * 功能描述： 初始化AT24CXX、I2C
 * 输入参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/16	     V1.0	  jackson	      创建
 ***********************************************************************/
void AT24CXX_Init(void)
{
	IOMUXC_SetPinMux(IOMUXC_UART4_TX_DATA_I2C1_SCL, 0x01);
	IOMUXC_SetPinMux(IOMUXC_UART4_RX_DATA_I2C1_SDA, 0x01);
	IOMUXC_SetPinConfig(IOMUXC_UART4_TX_DATA_I2C1_SCL, 0x70B0);
	IOMUXC_SetPinConfig(IOMUXC_UART4_RX_DATA_I2C1_SDA, 0X70B0);

	/* 配置I2C */
	i2c_init(I2C1);
}

/**********************************************************************
 * 函数名称： AT24CXX_Check
 * 功能描述： 检查AT24CXX是否正常
 * 输入参数： 无
 * 返 回 值： 是否成功
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/16	     V1.0	  jackson	      创建
 ***********************************************************************/
unsigned char AT24CXX_Check(void)
{
	unsigned char temp;
	temp = i2c_read_one_byte(AT24CXX_ADDR, 255, I2C1);	/* 避免每次开机都写AT24CXX */
	if(temp == 0X11)return 0;		   
	else {
		i2c_write_one_byte(AT24CXX_ADDR, 255, 0X11, I2C1);
	    temp = i2c_read_one_byte(AT24CXX_ADDR, 255, I2C1);	  
		if(temp == 0X11)return 0;
	}
	return 1;											  
}

/**********************************************************************
 * 函数名称： AT24CXX_Read
 * 功能描述： 在AT24CXX的指定地址开始读出指定个数的数据
 * 输入参数： 寄存器地址、读出数据、要读出数据的个数
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/16	     V1.0	  jackson	      创建
 ***********************************************************************/
void AT24CXX_Read(unsigned int addr, unsigned char *arr, unsigned int num)
{
	while(num--) {
		*arr++ = i2c_read_one_byte(AT24CXX_ADDR, addr++, I2C1);
		gpt2_chan1_delay_us(5000);
	}
}

/**********************************************************************
 * 函数名称： AT24CXX_Write
 * 功能描述： 在AT24CXX的指定地址开始写入指定个数的数据
 * 输入参数： 寄存器地址、写入数据、要写入数据的个数
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/16	     V1.0	  jackson	      创建
 ***********************************************************************/
void AT24CXX_Write(unsigned int addr,unsigned char *arr,unsigned int num)
{
	while(num--) {
		i2c_write_one_byte(AT24CXX_ADDR, addr++, *arr, I2C1);
		gpt2_chan1_delay_us(10000);
		arr++;
	}
}
		  			 		  						  					  				 	   		  	  	 	  

