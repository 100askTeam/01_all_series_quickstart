
#include "i2c.h"
void i2c_init(I2C_REGISTERS *I2C_BASE)
{
    /*I2C_I2CR是控制寄存器,
     * 可以: 使能I2C,使能中断, 选择主从模式.
     */

    /* 配置I2C控制器步骤: 关闭I2C,配置,打开I2C */

    /* 设置SCL时钟为100K
     * I2C的时钟源来源于IPG_CLK_ROOT=49.5Mhz
	 *	PLL2 = 528 MHz
	 *	PLL2_PFD2 = 528 *18 /24 = 396 MHz
	 *	IPG_CLK_ROOT = (PLL2_PFD2 / ahb_podf )/ ipg_podf = (396 MHz/4)/2 = 49.5Mhz
	 *	
	 *	PER_CLK_ROOT = IPG_CLK_ROOT/perclk_podf = 49.5 MHz/1 = 49.5 MHz
	 * 设置I2C的波特率为100K， 因此当分频值=49500000/100000=495	
	 * 参考Table 31-3. I2C_IFDR Register Field Values 表中0x37对应的512最接近
	 * 即寄存器IFDR的IC位设置为0X37
	 */	 
	I2C_BASE->I2CR &= ~(1 << 7);
	I2C_BASE->IFDR = 0x37;
	I2C_BASE->I2CR |= (1<<7);
}

uint8_t i2c_check(I2C_REGISTERS *I2C_BASE, uint32_t status)
{
	/* 检查是否发生仲裁丢失错误(arbitration lost) */
	if(status & (1<<4))
	{
		I2C_BASE->I2SR &= ~(1<<4);	/* 清除仲裁丢失错误位 			*/

		I2C_BASE->I2CR &= ~(1 << 7);	/* 复位I2C: 先关闭I2C 				*/
		I2C_BASE->I2CR |= (1 << 7);	/* 再打开I2C 				*/
		return I2C_ARBITRATIONLOST;
	} 
	else if(status & (1 << 0))     	/* 检查NAK */
	{
		return I2C_NAK;		/* 返回NAK(无应答) */
	}
	return I2C_OK;

}

uint8_t i2c_start(I2C_REGISTERS *I2C_BASE, uint8_t ucSlaveAddr, uint32_t ulOpcode)
{

	if(I2C_BASE->I2SR & (1 << 5))			/* I2C忙 */
		return 1;

	/*
         * 设置控制寄存器I2CR
         * bit[5]: 1 主模式(master)
         * bit[4]: 1 发送(transmit)
	 */
	I2C_BASE->I2CR |=  (1 << 5) | (1 << 4);

	/*
         * 设置数据寄存器I2DR
         * bit[7:0] : 要发送的数据, 
         * START信号后第一个数据是从设备地址
	 */ 
	I2C_BASE->I2DR = ((uint32_t)ucSlaveAddr << 1) | ((I2C_READ == ulOpcode)? 1 : 0);
	return 0;

}
uint8_t i2c_stop(I2C_REGISTERS *I2C_BASE)
{

	uint16_t usTimeout = 0xffff;

	/*
	 * 清除控制寄存器I2CR[5:3]
         * 发出STOP信号
	 */
	I2C_BASE->I2CR &= ~((1 << 5) | (1 << 4) | (1 << 3));

	/* 等待STOP信号确实发出去了 */
	while((I2C_BASE->I2SR & (1 << 5)))
	{
		usTimeout--;
		if(usTimeout == 0)	/* 超时跳出 */
			return I2C_TIMEOUT;
	}
	return I2C_OK;

}

uint8_t i2c_restart(I2C_REGISTERS *I2C_BASE, uint8_t ucSlaveAddr, uint32_t ulOpcode)
{

	/* I2C忙并且工作在从模式,跳出 */
	if(I2C_BASE->I2SR & (1 << 5) && (((I2C_BASE->I2CR) & (1 << 5)) == 0))		
		return 6;

	/*
         * 设置控制寄存器I2CR
         * bit[4]: 1 发送(transmit)
         * bit[2]: 1 产生重新开始信号(Repeat start)
	 */
	I2C_BASE->I2CR |=  (1 << 4) | (1 << 2);

	/*
         * 设置数据寄存器I2DR
         * bit[7:0] : 要发送的数据, 
         * START信号后第一个数据是从设备地址
	 */ 
	I2C_BASE->I2DR = ((uint32_t)ucSlaveAddr << 1) | ((I2C_READ == ulOpcode)? 1 : 0);
	
	return 0;

}


