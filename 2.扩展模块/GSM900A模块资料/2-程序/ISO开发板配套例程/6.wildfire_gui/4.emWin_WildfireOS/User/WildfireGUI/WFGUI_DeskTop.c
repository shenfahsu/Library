/**
  ******************************************************************************
  * @file    WFGUI_DeskTop.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   存储wildfire gui主界面
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 ISO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/*********************************************************************
*
*       Includes
*
**********************************************************************
*/
/*********************************************************************
*
*       System
*/
#include <stddef.h>
#include <stdio.h>

/*********************************************************************
*
*       STemwin GUI
*/
#include "ICONVIEW.h"
#include "TEXT.h"
#include "DIALOG.h"
#include "MESSAGEBOX.h"

/* BSP */
#include "ff.h"
#include "bsp_rtc.h"

/* WFGUI */
#include "WFGUI_Common.h"
#include "WFGUI_DeskTop.h"
#include "WFGUI_SDView.h"
#include "WFGUI_Time.h"
#include "WFGUI_Message.h"
#include "WFGUI_Phone.h"
#include "WFGUI_Camera.h"
#include "WFGUI_Snapshot.h"
#include "WFGUI_ImageReader.h"
#include "WFGUI_Temperature.h"
#include "WFGUI_TextReader.h"
#include "WFGUI_Keypad.h"
#include "WFGUI_UserAPP.h" 


/* 在调试部分功能的时候使用这个宏，可以减少ICON图标，减少下载时间 */
//#define  GUI_DEVELOP


/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define MSG_MOVE (WM_USER + 0)

#ifndef GUI_DEVELOP
/*********************************************************************
*
*       _aBitmapItem
*/
/*第一页的图标。数组元素分别是：图标数组，图标标题，图标说明(现在没有用图标说明)*/
static const BITMAP_ITEM _aBitmapItem[] = {
  {&bmWF_Floder, 			"Browser", 		"Use the browser to explore the www"},
  {&bmWF_Clock,   		"Clock", 			"Adjust current time and date"},
	{&bmWF_Temperature, "TEMP", 			"Temperature sensor"},
	{&bmWF_Picture,  		"Picture", 		"Picture viewer"},
	{&bmWF_Note,    		"Note", 			"Write a note"},
  {&bmWF_Message,			"Message", 		"Read or write message"},
  {&bmWF_Phone,  			"Phone",      "make a telephone call"},
  {&bmWF_Camera,			"Camera", 		"Take a phone"},
};


/*第二页的图标。数组元素分别是：图标数组，图标标题，图标说明(现在没有用图标说明)*/
static const BITMAP_ITEM _aBitmapItem2[] = {
	{&bmWF_Map,         "Map" ,       "Map"},
  {&bmWF_Calculator,  "Calculator", "Calculator"},
  {&bmWFGUI_UserApp,  "UserAPP",    "UserAPP"}, 

};





#else	//在调试时使用这个数组，减少代码量
/*********************************************************************
*
*       _aBitmapItem
*/
static const BITMAP_ITEM _aBitmapItem[] = {
//  {&bmWF_Floder, 			"Browser", 		"Use the browser to explore the www"},
//  {&bmWF_Clock,   		"Clock", 			"Adjust current time and date"},
	{&bmWF_Temperature, "TEMP", 			"Temperature sensor"},
//	{&bmWF_Picture,  		"Picture", 		"Picture viewer"},
//	{&bmWF_Note,    		"Note", 			"Write a note"},
//  {&bmWF_Message,			"Message", 		"Read or write message"},
//  {&bmWF_Phone,  			"Phone",      "make a telephone call"},
//  {&bmWF_Camera,			"Camera", 		"Take a phone"},
//	{&bmWF_Map,         "Map" ,       "Map"},
//  {&bmWF_Calculator,  "Calculator", "Calculator"},
};

/*第二页的图标。数组元素分别是：图标数组，图标标题，图标说明(现在没有用图标说明)*/
static const BITMAP_ITEM _aBitmapItem2[] = {
//	{&bmWF_Map,         "Map" ,       "Map"},
//  {&bmWF_Calculator,  "Calculator", "Calculator"},
//  {&bmWFGUI_UserApp,  "UserAPP",    "UserAPP"}, 

};
#endif

