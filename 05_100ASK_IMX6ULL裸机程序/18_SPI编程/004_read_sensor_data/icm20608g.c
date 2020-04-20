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
	icm20608g_write_addr(ICM20608G_SMPLRT_DIV,0x00);//采样率默认1K
	icm20608g_write_addr(ICM20608G_CONFIG, 0x00);//禁止FIFO
	icm20608g_write_addr(ICM20608G_GYRO_CONFIG,0x00);//使用默认量程和低通滤波器
	icm20608g_write_addr(ICM20608G_ACC_CONFIG,0x00);//使用默认量程
	icm20608g_write_addr(ICM20608G_ACC_CONFIG2,0x00);//使用默认低通滤波器
	icm20608g_write_addr(ICM20608G_LP_MODE_CFG,0x00);//关闭低功耗模式
	icm20608g_write_addr(ICM20608G_FIFO_EN,0x00);//禁止传感器FIFO
	icm20608g_write_addr(ICM20608G_PWR_MGMT_2,0x00);//使能传感器
	printf("icm20608g 初始化结束\n\r");
	return 0;
}
		  			 		  						  					  				 	   		  	  	 	  
/**********************************************************************
	 * 函数名称： icm20608g_read_len
	 * 功能描述： icm20608G从特定地址读取一定长度的数据，然后保存到指定地址
	 * 输入参数：@uc_addr：要读取的地址
	 			@buf :读取数据的缓存地址
	 			@uc_len：要读取数据的长度
	 * 输出参数： 读取结果
	 * 返 回 值： 成功则返回0
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/04		 V1.0	  芯晓		  创建
 ***********************************************************************/
unsigned char  icm20608g_read_len(unsigned char uc_addr,unsigned char *buf,unsigned char uc_len)
{
	unsigned char uc_cnt;
	uc_addr |= 0x80;/*地址最高位为1表示读取*/
	spi_select();
	spi_writeread(ESCPI3_BASE,uc_addr);
	for(uc_cnt=0;uc_cnt<uc_len;uc_cnt++)
	{
		buf[uc_cnt]=spi_writeread(ESCPI3_BASE,0xff);
	}
	spi_deselect();
	return 0;
}
/**********************************************************************
	 * 函数名称： print_x
	 * 功能描述： 将一定数量的数据通过串口进行打印
	 * 输入参数：@uc_buf：打印数据的缓存地址
	 			@uc_len :要打印数据的长度
	 * 输出参数： 无
	 * 返 回 值： 
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/04		 V1.0	  芯晓		  创建
 ***********************************************************************/

void print_x(unsigned char *uc_buf,unsigned char uc_len)
{
	unsigned char uc_cnt;
	for(uc_cnt=0;uc_cnt<uc_len;uc_cnt++)
	{
		printf("read %d : %x \n\r",uc_cnt,uc_buf[uc_cnt]);
	}	
}
/**********************************************************************
	 * 函数名称： icm20608g_read_acc
	 * 功能描述： 读取加速度原始数据信息
	 * 输入参数：无
	 * 输出参数： 读取结果
	 * 返 回 值： 成功则返回0 
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/04		 V1.0	  芯晓		  创建
 ***********************************************************************/
unsigned char  icm20608g_read_acc(void)
{
	unsigned char uc_buf[6];
	icm20608g_read_len(0x3b,uc_buf,6);
	icm20608g_get.acc_x_adc = (signed short)((uc_buf[0]<<8)|uc_buf[1]);
	icm20608g_get.acc_y_adc = (signed short)((uc_buf[2]<<8)|uc_buf[3]);
	icm20608g_get.acc_z_adc = (signed short)((uc_buf[4]<<8)|uc_buf[5]);
	printf("@@加速度icm20608g_read_acc \n\r");
	print_x(uc_buf,6);
	return 0;
}
/**********************************************************************
	 * 函数名称： icm20608g_read_gyro
	 * 功能描述： 读取角速度原始数据信息
	 * 输入参数：无
	 * 输出参数： 读取结果
	 * 返 回 值： 成功则返回0 
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/04		 V1.0	  芯晓		  创建
 ***********************************************************************/
unsigned char  icm20608g_read_gyro(void)
{
	unsigned char uc_buf[6];
	icm20608g_read_len(0x43,uc_buf,6);
	icm20608g_get.gyro_x_adc = (signed short)((uc_buf[0]<<8)|uc_buf[1]);
	icm20608g_get.gyro_y_adc = (signed short)((uc_buf[2]<<8)|uc_buf[3]);
	icm20608g_get.gyro_z_adc = (signed short)((uc_buf[4]<<8)|uc_buf[5]);
	printf("###角速度icm20608g_read_gyro \n\r");
	print_x(uc_buf,6);
	return 0;
}
		  			 		  						  					  				 	   		  	  	 	  
/**********************************************************************
	 * 函数名称： icm20608g_read_temp
	 * 功能描述： 读取温度原始数据信息
	 * 输入参数：无
	 * 输出参数： 读取结果
	 * 返 回 值： 成功则返回0 
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/04		 V1.0	  芯晓		  创建
 ***********************************************************************/
unsigned char  icm20608g_read_temp(void)
{
	unsigned char uc_buf[2];
	icm20608g_read_len(0x41,uc_buf,2);
	icm20608g_get.temp_adc = (signed short)((uc_buf[0]<<8)|uc_buf[1]);
	printf("$$$温度icm20608g_read_temp \n\r");
	print_x(uc_buf,2);
	return 0;
}




