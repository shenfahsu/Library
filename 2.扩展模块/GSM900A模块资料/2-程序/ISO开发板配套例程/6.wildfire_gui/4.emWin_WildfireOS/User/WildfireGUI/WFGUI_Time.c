/**
  ******************************************************************************
  * @file    WFGUI_Time.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ʱ��Ӧ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#include "bsp_rtc.h"

#include "WFGUI_Time.h"
#include "DIALOG.h"
#include "LISTWHEEL.h"
#include "BUTTON.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define LEFT   (1 << 0)
#define MIDDLE (1 << 1)
#define RIGHT  (1 << 2)
#define TOP    (1 << 3)
#define BOTTOM (1 << 4)

#define LIGHTBLUE 0xEE8822

#define GET_MODE (WM_USER + 0)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct {
  WM_HWIN           hWin;
  GUI_MEMDEV_Handle hMemOverlay;
  GUI_MEMDEV_Handle hMemRBorder;
  GUI_MEMDEV_Handle hMemLBorder;
  const GUI_FONT GUI_UNI_PTR * pFont;
} WHEEL;



/*********************************************************************
*
*       wheel ����
*/
static char * _apYear[] = {
"1970","1971","1972","1973","1974","1975","1976","1977","1978","1979","1980","1981","1982","1983","1984","1985","1986","1987","1988","1989","1990","1991",
"1992","1993","1994","1995","1996","1997","1998","1999","2000","2001","2002","2003","2004","2005","2006","2007","2008","2009","2010","2011","2012","2013",
"2014","2015","2016","2017","2018","2019","2020","2021","2022","2023","2024","2025","2026","2027","2028","2029","2030","2031","2032","2033","2034","2035",
"2036","2037","2038",
};

static char * _apMonth[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};

static char * _apDay[] = {
  "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31",
};

static char * _apHour[] =
{
"00","01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", NULL

};
static char * _apNumList[] =
{
"00","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30",
"31","32","33","34","35","36","37","38","39","40","41","42","43","44","45","46","47","48","49","50","51","52","53","54","55","56","57","58","59",
NULL

};



//͸����ɫ
#define TRANS0 0x11000000
#define TRANS1 0xEE000000
#define TRANS2 0xFF000000 

static WM_CALLBACK*     _pcbOldTimeWin = NULL;
/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/**
  * @brief  _OwnerDraw listwheel���Ի�����
	*					
  * @param  none
  * @retval none
  */
static int _OwnerDraw(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo) {
  int xSizeWin, ySizeWin;
  WM_HWIN hWin;

  hWin = pDrawItemInfo->hWin;
  //
  // Process messages
  //
  switch (pDrawItemInfo->Cmd) {
  case WIDGET_ITEM_GET_XSIZE:
    //
    // Return x-size of item
    //
    return LISTWHEEL_OwnerDraw(pDrawItemInfo);
  case WIDGET_ITEM_GET_YSIZE:
    //
    // Return y-size of item
    //
    return LISTWHEEL_OwnerDraw(pDrawItemInfo);
  case WIDGET_ITEM_DRAW:
    //
    // Draw item
    //
    return LISTWHEEL_OwnerDraw(pDrawItemInfo);
  case WIDGET_ITEM_DRAW_BACKGROUND:
    xSizeWin = WM_GetWindowSizeX(hWin);
    ySizeWin = WM_GetWindowSizeY(hWin);
    //
    // Draw background
    //
    GUI_SetColor(0x00FFFFFF);
    GUI_FillRect(4, 0, xSizeWin - 5, ySizeWin - 1);
    break;
  case WIDGET_ITEM_DRAW_OVERLAY:


		xSizeWin = WM_GetWindowSizeX(hWin);
		ySizeWin = WM_GetWindowSizeY(hWin);


		//
		// Create left border
		//
		GUI_SetColor(GUI_BLACK);
		GUI_DrawVLine(1, 0, ySizeWin - 1);
		GUI_SetColor(0x00CCCCCC);
		GUI_FillRect(1, 0, 3, ySizeWin - 1);


		//
		// Create right border
		//
		GUI_SetColor(GUI_BLACK);
		GUI_DrawVLine(2, 0, ySizeWin - 1);
		GUI_SetColor(GUI_LIGHTGRAY);
		GUI_FillRect(0, 0, 2, ySizeWin - 1);
		GUI_EnableAlpha(1);
		//
		// Create overlay device
		//
		//
		// Gray gradients at top
		//
		GUI_DrawGradientV(0,0,xSizeWin - 1,(ySizeWin * 2) / 10 - 1,TRANS0,TRANS1);
		GUI_DrawGradientV(0, (ySizeWin * 2) / 10,xSizeWin - 1, (ySizeWin / 2) - (30 / 2) - 1, TRANS1,TRANS2);
		

		//
		// Gray gradients at bottom
		//
		GUI_DrawGradientV(0, (ySizeWin / 2) + (30 / 2),xSizeWin - 1,(ySizeWin / 2) + (30 / 2) + 2, 0x88000000,TRANS2);
		GUI_DrawGradientV(0, (ySizeWin / 2) + (30 / 2) + 3, xSizeWin - 1, ySizeWin - (ySizeWin * 2) / 10 - 1,TRANS2,TRANS1);
		GUI_DrawGradientV(0, ySizeWin - (ySizeWin * 2) / 10,xSizeWin - 1, ySizeWin - 1,TRANS1,TRANS0);
		GUI_DrawGradientV(0, ySizeWin - 1,xSizeWin - 1, ySizeWin - 1,0x33000000, 0x33000000);
		//
		// Translucent glass effect in the middle
		//
		GUI_SetColor(0x80000000);
		GUI_DrawHLine((ySizeWin / 2) - (30 / 2), 0, xSizeWin - 1);
		GUI_SetColor(0x44000000);
		GUI_DrawHLine((ySizeWin / 2) + (30 / 2) - 1, 0, xSizeWin - 1);
		GUI_DrawGradientV(1, (ySizeWin / 2) - (30 / 2) + 1, xSizeWin - 2, (ySizeWin / 2) - 1,0x88FFFFFF, 0x55AAAAAA);
		GUI_DrawGradientV(1, (ySizeWin / 2),xSizeWin - 2,(ySizeWin / 2) + (30 / 2) - 2, 0xBB000000, 0xBB000000);

		//
		// Store result
		//

		 GUI_EnableAlpha(0);


    break;
  }
  return 0;
}




