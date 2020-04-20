#ifndef _OLED_H_
#define _OLED_H_

void oled_write_cmd_data(unsigned char uc_data,unsigned char uc_cmd);
int oled_init(void);
int oled_fill_data(unsigned char fill_Data);
void OLED_DIsp_Clear(void);
void OLED_DIsp_All(void);
void OLED_DIsp_Set_Pos(int x, int y);
void OLED_DIsp_Char(int x, int y, unsigned char c);
void OLED_DIsp_String(int x, int y, char *str);
void OLED_DIsp_CHinese(unsigned char x,unsigned char y,unsigned char no);
void OLED_DIsp_Test();

		  			 		  						  					  				 	   		  	  	 	  
#endif

