/**
  ******************************************************************************
  * @file    WFGUI_Message.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ��ϢӦ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include <stdlib.h>

#include "WFGUI_Message.h"
#include "WFGUI_Keypad.h"

#include "DIALOG.h"
#include "ff.h"

#include "sim900a.h"
#include "bsp_usart2.h"



#define MAX_PATH 256

static WM_CALLBACK*     _pcbOldMessageWin = NULL;


/*********************************************************************
*
*       ��ϢӦ��ͼ��
*/
static const BITMAP_ITEM _MessageIcon[] = {
  {&bmWF_Floder, "In box" , "Received messages box"},
  {&bmWF_Floder, "Out box" , "Sended messages box"},
  {&bmWF_Note,   "Draft box" , "Draft messages box"},

};


/**
  * @brief  _cbMesgCtrlWin��ܵĻص�����
	*					
  * @param  none
  * @retval none
  */
static void _cbMesgCtrlWin(WM_MESSAGE * pMsg)
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
	
	if (_pcbOldMessageWin) {
			(*_pcbOldMessageWin)(pMsg);	//���þɵĻص�����������δ�������
		}	
	
}



/**
  * @brief  _cbMesgNew�½���Ϣ�ص�����
	*					
  * @param  none
  * @retval none
  */
static void _cbMesgNew(WM_MESSAGE * pMsg)
{
	
	int NCode;
  int Id;
	int xSize,ySize;
	
	int numLen;
	int textLen;
	
	char *num;
	char *text;
	char *textUC;
	
	WM_HWIN hNum;
	WM_HWIN hText;
	HANDLE_LIST *appNode;
	
	hNum  = WM_GetDialogItem(pMsg->hWin,GUI_ID_MULTIEDIT0);
	hText = WM_GetDialogItem(pMsg->hWin,GUI_ID_MULTIEDIT1);
	
  switch (pMsg->MsgId) {
		
		case WM_NOTIFY_PARENT:							  //֪ͨ������
			
		Id    = WM_GetId(pMsg->hWinSrc);      // �ؼ���ID
		NCode = pMsg->Data.v;                 // ֪ͨ����
		
		switch(NCode){
				 
				 case WM_NOTIFICATION_RELEASED:
					if(Id == GUI_ID_BUTTON0)				//���Ͱ�ť
					{						
						/*��ȡ���ݳ���*/
						numLen  = MULTIEDIT_GetTextSize(hNum);
						textLen = MULTIEDIT_GetTextSize(hText);
						
						num 		= (char *)malloc(sizeof(char)*numLen);
						text		= (char *)malloc(sizeof(char)*textLen);			//UTF8�������Ϊ3�ֽ�һ��������Ϊ1�ֽ�һ��
						textUC	= (char *)malloc(sizeof(char)*numLen*2);		//UC����ȫΪ2�ֽ�һ��			
						
						MULTIEDIT_GetText(hNum,num,numLen);                 //�绰����,���ֵ�UTF8���뼴ASCII�룬����ת��
						
						MULTIEDIT_GetText(hText,text,textLen);              //��������					
						
            /* ���Ͷ��� */
						sim900a_sms_utf8(num,text,numLen,textLen);
												
						/*�ͷſռ�*/
						free(num);
						free(textUC);
						free(text);
											
					}
					else if(Id == GUI_ID_BUTTON1)		                      //���水ť
					{
            //TBD ����ݸ��ⲿ�ִ���δ���� ����������û�а���ʽ���棬ûʲô��
						FIL hFile;
						FRESULT res;
						UINT rwb;
            
            /*��ȡ���ݳ���*/
						numLen  = MULTIEDIT_GetTextSize(hNum);
						textLen = MULTIEDIT_GetTextSize(hText);
						
						num 		= (char *)malloc(sizeof(char)*numLen);
						text		= (char *)malloc(sizeof(char)*textLen);			//UTF8�������Ϊ3�ֽ�һ��������Ϊ1�ֽ�һ��
						
						MULTIEDIT_GetText(hNum,num,numLen);            //�绰����
						
						MULTIEDIT_GetText(hText,text,textLen);         //��������
						
						res = f_open(&hFile,"0:WF_OS/Mesg/draftbox/newdraft.txt",FA_WRITE|FA_CREATE_ALWAYS);
             
              if(res != FR_OK)
                return;
              
						f_write(&hFile,num,numLen,&rwb);
						
						f_write(&hFile,text,textLen,&rwb);
						
						f_close(&hFile);

					}
					else if(Id == GUI_ID_BUTTON2)		                      //ȡ����ť
					{				

							/* ��ȡapp�����Ӧ�������� */
							appNode = hAPPLinkedList_GetAppNode(pMsg->hWin);
							if(appNode != NULL)
							{
								/* ɾ��app���������ļ�¼ */	
								hAPPLinkedList_Del(appNode);
									
								/* ɾ������ */
								WM_DeleteWindow(WM_GetParent(pMsg->hWin));

							}
						}						
				 
					 break;
				 
				 default:
					 break;
				 
			 }
		
		break;

	
  case WM_PAINT:
    xSize = WM_GetWindowSizeX(pMsg->hWin);
    ySize = WM_GetWindowSizeY(pMsg->hWin);
		

		GUI_SetColor(GUI_LIGHTGRAY);
		GUI_FillRect(0,0,xSize,ySize);

    break;
	
  default:
    WM_DefaultProc(pMsg);
	break;
	
	}

}

