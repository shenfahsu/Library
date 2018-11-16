/**
  ******************************************************************************
  * @file    WFGUI_UserAPP.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �û�APP�ļ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "WFGUI_UserAPP.h"


/**
  * @brief  _cbUserAPP,�û��ص�����
  * @param  none
  * @retval none
  */
static void _cbUserAPP(WM_MESSAGE * pMsg)
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
  * @brief  WFGUI_UserAPP,�û�APP
  * @param  none
  * @retval none
  */
void WFGUI_UserAPP(void)
{
	/*��ʾ��Ϣ���ȴ������Ƹ����Ӧ�� */
	const char tips[] = {0xE7,0xAD,0x89,0xE5,0xBE,0x85,0xE6,0x82,0xA8,0xE5,0xAE,0x8C,0xE5,0x96,0x84,0xE6,0x9B,0xB4,0xE5,0xA4,0x9A,0xE7,0x9A,0x84,0xE5,0xBA,0x94,0xE7,0x94,0xA8,0x20};
	HANDLE_LIST *hUser = hAPPLinkedList_NewNode();
	
	/* �����绰���� */
	hUser->hAPP = WM_CreateWindowAsChild(0, 0, WinPara.xSizeWin,WinPara.ySizeWin ,WinPara.hWinMain , WM_CF_SHOW | WM_CF_STAYONTOP, _cbUserAPP, 0);	
	
	/* ��¼��ǰ���� */
	/* ��ӽ�㵽���� */
	hAPPLinkedList_AddTail(hUser);
	/* ��ctrl���ڷ�����Ϣ */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	/* show sorry */
	GUI_MessageBox(tips,"sorry",GUI_MESSAGEBOX_CF_MOVEABLE);


}



