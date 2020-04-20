#ifndef _ICM20608G_H_
#define _ICM20608G_H_

/*存储的是出厂自测的3轴加速度和3轴角速度传感器数据，用于与用户的自测输出数据比较*/
#define	ICM20608G_SELF_TEST_X_GYRO		0x00
#define	ICM20608G_SELF_TEST_Y_GYRO		0x01
#define	ICM20608G_SELF_TEST_Z_GYRO		0x02
#define	ICM20608G_SELF_TEST_X_ACC		0x0D
#define	ICM20608G_SELF_TEST_Y_ACC		0x0E
#define	ICM20608G_SELF_TEST_Z_ACC		0x0F
		  			 		  						  					  				 	   		  	  	 	  
/* 3轴角速度静态偏移 ，用于矫正直流偏压导致的传感器输出偏差*/
#define	ICM20608G_XGYRO_OFFS_USRH			0x13
#define	ICM20608G_XGYRO_OFFS_USRL			0x14
#define	ICM20608G_YGYRO_OFFS_USRH			0x15
#define	ICM20608G_YGYRO_OFFS_USRL			0x16
#define	ICM20608G_ZGYRO_OFFS_USRH			0x17
#define	ICM20608G_ZGYRO_OFFS_USRL			0x18
		  			 		  						  					  				 	   		  	  	 	  
/*ICM20608G608G的配置，包括采样率、加速度和角速度传感器配置、低功耗模式控制等*/
#define	ICM20608G_SMPLRT_DIV			0x19
#define	ICM20608G_CONFIG				0x1A
#define	ICM20608G_GYRO_CONFIG			0x1B
#define	ICM20608G_ACC_CONFIG			0x1C
#define	ICM20608G_ACC_CONFIG2			0x1D
#define	ICM20608G_LP_MODE_CFG			0x1E
#define	ICM20608G_ACC_WOM_THR			0x1F
#define	ICM20608G_FIFO_EN				0x23
#define	ICM20608G_FSYNC_INT				0x36
#define	ICM20608G_INT_PIN_CFG			0x37
#define	ICM20608G_INT_ENABLE			0x38
#define	ICM20608G_INT_STATUS			0x3A

/*3轴加速度传感器数据输出*/
#define	ICM20608G_ACC_XOUT_H			0x3B
#define	ICM20608G_ACC_XOUT_L			0x3C
#define	ICM20608G_ACC_YOUT_H			0x3D
#define	ICM20608G_ACC_YOUT_L			0x3E
#define	ICM20608G_ACC_ZOUT_H			0x3F
#define	ICM20608G_ACC_ZOUT_L			0x40
		  			 		  						  					  				 	   		  	  	 	  
/* 传感器温度输出 */
#define	ICM20608G_TEMP_OUT_H			0x41
#define	ICM20608G_TEMP_OUT_L			0x42

/* 3轴角速度传感器数据输出 */
#define	ICM20608G_GYRO_XOUT_H			0x43
#define	ICM20608G_GYRO_XOUT_L			0x44
#define	ICM20608G_GYRO_YOUT_H			0x45
#define	ICM20608G_GYRO_YOUT_L			0x46
#define	ICM20608G_GYRO_ZOUT_H			0x47
#define	ICM20608G_GYRO_ZOUT_L			0x48
/*传感器控制设置，包括FIFO的操作模式，电源模式*/
#define	ICM20608G_SIGNAL_PATH_RESET		0x68
#define	ICM20608G_ACC_INTEL_CTRL 		0x69
#define	ICM20608G_USER_CTRL				0x6A
#define	ICM20608G_PWR_MGMT_1			0x6B
#define	ICM20608G_PWR_MGMT_2			0x6C
#define	ICM20608G_FIFO_COUNTH			0x72
#define	ICM20608G_FIFO_COUNTL			0x73
#define	ICM20608G_FIFO_R_W				0x74
/*设备ID ,用于识别设备类型*/
#define	ICM20608G_WHO_AM_I 				0x75

/* 3轴加速度静态偏移 ，用于矫正直流偏压导致的传感器输出偏差*/
#define	ICM20608G_XACC_OFFSET_H			0x77
#define	ICM20608G_XACC_OFFSET_L			0x78
#define	ICM20608G_YACC_OFFSET_H			0x7A
#define	ICM20608G_YACC_OFFSET_L			0x7B
#define	ICM20608G_ZACC_OFFSET_H			0x7D
#define	ICM20608G_ZACC_OFFSET_L 		0x7E

/*采集到的原始数据存储*/
typedef struct 
{
	signed int gyro_x_adc; 
	signed int gyro_y_adc; 
	signed int gyro_z_adc; 
	signed int acc_x_adc; 
	signed int acc_y_adc; 
	signed int acc_z_adc; 
	signed int temp_adc; 
}ICM20608G_GYRO_ACC_adc;

void icm20608g_write_addr(unsigned char uc_addr,unsigned char uc_data);
unsigned char  icm20608g_read_addr(unsigned char uc_addr);
unsigned char  icm20608g_read_len(unsigned char uc_addr,unsigned char *buf,unsigned char uc_len);
unsigned char icm20608g_init(void);
unsigned char  icm20608g_read_acc(void);
unsigned char  icm20608g_read_gyro(void);
unsigned char  icm20608g_read_temp(void);


#endif