/* 全局变量 */
WIN_PARA WinPara;			//用户使用的窗口额外数据
SD_FONT SDFont;				//使用外部字体时需要用到的数据结构
	

/**
  * @brief  _cbBKWin,桌面背景的回调函数
  * @param  none
  * @retval none
  */
static void _cbBKWin(WM_MESSAGE * pMsg) {  

  switch (pMsg->MsgId) {

  case WM_PAINT:																			//重绘背景
  	GUI_SetColor(GUI_DARKGRAY);
		GUI_FillRect(0,0,LCD_GetXSize(),LCD_GetYSize());

    break;
	
	 default:		
      WM_DefaultProc(pMsg);
  }
}

/**
  * @brief  _cbStatusWin,状态栏回调函数
  * @param  none
  * @retval none
  */
static void _cbStatusWin(WM_MESSAGE * pMsg)
{
	char text_buffer[20];
	WM_HWIN hText;
	int        NCode;
  int        Id;
	
	 switch (pMsg->MsgId) {
		 
		 case WM_NOTIFY_PARENT:										//通知父窗口
			 
			 	Id    = WM_GetId(pMsg->hWinSrc);      // 控件的ID
				NCode = pMsg->Data.v;                 // 通知代码
			 
			 if(NCode == WM_NOTIFICATION_RELEASED)	//触摸释放消息	
				 {				 			
					 if(Id == GUI_ID_BUTTON0)						//野火Logo按键	
					 {					 
						/* 显示消息提示框 */
						MESSAGEBOX_Create("\n\n  Wildfire OS 1.0  \n\n","About",0);
						 					
					 } 
					 else if(Id == GUI_ID_TEXT0)				//时间文本框
					 {					
						/* 执行WFGUI_Time应用 */ 		
						WFGUI_Time();
						}							 
			
					}
							
			 
				break;
		 
		 
			case WM_PAINT:													//重绘背景
				GUI_SetColor(GUI_BLACK);
				GUI_FillRect(0,0,WinPara.xSizeLCD ,WinPara.yPosWin);
			
			break;
			
			case MY_MESSAGE_RTC:										//处理时间显示的信息，每秒钟修改一次时间
				
				/* 获取text句柄 */      
        hText = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
      
				/* 转换rtc值至北京时间 */
				RTC_TimeCovr(&systmtime);
      
				/* 转换成字符串 */
				sprintf(text_buffer,"%02d:%02d:%02d",systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec);
      
				/* 输出时间 */
				TEXT_SetText(hText,text_buffer);
				break;
			
		 
			default:		
      WM_DefaultProc(pMsg);
		 
	 }

}

/**
  * @brief  _cbIndexWin,索引窗口回调函数
  * @param  none
  * @retval none
  */
static void _cbIndexWin(WM_MESSAGE * pMsg)
{

	 switch (pMsg->MsgId) {
		 case WM_PAINT:										//重绘背景	
			 
		 		GUI_SetColor(GUI_BLACK);			
				GUI_FillRect(0,0,WM_GetWindowSizeX(pMsg->hWin) ,WM_GetWindowSizeY(pMsg->hWin));

			 				
				/* 绘制索引窗口 */
				GUI_SetColor(GUI_LIGHTGRAY);
			
				{
					char i,r;

					for(i=0;i<VIRTUAL_WIN_NUM;i++)	
					{
						if(i == WinPara.ctr_index)
							r=5;										//当前窗口的圆点半径大
						else
							r=2;										//非当前窗口的圆点半径小
						
						if(i< VIRTUAL_WIN_NUM/2)	//绘制圆点
						{
							/* 绘制索引窗口 */
							GUI_FillCircle(WM_GetWindowSizeX(pMsg->hWin)/2-10*i,WM_GetWindowSizeY(pMsg->hWin)/2,r);
						}
						else
						{
							/* 绘制索引窗口 */
							GUI_FillCircle(WM_GetWindowSizeX(pMsg->hWin)/2+10*i,WM_GetWindowSizeY(pMsg->hWin)/2,r);
						}
					}
				}		
			 break;	 		
	 

		 
		 default:
			 break;

		}
			 
}

/**
  * @brief  _cbButtonWin,控制按钮窗口回调函数
  * @param  none
  * @retval none
  */
