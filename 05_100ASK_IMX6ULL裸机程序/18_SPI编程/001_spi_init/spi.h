#ifndef _SPI_H_
#define _SPI_H_


typedef struct 
{
	volatile unsigned int RXDATA;
	volatile unsigned int TXDATA;
	volatile unsigned int CONREG;
	volatile unsigned int CONFIGREG;
	volatile unsigned int INTREG;
	volatile unsigned int DMAREG;
	volatile unsigned int STATREG;
	volatile unsigned int PERIODREG;
	volatile unsigned int TESTREG;   
	volatile unsigned int RESERVED[0x20];
	volatile unsigned int MSGATA;
	
	
}SPI_CTRL;
		  			 		  						  					  				 	   		  	  	 	  

#define ESCPI1_BASE (0x2008000)
#define ESCPI2_BASE (0x200c000)
#define ESCPI3_BASE (0x2010000)
#define ESCPI4_BASE (0x2014000)

		  			 		  						  					  				 	   		  	  	 	  
unsigned char  spi_init(SPI_CTRL * spi_num);
void spi_select(void);
void spi_deselect(void);
unsigned char spi_writeread(SPI_CTRL *spi_num,unsigned char uc_txdata);
unsigned char spi_test_rw(SPI_CTRL *spi_num);
#endif
