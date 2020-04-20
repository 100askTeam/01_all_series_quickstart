#include "spi.h"
#include "icm20608g.h"
#include "my_printf.h"
static ICM20608G_GYRO_ACC_adc icm20608g_get;
/**********************************************************************
	 * 函数名称： icm20608g_write_addr
	 * 功能描述： icm20608G向特定地址写入数据
	 * 输入参数：@uc_addr :要写入的地址
	 			@uc_data:要写入的数据
	 * 输出参数：无
	 * 返 回 值： 无
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/04		 V1.0	  芯晓		  创建
 ***********************************************************************/
void icm20608g_write_addr(unsigned char uc_addr,unsigned char uc_data)
{
unsigned char uc_read=0;
	uc_addr &= ~0x80;/*地址最高位为0表示写入*/
	spi_select();
	spi_writeread(ESCPI3_BASE,uc_addr);
	spi_writeread(ESCPI3_BASE,uc_data);
	spi_deselect();
}
/**********************************************************************
	 * 函数名称： icm20608g_read_addr
	 * 功能描述： icm20608G从特定地址读出数据
	 * 输入参数：@uc_addr :要读取的地址
	 * 输出参数：读出的数据
	 * 返 回 值：读出的数据
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/04		 V1.0	  芯晓		  创建
 ***********************************************************************/
unsigned char  icm20608g_read_addr(unsigned char uc_addr)
{
	unsigned char uc_read = 0;
	uc_addr |= 0x80;/*地址最高位为1表示读取*/	
	spi_select();
	spi_writeread(ESCPI3_BASE,uc_addr);
	uc_read=spi_writeread(ESCPI3_BASE,0xff);
	spi_deselect();
	return uc_read;
}
		  			 		  						  					  				 	   		  	  	 	  
/**********************************************************************
	 * 函数名称： icm20608g_init
	 * 功能描述： icm20608G的初始化
	 * 输入参数：无
	 * 输出参数： 初始化的结果
	 * 返 回 值： 成功则返回0，否则返回-1
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/04		 V1.0	  芯晓		  创建
 ***********************************************************************/
unsigned char icm20608g_init(void)
{
	unsigned char uc_dev_id = 0;
	spi_init(ESCPI3_BASE);
	printf("icm20608g 初始化开始\n\r");
//	icm20608_write_addr(ICM20608G_PWR_MGMT_1,0x80);//设备复位
	icm20608g_write_addr(ICM20608G_PWR_MGMT_1,0x01);//设备退出复位
	//读取设备id并对比，如果不等于0xaf，则退出初始化
	uc_dev_id = icm20608g_read_addr(ICM20608G_WHO_AM_I);
	printf("read icm20608g id is 0x%x\n\r",uc_dev_id);
	if(uc_dev_id!=0xAF)
	{
		printf("read id fail\n\r");
		return -1;
	}
	return 0;
}