/**
  * @brief  Mesg_New,�½���Ϣ����
  * @param  none
  * @retval none
  */
static void Mesg_New(char *path)
{
	WM_HWIN hFrameC;
	

	WM_HWIN hMulti;
	WM_HWIN hText;
	WM_HWIN hButton;
	
	HANDLE_LIST *hFrame = hAPPLinkedList_NewNode();


	hFrame->hAPP = FRAMEWIN_CreateEx(0,0,WinPara.xSizeWin,WinPara.ySizeWin,WinPara.hWinMain,WM_CF_SHOW,FRAMEWIN_CF_ACTIVE|FRAMEWIN_SF_MOVEABLE,GUI_ID_FRAMEWIN10,"Write a Message",0);

	/* ��ӽ�㵽���� */
	hAPPLinkedList_AddTail(hFrame);
	/* ��ctrl���ڷ�����Ϣ */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	

	//	TBD ʹ�ûص�����������޷��ƶ���ܴ��ڵ����
	_pcbOldMessageWin = WM_SetCallback(hFrame->hAPP, _cbMesgCtrlWin);	//��ȡ�ɵĻص�����ָ��
	
	/* �������ڰ�ť */
	FRAMEWIN_AddCloseButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 0);
	
	/* ��ȡ��ܴ����û����ľ�� */
	hFrameC = WM_GetClientWindow(hFrame->hAPP);

	/* ������Ϣapi�Ļص����� */
	WM_SetCallback(hFrameC,_cbMesgNew);	
	
	/* �绰���봰�� */
	hText = TEXT_CreateEx(5,0,65,20,hFrameC,WM_CF_SHOW,TEXT_CF_LEFT|TEXT_CF_VCENTER,GUI_ID_TEXT0,"number:");
	TEXT_SetFont(hText,GUI_FONT_8X13_1);
	hMulti = MULTIEDIT_CreateEx(65,0,120,20,hFrameC,WM_CF_SHOW,0,GUI_ID_MULTIEDIT0,50,"");
	MULTIEDIT_SetFont(hMulti,&SDFont.XFont);
	
	/* �������ݴ��� */
	hMulti = MULTIEDIT_CreateEx(0,25,WM_GetWindowSizeX(hFrameC), WM_GetWindowSizeY(hFrameC)-50,hFrameC,WM_CF_SHOW,MULTIEDIT_CF_AUTOSCROLLBAR_V,GUI_ID_MULTIEDIT1,50,"");
	MULTIEDIT_EnableBlink(hMulti,500,1);	
	MULTIEDIT_SetFont(hMulti,&SDFont.XFont);
	
	/* ���Ͱ�ť */
	hButton = BUTTON_CreateEx(10,WM_GetWindowSizeY(hFrameC)-20,40,20,hFrameC,WM_CF_SHOW,0,GUI_ID_BUTTON0);
	BUTTON_SetText(hButton,"Send");
	
	/* ���水ť */
	hButton = BUTTON_CreateEx(55,WM_GetWindowSizeY(hFrameC)-20,40,20,hFrameC,WM_CF_SHOW,0,GUI_ID_BUTTON1);
	BUTTON_SetText(hButton,"Save");
	
	/* ȡ����ť */
	hButton = BUTTON_CreateEx(105,WM_GetWindowSizeY(hFrameC)-20,40,20,hFrameC,WM_CF_SHOW,0,GUI_ID_BUTTON2);
	BUTTON_SetText(hButton,"Cancle");
	
	/* ��Ӽ��� */
	KeypadInit(hFrame->hAPP); 


}

