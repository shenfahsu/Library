/**
  ******************************************************************************
  * @file    WFGUI_Camera.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����ͷӦ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */


#include "WFGUI_Camera.h"
#include "dialog.h"
#include "bsp_ov7725.h"



/**
  * @brief  _cbCamera,����ͷ�ص�����
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
  * @brief  WFGUI_Camera,����ͷ
  * @param  none
  * @retval none
  */
void WFGUI_Camera(void)
{
	/*��ʾ��Ϣ����Ӧ�����Ӱ�ӵ������*/
	const char tips[] = {0xE8,0xAF,0xA5,0xE5,0xBA,0x94,0xE7,0x94,0xA8,0xE6,0xAD,0xA3,0xE5,0x8A,0xA0,0xE7,0x8F,0xAD,0xE5,0x8A,0xA0,0xE7,0x82,0xB9,0xE8,0xAE,0xBE,0xE8,0xAE,0xA1,0xE4,0xB8,0xAD,0x20};
	HANDLE_LIST *hCamera = hAPPLinkedList_NewNode();
	
	/* �����绰���� */
	hCamera->hAPP = WM_CreateWindowAsChild(0, 0, WinPara.xSizeWin,WinPara.ySizeWin ,WinPara.hWinMain , WM_CF_SHOW | WM_CF_STAYONTOP, _cbCamera, 0);	
	
	/* ��¼��ǰ���� */
	/* ��ӽ�㵽���� */
	hAPPLinkedList_AddTail(hCamera);
	/* ��ctrl���ڷ�����Ϣ */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	
	GUI_MessageBox(tips,"error",GUI_MESSAGEBOX_CF_MOVEABLE);

	
	/* ��ʼ������ͷ */
	//Camera_Init();

}


