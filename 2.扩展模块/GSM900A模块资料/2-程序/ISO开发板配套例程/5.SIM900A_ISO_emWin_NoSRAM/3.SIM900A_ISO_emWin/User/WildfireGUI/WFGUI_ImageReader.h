#ifndef __WFGUI_ImageReader_H
#define __WFGUI_ImageReader_H

#include "stm32f10x.h"
#include "GUI.h"

#define DRAW_SCALE 		1		//按比例画图功能，（速度比较慢）  1：使能 0：禁止


typedef int GUI_IMAGE_GetXSizeEx			(GUI_GET_DATA_FUNC * pfGetData, void * p);
typedef int GUI_IMAGE_GetYSizeEx			(GUI_GET_DATA_FUNC * pfGetData, void * p);
typedef int GUI_IMAGE_DrawEx    			(GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0);
typedef int GUI_IMAGE_DrawScaledEx		(GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0, int Num, int Denom);

typedef struct
{
	GUI_IMAGE_GetXSizeEx 		*GetXSizeEx;
	GUI_IMAGE_GetYSizeEx 		*GetYSizeEx;
	
	GUI_IMAGE_DrawEx 				*DrawEx;
	GUI_IMAGE_DrawScaledEx 	*DrawScaledEx;
	GUI_GET_DATA_FUNC				*GetDataEx;


}IMAGE_FUN;

void BMPReader(char * file_name);
void JPEGReader(char * file_name);
void PNGReader(char * file_name);
void GIFReader(char * file_name);
void ImageReader_FrameWin( char * file_name);

void WFGUI_ImageReader(void);




#endif

