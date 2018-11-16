/**
  ******************************************************************************
  * @file    WFGUI_Camera.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   摄像头应用
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */


#include "WFGUI_Camera.h"
#include "dialog.h"
#include "bsp_ov7725.h"



/**
  * @brief  _cbCamera,摄像头回调函数
  * @param  none
  * @retval none
  */
static void _cbCamera(WM_MESSAGE * pMsg)
{
	
	WM_HWIN hWin;
	int xSize,ySize;
  HANDLE_LIST *appNode;

	
	
	hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
  case WM_CREATE:
		
	
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
	
	case WM_PAINT:
		//
    // Draw background
    //
    xSize = WM_GetWindowSizeX(hWin);
    ySize = WM_GetWindowSizeY(hWin);
    GUI_SetColor(GUI_WHITE);
	
		GUI_FillRect(0,0,xSize,ySize);
	
		break;
	
	}	
}





/**
  * @brief  WFGUI_Camera,摄像头
  * @param  none
  * @retval none
  */
void WFGUI_Camera(void)
{
	/*提示消息：该应用正加班加点设计中*/
	const char tips[] = {0xE8,0xAF,0xA5,0xE5,0xBA,0x94,0xE7,0x94,0xA8,0xE6,0xAD,0xA3,0xE5,0x8A,0xA0,0xE7,0x8F,0xAD,0xE5,0x8A,0xA0,0xE7,0x82,0xB9,0xE8,0xAE,0xBE,0xE8,0xAE,0xA1,0xE4,0xB8,0xAD,0x20};
	HANDLE_LIST *hCamera = hAPPLinkedList_NewNode();
	
	/* 创建电话窗口 */
	hCamera->hAPP = WM_CreateWindowAsChild(0, 0, WinPara.xSizeWin,WinPara.ySizeWin ,WinPara.hWinMain , WM_CF_SHOW | WM_CF_STAYONTOP, _cbCamera, 0);	
	
	/* 记录当前窗口 */
	/* 添加结点到链表 */
	hAPPLinkedList_AddTail(hCamera);
	/* 向ctrl窗口发送消息 */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	
	GUI_MessageBox(tips,"error",GUI_MESSAGEBOX_CF_MOVEABLE);

	
	/* 初始化摄像头 */
	//Camera_Init();

}


