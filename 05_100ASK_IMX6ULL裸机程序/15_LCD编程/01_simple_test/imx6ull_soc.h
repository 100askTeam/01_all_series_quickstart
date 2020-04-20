#ifndef    _IMX6ULL_SOC_H_
#define	   _IMX6ULL_SOC_H_

/*LCD控制器基地址*/
#define LCD_CONTROLLER_BASE 		(0x21C8000u) 

/*时钟相关基地址*/
#define CCM_BASE                    (0x20C4000u)
#define CCM_ANALOG_BASE             (0x20C8000u)

/*LCD管脚复用相关基地址*/
#define SW_MUX_PART_LCD_BASE		(0x20E0104u)

/*LCD管脚硬件属性相关基地址*/
#define SW_PAD_PART_LCD_BASE		(0x20E0390u)
		  			 		  						  					  				 	   		  	  	 	  
#define SW_MUX			  (SW_MUX_CTL_Type*)SW_MUX_PART_LCD_BASE
#define SW_PAD            (SW_PAD_CTL_Type*)SW_PAD_PART_LCD_BASE
#define CCM               ((CCM_Type *)CCM_BASE)
#define CCM_ANALOG        ((CCM_ANALOG_Type *)CCM_ANALOG_BASE)
#define LCDIF             ((LCDIF_Type *)LCD_CONTROLLER_BASE)

typedef struct 	{
  volatile unsigned int IOMUXC_SW_MUX_CTL_PAD_LCD_CLK;
  volatile unsigned int IOMUXC_SW_MUX_CTL_PAD_LCD_ENABLE;
  volatile unsigned int IOMUXC_SW_MUX_CTL_PAD_LCD_HSYNC;
  volatile unsigned int IOMUXC_SW_MUX_CTL_PAD_LCD_VSYNC;
  volatile unsigned int IOMUXC_SW_MUX_CTL_PAD_LCD_RESET;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA00;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA01;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA02;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA03;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA04;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA05;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA06;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA07;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA08;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA09;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA10;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA11;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA12;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA13;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA14;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA15;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA16;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA17;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA18;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA19;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA20;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA21;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA22;
  volatile unsigned int SW_MUX_CTL_PAD_LCD_DATA23;
}SW_MUX_CTL_Type;

typedef struct   {
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_CLK;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_ENABLE;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_HSYNC;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_VSYNC;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_RESET;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA00;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA01;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA02;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA03;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA04;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA05;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA06;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA07;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA08;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA09;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA10;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA11;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA12;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA13;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA14;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA15;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA16;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA17;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA18;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA19;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA20;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA21;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA22;
  volatile unsigned int IOMUXC_SW_PAD_CTL_PAD_LCD_DATA23;
}SW_PAD_CTL_Type;


/*根据IMX6ULL芯片手册<<18.6 CCM Memory Map/Register Definition>>658页，定义CCM Memory的结构体,*/
typedef struct {
  volatile unsigned int CCR;                              
  volatile unsigned int CCDR;                            
  volatile  unsigned int CSR;                            
  volatile unsigned int CCSR;                           
  volatile unsigned int CACRR;                             
  volatile unsigned int CBCDR;                            
  volatile unsigned int CBCMR;                            
  volatile unsigned int CSCMR1;                            
  volatile unsigned int CSCMR2;                            
  volatile unsigned int CSCDR1;                            
  volatile unsigned int CS1CDR;                            
  volatile unsigned int CS2CDR;                            
  volatile unsigned int CDCDR;                             
  volatile unsigned int CHSCCDR;                          
  volatile unsigned int CSCDR2;                            
  volatile unsigned int CSCDR3;                            
       unsigned char RESERVED_0[8];
  volatile  unsigned int CDHIPR;                          
       unsigned char RESERVED_1[8];
  volatile unsigned int CLPCR;                             
  volatile unsigned int CISR;                            
  volatile unsigned int CIMR;                             
  volatile unsigned int CCOSR;                            
  volatile unsigned int CGPR;                             
  volatile unsigned int CCGR0;                           
  volatile unsigned int CCGR1;                            
  volatile unsigned int CCGR2;                           
  volatile unsigned int CCGR3;                            
  volatile unsigned int CCGR4;                            
  volatile unsigned int CCGR5;                            
  volatile unsigned int CCGR6;                             
       unsigned char RESERVED_2[4];
  volatile unsigned int CMEOR;                            
} CCM_Type;