static void _cbButtonWin(WM_MESSAGE * pMsg)
{
	int Id,NCode;
	WM_HWIN hItem;			
	
	 switch (pMsg->MsgId) {
		 
		 case WM_PAINT:										  //重绘窗口	
			GUI_SetColor(GUI_BLACK);			
			GUI_FillRect(0,0,WM_GetWindowSizeX(pMsg->hWin) ,WM_GetWindowSizeY(pMsg->hWin));
		 
			 break;
		 
		 case WM_CREATE:										//创建窗口
			
		 /*返回键*/
			hItem = BUTTON_CreateEx(0,0,40, 20, 
																		pMsg->hWin, WM_CF_SHOW|WM_CF_MEMDEV , 0, GUI_ID_BUTTON0);
			BUTTON_SetText(hItem, "<");
		 
		 
			 break;
		 
		 
		 case WM_NOTIFY_PARENT:							//处理子窗口消息	
				Id    = WM_GetId(pMsg->hWinSrc);
				NCode = pMsg->Data.v;
			
				if(NCode == WM_NOTIFICATION_RELEASED)
				{
					if(Id == GUI_ID_BUTTON0)
					{
				
							HANDLE_LIST *hAPPOnTop;
            
							/* 获取最上层的窗口句柄 */
							hAPPOnTop =	hAPPLinkedList_GetAppTop();
							if(hAPPOnTop != NULL)
							{
								WM_DeleteWindow(hAPPOnTop->hAPP);			//	关闭窗口
								
							}						
					
						
						}
						else if(Id == GUI_ID_BUTTON1)							//button1可以制作home键，现在没有实现			
						{			

						}

					}
			
			break;
		 
		 default:
			 break;

		}
			 
}

/**
  * @brief  _cbCtrlWin,控制栏回调函数
  * @param  none
  * @retval none
  */
static void _cbCtrlWin(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;	
	WM_HWIN hItemNext;	
	
		 switch (pMsg->MsgId) {			 
			 
			case WM_PAINT:	                             //重绘背景				
			
				GUI_SetColor(GUI_BLACK);			
				GUI_FillRect(0,0,WM_GetWindowSizeX(pMsg->hWin) ,WM_GetWindowSizeY(pMsg->hWin));			
			break;
			
			case MY_MESSAGE_CTRLCHANGE:
				if(hAPPLinkedList_GetAppTop()==NULL)			//如果没有app窗口，则控制栏显示桌面索引
				{
					hItem = WM_GetFirstChild(WinPara.hWinCtrl);
					hItemNext = WM_GetNextSibling(hItem);
					
					/* 根据回调函数来区分窗口 */
				 if(WM_GetCallback(hItem) == _cbIndexWin  && WM_GetCallback(hItemNext) == _cbButtonWin)
				 {
						/* 显示index窗口，隐藏button窗口 */
						WM_ShowWindow(hItem);
						WM_HideWindow(hItemNext);
						WM_Paint(hItem);								//要重绘索引窗口，更新索引
					 
					}
					else if(WM_GetCallback(hItemNext) == _cbIndexWin  && WM_GetCallback(hItem) == _cbButtonWin)
					{
						/* 显示index窗口，隐藏button窗口 */
						WM_ShowWindow(hItemNext);
						WM_HideWindow(hItem);

						WM_Paint(hItemNext);					//要重绘索引窗口，更新索引

					}	

				}
			else																//如果存在app窗口，显示控制按钮索引
				{		
					hItem = WM_GetFirstChild(WinPara.hWinCtrl);
					hItemNext = WM_GetNextSibling(hItem);

					/* 根据回调函数来区分窗口 */
					if(WM_GetCallback(hItem) == _cbIndexWin  && WM_GetCallback(hItemNext) == _cbButtonWin)
				 {
					 /* 显示button窗口，隐藏index窗口 */
						WM_ShowWindow(hItemNext);
						WM_HideWindow(hItem);
					}
					else if(WM_GetCallback(hItemNext) == _cbIndexWin  && WM_GetCallback(hItem) == _cbButtonWin)
					{
						/* 显示button窗口，隐藏index窗口 */
						WM_ShowWindow(hItem);
						WM_HideWindow(hItemNext);

					}	
					

				}
					
				break;				
	
		 
			default:		
      WM_DefaultProc(pMsg);
			break;
		 
	 }


}

/**
  * @brief  _cbMainWin,主窗口回调函数
  * @param  none
  * @retval none
  */
