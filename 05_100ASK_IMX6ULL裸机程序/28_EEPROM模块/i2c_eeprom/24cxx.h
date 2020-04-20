#ifndef _24CXX_H
#define _24CXX_H

//#include "common.h"
#include "timer.h"

#define AT24CXX_ADDR    	0X50	/* AT24CXX 器件地址 */
		  			 		  						  					  				 	   		  	  	 	  
#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  

void AT24CXX_Init(void);
unsigned char AT24CXX_WriteOneByte(unsigned char addr,unsigned char reg, unsigned char data);
unsigned char AT24CXX_ReadOneByte(unsigned char addr,unsigned char reg);
unsigned char AT24CXX_Check(void);
void AT24CXX_Read(unsigned int ReadAddr, unsigned char *arr, unsigned int num);
void AT24CXX_Write(unsigned int WriteAddr,unsigned char *arr,unsigned int num);

#endif
