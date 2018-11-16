/**
  ******************************************************************************
  * @file    WFGUI_DeskTop.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �洢wildfire gui������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� ISO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
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


/* �ڵ��Բ��ֹ��ܵ�ʱ��ʹ������꣬���Լ���ICONͼ�꣬��������ʱ�� */
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
/*��һҳ��ͼ�ꡣ����Ԫ�طֱ��ǣ�ͼ�����飬ͼ����⣬ͼ��˵��(����û����ͼ��˵��)*/
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


/*�ڶ�ҳ��ͼ�ꡣ����Ԫ�طֱ��ǣ�ͼ�����飬ͼ����⣬ͼ��˵��(����û����ͼ��˵��)*/
static const BITMAP_ITEM _aBitmapItem2[] = {
	{&bmWF_Map,         "Map" ,       "Map"},
  {&bmWF_Calculator,  "Calculator", "Calculator"},
  {&bmWFGUI_UserApp,  "UserAPP",    "UserAPP"}, 

};





#else	//�ڵ���ʱʹ��������飬���ٴ�����
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

/*�ڶ�ҳ��ͼ�ꡣ����Ԫ�طֱ��ǣ�ͼ�����飬ͼ����⣬ͼ��˵��(����û����ͼ��˵��)*/
static const BITMAP_ITEM _aBitmapItem2[] = {
//	{&bmWF_Map,         "Map" ,       "Map"},
//  {&bmWF_Calculator,  "Calculator", "Calculator"},
//  {&bmWFGUI_UserApp,  "UserAPP",    "UserAPP"}, 

};
#endif

/* ȫ�ֱ��� */
WIN_PARA WinPara;			//�û�ʹ�õĴ��ڶ�������
SD_FONT SDFont;				//ʹ���ⲿ����ʱ��Ҫ�õ������ݽṹ
	

/**
  * @brief  _cbBKWin,���汳���Ļص�����
  * @param  none
  * @retval none
  */
static void _cbBKWin(WM_MESSAGE * pMsg) {  

  switch (pMsg->MsgId) {

  case WM_PAINT:																			//�ػ汳��
  	GUI_SetColor(GUI_DARKGRAY);
		GUI_FillRect(0,0,LCD_GetXSize(),LCD_GetYSize());

    break;
	
	 default:		
      WM_DefaultProc(pMsg);
  }
}

/**
  * @brief  _cbStatusWin,״̬���ص�����
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
		 
		 case WM_NOTIFY_PARENT:										//֪ͨ������
			 
			 	Id    = WM_GetId(pMsg->hWinSrc);      // �ؼ���ID
				NCode = pMsg->Data.v;                 // ֪ͨ����
			 
			 if(NCode == WM_NOTIFICATION_RELEASED)	//�����ͷ���Ϣ	
				 {				 			
					 if(Id == GUI_ID_BUTTON0)						//Ұ��Logo����	
					 {					 
						/* ��ʾ��Ϣ��ʾ�� */
						MESSAGEBOX_Create("\n\n  Wildfire OS 1.0  \n\n","About",0);
						 					
					 } 
					 else if(Id == GUI_ID_TEXT0)				//ʱ���ı���
					 {					
						/* ִ��WFGUI_TimeӦ�� */ 		
						WFGUI_Time();
						}							 
			
					}
							
			 
				break;
		 
		 
			case WM_PAINT:													//�ػ汳��
				GUI_SetColor(GUI_BLACK);
				GUI_FillRect(0,0,WinPara.xSizeLCD ,WinPara.yPosWin);
			
			break;
			
			case MY_MESSAGE_RTC:										//����ʱ����ʾ����Ϣ��ÿ�����޸�һ��ʱ��
				
				/* ��ȡtext��� */      
        hText = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
      
				/* ת��rtcֵ������ʱ�� */
				RTC_TimeCovr(&systmtime);
      
				/* ת�����ַ��� */
				sprintf(text_buffer,"%02d:%02d:%02d",systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec);
      
				/* ���ʱ�� */
				TEXT_SetText(hText,text_buffer);
				break;
			
		 
			default:		
      WM_DefaultProc(pMsg);
		 
	 }

}