/**
  * @brief  _CreateListWheel ����ListWheel�Ļص�����
	*					
  * @param  none
  * @retval none
  */
static int _CreateListWheel(int x, int y, int xSize, int ySize, int Id, char ** apText, int NumItems, int Side, int TextAlign, WM_HWIN hParent) {
  WM_HWIN           hWin;
  int i, LineHeight;

  //
  // Create LISTWHEEL object
  //
  LineHeight    = 30;
	hWin          = LISTWHEEL_CreateEx(x, y, xSize, ySize, hParent, WM_CF_SHOW | WM_CF_HASTRANS, 0, Id,NULL);
	LISTWHEEL_SetFont(hWin, GUI_FONT_8X16);
  LISTWHEEL_SetTextAlign(hWin, TextAlign);
  LISTWHEEL_SetSnapPosition(hWin, (ySize - LineHeight) / 2);
	#if 1
  LISTWHEEL_SetOwnerDraw(hWin, _OwnerDraw);
	#endif
  LISTWHEEL_SetLineHeight(hWin, LineHeight);
  LISTWHEEL_SetTextColor(hWin, LISTWHEEL_CI_SEL, LIGHTBLUE);
  for (i = 0; i < NumItems; i++) {
    LISTWHEEL_AddString(hWin, *(apText + i));
  }
  if (TextAlign & GUI_TA_RIGHT) {
    LISTWHEEL_SetRBorder(hWin, 10);
  }

  
  return 0;
}




/**
  * @brief  _cbTimeWin framewin��ܵĻص�����
	*					
  * @param  none
  * @retval none
  */
static void _cbTimeWin(WM_MESSAGE * pMsg)
{
	HANDLE_LIST	*appNode;
	
		switch (pMsg->MsgId) {			//��Ϣ����

#if 1			//��֣�����Ҫ�������
		case WM_NOTIFY_PARENT:		//֪ͨ		
			break;//֪ͨ����break
#endif	
			
			case WM_DELETE:
				
				/* ��ȡapp�����Ӧ�������� */
				appNode = hAPPLinkedList_GetAppNode(pMsg->hWin);
				if(appNode != NULL)
				{
					/* ɾ��app���������ļ�¼ */	
					hAPPLinkedList_Del(appNode);
				
					/* ������Ϣ֪ͨctrl����*/		
					WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);	
				}
			
				break;
		
			default:		
      WM_DefaultProc(pMsg);
			
		}//MsgId
	
	if (_pcbOldTimeWin) {
			(*_pcbOldTimeWin)(pMsg);	//���þɵĻص�����������δ�������
		}	
	
}

