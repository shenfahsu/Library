/**
  ******************************************************************************
  * @file    WFGUI_Temperature.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   实时温度显示器
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include <stdlib.h>
#include <string.h>

#include "DIALOG.h"
#include "GRAPH.h"
#include "WFGUI_Common.h"
#include "WFGUI_Temperature.h"

#include "bsp_TIMbase.h"
#include "bsp_dht11.h"
#include "bsp_ds18b20.h"
#include "bsp_internal_temp.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define MAX_VALUE 180

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static GRAPH_DATA_Handle  _ahData[3]; /* Array of handles for the GRAPH_DATA objects */
static GRAPH_SCALE_Handle _hScaleV;   /* Handle of vertical scale */
static GRAPH_SCALE_Handle _hScaleH;   /* Handle of horizontal scale */

static int _Stop;

static GUI_COLOR _aColor[] = {GUI_RED, GUI_GREEN, GUI_LIGHTBLUE}; /* Array of colors for the GRAPH_DATA objects */

/* Dialog ressource */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Temperature Sensor",  0               ,   0,   0, 240, 280, FRAMEWIN_CF_MOVEABLE },	//框架窗口
  { GRAPH_CreateIndirect,     0,                   GUI_ID_GRAPH0    ,   0,   0, 230, 200 },												//图形控件
  { TEXT_CreateIndirect,      "X:",								 GUI_ID_TEXT0     ,   0, 205,  50,  20 ,TEXT_CF_VCENTER},												//space x文本
  { TEXT_CreateIndirect,      "Y:",								 GUI_ID_TEXT1     ,   0, 225,  50,  20 ,TEXT_CF_VCENTER },												//space y文本
  { SLIDER_CreateIndirect,    0,                   GUI_ID_SLIDER0   ,  10, 205,  60,  16 },												//space x 滑动条
  { SLIDER_CreateIndirect,    0,                   GUI_ID_SLIDER1   ,  10, 225,  60,  16 },												//space y 滑动条
  { CHECKBOX_CreateIndirect,  0,                   GUI_ID_CHECK0    ,  80, 205,  50,   0 },
  { CHECKBOX_CreateIndirect,  0,                   GUI_ID_CHECK1    ,  80, 225,  50,   0 },
  { CHECKBOX_CreateIndirect,  0,                   GUI_ID_CHECK6    , 130, 205,  50,   0 },
  { CHECKBOX_CreateIndirect,  0,                   GUI_ID_CHECK7    , 130, 225,  50,   0 },
  { BUTTON_CreateIndirect,    "Max",       				 GUI_ID_BUTTON0   , 185, 205,  40,  18 },
  { CHECKBOX_CreateIndirect,  0,                   GUI_ID_CHECK8    , 185, 225,  40,   0 },
};


static enum SENSOR_TYPE sensor_type = INTERNAL;	//默认为内部温度传感器

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/




/**
  * @brief  _UserDraw 用户自定义的绘制回调函数
	*					
  * @param  hWin 窗口的句柄
  *					Stage @prg GRAPH_DRAW_FIRST 填充数据区的背景后调用函数。
									@prg GRAPH_DRAW_LAST  绘制所有图形项目后调用函数。
  * @retval none
  */
static void _UserDraw(WM_HWIN hWin, int Stage) {
  if (Stage == GRAPH_DRAW_LAST) {
    GUI_SetFont(&GUI_Font13_ASCII);
		
		switch(sensor_type)
		{
			
			case DHT11:
			//	GUI_DispStringAt("Sensor:DHT11",120,5);
			
				/* 显示温度说明 */
				GUI_SetColor(_aColor[0]);
				GUI_DispStringAt("-- DHT11 temperature:'C",30,5);
			
				/* 显示湿度说明 */
				GUI_SetColor(_aColor[1]);
				GUI_DispStringAt("-- DHT11 humidity:%",30,15);

				break;
			case DS18B20:
				/* 显示温度说明 */
				GUI_SetColor(_aColor[0]);
				GUI_DispStringAt("-- DS18B20 temperature:'C",30,5);

				break;
			case INTERNAL:
				/* 显示温度说明 */
				GUI_SetColor(_aColor[0]);
				GUI_DispStringAt("-- internal chip adc temperature:'C",30,5);

				break;
			
			default:
				break;


	}

  }
}

/**
  * @brief  _ForEach 遍历子窗口时的回调函数，用于隐藏/显示所有子控件
	*					
  * @param  hWin 窗口的句柄
	*					pData 传递的数据
  * @retval none
  */