/**
  * @brief  Mesg_Read,��ȡ��Ϣ����
  * @param  none
  * @retval none
  */
static void Mesg_Read(char *path)
{


}



/**
  * @brief  _cbOutBox,�ݸ���ص�����
  * @param  none
  * @retval none
  */
static void _cbOutBox(WM_MESSAGE * pMsg)
{
	
	int        NCode;
  int        Id;
	int        Sel;
	
	switch (pMsg->MsgId) 
	{
		case WM_NOTIFY_PARENT:
		 
			Id    = WM_GetId(pMsg->hWinSrc);      // �ؼ���ID
			NCode = pMsg->Data.v;                 // ֪ͨ����
				 
			switch (Id) {
				case GUI_ID_LISTVIEW0:							// ListView
					switch (NCode) {
						case WM_NOTIFICATION_RELEASED:
						Sel = LISTVIEW_GetSel(pMsg->hWinSrc);
							if(Sel == 0)
							{								
								Mesg_New((char*)Sel);       //�����µ���Ϣ
							}
							else
							{
								Mesg_New("0");
							}

							break;								
						}		//switch (NCode)					
						
				break;
			}//	switch (Id) 
			
		break;
			
		default:
			break;
		
		case WM_PAINT:
			
			GUI_SetColor(GUI_WHITE);
			GUI_FillRect(0,0,WM_GetWindowSizeX(pMsg->hWin),WM_GetWindowSizeY(pMsg->hWin));
		
			break;
		
	}
	

}


/**
  * @brief  _cbDraftBox,�ݸ���ص�����
  * @param  none
  * @retval none
  */
static void _cbDraftBox(WM_MESSAGE * pMsg)
{	
	int        NCode;
  int        Id;
	int        Sel;
	
	switch (pMsg->MsgId) 
	{
		case WM_NOTIFY_PARENT:
		 
			Id    = WM_GetId(pMsg->hWinSrc);      // �ؼ���ID
			NCode = pMsg->Data.v;                 // ֪ͨ����
				 
			switch (Id) {
				case GUI_ID_LISTVIEW0:								// ListView
					switch (NCode) {
						case WM_NOTIFICATION_RELEASED:
						Sel = LISTVIEW_GetSel(pMsg->hWinSrc);
							if(Sel == 0)
							{
								Mesg_New("0");
							}
							else
							{
								Mesg_Read("0");
							}

							break;								
						}		//switch (NCode)					
						
				break;
			}//	switch (Id) 
			
		break;
			
		default:
			break;
		
		case WM_PAINT:
			
			GUI_SetColor(GUI_WHITE);
			GUI_FillRect(0,0,WM_GetWindowSizeX(pMsg->hWin),WM_GetWindowSizeY(pMsg->hWin));
		
			break;
		
	}
	

}