/**
  * @brief  _cbBkWheel,wheel�Ļص����ڣ����������Ϣ
  * @param  pMsg
  * @retval none
  */
static void _cbBkWheel(WM_MESSAGE * pMsg) {
  int xSize, ySize;
	int        Id;
  int        NCode;
	
	char text_buffer[30];
	
	uint16_t ListWheelPosY;
	WM_HWIN hText;
	WM_HWIN hListWheel;
	struct rtc_time SrcTm;
	
	static char WheelChange = 0;
	
	ListWheelPosY = WM_GetWindowSizeY(pMsg->hWin)-140;  //140������ʾlistwheel��20������ʾbutton


  switch (pMsg->MsgId) {
 
		case WM_NOTIFY_PARENT:
			
					Id    = WM_GetId(pMsg->hWinSrc);      // �ؼ���ID
					NCode = pMsg->Data.v;  
		
					switch(NCode)
					{
							//�����е�listwheel�ı䶼�������²�����ѡ�����������ʹ�������ɫ��
							case WM_NOTIFICATION_SEL_CHANGED:
							
							LISTWHEEL_SetSel(pMsg->hWinSrc,LISTWHEEL_GetPos(pMsg->hWinSrc));
							break;
							
							case WM_NOTIFICATION_RELEASED:
								if(Id == GUI_ID_OK)
								{
									/* �� */
									hListWheel 		= WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTWHEEL0);
									SrcTm.tm_year = LISTWHEEL_GetSel(hListWheel);
									SrcTm.tm_year += 1970;
									
									/* �� */
									hListWheel 		= WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTWHEEL1);
									SrcTm.tm_mon 	= LISTWHEEL_GetSel(hListWheel);
									SrcTm.tm_mon	+= 1;
									
									/* �� */
									hListWheel 		= WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTWHEEL2);
									SrcTm.tm_mday = LISTWHEEL_GetSel(hListWheel);
									SrcTm.tm_mday += 1;
									
									/* ʱ */
									hListWheel 		= WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTWHEEL3);
									SrcTm.tm_hour = LISTWHEEL_GetSel(hListWheel);
									
									/* �� */
									hListWheel 		= WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTWHEEL4);
									SrcTm.tm_min = LISTWHEEL_GetSel(hListWheel);
									
									/* �� */
									hListWheel 		= WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTWHEEL5);
									SrcTm.tm_sec = LISTWHEEL_GetSel(hListWheel);
									
									
									Time_Adjust_LCD(&systmtime,&SrcTm);
									
									
									
								}
								break;
							
							default:
								break;
					}
    break;
	

	
	case MY_MESSAGE_RTC:		

		/* ת��rtcֵ������ʱ�� ����status�����Ѿ�ת�����ˣ�����ת�� */
		//RTC_TimeCovr(&systmtime);

		/* ת�����ַ��� */
		sprintf(text_buffer,"%02d:%02d",systmtime.tm_hour,systmtime.tm_min);
		/* ��ȡtext��� */
		hText = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
		/* ���ʱ�� */
		TEXT_SetText(hText,text_buffer);
	
		/* ת�����ַ��� */
		sprintf(text_buffer,"%02d",systmtime.tm_sec);
		
    /* ��ȡtext��� */
		hText = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
		
    /* ���ʱ�� */
		TEXT_SetText(hText,text_buffer);
	
		/* ת�����ַ��� */
		sprintf(text_buffer,"%04d.%02d.%02d",systmtime.tm_year,systmtime.tm_mon,systmtime.tm_mday);
		
    /* ��ȡtext��� */
		hText = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT3);
		
    /* ���ʱ�� */
		TEXT_SetText(hText,text_buffer);
		
		/* ��ȡtext��� */
		hText = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2);
		
		/* ������� */
		switch(systmtime.tm_wday)
		{
			case 1:
				/* ���ʱ�� */
				TEXT_SetText(hText,"Monday");				
				break;
			
				case 2:
				/* ���ʱ�� */
				TEXT_SetText(hText,"Tuesday");				
				break;
									
				case 3:
				/* ���ʱ�� */
				TEXT_SetText(hText,"Wednesday");				
				break;
												
				case 4:
				/* ���ʱ�� */
				TEXT_SetText(hText,"Thursday");				
				break;
				
				case 5:
				/* ���ʱ�� */
				TEXT_SetText(hText,"Friday");				
				break;
															
				case 6:
				/* ���ʱ�� */
				TEXT_SetText(hText,"Saturday");				
				break;			
				
				case 7:
				/* ���ʱ�� */
				TEXT_SetText(hText,"Sunday");				
				break;
			
			default:
				break;

			}
			
			if(WheelChange == 0)
			{
				/* �� */
				hListWheel = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTWHEEL0);
				LISTWHEEL_MoveToPos(hListWheel,systmtime.tm_year - 1970);
				LISTWHEEL_SetSel(hListWheel,systmtime.tm_year - 1970);
				
				/* �� */
				hListWheel = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTWHEEL1);
				LISTWHEEL_MoveToPos(hListWheel,systmtime.tm_mon - 1);
				LISTWHEEL_SetSel(hListWheel,systmtime.tm_mon - 1);
				
				/* �� */
				hListWheel = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTWHEEL2);
				LISTWHEEL_MoveToPos(hListWheel,systmtime.tm_mday - 1);
				LISTWHEEL_SetSel(hListWheel,systmtime.tm_mday - 1);
				
				/* ʱ */
				hListWheel = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTWHEEL3);
				LISTWHEEL_MoveToPos(hListWheel,systmtime.tm_hour );
				LISTWHEEL_SetSel(hListWheel,systmtime.tm_hour );
				
				/* �� */
				hListWheel 		= WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTWHEEL4);
				LISTWHEEL_MoveToPos(hListWheel,systmtime.tm_min );
				LISTWHEEL_SetSel(hListWheel,systmtime.tm_min);
				
				/* �� */
				hListWheel = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTWHEEL5);
				LISTWHEEL_MoveToPos(hListWheel,systmtime.tm_sec );
				LISTWHEEL_SetSel(hListWheel,systmtime.tm_sec);

				
				WheelChange = 1;

				}
			

	
		break;
		
  case WM_PAINT:                                            //�ػ汳��
    xSize = WM_GetWindowSizeX(pMsg->hWin);
    ySize = WM_GetWindowSizeY(pMsg->hWin);		

    GUI_DrawGradientV(0, 0, xSize - 1, ySize - 1, 0xfffff56c, 0xffffffe8);
		GUI_DrawGradientV(0,0,240,ListWheelPosY,0xffff0d23,0xfffff56c);

    break;
  default:
    WM_DefaultProc(pMsg);
  }
}