static void _ForEach(WM_HWIN hWin, void * pData) {
  int Id, FullScreenMode;
  FullScreenMode = *(int *)pData;
  Id = WM_GetId(hWin);
  if ((Id == GUI_ID_GRAPH0) || (Id == GUI_ID_BUTTON0) || (Id == GUI_ID_VSCROLL) || (Id == GUI_ID_HSCROLL)) {
    return;
  }
  if (FullScreenMode) {
    WM_HideWindow(hWin);
  } else {
    WM_ShowWindow(hWin);
  }
}



/**
  * @brief  _ToggleFullScreenMode 反转屏幕的全屏或窗口状态
	*					
  * @param  hDlg 窗口的句柄
  * @retval none
  */
static void _ToggleFullScreenMode(WM_HWIN hDlg) {
  static int FullScreenMode;
  static GUI_RECT Rect;
  static unsigned ScalePos;
  WM_HWIN hGraph, hButton;
  hGraph  = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
  hButton = WM_GetDialogItem(hDlg, GUI_ID_BUTTON0);
  FullScreenMode ^= 1;
  if (FullScreenMode) {			
    WM_HWIN hClient;
    GUI_RECT RectInside;
		
		/* 进入全屏模式 */
    hClient = WM_GetClientWindow(hDlg);
    BUTTON_SetText(hButton, "Back");
    WM_MoveWindow(hButton, 0, 11);
    FRAMEWIN_SetTitleVis(hDlg, 0);
		
		/* 获取窗口大小，保存原窗口大小 */
    WM_GetWindowRectEx(hClient, &RectInside);
    WM_GetWindowRectEx(hGraph, &Rect);
		
		/* 隐藏所有子控件 */
    WM_ForEachDesc(hClient, _ForEach, &FullScreenMode); 
		
		/* 根据窗口调整图形控件全屏 */
    WM_SetWindowPos(hGraph, WM_GetWindowOrgX(hClient), WM_GetWindowOrgX(hClient), RectInside.x1, RectInside.y1);
    ScalePos = GRAPH_SCALE_SetPos(_hScaleH, RectInside.y1 - 20);

  } else {
    /* Return to normal mode */
    BUTTON_SetText(hButton, "Max");
    WM_MoveWindow(hButton, 0, -11);
		
		/* 显示所有子控件 */
    WM_ForEachDesc(WM_GetClientWindow(hDlg), _ForEach, &FullScreenMode); 
		
		/* 还原窗口原大小 */
    WM_SetWindowPos(hGraph, Rect.x0, Rect.y0, Rect.x1 - Rect.x0 + 1, Rect.y1 - Rect.y0 + 1);
    FRAMEWIN_SetTitleVis(hDlg, 1);
    GRAPH_SCALE_SetPos(_hScaleH, ScalePos);
  }
}


/**
  * @brief  _cbCallback 温度计窗口回调函数
	*					
  * @param  pMsg
  * @retval none
  */