/*根据IMX6ULL芯片手册<<34.6 eLCDIF Memory Map/Register Definition>>2165页，定义eLCDIF的结构体,*/
typedef struct {
  volatile unsigned int CTRL;                              
  volatile unsigned int CTRL_SET;                        
  volatile unsigned int CTRL_CLR;                         
  volatile unsigned int CTRL_TOG;                         
  volatile unsigned int CTRL1;                             
  volatile unsigned int CTRL1_SET;                         
  volatile unsigned int CTRL1_CLR;                       
  volatile unsigned int CTRL1_TOG;                       
  volatile unsigned int CTRL2;                            
  volatile unsigned int CTRL2_SET;                       
  volatile unsigned int CTRL2_CLR;                        
  volatile unsigned int CTRL2_TOG;                        
  volatile unsigned int TRANSFER_COUNT;   
       unsigned char RESERVED_0[12];
  volatile unsigned int CUR_BUF;                          
       unsigned char RESERVED_1[12];
  volatile unsigned int NEXT_BUF;                        
       unsigned char RESERVED_2[12];
  volatile unsigned int TIMING;                          
       unsigned char RESERVED_3[12];
  volatile unsigned int VDCTRL0;                         
  volatile unsigned int VDCTRL0_SET;                      
  volatile unsigned int VDCTRL0_CLR;                     
  volatile unsigned int VDCTRL0_TOG;                     
  volatile unsigned int VDCTRL1;                          
       unsigned char RESERVED_4[12];
  volatile unsigned int VDCTRL2;                          
       unsigned char RESERVED_5[12];
  volatile unsigned int VDCTRL3;                          
       unsigned char RESERVED_6[12];
  volatile unsigned int VDCTRL4;                           
       unsigned char RESERVED_7[12];
  volatile unsigned int DVICTRL0;    
  	   unsigned char RESERVED_8[12];
  volatile unsigned int DVICTRL1;                         
       unsigned char RESERVED_9[12];
  volatile unsigned int DVICTRL2;                        
       unsigned char RESERVED_10[12];
  volatile unsigned int DVICTRL3;                        
       unsigned char RESERVED_11[12];
  volatile unsigned int DVICTRL4;                          
       unsigned char RESERVED_12[12];
  volatile unsigned int CSC_COEFF0;  
  	   unsigned char RESERVED_13[12];
  volatile unsigned int CSC_COEFF1;                        
       unsigned char RESERVED_14[12];
  volatile unsigned int CSC_COEFF2;                        
       unsigned char RESERVED_15[12];
  volatile unsigned int CSC_COEFF3;                        
       unsigned char RESERVED_16[12];
  volatile unsigned int CSC_COEFF4;   
  	   unsigned char RESERVED_17[12];
  volatile unsigned int CSC_OFFSET;  
       unsigned char RESERVED_18[12];
  volatile unsigned int CSC_LIMIT;  
       unsigned char RESERVED_19[12];
  volatile unsigned int DATA;                              
       unsigned char RESERVED_20[12];
  volatile unsigned int BM_ERROR_STAT;                     
       unsigned char RESERVED_21[12];
  volatile unsigned int CRC_STAT;                        
       unsigned char RESERVED_22[12];
  volatile  unsigned int STAT;                             
       unsigned char RESERVED_23[76];
  volatile unsigned int THRES;                             
       unsigned char RESERVED_24[12];
  volatile unsigned int AS_CTRL;                           
       unsigned char RESERVED_25[12];
  volatile unsigned int AS_BUF;                            
       unsigned char RESERVED_26[12];
  volatile unsigned int AS_NEXT_BUF;                     
       unsigned char RESERVED_27[12];
  volatile unsigned int AS_CLRKEYLOW;                    
       unsigned char RESERVED_28[12];
  volatile unsigned int AS_CLRKEYHIGH;                   
       unsigned char RESERVED_29[12];
  volatile unsigned int SYNC_DELAY;                      
} LCDIF_Type;