/**
  * @brief  Mesg_InBox,�ռ��䴦����
  * @param  none
  * @retval none
  */
static void Mesg_InBox(void)
{
	WM_HWIN hFrameC;
	WM_HWIN hListView;	
	WM_HWIN hHeader;
	
	HANDLE_LIST *hFrame = hAPPLinkedList_NewNode();


	hFrame->hAPP = FRAMEWIN_CreateEx(0,0,WinPara.xSizeWin,WinPara.ySizeWin,WinPara.hWinMain,WM_CF_SHOW,FRAMEWIN_CF_ACTIVE,GUI_ID_FRAMEWIN10,"In box",0);

		/* ��ӽ�㵽���� */
	hAPPLinkedList_AddTail(hFrame);
	/* ��ctrl���ڷ�����Ϣ */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//WM_SetCallback(hFrame->hAPP,_cbSDViewWin);

	//	TBD ʹ�ûص�����������޷��ƶ���ܴ��ڵ����
	_pcbOldMessageWin = WM_SetCallback(hFrame->hAPP, _cbMesgCtrlWin);	//��ȡ�ɵĻص�����ָ��

	
	/* �������ڰ�ť */
	FRAMEWIN_AddCloseButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 0);

	/* ��ȡ��ܴ����û����ľ�� */
	hFrameC = WM_GetClientWindow(hFrame->hAPP);

	FRAMEWIN_SetClientColor(hFrame->hAPP,GUI_WHITE);
	
	hListView = LISTVIEW_CreateEx(0,0, WM_GetWindowSizeX(hFrameC), WM_GetWindowSizeY(hFrameC),hFrameC,WM_CF_SHOW,NULL,GUI_ID_LISTVIEW0);

	hHeader = LISTVIEW_GetHeader(hListView);
	
	HEADER_AddItem(hHeader,WM_GetWindowSizeX(hFrameC)/4,"time",GUI_TA_HCENTER);
	
	HEADER_AddItem(hHeader,WM_GetWindowSizeX(hFrameC)/4,"number",GUI_TA_HCENTER);
	
	HEADER_AddItem(hHeader,WM_GetWindowSizeX(hFrameC)/2,"shortcut",GUI_TA_HCENTER);

}

/**
  * @brief  Mesg_OutBox,�����䴦����
  * @param  none
  * @retval none
  */
static void Mesg_OutBox(void)
{

	WM_HWIN hFrameC;
	WM_HWIN hListView;	
	
	const GUI_ConstString NewMesg[]={"NewMesg","-","-"};

	HANDLE_LIST *hFrame = hAPPLinkedList_NewNode();

	hFrame->hAPP = FRAMEWIN_CreateEx(0,0,WinPara.xSizeWin,WinPara.ySizeWin,WinPara.hWinMain,WM_CF_SHOW,FRAMEWIN_CF_ACTIVE,GUI_ID_FRAMEWIN10,"Out box",0);

		/* ��ӽ�㵽���� */
	hAPPLinkedList_AddTail(hFrame);
	/* ��ctrl���ڷ�����Ϣ */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//WM_SetCallback(hFrame->hAPP,_cbSDViewWin);

	//	TBD ʹ�ûص�����������޷��ƶ���ܴ��ڵ����
	_pcbOldMessageWin = WM_SetCallback(hFrame->hAPP, _cbMesgCtrlWin);	//��ȡ�ɵĻص�����ָ��


	/* �������ڰ�ť */
	FRAMEWIN_AddCloseButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 0);

	/* ��ȡ��ܴ����û����ľ�� */
	hFrameC = WM_GetClientWindow(hFrame->hAPP);

	FRAMEWIN_SetClientColor(hFrame->hAPP,GUI_WHITE);	

	/* ������Ϣapi�Ļص����� */
	WM_SetCallback(hFrameC,_cbOutBox);

	FRAMEWIN_SetClientColor(hFrame->hAPP,GUI_WHITE);
	
	hListView = LISTVIEW_CreateEx(0,0, WM_GetWindowSizeX(hFrameC), WM_GetWindowSizeY(hFrameC),hFrameC,WM_CF_SHOW,NULL,GUI_ID_LISTVIEW0);


  LISTVIEW_AddColumn(hListView, WM_GetWindowSizeX(hFrameC)/4, "Time",    GUI_TA_CENTER);
	LISTVIEW_AddColumn(hListView, WM_GetWindowSizeX(hFrameC)/4, "Number",  GUI_TA_CENTER);
	LISTVIEW_AddColumn(hListView, WM_GetWindowSizeX(hFrameC)/2, "Shortcut",GUI_TA_CENTER);
	
	LISTVIEW_AddRow(hListView,NewMesg);

}

