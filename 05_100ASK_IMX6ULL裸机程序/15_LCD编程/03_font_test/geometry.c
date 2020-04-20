
#include "framebuffer.h"

/*
 * http://blog.csdn.net/p1126500468/article/details/50428613
 */

 //-------------画圆函数。参数：圆心，半径，颜色----------	
 // 	   画1/8圆 然后其他7/8对称画  
 // 		 ---------------->X  
 // 		 |(0,0)   0  
 // 		 |	   7	 1	
 // 		 |	  6 	  2  
 // 		 |	   5	 3	
 // 	  (Y)V		  4  
 //  
 // 	 L = x^2 + y^2 - r^2  
 
 /**********************************************************************
  * 函数名称： draw_circle
  * 功能描述： 画圆函数
  * 输入参数： 圆心的x坐标，圆心的y坐标，半径，颜色
  * 输出参数： 无
  * 返 回 值： 无
  * 修改日期		版本号	  修改人 	   修改内容
  * -----------------------------------------------
  * 2020/02/26		  V1.0	   zh(angenao)		   创建
  ***********************************************************************/
 void draw_circle(int x, int y, int r, int color)  
 {	
	 int a, b, num;  
	 a = 0;  
	 b = r;  
	 while(22 * b * b >= r * r) 		 // 1/8圆即可   
	 {	
		 fb_put_pixel(x + a, y - b,color); // 0~1  
		 fb_put_pixel(x - a, y - b,color); // 0~7  
		 fb_put_pixel(x - a, y + b,color); // 4~5  
		 fb_put_pixel(x + a, y + b,color); // 4~3  
   
		 fb_put_pixel(x + b, y + a,color); // 2~3  
		 fb_put_pixel(x + b, y - a,color); // 2~1  
		 fb_put_pixel(x - b, y - a,color); // 6~7  
		 fb_put_pixel(x - b, y + a,color); // 6~5  
		   
		 a++;  
		 num = (a * a + b * b) - r*r;  
		 if(num > 0)  
		 {	
			 b--;  
			 a--;  
		 }	
	 }	
 }	

 
 /**********************************************************************
  * 函数名称： draw_line
  * 功能描述： 画线函数
  * 输入参数： 线段起点x坐标，线段起点y坐标，线段末尾x坐标，线段末尾y坐标，颜色
  * 输出参数： 无
  * 返 回 值： 无
  * 修改日期		版本号	  修改人 	   修改内容
  * -----------------------------------------------
  * 2020/02/26		  V1.0	   zh(angenao)		   创建
  ***********************************************************************/
 //-----------画线。参数：起始坐标，终点坐标，颜色--------	
 void draw_line(int x1,int y1,int x2,int y2,int color)	
 {	
	 int dx,dy,e;  
	 dx=x2-x1;	 
	 dy=y2-y1;	
	 if(dx>=0)	
	 {	
		 if(dy >= 0) // dy>=0  
		 {	
			 if(dx>=dy) // 1/8 octant  
			 {	
				 e=dy-dx/2;  
				 while(x1<=x2)	
				 {	
					 fb_put_pixel(x1,y1,color);  
					 if(e>0){y1+=1;e-=dx;}	   
					 x1+=1;  
					 e+=dy;  
				 }	
			 }	
			 else		 // 2/8 octant	
			 {	
				 e=dx-dy/2;  
				 while(y1<=y2)	
				 {	
					 fb_put_pixel(x1,y1,color);  
					 if(e>0){x1+=1;e-=dy;}	   
					 y1+=1;  
					 e+=dx;  
				 }	
			 }	
		 }	
		 else			// dy<0  
		 {	
			 dy=-dy;   // dy=abs(dy)  
			 if(dx>=dy) // 8/8 octant  
			 {	
				 e=dy-dx/2;  
				 while(x1<=x2)	
				 {	
					 fb_put_pixel(x1,y1,color);  
					 if(e>0){y1-=1;e-=dx;}	   
					 x1+=1;  
					 e+=dy;  
				 }	
			 }	
			 else		 // 7/8 octant	
			 {	
				 e=dx-dy/2;  
				 while(y1>=y2)	
				 {	
					 fb_put_pixel(x1,y1,color);  
					 if(e>0){x1+=1;e-=dy;}	   
					 y1-=1;  
					 e+=dx;  
				 }	
			 }	
		 }	   
	 }	
	 else //dx<0  
	 {	
		 dx=-dx;	 //dx=abs(dx)  
		 if(dy >= 0) // dy>=0  
		 {	
			 if(dx>=dy) // 4/8 octant  
			 {	
				 e=dy-dx/2;  
				 while(x1>=x2)	
				 {	
					 fb_put_pixel(x1,y1,color);  
					 if(e>0){y1+=1;e-=dx;}	   
					 x1-=1;  
					 e+=dy;  
				 }	
			 }	
			 else		 // 3/8 octant	
			 {	
				 e=dx-dy/2;  
				 while(y1<=y2)	
				 {	
					 fb_put_pixel(x1,y1,color);  
					 if(e>0){x1-=1;e-=dy;}	   
					 y1+=1;  
					 e+=dx;  
				 }	
			 }	
		 }	
		 else			// dy<0  
		 {	
			 dy=-dy;   // dy=abs(dy)  
			 if(dx>=dy) // 5/8 octant  
			 {	
				 e=dy-dx/2;  
				 while(x1>=x2)	
				 {	
					 fb_put_pixel(x1,y1,color);  
					 if(e>0){y1-=1;e-=dx;}	   
					 x1-=1;  
					 e+=dy;  
				 }	
			 }	
			 else		 // 6/8 octant	
			 {	
				 e=dx-dy/2;  
				 while(y1>=y2)	
				 {	
					 fb_put_pixel(x1,y1,color);  
					 if(e>0){x1-=1;e-=dy;}	   
					 y1-=1;  
					 e+=dx;  
				 }	
			 }	
		 }	   
	 }	
 }	