/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/**
  * @brief  WFGUI_Time,ʱ����ʾ
  * @param  none
  * @retval none
  */
void WFGUI_Time(void) {
  WM_HWIN hBkWheel;
  WM_HWIN hFrameC;								//�Ӵ��ھ��
	WM_HWIN hText;
	WM_HWIN hButton;
	uint16_t ListWheelPosY;
	
	HANDLE_LIST *hFrame = hAPPLinkedList_NewNode();

	WM_SetCreateFlags(WM_CF_MEMDEV);
	
	hFrame->hAPP = FRAMEWIN_CreateEx( 0,
                                    0,
                                    WinPara.xSizeWin,
                                    WinPara.ySizeWin,
                                    WinPara.hWinMain,
                                    WM_CF_SHOW,
                                    FRAMEWIN_CF_ACTIVE|FRAMEWIN_CF_MOVEABLE,GUI_ID_FRAMEWIN8,
                                    "Time",
                                    0);

	/* �������ڰ�ť */
  FRAMEWIN_AddCloseButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 0);
  FRAMEWIN_AddMaxButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 1);
  FRAMEWIN_AddMinButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 2);
	
	/* ��ӽ�㵽���� */
	hAPPLinkedList_AddTail(hFrame);
	/* ��ctrl���ڷ�����Ϣ */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	_pcbOldTimeWin = WM_SetCallback(hFrame->hAPP, _cbTimeWin);	//��ȡ�ɵĻص�����ָ��
	
	/* ��ȡ��ܴ����û����ľ�� */
	hFrameC = WM_GetClientWindow(hFrame->hAPP); 
	
	ListWheelPosY = WM_GetWindowSizeY(hFrameC)-140;  //140������ʾlistwheel��20������ʾbutton

	//
  // ������������ʾlistwheel
  //
	hBkWheel = WM_CreateWindowAsChild(0, 0, WM_GetWindowSizeX(hFrameC), WM_GetWindowSizeY(hFrameC), hFrameC, WM_CF_SHOW, _cbBkWheel, 0);
	
	/* ������ʾ��ǰʱ����ı� */
	//������ʾʱ��
	hText = TEXT_CreateEx(0,ListWheelPosY-40,WM_GetWindowSizeX(hFrameC),40,hBkWheel,WM_CF_SHOW,GUI_TA_LEFT,GUI_ID_TEXT4," Time Setting:");
	TEXT_SetFont(hText,&GUI_Font20B_ASCII);
	TEXT_SetTextColor(hText,GUI_RED);
	TEXT_SetBkColor(hText,GUI_WHITE);
	
  //������ʱ���� ע��
	hText = TEXT_CreateEx(0,ListWheelPosY-15,WM_GetWindowSizeX(hFrameC),40,hBkWheel,WM_CF_SHOW,GUI_TA_LEFT,GUI_ID_TEXT5,"  Year    Month  Day   Hour  Min  Sec");
	TEXT_SetFont(hText,&GUI_Font16B_ASCII);
	TEXT_SetTextColor(hText,GUI_BLUE);
	TEXT_SetBkColor(hText,GUI_WHITE);


	/* OK ���� */
	hButton = BUTTON_CreateEx(WM_GetWindowSizeX(hFrameC)-80,ListWheelPosY-38,40,20,hBkWheel,WM_CF_SHOW,0,GUI_ID_OK);
	BUTTON_SetText(hButton,"OK");
	//
  // ����ListWheel
  //  
  _CreateListWheel(0,ListWheelPosY,  50, 140, GUI_ID_LISTWHEEL0, _apYear,  GUI_COUNTOF(_apYear),  RIGHT,  GUI_TA_VCENTER | GUI_TA_HCENTER, hBkWheel);
  _CreateListWheel(50,ListWheelPosY, 45, 140, GUI_ID_LISTWHEEL1, _apMonth, GUI_COUNTOF(_apMonth), MIDDLE, GUI_TA_VCENTER | GUI_TA_RIGHT,   hBkWheel);
  _CreateListWheel(95,ListWheelPosY,  35, 140, GUI_ID_LISTWHEEL2, _apDay,   GUI_COUNTOF(_apDay),   LEFT,   GUI_TA_VCENTER | GUI_TA_HCENTER, hBkWheel);
  _CreateListWheel(130,ListWheelPosY,  35, 140, GUI_ID_LISTWHEEL3, _apHour,   GUI_COUNTOF(_apHour),  RIGHT,  GUI_TA_VCENTER | GUI_TA_HCENTER, hBkWheel);
  _CreateListWheel(165,ListWheelPosY, 35, 140, GUI_ID_LISTWHEEL4, _apNumList, GUI_COUNTOF(_apNumList), MIDDLE, GUI_TA_VCENTER | GUI_TA_RIGHT,   hBkWheel);
  _CreateListWheel(200,ListWheelPosY,  35, 140, GUI_ID_LISTWHEEL5, _apNumList,   GUI_COUNTOF(_apNumList),   LEFT,   GUI_TA_VCENTER | GUI_TA_HCENTER, hBkWheel);

	
	GUI_Delay(50);	
	
	/* ������ʾ��ǰʱ����ı� */
	//������ʾʱ��
	hText = TEXT_CreateEx(10,20,70,40,hBkWheel,WM_CF_SHOW,GUI_TA_LEFT,GUI_ID_TEXT0,"");
	TEXT_SetFont(hText,&GUI_Font32B_ASCII);
	TEXT_SetTextColor(hText,GUI_WHITE);
	
	//��ʾ��
	hText = TEXT_CreateEx(90,28,30,40,hBkWheel,WM_CF_SHOW,GUI_TA_LEFT,GUI_ID_TEXT1,"");
	TEXT_SetFont(hText,&GUI_Font20B_ASCII);
	TEXT_SetTextColor(hText,GUI_WHITE);
	
	//��ʾ����
	hText = TEXT_CreateEx(130,15,80,40,hBkWheel,WM_CF_SHOW,GUI_TA_RIGHT,GUI_ID_TEXT2,"");
	TEXT_SetFont(hText,&GUI_Font16B_ASCII);
	TEXT_SetTextColor(hText,GUI_WHITE);
	
	//��ʾ����
	hText = TEXT_CreateEx(130,32,80,40,hBkWheel,WM_CF_SHOW,GUI_TA_RIGHT,GUI_ID_TEXT3,"");
	TEXT_SetFont(hText,&GUI_Font13B_ASCII);
	TEXT_SetTextColor(hText,GUI_WHITE);
	
	//��ȫ�ֱ�����ֵ����������rtc�жϷ�����Ϣʱ�õ�
	WinPara.hWinTime = hBkWheel;
  

}

/*************************** End of file ****************************/