/**
  * @brief  Mesg_DraftBox,�ݸ��䴦����
  * @param  none
  * @retval none
  */
static void Mesg_DraftBox(void)
{
	WM_HWIN hFrameC;
	WM_HWIN hListView;	
	
	const GUI_ConstString NewDraft[]={"NewDraft","-","-"};

	HANDLE_LIST *hFrame = hAPPLinkedList_NewNode();


	hFrame->hAPP = FRAMEWIN_CreateEx(0,0,WinPara.xSizeWin,WinPara.ySizeWin,WinPara.hWinMain,WM_CF_SHOW,FRAMEWIN_CF_ACTIVE,GUI_ID_FRAMEWIN10,"Draft box",0);

		/* ��ӽ�㵽���� */
	hAPPLinkedList_AddTail(hFrame);
	/* ��ctrl���ڷ�����Ϣ */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	//WM_SetCallback(hFrame->hAPP,_cbSDViewWin);

	//	TBD ʹ�ûص�����������޷��ƶ���ܴ��ڵ����
	_pcbOldMessageWin = WM_SetCallback(hFrame->hAPP, _cbMesgCtrlWin);	//��ȡ�ɵĻص�����ָ��

	
	/* �������ڰ�ť */
	FRAMEWIN_AddCloseButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 0);

	/* ��ȡ��ܴ����û����ľ�� */
	hFrameC = WM_GetClientWindow(hFrame->hAPP);

	/* ������Ϣapi�Ļص����� */
	WM_SetCallback(hFrameC,_cbDraftBox);

	FRAMEWIN_SetClientColor(hFrame->hAPP,GUI_WHITE);
	
	hListView = LISTVIEW_CreateEx(0,0, WM_GetWindowSizeX(hFrameC), WM_GetWindowSizeY(hFrameC),hFrameC,WM_CF_SHOW,NULL,GUI_ID_LISTVIEW0);

	LISTVIEW_GetHeader(hListView);
	

  LISTVIEW_AddColumn(hListView, WM_GetWindowSizeX(hFrameC)/4, "Time",    GUI_TA_CENTER);
	LISTVIEW_AddColumn(hListView, WM_GetWindowSizeX(hFrameC)/4, "Number",  GUI_TA_CENTER);
	LISTVIEW_AddColumn(hListView, WM_GetWindowSizeX(hFrameC)/2, "Shortcut",GUI_TA_CENTER);
	
	LISTVIEW_AddRow(hListView,NewDraft);		

}


/**
  * @brief  _cbMesgWin,���ſ�ص�����
  * @param  none
  * @retval none
  */
