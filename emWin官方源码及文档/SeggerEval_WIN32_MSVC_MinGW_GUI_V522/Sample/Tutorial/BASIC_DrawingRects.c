/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2013  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.22 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : BASIC_DrawingRects.c
Purpose     : Filling screen with random rectangles
----------------------------------------------------------------------
*/

#include <stdlib.h>

#include "GUI.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
  const U32 aColor[] = {
    GUI_BLUE        ,    GUI_GREEN       ,    GUI_RED         ,    GUI_CYAN        ,    GUI_MAGENTA     ,    GUI_YELLOW      ,
    GUI_LIGHTBLUE   ,    GUI_LIGHTGREEN  ,    GUI_LIGHTRED    ,    GUI_LIGHTCYAN   ,    GUI_LIGHTMAGENTA,    GUI_LIGHTYELLOW ,
    GUI_DARKBLUE    ,    GUI_DARKGREEN   ,    GUI_DARKRED     ,    GUI_DARKCYAN    ,    GUI_DARKMAGENTA ,    GUI_DARKYELLOW  ,
  };
  int x0, y0, Index;
  int xSize, ySize;
  int xSizeRect, ySizeRect;
  U32 PixelCnt;

  GUI_Init();
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  PixelCnt = 0;
  while (1) {
    x0 = rand() % xSize;
    y0 = rand() % ySize;
    xSizeRect = rand() % (xSize - x0);
    ySizeRect = rand() % (ySize - y0);
    Index = rand() % GUI_COUNTOF(aColor);
    GUI_SetColor(aColor[Index]);
    GUI_FillRect(x0, y0, x0 + xSizeRect - 1, y0 + ySizeRect - 1);
    PixelCnt += xSizeRect * ySizeRect;
  }
}

/*************************** End of file ****************************/