static void _cbMainWin(WM_MESSAGE * pMsg)
{

	switch (pMsg->MsgId) {
			 
			 case WM_NOTIFY_PARENT:

				break;											
			 
				 
		case MY_MESSAGE_SNAPSHOT:			  //处理截图消息
       
        /* 调用截图应用 */
				WFGUI_Snapshot() ;
	
				break;
		
	 
		case WM_PAINT:	              //重绘背景
				GUI_SetColor(GUI_BLACK);
				GUI_FillRect(0,0,WinPara.xSizeWin ,WinPara.ySizeWin);

			break;				      
		
		 
			default:		
      WM_DefaultProc(pMsg);
		 
	 }


}

/**
  * @brief  ShowTips显示让用户等待的信息，在进入App之前使用
  * @param  none
  * @retval none
  */
static void ShowTips(WM_HWIN hWin)
{
		/* 输出等待信息 */
		WM_SelectWindow(hWin);
		GUI_SetColor(GUI_BLACK);
		GUI_FillRect(0,0,WinPara.xSizeWin *VIRTUAL_WIN_NUM  ,WinPara.ySizeWin);
		GUI_SetFont(&SDFont.XFont);
		GUI_SetColor(GUI_WHITE);
		GUI_DispStringHCenterAt("Please wait...", WinPara.xSizeWin >> 1, WinPara.ySizeWin / 3);
}


/**
  * @brief  Icon窗口回调函数
  * @param  none
  * @retval none
  */