/*根据IMX6ULL芯片手册<<18.7 CCM Analog Memory Map/Register Definition>>的710页，定义CCM Analog Memory的结构体,*/
typedef struct {
  volatile unsigned int PLL_ARM;                         
  volatile unsigned int PLL_ARM_SET;                       
  volatile unsigned int PLL_ARM_CLR;                       
  volatile unsigned int PLL_ARM_TOG;                       
  volatile unsigned int PLL_USB1;                         
  volatile unsigned int PLL_USB1_SET;                     
  volatile unsigned int PLL_USB1_CLR;                     
  volatile unsigned int PLL_USB1_TOG;                      
  volatile unsigned int PLL_USB2;                          
  volatile unsigned int PLL_USB2_SET;                     
  volatile unsigned int PLL_USB2_CLR;                      
  volatile unsigned int PLL_USB2_TOG;                     
  volatile unsigned int PLL_SYS;                           
  volatile unsigned int PLL_SYS_SET;                      
  volatile unsigned int PLL_SYS_CLR;                     
  volatile unsigned int PLL_SYS_TOG;                     
  volatile unsigned int PLL_SYS_SS;                      
       unsigned char RESERVED_0[12];
  volatile unsigned int PLL_SYS_NUM;                      
       unsigned char RESERVED_1[12];
  volatile unsigned int PLL_SYS_DENOM;                     
       unsigned char RESERVED_2[12];
  volatile unsigned int PLL_AUDIO;                         
  volatile unsigned int PLL_AUDIO_SET;                     
  volatile unsigned int PLL_AUDIO_CLR;                    
  volatile unsigned int PLL_AUDIO_TOG;                   
  volatile unsigned int PLL_AUDIO_NUM;                   
       unsigned char RESERVED_3[12];
  volatile unsigned int PLL_AUDIO_DENOM;                 
       unsigned char RESERVED_4[12];
  volatile unsigned int PLL_VIDEO;                        
  volatile unsigned int PLL_VIDEO_SET;                     
  volatile unsigned int PLL_VIDEO_CLR;                     
  volatile unsigned int PLL_VIDEO_TOG;                    
  volatile unsigned int PLL_VIDEO_NUM;                     
       unsigned char RESERVED_5[12];
  volatile unsigned int PLL_VIDEO_DENOM;                   
       unsigned char RESERVED_6[28];
  volatile unsigned int PLL_ENET;                         
  volatile unsigned int PLL_ENET_SET;                    
  volatile unsigned int PLL_ENET_CLR;                      
  volatile unsigned int PLL_ENET_TOG;                      
  volatile unsigned int PFD_480;                           
  volatile unsigned int PFD_480_SET;                       
  volatile unsigned int PFD_480_CLR;                    
  volatile unsigned int PFD_480_TOG;                      
  volatile unsigned int PFD_528;                        
  volatile unsigned int PFD_528_SET;                     
  volatile unsigned int PFD_528_CLR;                   
  volatile unsigned int PFD_528_TOG;                     
       unsigned char RESERVED_7[64];
  volatile unsigned int MISC0;                            
  volatile unsigned int MISC0_SET;                        
  volatile unsigned int MISC0_CLR;                       
  volatile unsigned int MISC0_TOG;                        
  volatile unsigned int MISC1;                             
  volatile unsigned int MISC1_SET;                        
  volatile unsigned int MISC1_CLR;                        
  volatile unsigned int MISC1_TOG;                         
  volatile unsigned int MISC2;                             
  volatile unsigned int MISC2_SET;                         
  volatile unsigned int MISC2_CLR;                       
  volatile unsigned int MISC2_TOG;                         
} CCM_ANALOG_Type;

#endif
