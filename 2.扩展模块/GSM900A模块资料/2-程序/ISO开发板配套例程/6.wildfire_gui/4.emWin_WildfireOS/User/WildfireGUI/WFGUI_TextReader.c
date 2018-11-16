/**
  ******************************************************************************
  * @file    WFGUI_TextReader.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �ı������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ff.h"
#include "DIALOG.h"

#include "bsp_usart1.h"
#include "WFGUI_Common.h"
#include "WFGUI_SDView.h"
#include "WFGUI_TextReader.h"

static WM_CALLBACK*     _pcbOldSDViewWin = NULL;


/**
  * @brief  _cbTextReaderWin framewin��ܵĻص�����
	*					
  * @param  none
  * @retval none
  */
static void _cbTextReaderWin(WM_MESSAGE * pMsg)
{
	HANDLE_LIST *appNode;
	
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
	
	if (_pcbOldSDViewWin) {
			(*_pcbOldSDViewWin)(pMsg);	//���þɵĻص�����������δ�������
		}	
	
}


/**
  * @brief  �ı��Ķ���
  * @param  Ҫ�򿪵��ļ���  
  * @retval ��
  */
void TextReader(char *file_name )
{	
	FIL hFile;
	unsigned int rw_num;			//�Ѷ�����д���ֽ���
	FRESULT fres;							//���ؽ��
	char* read_buffer; 

	WM_HWIN hFrameC;
	WM_HWIN hMultiEdit;
	
	HANDLE_LIST *hFrame = hAPPLinkedList_NewNode();

	
	read_buffer = (char * ) malloc(300* sizeof(char));  					//Ϊ�洢Ŀ¼����ָ�����ռ�
	*read_buffer ='\0';

	hFrame->hAPP = FRAMEWIN_CreateEx(	0,
                                    0,
                                    WinPara.xSizeWin,
                                    WinPara.ySizeWin,
                                    WinPara.hWinMain,
                                    WM_CF_SHOW,
                                    FRAMEWIN_CF_ACTIVE|FRAMEWIN_CF_MOVEABLE,
                                    GUI_ID_FRAMEWIN1,
                                    "eBook Reader",
                                    0);															

	/* �������ڰ�ť */
	FRAMEWIN_AddCloseButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 0);
	FRAMEWIN_AddMaxButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 1);
	FRAMEWIN_AddMinButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 2);
	
	
	/* ��ӽ�㵽���� */
	hAPPLinkedList_AddTail(hFrame);
	
  /* ��ctrl���ڷ�����Ϣ */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	_pcbOldSDViewWin = WM_SetCallback(hFrame->hAPP, _cbTextReaderWin);	//��ȡ�ɵĻص�����ָ��

	
	FRAMEWIN_SetText(hFrame->hAPP,file_name);
	
	/* ��ȡ��ܴ����û����ľ�� */
	hFrameC = WM_GetClientWindow(hFrame->hAPP);
	
	DEBUG("file_name =%s",file_name);	
	
	fres = f_open (&hFile, file_name, FA_READ ); 				                //��Ҫ�Ķ����ļ�
  
  if(fres != FR_OK)
      return ;
	
  /* �����ı��Ķ��ؼ� */
	hMultiEdit=MULTIEDIT_CreateEx(0,
                                0,
                                WM_GetWindowSizeX(hFrameC),
                                WM_GetWindowSizeY(hFrameC),
                                hFrameC,WM_CF_SHOW,
                                MULTIEDIT_CF_READONLY|MULTIEDIT_CF_AUTOSCROLLBAR_V,
                                GUI_ID_MULTIEDIT0,
                                0,
                                0);
                                
	MULTIEDIT_SetFont(hMultiEdit,GUI_FONT_13B_ASCII);
	MULTIEDIT_SetWrapWord(hMultiEdit);

  //TBD ������ʾ�ı����ڶ�ȡ���ļ�ʱ�����
	while(hFile.fptr!=hFile.fsize) 		                                  //���ָ��ָ�����ļ�β����ʾ����ȫ������
	{
		fres = f_read(&hFile, read_buffer, 300, &rw_num);
		if(rw_num<300)
			read_buffer[rw_num]='\0';		                                    //�����ļ�β���Ͻ�����
		
		MULTIEDIT_AddText(hMultiEdit,read_buffer);		
	}
  
	fres = f_close (&hFile);	
	
	DEBUG("%s",read_buffer);
	
	free(read_buffer);								//�ͷ�malloc�ռ�		
	read_buffer = NULL;



}




/**
  * @brief  WFGUI_TextReader,WFGUI textReader ������
  * @param  none
  * @retval none
  */
void WFGUI_TextReader(void)
{

  /* ɨ���ı��ļ�������Ŀ¼�� */
	Fill_TreeView(TEXTFILE,TEXT_LIST_PATH); 

}