static void _cbTEMPWin(WM_MESSAGE * pMsg) {
  int i, NCode, Id, Value;
  WM_HWIN hDlg, hItem;
	HANDLE_LIST *appNode;
	
	
  hDlg = pMsg->hWin;
	
  switch (pMsg->MsgId) {
		
  case WM_INIT_DIALOG:																//初始化
    hItem = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
    /* 添加数据到图形控件 */
	
		/* 产生随机数 */
    for (i = 0; i < GUI_COUNTOF(_aColor); i++) {
//      _aValue[i] = rand() % 180;
      _ahData[i] = GRAPH_DATA_YT_Create(_aColor[i], 500, 0, 0);
      GRAPH_AttachData(hItem, _ahData[i]);
    }
		
    /* 设置图形控件属性 */
    GRAPH_SetGridDistY(hItem, 5);
    GRAPH_SetGridVis(hItem, 1);				//风格可见
    GRAPH_SetGridFixedX(hItem, 1);		//固定x轴
    GRAPH_SetUserDraw(hItem, _UserDraw);
    /* 添加垂直刻度 */
    _hScaleV = GRAPH_SCALE_Create( 15, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 5);
    GRAPH_SCALE_SetTextColor(_hScaleV, GUI_YELLOW);
    GRAPH_AttachScale(hItem, _hScaleV);
		GRAPH_SCALE_SetFactor(_hScaleV,(float)(1.0/SCALE_FACTOR));

		
    /* 添加水平刻度 */
    _hScaleH = GRAPH_SCALE_Create(WM_GetWindowSizeY(hItem)-25, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 10);
    GRAPH_SCALE_SetTextColor(_hScaleH, GUI_DARKGREEN);
    GRAPH_AttachScale(hItem, _hScaleH);
    /* 初始化字体 */
		hItem = WM_GetDialogItem(hDlg, GUI_ID_TEXT0);
		TEXT_SetFont(hItem,GUI_FONT_6X8);
		hItem = WM_GetDialogItem(hDlg, GUI_ID_TEXT1);
		TEXT_SetFont(hItem,GUI_FONT_6X8);
		/* 初始化 check boxes */
    hItem = WM_GetDialogItem(hDlg, GUI_ID_CHECK0);
    CHECKBOX_SetText(hItem, "Stop");
    hItem = WM_GetDialogItem(hDlg, GUI_ID_CHECK1);
    CHECKBOX_SetText(hItem, "Grid");
    CHECKBOX_SetState(hItem, 1);
    hItem = WM_GetDialogItem(hDlg, GUI_ID_CHECK6);
    CHECKBOX_SetText(hItem, "HScroll");
    CHECKBOX_SetState(hItem, 1);
    hItem = WM_GetDialogItem(hDlg, GUI_ID_CHECK7);
    CHECKBOX_SetText(hItem, "VScroll");
    hItem = WM_GetDialogItem(hDlg, GUI_ID_CHECK8);
    CHECKBOX_SetText(hItem, "MirX");
    /* 初始化滑动条 */
    hItem = WM_GetDialogItem(hDlg, GUI_ID_SLIDER0);
    SLIDER_SetRange(hItem, 0, 10);
    SLIDER_SetValue(hItem, 5);
    SLIDER_SetNumTicks(hItem, 6);
    hItem = WM_GetDialogItem(hDlg, GUI_ID_SLIDER1);
    SLIDER_SetRange(hItem, 0, 20);
    SLIDER_SetValue(hItem, 5);
    SLIDER_SetNumTicks(hItem, 6);

    /* 初始化按键窗口 */
    hItem = WM_GetDialogItem(hDlg, GUI_ID_BUTTON0);
    WM_SetStayOnTop(hItem, 1);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
    NCode = pMsg->Data.v;                 /* Notification code */
    switch (NCode) {
    case WM_NOTIFICATION_CLICKED:
      switch (Id) {
      case GUI_ID_BUTTON0:
				/* 切换全屏状态 */
        _ToggleFullScreenMode(hDlg);
        break;
      }
      break;
    case WM_NOTIFICATION_VALUE_CHANGED:
      switch (Id) {
      case GUI_ID_CHECK0:
        /* 切换停止状态 */
        _Stop ^= 1;
        break;
      case GUI_ID_CHECK1:
        /* 切换是否有网格 */
        hItem = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
        GRAPH_SetGridVis(hItem, CHECKBOX_IsChecked(WM_GetDialogItem(hDlg, GUI_ID_CHECK1)));
        break;
      case GUI_ID_SLIDER0:
        /* 设置水平网格大小 */
        hItem = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
        Value = SLIDER_GetValue(pMsg->hWinSrc) * 10;
        GRAPH_SetGridDistX(hItem, Value);
        GRAPH_SCALE_SetTickDist(_hScaleH, Value);
        break;
      case GUI_ID_SLIDER1:
        /* 设置垂直网格大小 */
        hItem = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
        Value = SLIDER_GetValue(pMsg->hWinSrc) * 5;
        GRAPH_SetGridDistY(hItem, Value);
        GRAPH_SCALE_SetTickDist(_hScaleV, Value);
        break;
      case GUI_ID_CHECK6:
        /* 设置是否使用水平滚动条 */
        hItem = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
        if (CHECKBOX_IsChecked(WM_GetDialogItem(hDlg, GUI_ID_CHECK6))) {
          GRAPH_SetVSizeX(hItem, 500);
        } else {
          GRAPH_SetVSizeX(hItem, 0);
        }
        break;
      case GUI_ID_CHECK7:
        /* 设置是否使用垂直滚动条 */
        hItem = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
        if (CHECKBOX_IsChecked(WM_GetDialogItem(hDlg, GUI_ID_CHECK7))) {
          GRAPH_SetVSizeY(hItem, 400);
        } else {
          GRAPH_SetVSizeY(hItem, 0);
        }
        break;
      case GUI_ID_CHECK8:
        /* 设置是否镜像图形 */
        hItem = WM_GetDialogItem(hDlg, GUI_ID_GRAPH0);
        for (i = 0; i < GUI_COUNTOF(_aColor); i++) {
          if (CHECKBOX_IsChecked(WM_GetDialogItem(hDlg, GUI_ID_CHECK8))) {
            GRAPH_DATA_YT_SetAlign(_ahData[i], GRAPH_ALIGN_LEFT);
            GRAPH_DATA_YT_MirrorX (_ahData[i], 1);
          } else {
            GRAPH_DATA_YT_SetAlign(_ahData[i], GRAPH_ALIGN_RIGHT);
            GRAPH_DATA_YT_MirrorX (_ahData[i], 0);
          }
        }
        break;
      }
      break;
    }
    break;
		
		
	case WM_DELETE:
		/* 获取app句柄对应的链表结点 */
		appNode = hAPPLinkedList_GetAppNode(pMsg->hWin);
		if(appNode != NULL)
		{
			/* 删除app句柄链表里的记录 */	
			hAPPLinkedList_Del(appNode);
		
			/* 发送消息通知ctrl窗口*/		
			WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);	
		}
		break;		
		
  default:
    WM_DefaultProc(pMsg);
  }
}