static void _cbIconWin(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;
	WM_HWIN hItem;
	WM_HWIN hParent;
	
	WM_MOTION_INFO * pInfo;

	int        NCode;
  int        Id;
  int        Sel;
	
	unsigned char i,j;
	hParent = pMsg->hWin;
	
	switch (pMsg->MsgId) {
	
		case WM_CREATE:			
							
				for(i=0;i<VIRTUAL_WIN_NUM;i++)
				{

					/*
					* 创建图标控件
					*/
				
					hWin = ICONVIEW_CreateEx(i*240, 5, WinPara.xSizeWin,  WinPara.ySizeWin, 
																	 hParent, WM_CF_SHOW | WM_CF_HASTRANS, 
																	 0, GUI_ID_ICONVIEW0+i, 55, 70);
									
					/* 设置选定图标的颜色 */
					ICONVIEW_SetBkColor(hWin, ICONVIEW_CI_SEL, GUI_BLUE | 0xC0000000);
					/* 设置图标的字体 */
					ICONVIEW_SetFont(hWin, &GUI_Font13B_ASCII);
					
					/* 设置初始选择的图标为 -1 (表示尚未选择)*/
					ICONVIEW_SetSel(hWin,-1);
				}
				
				/* 设置当前索引 */	
				WinPara.ctr_index = 0;
				
				#if 0
				/* 显示图标 */
				for(i = 0,j = 0;i<VIRTUAL_WIN_NUM && j < GUI_COUNTOF(_aBitmapItem) ;i++ )
				{				
					hWin = WM_GetDialogItem(pMsg->hWin,GUI_ID_ICONVIEW0+i);
					
					/*
					* 添加图标到图标控件
					*/
					for (j = (0 + i*ICON_PAGE_NUM); (j - i*ICON_PAGE_NUM)< ICON_PAGE_NUM  && j < GUI_COUNTOF(_aBitmapItem); j++)
					{    

						ICONVIEW_AddBitmapItem(hWin, _aBitmapItem[j].pBitmap, _aBitmapItem[j].pText);
					}		
				
				}
        
        #else
          /* 显示第一个图标页 */
          hWin = WM_GetDialogItem(pMsg->hWin,GUI_ID_ICONVIEW0);
					
					/*
					* 添加图标到图标控件
					*/
					for (j = 0 ; j < ICON_PAGE_NUM  && j < GUI_COUNTOF(_aBitmapItem); j++)
					{    

						ICONVIEW_AddBitmapItem(hWin, _aBitmapItem[j].pBitmap, _aBitmapItem[j].pText);
					}		
          
          
          /* 显示第二个图标页 */
          hWin = WM_GetDialogItem(pMsg->hWin,GUI_ID_ICONVIEW1);
					
					/*
					* 添加图标到图标控件
					*/
					for (j = 0 ; j < ICON_PAGE_NUM  && j < GUI_COUNTOF(_aBitmapItem2); j++)
					{    

						ICONVIEW_AddBitmapItem(hWin, _aBitmapItem2[j].pBitmap, _aBitmapItem2[j].pText);
					}		
        #endif
		break;

		case WM_NOTIFY_PARENT:

					Id    = WM_GetId(pMsg->hWinSrc);      // 控件的ID
					NCode = pMsg->Data.v;                 // 通知代码
	 
						switch (Id) {
							
						case GUI_ID_ICONVIEW0:								// 图标控件0
							
							switch (NCode) {
								case WM_NOTIFICATION_RELEASED:
								//case WM_NOTIFICATION_SEL_CHANGED:
										/*
										* 获取当前选中的图标
										*/
										Sel   = ICONVIEW_GetSel(pMsg->hWinSrc);
										
										switch(Sel)
										{
#ifndef GUI_DEVELOP											
											case 0:
													GUI_Delay(500);								//短暂延时以显示触摸效果
											
													ShowTips(pMsg->hWin);					//提示让用户等待

													WFGUI_SDView();	
												break;
											
											case 1:
													GUI_Delay(500);								//短暂延时以显示触摸效果

													ShowTips(pMsg->hWin);					//提示让用户等待

													
													WFGUI_Time();
											
												break;	
                      
                     case 2:
                          
                          GUI_Delay(500);										

                          ShowTips(pMsg->hWin);					//提示让用户等待
                        
                          WFGUI_Temperature();
                          
                        break;
                      
                      case 3:
                          GUI_Delay(500);										

                          ShowTips(pMsg->hWin);					//提示让用户等待
                        
                          WFGUI_ImageReader();
                        
                      break;
                    
                      case 4:
                          GUI_Delay(500);										

                          ShowTips(pMsg->hWin);					//提示让用户等待
                        
                          WFGUI_TextReader();
                        
                      break;
											
											case 5:
												GUI_Delay(500);								//短暂延时以显示触摸效果

												ShowTips(pMsg->hWin);					//提示让用户等待

											
												WFGUI_Message();											
											
												break;
											
											case 6:
												
												GUI_Delay(500);								//短暂延时以显示触摸效果										

												ShowTips(pMsg->hWin);					//提示让用户等待

											
												WFGUI_Phone();
											
											
											break;
										
											case 7:
												
												GUI_Delay(500);										

												ShowTips(pMsg->hWin);					//提示让用户等待
											
												WFGUI_Camera();
												
											break;
										
																				
#else

								
								case 0:

									GUI_Delay(500);								//短暂延时以显示触摸效果

									ShowTips(pMsg->hWin);					//提示让用户等待

									WFGUI_Temperature(); 

								break;
#endif											
												
									default:
										break;
										}	

						/* 获取ICON的句柄 */
						hItem = WM_GetDialogItem(hParent, GUI_ID_ICONVIEW0);
					
						/* 设置初始选择的图标为 -1 (表示尚未选择)*/
						ICONVIEW_SetSel(hItem,-1);										

							break;//WM_NOTIFICATION_RELEASED
						}				//NCODE
						

						
				break;			//ICONVIEW0
						
				case GUI_ID_ICONVIEW1:								// 图标控件1
					
					if(NCode == WM_NOTIFICATION_RELEASED)
					{
						
						/*
						* 获取当前选中的图标
						*/
						Sel   = ICONVIEW_GetSel(pMsg->hWinSrc);
						
						switch(Sel)
						{
							case 0:
              case 1:
              case 2:
								
									GUI_Delay(500);										

									ShowTips(pMsg->hWin);					//提示让用户等待
								
                  WFGUI_UserAPP();

								break;
							
							
							default:
								break;

							}
							
									
					/* 获取ICON的句柄 */
					hItem = WM_GetDialogItem(hParent, GUI_ID_ICONVIEW1);
				
					/* 设置初始选择的图标为 -1 (表示尚未选择)*/
					ICONVIEW_SetSel(hItem,-1);

					}					
				
		
					break;
				
				default:
					break;
					}					//ID
					

			break;				//NOTIFYPARENT	
			 
	 case WM_MOTION:
			pInfo = (WM_MOTION_INFO *)pMsg->Data.p;
				 if (pInfo) {
						switch (pInfo->Cmd) 
					 {
							case WM_MOTION_INIT:
							pInfo->SnapX = 240;		//像素对齐
							pInfo->SnapY = 0;
							pInfo->Period = 500;

							break;
							
							case WM_MOTION_MOVE:
									/* 禁止图标控件 ，防止误触发*/
									for(i=0;i<VIRTUAL_WIN_NUM;i++)
									{
										hWin = WM_GetDialogItem(pMsg->hWin,GUI_ID_ICONVIEW0+i);
										WM_DisableWindow(hWin);
									}
			
									break;
									
							case WM_MOTION_GETPOS:
//								DEBUG("\r\n dx = %d",pInfo->dx);
//								DEBUG("\r\n dy = %d",pInfo->dy);
//							DEBUG("\r\n xpos = %d",pInfo->xPos);
//							DEBUG("\r\n ypos = %d",pInfo->yPos);
								break;								
									
							default:
								break;						
						}
						/* 移动结束 */
						if (pInfo->FinalMove)
						{
								/* 移动结束，使能图标控件 */
								for(i=0;i<VIRTUAL_WIN_NUM;i++)
								{
									hWin = WM_GetDialogItem(pMsg->hWin,GUI_ID_ICONVIEW0+i);
									WM_EnableWindow(hWin);
								}
								

								if(pInfo->xPos >= -120 && pInfo->xPos )	//xpos指向窗口移动的距离，向左移动为负。一般第一窗口移动结束时xpos=-1
								{
									WinPara.ctr_index = 0;			//第一个窗口
									
									WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);

									//WM_Paint(WinPara.hWinCtrl);	//重绘ctrl窗口，刷新index

									}
								else if(pInfo->xPos < -120)		//一般第二窗口移动结束时xpos=-239
								{
									WinPara.ctr_index = 1;			//第二个窗口
									WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);

								//	WM_Paint(WinPara.hWinCtrl);	//重绘ctrl窗口，刷新index
								}
							

						}
				 }	

			break;

	 case WM_PAINT:	//重绘背景
				GUI_SetColor(GUI_BLACK);
				GUI_FillRect(0,0,WinPara.xSizeWin *VIRTUAL_WIN_NUM  ,WinPara.ySizeWin);
 
	 
			break;				//WM_PAINT
	 
	 default:		
      WM_DefaultProc(pMsg);

		}


}






