#include "gic.h"
#include "my_printf.h"

unsigned char write_state = 0;
unsigned char read_state = 0;

GIC_Type * get_gic_base(void)
{
	GIC_Type *dst;

	__asm volatile ("mrc p15, 4, %0, c15, c0, 0" : "=r" (dst)); 

	return dst;
}

void gic_init(void)
{
	u32 i, irq_num;

	GIC_Type *gic = get_gic_base();

	/* the maximum number of interrupt IDs that the GIC supports */
	irq_num = (gic->D_TYPER & 0x1F) + 1;

	/* On POR, all SPI is in group 0, level-sensitive and using 1-N model */
	
	/* Disable all PPI, SGI and SPI */
	for (i = 0; i < irq_num; i++)
	  gic->D_ICENABLER[i] = 0xFFFFFFFFUL;

	/* The priority mask level for the CPU interface. If the priority of an 
	 * interrupt is higher than the value indicated by this field, 
	 * the interface signals the interrupt to the processor.
	 */
	gic->C_PMR = (0xFFUL << (8 - 5)) & 0xFFUL;
	
	/* No subpriority, all priority level allows preemption */
	gic->C_BPR = 7 - 5;
	
	/* Enables the forwarding of pending interrupts from the Distributor to the CPU interfaces.
	 * Enable group0 distribution
	 */
	gic->D_CTLR = 1UL;
	
	/* Enables the signaling of interrupts by the CPU interface to the connected processor
	 * Enable group0 signaling 
	 */
	gic->C_CTLR = 1UL;
}

void gic_enable_irq(IRQn_Type nr)
{
	GIC_Type *gic = get_gic_base();

	/* The GICD_ISENABLERs provide a Set-enable bit for each interrupt supported by the GIC.
	 * Writing 1 to a Set-enable bit enables forwarding of the corresponding interrupt from the
	 * Distributor to the CPU interfaces. Reading a bit identifies whether the interrupt is enabled.
	 */
	gic->D_ISENABLER[nr >> 5] = (uint32_t)(1UL << (nr & 0x1FUL));

}

void gic_disable_irq(IRQn_Type nr)
{
	GIC_Type *gic = get_gic_base();

	/* The GICD_ICENABLERs provide a Clear-enable bit for each interrupt supported by the
	 * GIC. Writing 1 to a Clear-enable bit disables forwarding of the corresponding interrupt from
     * the Distributor to the CPU interfaces. Reading a bit identifies whether the interrupt is enabled. 
	 */
	gic->D_ICENABLER[nr >> 5] = (uint32_t)(1UL << (nr & 0x1FUL));
}

void handle_irq_c(void)
{
	int nr;
	//static int val;

	GIC_Type *gic = get_gic_base();
	/* The processor reads GICC_IAR to obtain the interrupt ID of the
	 * signaled interrupt. This read acts as an acknowledge for the interrupt
	 */
	nr = gic->C_IAR;
	// printf("irq %d is happened\r\n", nr);

	switch(nr)
	{
		case GPIO5_Combined_0_15_IRQn:
		{
			/* read GPIO5_DR to get GPIO5_IO01 status*/
			if((GPIO5->DR >> 1 ) & 0x1) {
    			write_state = 1;
				printf("key 1 is release\r\n");
			} else {
				printf("key 1 is press\r\n");
			}
			/* write 1 to clear GPIO5_IO03 interrput status*/
			GPIO5->ISR |= (1 << 1);
			break;
		}
		
		case GPIO4_Combined_0_15_IRQn:
		{
			/* read GPIO4_DR to get GPIO4_IO014 status*/
			if((GPIO4->DR >> 14 ) & 0x1){
    			read_state = 1;
				printf("key 2 is release\r\n");
			}
			else
				printf("key 2 is press\r\n");
			/* write 1 to clear GPIO4_IO014 interrput status*/
			GPIO4->ISR |= (1 << 14);
			break;
		}

		case UART6_IRQn:
		{
			while(((UART6->USR2) & 0X01) == 0);
			char num = UART6->URXD;
			// printf("UART6->URXD		%d \r\n", num);

			if((GPS_RX_STA & (1<<15)) == 0) { 
				if(GPS_RX_STA < 1000)
				{
					EPIT1->CMPR	= 0;
					if(GPS_RX_STA == 0)
					{
						EPIT1->CR |= 1<<0;	/* 使能EPIT1 */ 
					}
					GPS_RX_BUF[GPS_RX_STA++] = num;
				} else {
					GPS_RX_STA |= 1<<15;
				}
			}
    		UART6->USR1 |= (1 << 9);
			break;
		}

		case UART1_IRQn:
		{
			while(((UART1->USR2) & 0X01) == 0);
			// char num = UART1->URXD;
			// printf("UART1->URXD		%d \r\n", num);

    		UART1->USR1 |= (1 << 9);
			break;
		}
		
		case EPIT1_IRQn:
		{
			if(EPIT1->SR & (1<<0)) 			/* 判断比较事件发生 */
			{
				GPS_RX_STA |= 1<<15;
				EPIT1->CMPR	= 0;
				EPIT1->CR &= ~(1<<0);	/* 关闭EPIT1 */ 
			}
			EPIT1->SR |= 1<<0; 	
			break;
		}

		default:
			break;
	}

	/* write GICC_EOIR inform the CPU interface that it has completed 
	 * the processing of the specified interrupt 
	 */
	gic->C_EOIR = nr;
}