void i2c_write(I2C_REGISTERS *I2C_BASE, const uint8_t *pbuf, uint32_t len)
{
	/* 等待数据寄存器就绪,可以再次发送数据 */
	while(!(I2C_BASE->I2SR & (1 << 7))); 
	
	I2C_BASE->I2SR &= ~(1 << 1); 	  /* 清除IICIF */
	I2C_BASE->I2CR |= 1 << 4;	      /* 发送数据(transmit) */
	while(len--)
	{
		I2C_BASE->I2DR = *pbuf++; 	    /* 将buf中的数据写入到数据寄存器I2DR */
		
		while(!(I2C_BASE->I2SR & (1 << 1)));  /* 等待传输完成,完成或失败,中断状态位被置1 */	
		I2C_BASE->I2SR &= ~(1 << 1);			/* 清除中断状态位 */

		/* 检查有无错误 */
		if(i2c_check(I2C_BASE, I2C_BASE->I2SR))
			break;
	}
	
	I2C_BASE->I2SR &= ~(1 << 1);     /* 清除中断状态位 */
	i2c_stop(I2C_BASE); 	         /* 发送停止信号 */

}

void i2c_read(I2C_REGISTERS *I2C_BASE, uint8_t *pbuf, uint32_t len)
{
	volatile uint8_t dummy = 0;
	dummy++; 	/* 防止编译警告 */

	/* 等待数据寄存器就绪 */
	while(!(I2C_BASE->I2SR & (1 << 7))); 
	
	I2C_BASE->I2SR &= ~(1 << 1); 			   /* 清除IICIF */
	I2C_BASE->I2CR &= ~((1 << 4) | (1 << 3));	/* 接收数据: Receive,TXAK */
	
	/* 如果只接收一个字节数据的话发送NACK信号 */
	if(len == 1)
        I2C_BASE->I2CR |= (1 << 3);

	dummy = I2C_BASE->I2DR; /* 假读 */


	while(len--)
	{
		while(!(I2C_BASE->I2SR & (1 << 1))); 	/* 等待传输完成 */	
		I2C_BASE->I2SR &= ~(1 << 1);			/* 清除标志位 */

	 	if(len == 0)
        {
        	i2c_stop(I2C_BASE); 			/* 发送停止信号 */
        }

        if(len == 1)
        {
            I2C_BASE->I2CR |= (1 << 3);
        }
		*pbuf++ = I2C_BASE->I2DR;
	}

}