/*********************************************************************
*
*       _cbGetData
*
* Function description
*   Callback function for getting font data
*
* Parameters:
*   Off      - Position of XBF file to be read
*   NumBytes - Number of requested bytes
*   pVoid    - Application defined pointer
*   pBuffer  - Pointer to buffer to be filled by the function
*
* Return value:
*   0 on success, 1 on error
*/
static int _cbGetData(U32 Off, U16 NumBytes, void * pVoid, void * pBuffer) {
  FIL hFile;
  UINT  NumBytesRead;
	FRESULT fres;

  hFile = *(FIL *)pVoid;
  //
  // Set file pointer to the requested position
  //
	fres = f_lseek(&hFile, Off) ;
  if (fres != FR_OK) {
    return 1; // Error
  }
  //
  // Read font data
  //
	fres = f_read(&hFile, pBuffer, NumBytes, &NumBytesRead);

	if (fres != FR_OK) {
    return 1; // Error
  }
	
  if (NumBytesRead != NumBytes) {
    return 1; // Error
  }
  return 0;   // Ok
}



/**
  * @brief  CreateFont,使用SD卡的字库文件创建字体
	*					
  * @param  none
  * @retval none
  */
static void CreateFont(SD_FONT *ExFont)
{
	FRESULT res;

  res = f_open(&ExFont->hFontFile,XBF_FONT_PATH,FA_READ|FA_OPEN_EXISTING);
	if (res == FR_OK) {
		
	 /* 成功打开字库文件 */
	GUI_XBF_CreateFont(	&ExFont->XFont,             						// 指向创建得的字体数据结构存储位置
											&ExFont->XBF_Data,         							// 指向XBF_DATA数据结构
											GUI_XBF_TYPE_PROP_AA2_EXT, 							// 要创建的字体类型
											_cbGetData,        											// 读取字库文件的回调函数
											&ExFont->hFontFile);            				// 指向字库文件的文件索引
		
	/* 设置GUI字体 */
	GUI_SetFont(&ExFont->XFont);
		
	/* 设置文本字体 */	
	TEXT_SetDefaultFont(&ExFont->XFont);
		
	/* 设置GUI字体编码 */
	GUI_UC_SetEncodeUTF8();
	}


}

