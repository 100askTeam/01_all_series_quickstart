#ifndef   __GIC_H__
#define   __GIC_H__

#include "common.h"

void gic_init(void);
void gic_enable_irq(IRQn_Type nr);
void gic_disable_irq(IRQn_Type nr);


typedef void (*irq_handler_t) (uint32_t nr, void *param);

void request_irq(IRQn_Type irq, irq_handler_t handler, void *param);
void system_init_irq_table(void);
		  			 		  						  					  				 	   		  	  	 	  

/**
 * @brief IRQ handle for specific IRQ
 */
typedef struct _sys_irq_handle
{
    irq_handler_t irq_handler; /**< IRQ handler for specific IRQ */
    void *param;                 /**< User param for handler callback */
} sys_irq_handle_t;


#endif