uint8_t i2c_transfer(I2C_REGISTERS *I2C_BASE, I2C_TRANSFER *transfer)
{
	uint32_t ulRet = 0;
	uint32_t ulOpcode = transfer->ulOpcode;

	/*开始前准备工作，清除标志位
	 *bit-4 IAL 仲裁位，bit-1 IIF 中断标志位
	 */
	I2C_BASE->I2SR &= ~((1 << 1) | (1 << 4));
	/* 等待传输完成 */
	while(!((I2C_BASE->I2SR >> 7) & 0X1)){}; 

	/* 如果要读某个寄存区,寄存器地址要先"写"给从设备
	 * 所以方向要"先写","后读"
	 */
    if ((transfer->ulSubAddressLen > 0) && (transfer->ulOpcode == I2C_READ))
    {
        ulOpcode = I2C_WRITE;
    }
	ulRet = i2c_start(I2C_BASE, transfer->ucSlaveAddress, ulOpcode);

	if (ulRet)
	{
		return ulRet;
	}
	
	/* 等待传输完成: 中断状态为会被置1 */
	while(!(I2C_BASE->I2SR & (1 << 1))){};

	/* 检查是否出错 */
	ulRet = i2c_check(I2C_BASE, I2C_BASE->I2SR);

	if (ulRet)
	{
	    i2c_stop(I2C_BASE); 			/* 发送停止信号 */
		return ulRet;
	}

	/*如果ulSubAddressLen不为0，表示要发送寄存器地址*/
	if (transfer->ulSubAddressLen)
	{
		do
		{
			/* 清除中断状态位 */
		    I2C_BASE->I2SR &= ~(1 << 1); 
			
			/* 调整长度, 也许寄存器地址有多个字节, 本程序最多支持4字节 */
			transfer->ulSubAddressLen--;

			I2C_BASE->I2DR = ((transfer->ulSubAddress) >> (8 * transfer->ulSubAddressLen)); 
  
			while(!(I2C_BASE->I2SR & (1 << 1)));  	/* 等待传输完成: 中断状态位被置1 */

            /* 检查是否出错 */
            ulRet = i2c_check(I2C_BASE, I2C_BASE->I2SR);
            if(ulRet)
            {
             	i2c_stop(I2C_BASE); 				/* 出错:发送停止信号 */
             	return ulRet;
            }
		}
		while ((transfer->ulSubAddressLen > 0) && (ulRet == I2C_OK));

		if (I2C_READ == transfer->ulOpcode)
		{
            I2C_BASE->I2SR &= ~(1 << 1);			/* 清除中断状态位 */
            i2c_restart(I2C_BASE, transfer->ucSlaveAddress, I2C_READ); /* 发送重复开始信号和从机地址 */
    		while(!(I2C_BASE->I2SR & (1 << 1))){}; /* 等待传输完成: 中断状态位被置1 */

            /* 检查是否出错 */
			ulRet = i2c_check(I2C_BASE, I2C_BASE->I2SR);
			
            if(ulRet)
            {
             	ulRet = I2C_ADDRNAK;
                i2c_stop(I2C_BASE); 		/* 出错:发送停止信号 */
                return ulRet;  
            }
           	       

		}
		
	}
    /* 发送数据 */
    if ((I2C_WRITE == transfer->ulOpcode) && (transfer->ulLenth > 0))
    {
    	i2c_write(I2C_BASE, transfer->pbuf, transfer->ulLenth);
	}

    /* 读取数据 */
    if ((I2C_READ == transfer->ulOpcode) && (transfer->ulLenth > 0))
    {
       	i2c_read(I2C_BASE, transfer->pbuf, transfer->ulLenth);
	}
	return 0;	

}
uint8_t i2c_write_one_byte(uint8_t addr,uint8_t reg, uint8_t data,I2C_REGISTERS *I2C_BASE)
{
    uint8_t status = 0;
    uint8_t writedata=data;
    I2C_TRANSFER transfer;
	
    /* 配置I2C xfer结构体 */
   	transfer.ucSlaveAddress = addr; 			/* 备地址 				*/
    transfer.ulOpcode = I2C_WRITE;			    /* 数据方向:写 			*/
    transfer.ulSubAddress = reg;				/* 发出设备地址后马上发寄存器地址 			*/
    transfer.ulSubAddressLen = 1;				/* 地址长度一个字节 			*/
    transfer.pbuf = &writedata;				    /* 要发出的数据 				*/
    transfer.ulLenth = 1;  					    /* 数据长度1个字节			*/

    status = i2c_transfer(I2C_BASE, &transfer);
    return status;
}

uint8_t i2c_read_one_byte(uint8_t addr, uint8_t reg,I2C_REGISTERS *I2C_BASE)
{
	uint8_t val=0;
    uint8_t status = 0;	
    I2C_TRANSFER transfer;

	transfer.ucSlaveAddress = addr;				/* 设备地址 				*/
    transfer.ulOpcode = I2C_READ;			    /* 数据方向:读 				*/
    transfer.ulSubAddress = reg;				/* 发出设备地址后马上发寄存器地址,
                                                 * 这是一个写操作 			
                                                 * 之后会再次发出设备地址,读数据
                                                 */
    transfer.ulSubAddressLen = 1;				/* 地址长度一个字节 			*/
    transfer.pbuf = &val;						/* 接收数据缓冲区 				*/
    transfer.ulLenth = 1;					    /* 要读取的数据长度:1			*/

    status = i2c_transfer(I2C_BASE, &transfer);
	return val;
}


