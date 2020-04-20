#include "spi.h"
#include "spi_oled.h"

int  main()
{	
	//使用SPI_A接口调试
	unsigned char uc_cnt;
	oled_init(); //初始化TLC5615
	OLED_DIsp_Clear(); //清屏
	//OLED_DIsp_All();

	OLED_DIsp_Test();
	return 0;
}