static void _cbMesgWin(WM_MESSAGE * pMsg)
{
	WM_HWIN    hWin;
	WM_HWIN    hIcon;
	
	HANDLE_LIST *appNode;
	
	int        NCode;
  int        Id;
	int        Sel;
	
	char 			 i;
	
	hWin = pMsg->hWin;	

	switch (pMsg->MsgId) 
	{
		case WM_CREATE:	
			/*
			* ����ͼ��ؼ�
			*/
			hIcon = ICONVIEW_CreateEx(20, 0,  WinPara.xSizeWin,  WinPara.ySizeWin, 
															 pMsg->hWin, WM_CF_SHOW | WM_CF_HASTRANS, 
															 0, GUI_ID_ICONVIEW0, 55, 70);
			
			/*
			* ���ͼ�굽ͼ��ؼ�
			*/
			for (i = 0; i < GUI_COUNTOF(_MessageIcon); i++) {    

			ICONVIEW_AddBitmapItem(hIcon, _MessageIcon[i].pBitmap, _MessageIcon[i].pText);
			}		

			/* ����ѡ��ͼ�����ɫ */
			ICONVIEW_SetBkColor(hIcon, ICONVIEW_CI_SEL, GUI_BLUE | 0xC0000000);
			
			/* ����ͼ������� */
			ICONVIEW_SetFont(hIcon, &GUI_Font13B_ASCII);
			
			/* ���ó�ʼѡ���ͼ��Ϊ -1 (��ʾ��δѡ��)*/
			ICONVIEW_SetSel(hIcon,-1);		
		
		
			break;			
		
		case WM_NOTIFY_PARENT:
		 
			Id    = WM_GetId(pMsg->hWinSrc);      // �ؼ���ID
			NCode = pMsg->Data.v;                 // ֪ͨ����
				 
			switch (Id) {
				case GUI_ID_ICONVIEW0:								// ͼ��ؼ�
					switch (NCode) {
						case WM_NOTIFICATION_RELEASED:
							Sel   = ICONVIEW_GetSel(pMsg->hWinSrc);
							switch(Sel)
							{
								case 0:
									GUI_Delay(500);								//������ʱ����ʾ����Ч��
									Mesg_InBox();	
								
									break;
								
								case 1:
									GUI_Delay(500);								//������ʱ����ʾ����Ч��
									Mesg_OutBox();
								
									break;
								
								case 2:
									GUI_Delay(500);								//������ʱ����ʾ����Ч��
									Mesg_DraftBox();
								
									break;
								
								default:
									break;	
								
							}//	switch(Sel)
							
							/* ��ȡICON�ľ�� */
							hIcon = WM_GetDialogItem(hWin, GUI_ID_ICONVIEW0);
						
							/* ���ó�ʼѡ���ͼ��Ϊ -1 (��ʾ��δѡ��)*/
							ICONVIEW_SetSel(hIcon,-1);	

							break;								
						}		//switch (NCode)					
						
				break;
			}//	switch (Id) 
			
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
			
			GUI_SetColor(GUI_BLACK);
			GUI_FillRect(0,0,WM_GetWindowSizeX(pMsg->hWin),WM_GetWindowSizeY(pMsg->hWin));
		
			break;
		
			default:
			break;
		
	}
	
	
}



/**
  * @brief  WFGUI_Message,����
  * @param  none
  * @retval none
  */
void WFGUI_Message(void)
{
	HANDLE_LIST *hMessage = hAPPLinkedList_NewNode();

	/* �������Ŵ��� */
	hMessage->hAPP = WM_CreateWindowAsChild(0,
																		0,
																		WinPara.xSizeWin,
																		WinPara.ySizeWin ,
																		WinPara.hWinMain ,
																		WM_CF_SHOW , 
																		_cbMesgWin,
																		0);	
	
	/* ��ӽ�㵽���� */
	hAPPLinkedList_AddTail(hMessage);
	/* ��ctrl���ڷ�����Ϣ */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
		/* ��ʼ�������ģ�� */
	if (sim900a_init()!= 0)
	{
		GUI_MessageBox("\r\n No detected SIM900A module! \r\n","error",GUI_MESSAGEBOX_CF_MOVEABLE);
		}
	
}




