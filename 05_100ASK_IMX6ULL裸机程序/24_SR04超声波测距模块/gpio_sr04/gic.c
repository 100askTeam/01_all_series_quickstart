#include "gic.h"
#include "my_printf.h"

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

static unsigned long int sr04_echo_start = 0;
static unsigned long int sr04_echo_stop = 0;

void handle_irq_c(void)
{
	int nr;

	GIC_Type *gic = get_gic_base();
	/* The processor reads GICC_IAR to obtain the interrupt ID of the
	 * signaled interrupt. This read acts as an acknowledge for the interrupt
	 */
	nr = gic->C_IAR;
	//printf("irq %d is happened\r\n", nr);

	switch(nr)
	{		
		case GPIO4_Combined_16_31_IRQn:
		{
			/* read GPIO4_DR to get GPIO4_IO020 status*/
			if((GPIO4->DR >> 20 ) & 0x1)
			{
				gpt2_start();
				sr04_echo_start = GPT2->CNT;
			}
			else
			{
				sr04_echo_stop = GPT2->CNT;
				gpt2_stop();
				
				gic_disable_irq(GPIO4_Combined_16_31_IRQn);
				
				int distance = (sr04_echo_stop-sr04_echo_start)*17/66/100;
				printf("distance=%d mm\n\r", distance);
				
			}
			/* write 1 to clear GPIO4_IO020 interrput status*/
			GPIO4->ISR |= (1 << 20);
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