/**
  * @brief  InitTEMPSensor 检测、初始化温度传感器
	*					
  * @param  none
  * @retval none
  */
static void InitTEMPSensor(void)
{
	DHT11_Data_TypeDef DHT11_Data;

	/* 初始化定时器，用于dht11及ds18b20等精确延时 */
	TIM6_Configuration();
	
	/*初始化DTT11的引脚*/
	DHT11_GPIO_Config();

	/* 检测是否有DHT11传感器 */
	if(Read_DHT11(&DHT11_Data) == SUCCESS)	//检测dht11	
	{		
		sensor_type = DHT11;	
	}
	else if(DS18B20_Init() == SUCCESS)			//检测ds18b20
	{
		sensor_type = DS18B20;	
		
		/* DS18B20刚上电的数据是85度，舍弃*/
		DS18B20_Get_Temp();
		GUI_Delay(800);

	}		
	else																		//使用内部温度检测
	{
		Temp_ADC1_Init();

	}
}



/**
  * @brief  GetTEMPData 获取温度数据
	*					
  * @param  none
  * @retval none
  */
static void GetTEMPData(void)
{
	
	DHT11_Data_TypeDef DHT11_Data;

	switch (sensor_type)
	{
		case DHT11:			
			if(Read_DHT11(&DHT11_Data) == SUCCESS)
			{
			  GRAPH_DATA_YT_AddValue(_ahData[0], DHT11_Data.temp_int*SCALE_FACTOR);
				GRAPH_DATA_YT_AddValue(_ahData[1], DHT11_Data.humi_int*SCALE_FACTOR);

			}

			break;
		
		case DS18B20:
			GRAPH_DATA_YT_AddValue(_ahData[0],DS18B20_Get_Temp()*SCALE_FACTOR);
		
			break;
		
		case INTERNAL:
			GRAPH_DATA_YT_AddValue(_ahData[0],Get_ChipTemperature()*SCALE_FACTOR);
			break;
		
		default:
			break;

	}

}



/**
  * @brief  WFGUI_Temperature 温度计主函数
	*					
  * @param  none
  * @retval none
  */
void WFGUI_Temperature(void) {
  WM_HWIN  hGraph = 0;
	
	HANDLE_LIST *hDlg = hAPPLinkedList_NewNode();
	
  WM_SetCreateFlags(WM_CF_MEMDEV);
	
	/* 根据资源表，创建温度计窗口 */
  hDlg->hAPP = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbTEMPWin,WinPara.hWinMain , 0, 0);
  
	/* 添加结点到链表 */
	hAPPLinkedList_AddTail(hDlg);
	/* 向ctrl窗口发送消息 */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);

	InitTEMPSensor();	

	/* 如果本窗口有效，则一直更新数据 */
	while (WM_IsWindow( hDlg->hAPP)) {
    if (!_Stop) 
		{
			GUI_Delay(10);
      if (!hGraph) 
			{
        hGraph = WM_GetDialogItem( hDlg->hAPP, GUI_ID_GRAPH0);
      }
      //_AddValues(hGraph);
			GetTEMPData();
    }
			GUI_Delay(50);
  }
}

/*************************** End of file ****************************/