/**
  * @brief  SetDefaultSkin,设置gui的默认皮肤
	*					
  * @param  none
  * @retval none
  */
static void SetDefaultSkin(void)
{
	/* 设置各种控件的皮肤为类win7的皮肤 */
  BUTTON_SetDefaultSkin   (BUTTON_SKIN_FLEX);
  CHECKBOX_SetDefaultSkin (CHECKBOX_SKIN_FLEX);
  DROPDOWN_SetDefaultSkin (DROPDOWN_SKIN_FLEX);
  FRAMEWIN_SetDefaultSkin (FRAMEWIN_SKIN_FLEX);
  HEADER_SetDefaultSkin   (HEADER_SKIN_FLEX);
  PROGBAR_SetDefaultSkin  (PROGBAR_SKIN_FLEX);
  RADIO_SetDefaultSkin    (RADIO_SKIN_FLEX);
  SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
  SLIDER_SetDefaultSkin   (SLIDER_SKIN_FLEX);
	
	/* 设置滑动条 */
	SCROLLBAR_SetDefaultWidth(15);						//宽度
	SCROLLBAR_SetThumbSizeMin(10);						//滑块最小size	
	
	/* 创建sd卡外部字体 */
	CreateFont(&SDFont);
	
	/* 设置默认字体 */
	TEXT_SetDefaultFont(&SDFont.XFont);

	/* 设置framwin  */
	FRAMEWIN_SetDefaultTitleHeight(20);					//标题栏高度
	FRAMEWIN_SetDefaultFont(GUI_FONT_8X16);			//默认字体
	FRAMEWIN_SetDefaultTextColor(1,GUI_BLACK); 	//设置激活状态时的标题文字颜色
	WM_SetDesktopColor(GUI_BLACK);
}

/**
  * @brief  CreatStatusWin，创建顶部的状态栏
  * @param  none
  * @retval none
  */
static void CreatStatusWin(void)
{
	WM_HWIN hText;
	WM_HWIN hButton;

	WinPara.hWinStatus = WM_CreateWindowAsChild(
																							0,				//窗口位置、大小
																							0,
																							WinPara.xSizeLCD,
																							WinPara.yPosWin,	
																							WM_HBKWIN, WM_CF_SHOW | WM_CF_MEMDEV, _cbStatusWin, sizeof(WIN_PARA *)
																						);
	
	/* 顶部的 "wildfire OS "文本 */
	hText = TEXT_CreateEx(0, 0, WinPara.xSizeWin , 20, WinPara.hWinStatus, WM_CF_SHOW, GUI_TA_HCENTER|TEXT_CF_VCENTER, GUI_ID_TEXT1, "Wildfire OS");
  TEXT_SetFont(hText, GUI_FONT_16B_ASCII);
  TEXT_SetTextColor(hText, 0xFF4040);
	
	/* 状态栏的时间显示文本 */
	hText = TEXT_CreateEx(WinPara.xSizeLCD-55,5,55,20,WinPara.hWinStatus,WM_CF_SHOW,TEXT_CF_LEFT,GUI_ID_TEXT0,"");
	TEXT_SetBkColor(hText,GUI_INVALID_COLOR);
	TEXT_SetTextColor(hText,GUI_WHITE);
	TEXT_SetFont(hText,GUI_FONT_13B_ASCII);
	
	/* 野火Logo 按钮 */
	hButton = BUTTON_CreateEx(2,0,40,20,WinPara.hWinStatus,WM_CF_SHOW,0,GUI_ID_BUTTON0);
	BUTTON_SetBitmap(hButton,BUTTON_BI_UNPRESSED,&bmWF_Logo);
	BUTTON_SetBitmap(hButton,BUTTON_BI_PRESSED,&bmWF_LogoPr);	

}



/**
  * @brief  CreatCtrlWin，创建底部的控制栏
  * @param  none
  * @retval none
  */