/**
  * @brief  _cbIndexWin,�������ڻص�����
  * @param  none
  * @retval none
  */
static void _cbIndexWin(WM_MESSAGE * pMsg)
{

	 switch (pMsg->MsgId) {
		 case WM_PAINT:										//�ػ汳��	
			 
		 		GUI_SetColor(GUI_BLACK);			
				GUI_FillRect(0,0,WM_GetWindowSizeX(pMsg->hWin) ,WM_GetWindowSizeY(pMsg->hWin));

			 				
				/* ������������ */
				GUI_SetColor(GUI_LIGHTGRAY);
			
				{
					char i,r;

					for(i=0;i<VIRTUAL_WIN_NUM;i++)	
					{
						if(i == WinPara.ctr_index)
							r=5;										//��ǰ���ڵ�Բ��뾶��
						else
							r=2;										//�ǵ�ǰ���ڵ�Բ��뾶С
						
						if(i< VIRTUAL_WIN_NUM/2)	//����Բ��
						{
							/* ������������ */
							GUI_FillCircle(WM_GetWindowSizeX(pMsg->hWin)/2-10*i,WM_GetWindowSizeY(pMsg->hWin)/2,r);
						}
						else
						{
							/* ������������ */
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
  * @brief  _cbButtonWin,���ư�ť���ڻص�����
  * @param  none
  * @retval none
  */
static void _cbButtonWin(WM_MESSAGE * pMsg)
{
	int Id,NCode;
	WM_HWIN hItem;			
	
	 switch (pMsg->MsgId) {
		 
		 case WM_PAINT:										  //�ػ洰��	
			GUI_SetColor(GUI_BLACK);			
			GUI_FillRect(0,0,WM_GetWindowSizeX(pMsg->hWin) ,WM_GetWindowSizeY(pMsg->hWin));
		 
			 break;
		 
		 case WM_CREATE:										//��������
			
		 /*���ؼ�*/
			hItem = BUTTON_CreateEx(0,0,40, 20, 
																		pMsg->hWin, WM_CF_SHOW|WM_CF_MEMDEV , 0, GUI_ID_BUTTON0);
			BUTTON_SetText(hItem, "<");
		 
		 
			 break;
		 
		 
		 case WM_NOTIFY_PARENT:							//�����Ӵ�����Ϣ	
				Id    = WM_GetId(pMsg->hWinSrc);
				NCode = pMsg->Data.v;
			
				if(NCode == WM_NOTIFICATION_RELEASED)
				{
					if(Id == GUI_ID_BUTTON0)
					{
				
							HANDLE_LIST *hAPPOnTop;
            
							/* ��ȡ���ϲ�Ĵ��ھ�� */
							hAPPOnTop =	hAPPLinkedList_GetAppTop();
							if(hAPPOnTop != NULL)
							{
								WM_DeleteWindow(hAPPOnTop->hAPP);			//	�رմ���
								
							}						
					
						
						}
						else if(Id == GUI_ID_BUTTON1)							//button1��������home��������û��ʵ��			
						{			

						}

					}
			
			break;
		 
		 default:
			 break;

		}
			 
}

/**
  * @brief  _cbCtrlWin,�������ص�����
  * @param  none
  * @retval none
  */
static void _cbCtrlWin(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;	
	WM_HWIN hItemNext;	
	
		 switch (pMsg->MsgId) {			 
			 
			case WM_PAINT:	                             //�ػ汳��				
			
				GUI_SetColor(GUI_BLACK);			
				GUI_FillRect(0,0,WM_GetWindowSizeX(pMsg->hWin) ,WM_GetWindowSizeY(pMsg->hWin));			
			break;
			
			case MY_MESSAGE_CTRLCHANGE:
				if(hAPPLinkedList_GetAppTop()==NULL)			//���û��app���ڣ����������ʾ��������
				{
					hItem = WM_GetFirstChild(WinPara.hWinCtrl);
					hItemNext = WM_GetNextSibling(hItem);
					
					/* ���ݻص����������ִ��� */
				 if(WM_GetCallback(hItem) == _cbIndexWin  && WM_GetCallback(hItemNext) == _cbButtonWin)
				 {
						/* ��ʾindex���ڣ�����button���� */
						WM_ShowWindow(hItem);
						WM_HideWindow(hItemNext);
						WM_Paint(hItem);								//Ҫ�ػ��������ڣ���������
					 
					}
					else if(WM_GetCallback(hItemNext) == _cbIndexWin  && WM_GetCallback(hItem) == _cbButtonWin)
					{
						/* ��ʾindex���ڣ�����button���� */
						WM_ShowWindow(hItemNext);
						WM_HideWindow(hItem);

						WM_Paint(hItemNext);					//Ҫ�ػ��������ڣ���������

					}	

				}
			else																//�������app���ڣ���ʾ���ư�ť����
				{		
					hItem = WM_GetFirstChild(WinPara.hWinCtrl);
					hItemNext = WM_GetNextSibling(hItem);

					/* ���ݻص����������ִ��� */
					if(WM_GetCallback(hItem) == _cbIndexWin  && WM_GetCallback(hItemNext) == _cbButtonWin)
				 {
					 /* ��ʾbutton���ڣ�����index���� */
						WM_ShowWindow(hItemNext);
						WM_HideWindow(hItem);
					}
					else if(WM_GetCallback(hItemNext) == _cbIndexWin  && WM_GetCallback(hItem) == _cbButtonWin)
					{
						/* ��ʾbutton���ڣ�����index���� */
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
  * @brief  _cbMainWin,�����ڻص�����
  * @param  none
  * @retval none
  */
static void _cbMainWin(WM_MESSAGE * pMsg)
{

	switch (pMsg->MsgId) {
			 
			 case WM_NOTIFY_PARENT:

				break;											
			 
				 
		case MY_MESSAGE_SNAPSHOT:			  //�����ͼ��Ϣ
       
        /* ���ý�ͼӦ�� */
				WFGUI_Snapshot() ;
	
				break;
		
	 
		case WM_PAINT:	              //�ػ汳��
				GUI_SetColor(GUI_BLACK);
				GUI_FillRect(0,0,WinPara.xSizeWin ,WinPara.ySizeWin);

			break;				      
		
		 
			default:		
      WM_DefaultProc(pMsg);
		 
	 }


}

/**
  * @brief  ShowTips��ʾ���û��ȴ�����Ϣ���ڽ���App֮ǰʹ��
  * @param  none
  * @retval none
  */
static void ShowTips(WM_HWIN hWin)
{
		/* ����ȴ���Ϣ */
		WM_SelectWindow(hWin);
		GUI_SetColor(GUI_BLACK);
		GUI_FillRect(0,0,WinPara.xSizeWin *VIRTUAL_WIN_NUM  ,WinPara.ySizeWin);
		GUI_SetFont(&SDFont.XFont);
		GUI_SetColor(GUI_WHITE);
		GUI_DispStringHCenterAt("Please wait...", WinPara.xSizeWin >> 1, WinPara.ySizeWin / 3);
}


/**
  * @brief  Icon���ڻص�����
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
					* ����ͼ��ؼ�
					*/
				
					hWin = ICONVIEW_CreateEx(i*240, 5, WinPara.xSizeWin,  WinPara.ySizeWin, 
																	 hParent, WM_CF_SHOW | WM_CF_HASTRANS, 
																	 0, GUI_ID_ICONVIEW0+i, 55, 70);
									
					/* ����ѡ��ͼ�����ɫ */
					ICONVIEW_SetBkColor(hWin, ICONVIEW_CI_SEL, GUI_BLUE | 0xC0000000);
					/* ����ͼ������� */
					ICONVIEW_SetFont(hWin, &GUI_Font13B_ASCII);
					
					/* ���ó�ʼѡ���ͼ��Ϊ -1 (��ʾ��δѡ��)*/
					ICONVIEW_SetSel(hWin,-1);
				}
				
				/* ���õ�ǰ���� */	
				WinPara.ctr_index = 0;
				
				#if 0
				/* ��ʾͼ�� */
				for(i = 0,j = 0;i<VIRTUAL_WIN_NUM && j < GUI_COUNTOF(_aBitmapItem) ;i++ )
				{				
					hWin = WM_GetDialogItem(pMsg->hWin,GUI_ID_ICONVIEW0+i);
					
					/*
					* ���ͼ�굽ͼ��ؼ�
					*/
					for (j = (0 + i*ICON_PAGE_NUM); (j - i*ICON_PAGE_NUM)< ICON_PAGE_NUM  && j < GUI_COUNTOF(_aBitmapItem); j++)
					{    

						ICONVIEW_AddBitmapItem(hWin, _aBitmapItem[j].pBitmap, _aBitmapItem[j].pText);
					}		
				
				}
        
        #else
          /* ��ʾ��һ��ͼ��ҳ */
          hWin = WM_GetDialogItem(pMsg->hWin,GUI_ID_ICONVIEW0);
					
					/*
					* ���ͼ�굽ͼ��ؼ�
					*/
					for (j = 0 ; j < ICON_PAGE_NUM  && j < GUI_COUNTOF(_aBitmapItem); j++)
					{    

						ICONVIEW_AddBitmapItem(hWin, _aBitmapItem[j].pBitmap, _aBitmapItem[j].pText);
					}		
          
          
          /* ��ʾ�ڶ���ͼ��ҳ */
          hWin = WM_GetDialogItem(pMsg->hWin,GUI_ID_ICONVIEW1);
					
					/*
					* ���ͼ�굽ͼ��ؼ�
					*/
					for (j = 0 ; j < ICON_PAGE_NUM  && j < GUI_COUNTOF(_aBitmapItem2); j++)
					{    

						ICONVIEW_AddBitmapItem(hWin, _aBitmapItem2[j].pBitmap, _aBitmapItem2[j].pText);
					}		
        #endif
		break;

		case WM_NOTIFY_PARENT:

					Id    = WM_GetId(pMsg->hWinSrc);      // �ؼ���ID
					NCode = pMsg->Data.v;                 // ֪ͨ����
	 
						switch (Id) {
							
						case GUI_ID_ICONVIEW0:								// ͼ��ؼ�0
							
							switch (NCode) {
								case WM_NOTIFICATION_RELEASED:
								//case WM_NOTIFICATION_SEL_CHANGED:
										/*
										* ��ȡ��ǰѡ�е�ͼ��
										*/
										Sel   = ICONVIEW_GetSel(pMsg->hWinSrc);
										
										switch(Sel)
										{
#ifndef GUI_DEVELOP											
											case 0:
													GUI_Delay(500);								//������ʱ����ʾ����Ч��
											
													ShowTips(pMsg->hWin);					//��ʾ���û��ȴ�

													WFGUI_SDView();	
												break;
											
											case 1:
													GUI_Delay(500);								//������ʱ����ʾ����Ч��

													ShowTips(pMsg->hWin);					//��ʾ���û��ȴ�

													
													WFGUI_Time();
											
												break;	
                      
                     case 2:
                          
                          GUI_Delay(500);										

                          ShowTips(pMsg->hWin);					//��ʾ���û��ȴ�
                        
                          WFGUI_Temperature();
                          
                        break;
                      
                      case 3:
                          GUI_Delay(500);										

                          ShowTips(pMsg->hWin);					//��ʾ���û��ȴ�
                        
                          WFGUI_ImageReader();
                        
                      break;
                    
                      case 4:
                          GUI_Delay(500);										

                          ShowTips(pMsg->hWin);					//��ʾ���û��ȴ�
                        
                          WFGUI_TextReader();
                        
                      break;
											
											case 5:
												GUI_Delay(500);								//������ʱ����ʾ����Ч��

												ShowTips(pMsg->hWin);					//��ʾ���û��ȴ�

											
												WFGUI_Message();											
											
												break;
											
											case 6:
												
												GUI_Delay(500);								//������ʱ����ʾ����Ч��										

												ShowTips(pMsg->hWin);					//��ʾ���û��ȴ�

											
												WFGUI_Phone();
											
											
											break;
										
											case 7:
												
												GUI_Delay(500);										

												ShowTips(pMsg->hWin);					//��ʾ���û��ȴ�
											
												WFGUI_Camera();
												
											break;
										
																				
#else

								
								case 0:

									GUI_Delay(500);								//������ʱ����ʾ����Ч��

									ShowTips(pMsg->hWin);					//��ʾ���û��ȴ�

									WFGUI_Temperature(); 

								break;
#endif											
												
									default:
										break;
										}	

						/* ��ȡICON�ľ�� */
						hItem = WM_GetDialogItem(hParent, GUI_ID_ICONVIEW0);
					
						/* ���ó�ʼѡ���ͼ��Ϊ -1 (��ʾ��δѡ��)*/
						ICONVIEW_SetSel(hItem,-1);										

							break;//WM_NOTIFICATION_RELEASED
						}				//NCODE
						

						
				break;			//ICONVIEW0
						
				case GUI_ID_ICONVIEW1:								// ͼ��ؼ�1
					
					if(NCode == WM_NOTIFICATION_RELEASED)
					{
						
						/*
						* ��ȡ��ǰѡ�е�ͼ��
						*/
						Sel   = ICONVIEW_GetSel(pMsg->hWinSrc);
						
						switch(Sel)
						{
							case 0:
              case 1:
              case 2:
								
									GUI_Delay(500);										

									ShowTips(pMsg->hWin);					//��ʾ���û��ȴ�
								
                  WFGUI_UserAPP();

								break;
							
							
							default:
								break;

							}
							
									
					/* ��ȡICON�ľ�� */
					hItem = WM_GetDialogItem(hParent, GUI_ID_ICONVIEW1);
				
					/* ���ó�ʼѡ���ͼ��Ϊ -1 (��ʾ��δѡ��)*/
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
							pInfo->SnapX = 240;		//���ض���
							pInfo->SnapY = 0;
							pInfo->Period = 500;

							break;
							
							case WM_MOTION_MOVE:
									/* ��ֹͼ��ؼ� ����ֹ�󴥷�*/
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
						/* �ƶ����� */
						if (pInfo->FinalMove)
						{
								/* �ƶ�������ʹ��ͼ��ؼ� */
								for(i=0;i<VIRTUAL_WIN_NUM;i++)
								{
									hWin = WM_GetDialogItem(pMsg->hWin,GUI_ID_ICONVIEW0+i);
									WM_EnableWindow(hWin);
								}
								

								if(pInfo->xPos >= -120 && pInfo->xPos )	//xposָ�򴰿��ƶ��ľ��룬�����ƶ�Ϊ����һ���һ�����ƶ�����ʱxpos=-1
								{
									WinPara.ctr_index = 0;			//��һ������
									
									WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);

									//WM_Paint(WinPara.hWinCtrl);	//�ػ�ctrl���ڣ�ˢ��index

									}
								else if(pInfo->xPos < -120)		//һ��ڶ������ƶ�����ʱxpos=-239
								{
									WinPara.ctr_index = 1;			//�ڶ�������
									WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);

								//	WM_Paint(WinPara.hWinCtrl);	//�ػ�ctrl���ڣ�ˢ��index
								}
							

						}
				 }	

			break;

	 case WM_PAINT:	//�ػ汳��
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
  * @brief  CreateFont,ʹ��SD�����ֿ��ļ���������
	*					
  * @param  none
  * @retval none
  */
static void CreateFont(SD_FONT *ExFont)
{
	FRESULT res;

  res = f_open(&ExFont->hFontFile,XBF_FONT_PATH,FA_READ|FA_OPEN_EXISTING);
	if (res == FR_OK) {
		
	 /* �ɹ����ֿ��ļ� */
	GUI_XBF_CreateFont(	&ExFont->XFont,             						// ָ�򴴽��õ��������ݽṹ�洢λ��
											&ExFont->XBF_Data,         							// ָ��XBF_DATA���ݽṹ
											GUI_XBF_TYPE_PROP_AA2_EXT, 							// Ҫ��������������
											_cbGetData,        											// ��ȡ�ֿ��ļ��Ļص�����
											&ExFont->hFontFile);            				// ָ���ֿ��ļ����ļ�����
		
	/* ����GUI���� */
	GUI_SetFont(&ExFont->XFont);
		
	/* �����ı����� */	
	TEXT_SetDefaultFont(&ExFont->XFont);
		
	/* ����GUI������� */
	GUI_UC_SetEncodeUTF8();
	}


}

/**
  * @brief  SetDefaultSkin,����gui��Ĭ��Ƥ��
	*					
  * @param  none
  * @retval none
  */
static void SetDefaultSkin(void)
{
	/* ���ø��ֿؼ���Ƥ��Ϊ��win7��Ƥ�� */
  BUTTON_SetDefaultSkin   (BUTTON_SKIN_FLEX);
  CHECKBOX_SetDefaultSkin (CHECKBOX_SKIN_FLEX);
  DROPDOWN_SetDefaultSkin (DROPDOWN_SKIN_FLEX);
  FRAMEWIN_SetDefaultSkin (FRAMEWIN_SKIN_FLEX);
  HEADER_SetDefaultSkin   (HEADER_SKIN_FLEX);
  PROGBAR_SetDefaultSkin  (PROGBAR_SKIN_FLEX);
  RADIO_SetDefaultSkin    (RADIO_SKIN_FLEX);
  SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
  SLIDER_SetDefaultSkin   (SLIDER_SKIN_FLEX);
	
	/* ���û����� */
	SCROLLBAR_SetDefaultWidth(15);						//���
	SCROLLBAR_SetThumbSizeMin(10);						//������Сsize	
	
	/* ����sd���ⲿ���� */
	CreateFont(&SDFont);
	
	/* ����Ĭ������ */
	TEXT_SetDefaultFont(&SDFont.XFont);

	/* ����framwin  */
	FRAMEWIN_SetDefaultTitleHeight(20);					//�������߶�
	FRAMEWIN_SetDefaultFont(GUI_FONT_8X16);			//Ĭ������
	FRAMEWIN_SetDefaultTextColor(1,GUI_BLACK); 	//���ü���״̬ʱ�ı���������ɫ
	WM_SetDesktopColor(GUI_BLACK);
}

/**
  * @brief  CreatStatusWin������������״̬��
  * @param  none
  * @retval none
  */
static void CreatStatusWin(void)
{
	WM_HWIN hText;
	WM_HWIN hButton;

	WinPara.hWinStatus = WM_CreateWindowAsChild(
																							0,				//����λ�á���С
																							0,
																							WinPara.xSizeLCD,
																							WinPara.yPosWin,	
																							WM_HBKWIN, WM_CF_SHOW | WM_CF_MEMDEV, _cbStatusWin, sizeof(WIN_PARA *)
																						);
	
	/* ������ "wildfire OS "�ı� */
	hText = TEXT_CreateEx(0, 0, WinPara.xSizeWin , 20, WinPara.hWinStatus, WM_CF_SHOW, GUI_TA_HCENTER|TEXT_CF_VCENTER, GUI_ID_TEXT1, "Wildfire OS");
  TEXT_SetFont(hText, GUI_FONT_16B_ASCII);
  TEXT_SetTextColor(hText, 0xFF4040);
	
	/* ״̬����ʱ����ʾ�ı� */
	hText = TEXT_CreateEx(WinPara.xSizeLCD-55,5,55,20,WinPara.hWinStatus,WM_CF_SHOW,TEXT_CF_LEFT,GUI_ID_TEXT0,"");
	TEXT_SetBkColor(hText,GUI_INVALID_COLOR);
	TEXT_SetTextColor(hText,GUI_WHITE);
	TEXT_SetFont(hText,GUI_FONT_13B_ASCII);
	
	/* Ұ��Logo ��ť */
	hButton = BUTTON_CreateEx(2,0,40,20,WinPara.hWinStatus,WM_CF_SHOW,0,GUI_ID_BUTTON0);
	BUTTON_SetBitmap(hButton,BUTTON_BI_UNPRESSED,&bmWF_Logo);
	BUTTON_SetBitmap(hButton,BUTTON_BI_PRESSED,&bmWF_LogoPr);	

}



/**
  * @brief  CreatCtrlWin�������ײ��Ŀ�����
  * @param  none
  * @retval none
  */
static void CreatCtrlWin(void)
{
	WM_HWIN hButtonWin;
	
	
	WinPara.hWinCtrl = WM_CreateWindowAsChild(
																							0,											
																							WinPara.yPosWin +WinPara.ySizeWin ,	//�ײ�λ��
																							WinPara.xSizeLCD,
																							WinPara.ySizeLCD -(WinPara.yPosWin +WinPara.ySizeWin),	//�ײ�ʣ����
																							WM_HBKWIN, WM_CF_SHOW | WM_CF_MEMDEV, _cbCtrlWin, sizeof(WIN_PARA *)
																						);
	
	/* ������������ (��СԲ���ʾ��ǰ��ͼ��ҳ��)*/
	WM_CreateWindowAsChild(0,0,WM_GetWindowSizeX(WinPara.hWinCtrl),WM_GetWindowSizeY(WinPara.hWinCtrl),
													WinPara.hWinCtrl,WM_CF_SHOW,_cbIndexWin,0);
													
	/* ��������button����(��button��Ӧ�ó���״̬�� ����ʾ���ذ�ť) */
	hButtonWin = WM_CreateWindowAsChild(0,0,WM_GetWindowSizeX(WinPara.hWinCtrl),WM_GetWindowSizeY(WinPara.hWinCtrl),
													WinPara.hWinCtrl,WM_CF_SHOW,_cbButtonWin,0);
	/* Ĭ������button���� */	
	GUI_Delay(10);	
	WM_HideWindow(hButtonWin);
	GUI_Delay(10);
													
}


/**
  * @brief  CreatMainWin����������ʾ����
  * @param  none
  * @retval none
  */
static void CreatMainWin(void)
{
	
	WinPara.hWinMain = WM_CreateWindowAsChild(
																							WinPara.xPosWin ,											
																							WinPara.yPosWin ,	//λ��
																							WinPara.xSizeWin,
																							WinPara.ySizeWin,	//�ײ�ʣ����
																							WM_HBKWIN, WM_CF_SHOW | WM_CF_MEMDEV, _cbMainWin, sizeof(WIN_PARA *)
																						);	
		/* ����iconͼ�괰�� */
	 WM_CreateWindowAsChild(	0 ,											
														0 ,	//λ��
														WinPara.xSizeWin * VIRTUAL_WIN_NUM,
														WinPara.ySizeWin,	//��
														WinPara.hWinMain,
														WM_CF_SHOW | WM_CF_MEMDEV|WM_CF_MOTION_X,
														_cbIconWin,
														0
														);							
																	
	

}




/**
  * @brief  CreatDeskTop������gui����
  * @param  none
  * @retval none
  */
static void CreatDeskTop(void)
{

	//׼������3�����ڣ�������ʹ�õ����û���������������ڻص�������ʹ��
	WinPara.xSizeLCD = LCD_GetXSize();						//LCD��Ļ�ߴ�
	WinPara.ySizeLCD = LCD_GetYSize();						//LCD��Ļ�ߴ�
	WinPara.xSizeWin = WinPara.xSizeLCD;					//���ڳߴ�
	WinPara.ySizeWin = WinPara.ySizeLCD -20*2;		//���ڳߴ� ��Ļ��С��ȥ״̬���Ϳ�����
	WinPara.xPosWin	 = 0;													//���ڵ���ʼλ��
	WinPara.yPosWin  = 20;												//���ڵ���ʼλ��
	WinPara.xSizeWinVirtual = WinPara.xSizeWin * VIRTUAL_WIN_NUM;	//���ⴰ�ڳߴ磬��������
	
	/* ����Ĭ��Ƥ�� */
	SetDefaultSkin();

		
  WM_EnableMemdev(WM_HBKWIN);
	
	/* ���ñ�������Ļص����� */
  WM_SetCallback(WM_HBKWIN, _cbBKWin);	
	
	/* ������������ ״̬������������������*/
	CreatStatusWin();
	CreatCtrlWin();
	CreatMainWin();
	
	/* ��ʼ��app���� */													
	hAPPLinkedList_Init();	

	
}


/*********************************************************************
*
*       Public routines
*				���к���
*
**********************************************************************
*/

/**
  * @brief  WFGUI_MainTask,WFGUI������
  * @param  none
  * @retval none
  */
void WFGUI_MainTask(void) {
  
	WM_MOTION_Enable(1);
  WM_MOTION_SetDefaultPeriod(1000);
	
	CreatDeskTop();			//����gui����

  while (1) {
		
    GUI_Delay(50);

  }
}

/*************************** End of file ****************************/

