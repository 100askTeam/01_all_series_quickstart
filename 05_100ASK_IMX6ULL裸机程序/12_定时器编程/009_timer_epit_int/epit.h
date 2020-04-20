#ifndef   __EPIT_H__
#define   __EPIT_H__

#include "common.h"
		  			 		  						  					  				 	   		  	  	 	  
void epit_init(EPIT_Type *base, unsigned int us);
void epit_enable_interrupt(EPIT_Type *base, int on);
void epit_run(EPIT_Type *base, int on);

#endif

