#include "gic.h"
#include "my_printf.h"

/* Local irq table and nesting level value */
static sys_irq_handle_t irq_table[160];

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

/* Local IRQ functions */
static void default_irq_handler (uint32_t nr, void *param) {
	printf("irq%d have not set irq handler, use default stead\n", nr);
}

void request_irq(IRQn_Type irq, irq_handler_t handler, void *param)
{
  irq_table[irq].irq_handler = handler;
  irq_table[irq].param = param;
}


void system_init_irq_table(void)
{
	uint32_t i;
	
	/* First set all handler to default */
	for (i = 0; i < (sizeof(irq_table) / sizeof(irq_table[0]) - 1); i++) {
	  request_irq((IRQn_Type)i, default_irq_handler, NULL);
	}
}


void handle_irq_c(void)
{
	int nr;

	GIC_Type *gic = get_gic_base();
	/* The processor reads GICC_IAR to obtain the interrupt ID of the
	 * signaled interrupt. This read acts as an acknowledge for the interrupt
	 */
	nr = gic->C_IAR;
	printf("irq %d is happened\r\n", nr);
	
	irq_table[nr].irq_handler(nr, irq_table[nr].param);

	/* write GICC_EOIR inform the CPU interface that it has completed 
	 * the processing of the specified interrupt 
	 */
	gic->C_EOIR = nr;
}


