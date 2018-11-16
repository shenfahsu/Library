#ifndef __WFGUI_KEYPAD_H
#define __WFGUI_KEYPAD_H

#include "WFGUI_Common.h"
/*********************************************************************
*
*       Types
*
**********************************************************************
*/

#define COLOR_BACK0            0xFF3333
#define COLOR_BACK1            0x550000
#define COLOR_BORDER           0x444444
#define COLOR_KEYPAD0          0xAAAAAA
#define COLOR_KEYPAD1          0x555555
#define BUTTON_COLOR0          0xEEEEEE
#define BUTTON_COLOR1          0xCCCCCC
#define BUTTON_COLOR2          0xCCCCCC
#define BUTTON_COLOR3          0xAAAAAA
#define BUTTON_SKINFLEX_RADIUS 4
#define ID_BUTTON              (GUI_ID_USER + 0)
#define APP_INIT_LOWERCASE     (WM_USER + 0)

#define ID_TEXT					(GUI_ID_USER+40)



typedef struct {
  int          xPos;
  int          yPos;
  int          xSize;
  int          ySize;
  const char * acLabel;
  GUI_COLOR	   bkColor;
  void (* pfDraw)(WM_HWIN hWin);
} BUTTON_DATA;



void KeypadInit(WM_HWIN hFrame);
#endif

