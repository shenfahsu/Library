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
File        : MOVIE_ShowFromFS.c
Purpose     : Shows how to play a movie directly from a file system.
----------------------------------------------------------------------
*/

#ifndef SKIP_TEST

#include "GUI.h"

#ifdef WIN32
  #include <windows.h>
#else
  #include "FS.h"
#endif

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _cbNotify
*
* Purpose:
*   Callback function for movie player. Uses multiple buffering if
*   available to avoid tearing effects.
*/
void _cbNotify(GUI_HMEM hMem, int Notification, U32 CurrentFrame) {
  switch (Notification) {
  case GUI_MOVIE_NOTIFICATION_PREDRAW:
    GUI_MULTIBUF_Begin();
    break;
  case GUI_MOVIE_NOTIFICATION_POSTDRAW:
    GUI_MULTIBUF_End();
    break;
  case GUI_MOVIE_NOTIFICATION_STOP:
    break;
  }
}

/*********************************************************************
*
*       _GetData
*
* Purpose:
*   Reading data directly from file system
*/
int _GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off) {
  U32 NumBytesRead;
#ifdef WIN32
  HANDLE hFile;

  hFile = *(HANDLE *)p;
  SetFilePointer(hFile, Off, 0, FILE_BEGIN);
  ReadFile(hFile, (U8 *)*ppData, NumBytes, &NumBytesRead, NULL);
#else
  FS_FILE * pFile;

  pFile = (FS_FILE *)p;
  FS_SetFilePos(pFile, Off, FS_FILE_BEGIN);
  NumBytesRead = FS_Read(pFile, (U8 *)*ppData, NumBytes);
#endif
  return NumBytesRead;
}

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
  GUI_MOVIE_INFO   Info;
  GUI_MOVIE_HANDLE hMovie;
  int              xSize, ySize;
#ifdef WIN32
  HANDLE           hFile;
  const char       acFileName[] = "C:\\Work\\Grafik\\Video\\FFmpeg\\FFmpeg.emf";
  #define PARAM    &hFile
#else
  FS_FILE        * pFile;
  const char       acFileName[] = "\\FFmpeg.emf";
  #define PARAM    pFile
#endif

  GUI_Init();
  //
  // Get display size
  //
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  //
  // Create file handle
  //
#ifdef WIN32
  hFile = CreateFile(acFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
#else
  FS_Init();
  pFile = FS_FOpen(acFileName, "r");
#endif
  //
  // Get physical size of movie
  //
  if (GUI_MOVIE_GetInfoEx(_GetData, PARAM, &Info) == 0) {
    //
    // Check if display size fits
    //
    if ((Info.xSize <= xSize) && (Info.ySize <= ySize)) {
      //
      // Create and play movie
      //
      hMovie = GUI_MOVIE_CreateEx(_GetData, PARAM, _cbNotify);
      if (hMovie) {
        GUI_MOVIE_Show(hMovie, (xSize - Info.xSize) / 2, (ySize - Info.ySize) / 2, 1);
      }
    } else {
      //
      // Error message
      //
      GUI_SetFont(GUI_FONT_13_ASCII);
      GUI_DispStringHCenterAt("Video can not be shown.\n\nDisplay size too small.", xSize / 2, (ySize - GUI_GetFontSizeY()) / 2);
    }
  }
  while (1) {
    GUI_Exec();
    GUI_X_Delay(1);
  }
}

#endif

/*************************** End of file ****************************/