static void CreatCtrlWin(void)
{
	WM_HWIN hButtonWin;
	
	
	WinPara.hWinCtrl = WM_CreateWindowAsChild(
																							0,											
																							WinPara.yPosWin +WinPara.ySizeWin ,	//底部位置
																							WinPara.xSizeLCD,
																							WinPara.ySizeLCD -(WinPara.yPosWin +WinPara.ySizeWin),	//底部剩余宽度
																							WM_HBKWIN, WM_CF_SHOW | WM_CF_MEMDEV, _cbCtrlWin, sizeof(WIN_PARA *)
																						);
	
	/* 创建索引窗口 (以小圆点表示当前的图标页面)*/
	WM_CreateWindowAsChild(0,0,WM_GetWindowSizeX(WinPara.hWinCtrl),WM_GetWindowSizeY(WinPara.hWinCtrl),
													WinPara.hWinCtrl,WM_CF_SHOW,_cbIndexWin,0);
													
	/* 创建按键button窗口(该button在应用程序状态下 会显示返回按钮) */
	hButtonWin = WM_CreateWindowAsChild(0,0,WM_GetWindowSizeX(WinPara.hWinCtrl),WM_GetWindowSizeY(WinPara.hWinCtrl),
													WinPara.hWinCtrl,WM_CF_SHOW,_cbButtonWin,0);
	/* 默认隐藏button窗口 */	
	GUI_Delay(10);	
	WM_HideWindow(hButtonWin);
	GUI_Delay(10);
													
}


/**
  * @brief  CreatMainWin，创建主显示窗口
  * @param  none
  * @retval none
  */
static void CreatMainWin(void)
{
	
	WinPara.hWinMain = WM_CreateWindowAsChild(
																							WinPara.xPosWin ,											
																							WinPara.yPosWin ,	//位置
																							WinPara.xSizeWin,
																							WinPara.ySizeWin,	//底部剩余宽度
																							WM_HBKWIN, WM_CF_SHOW | WM_CF_MEMDEV, _cbMainWin, sizeof(WIN_PARA *)
																						);	
		/* 创建icon图标窗口 */
	 WM_CreateWindowAsChild(	0 ,											
														0 ,	//位置
														WinPara.xSizeWin * VIRTUAL_WIN_NUM,
														WinPara.ySizeWin,	//高
														WinPara.hWinMain,
														WM_CF_SHOW | WM_CF_MEMDEV|WM_CF_MOTION_X,
														_cbIconWin,
														0
														);							
																	
	

}




/**
  * @brief  CreatDeskTop，创建gui桌面
  * @param  none
  * @retval none
  */
static void CreatDeskTop(void)
{

	//准备建立3个窗口，以下是使用到的用户定义参数，方便在回调函数中使用
	WinPara.xSizeLCD = LCD_GetXSize();						//LCD屏幕尺寸
	WinPara.ySizeLCD = LCD_GetYSize();						//LCD屏幕尺寸
	WinPara.xSizeWin = WinPara.xSizeLCD;					//窗口尺寸
	WinPara.ySizeWin = WinPara.ySizeLCD -20*2;		//窗口尺寸 屏幕大小减去状态栏和控制栏
	WinPara.xPosWin	 = 0;													//窗口的起始位置
	WinPara.yPosWin  = 20;												//窗口的起始位置
	WinPara.xSizeWinVirtual = WinPara.xSizeWin * VIRTUAL_WIN_NUM;	//虚拟窗口尺寸，用于桌面
	
	/* 设置默认皮肤 */
	SetDefaultSkin();

		
  WM_EnableMemdev(WM_HBKWIN);
	
	/* 设置背景桌面的回调函数 */
  WM_SetCallback(WM_HBKWIN, _cbBKWin);	
	
	/* 创建三个窗口 状态栏、控制栏、主窗口*/
	CreatStatusWin();
	CreatCtrlWin();
	CreatMainWin();
	
	/* 初始化app链表 */													
	hAPPLinkedList_Init();	

	
}


/*********************************************************************
*
*       Public routines
*				公有函数
*
**********************************************************************
*/

/**
  * @brief  WFGUI_MainTask,WFGUI主函数
  * @param  none
  * @retval none
  */
void WFGUI_MainTask(void) {
  
	WM_MOTION_Enable(1);
  WM_MOTION_SetDefaultPeriod(1000);
	
	CreatDeskTop();			//创建gui桌面

  while (1) {
		
    GUI_Delay(50);

  }
}

/*************************** End of file ****************************/

