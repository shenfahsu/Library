/**
  ******************************************************************************
  * @file    WFGUI_Message.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   信息应用
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
*       信息应用图标
*/
static const BITMAP_ITEM _MessageIcon[] = {
  {&bmWF_Floder, "In box" , "Received messages box"},
  {&bmWF_Floder, "Out box" , "Sended messages box"},
  {&bmWF_Note,   "Draft box" , "Draft messages box"},

};


/**
  * @brief  _cbMesgCtrlWin框架的回调函数
	*					
  * @param  none
  * @retval none
  */
static void _cbMesgCtrlWin(WM_MESSAGE * pMsg)
{
	HANDLE_LIST *appNode;
	
		switch (pMsg->MsgId) {			//消息类型

#if 1			//奇怪，必须要有这个项
		case WM_NOTIFY_PARENT:		//通知		
			break;//通知代码break
#endif	
			
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
			
		}//MsgId
	
	if (_pcbOldMessageWin) {
			(*_pcbOldMessageWin)(pMsg);	//调用旧的回调函数处理部分未处理过程
		}	
	
}



/**
  * @brief  _cbMesgNew新建信息回调函数
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
		
		case WM_NOTIFY_PARENT:							  //通知父窗口
			
		Id    = WM_GetId(pMsg->hWinSrc);      // 控件的ID
		NCode = pMsg->Data.v;                 // 通知代码
		
		switch(NCode){
				 
				 case WM_NOTIFICATION_RELEASED:
					if(Id == GUI_ID_BUTTON0)				//发送按钮
					{						
						/*获取数据长度*/
						numLen  = MULTIEDIT_GetTextSize(hNum);
						textLen = MULTIEDIT_GetTextSize(hText);
						
						num 		= (char *)malloc(sizeof(char)*numLen);
						text		= (char *)malloc(sizeof(char)*textLen);			//UTF8编码可能为3字节一个，可能为1字节一个
						textUC	= (char *)malloc(sizeof(char)*numLen*2);		//UC编码全为2字节一个			
						
						MULTIEDIT_GetText(hNum,num,numLen);                 //电话号码,数字的UTF8编码即ASCII码，无需转换
						
						MULTIEDIT_GetText(hText,text,textLen);              //短信内容					
						
            /* 发送短信 */
						sim900a_sms_utf8(num,text,numLen,textLen);
												
						/*释放空间*/
						free(num);
						free(textUC);
						free(text);
											
					}
					else if(Id == GUI_ID_BUTTON1)		                      //保存按钮
					{
            //TBD 保存草稿这部分代码未测试 ，而且这样没有按格式保存，没什么用
						FIL hFile;
						FRESULT res;
						UINT rwb;
            
            /*获取数据长度*/
						numLen  = MULTIEDIT_GetTextSize(hNum);
						textLen = MULTIEDIT_GetTextSize(hText);
						
						num 		= (char *)malloc(sizeof(char)*numLen);
						text		= (char *)malloc(sizeof(char)*textLen);			//UTF8编码可能为3字节一个，可能为1字节一个
						
						MULTIEDIT_GetText(hNum,num,numLen);            //电话号码
						
						MULTIEDIT_GetText(hText,text,textLen);         //短信内容
						
						res = f_open(&hFile,"0:WF_OS/Mesg/draftbox/newdraft.txt",FA_WRITE|FA_CREATE_ALWAYS);
             
              if(res != FR_OK)
                return;
              
						f_write(&hFile,num,numLen,&rwb);
						
						f_write(&hFile,text,textLen,&rwb);
						
						f_close(&hFile);

					}
					else if(Id == GUI_ID_BUTTON2)		                      //取消按钮
					{				

							/* 获取app句柄对应的链表结点 */
							appNode = hAPPLinkedList_GetAppNode(pMsg->hWin);
							if(appNode != NULL)
							{
								/* 删除app句柄链表里的记录 */	
								hAPPLinkedList_Del(appNode);
									
								/* 删除窗口 */
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
  * @brief  Mesg_New,新建消息函数
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

	/* 添加结点到链表 */
	hAPPLinkedList_AddTail(hFrame);
	/* 向ctrl窗口发送消息 */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	

	//	TBD 使用回调函数会出现无法移动框架窗口的情况
	_pcbOldMessageWin = WM_SetCallback(hFrame->hAPP, _cbMesgCtrlWin);	//获取旧的回调函数指针
	
	/* 创建窗口按钮 */
	FRAMEWIN_AddCloseButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 0);
	
	/* 获取框架窗口用户区的句柄 */
	hFrameC = WM_GetClientWindow(hFrame->hAPP);

	/* 设置信息api的回调函数 */
	WM_SetCallback(hFrameC,_cbMesgNew);	
	
	/* 电话号码窗口 */
	hText = TEXT_CreateEx(5,0,65,20,hFrameC,WM_CF_SHOW,TEXT_CF_LEFT|TEXT_CF_VCENTER,GUI_ID_TEXT0,"number:");
	TEXT_SetFont(hText,GUI_FONT_8X13_1);
	hMulti = MULTIEDIT_CreateEx(65,0,120,20,hFrameC,WM_CF_SHOW,0,GUI_ID_MULTIEDIT0,50,"");
	MULTIEDIT_SetFont(hMulti,&SDFont.XFont);
	
	/* 短信内容窗口 */
	hMulti = MULTIEDIT_CreateEx(0,25,WM_GetWindowSizeX(hFrameC), WM_GetWindowSizeY(hFrameC)-50,hFrameC,WM_CF_SHOW,MULTIEDIT_CF_AUTOSCROLLBAR_V,GUI_ID_MULTIEDIT1,50,"");
	MULTIEDIT_EnableBlink(hMulti,500,1);	
	MULTIEDIT_SetFont(hMulti,&SDFont.XFont);
	
	/* 发送按钮 */
	hButton = BUTTON_CreateEx(10,WM_GetWindowSizeY(hFrameC)-20,40,20,hFrameC,WM_CF_SHOW,0,GUI_ID_BUTTON0);
	BUTTON_SetText(hButton,"Send");
	
	/* 保存按钮 */
	hButton = BUTTON_CreateEx(55,WM_GetWindowSizeY(hFrameC)-20,40,20,hFrameC,WM_CF_SHOW,0,GUI_ID_BUTTON1);
	BUTTON_SetText(hButton,"Save");
	
	/* 取消按钮 */
	hButton = BUTTON_CreateEx(105,WM_GetWindowSizeY(hFrameC)-20,40,20,hFrameC,WM_CF_SHOW,0,GUI_ID_BUTTON2);
	BUTTON_SetText(hButton,"Cancle");
	
	/* 添加键盘 */
	KeypadInit(hFrame->hAPP); 


}

/**
  * @brief  Mesg_Read,读取消息函数
  * @param  none
  * @retval none
  */
static void Mesg_Read(char *path)
{


}



/**
  * @brief  _cbOutBox,草稿箱回调函数
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
		 
			Id    = WM_GetId(pMsg->hWinSrc);      // 控件的ID
			NCode = pMsg->Data.v;                 // 通知代码
				 
			switch (Id) {
				case GUI_ID_LISTVIEW0:							// ListView
					switch (NCode) {
						case WM_NOTIFICATION_RELEASED:
						Sel = LISTVIEW_GetSel(pMsg->hWinSrc);
							if(Sel == 0)
							{								
								Mesg_New((char*)Sel);       //创建新的信息
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
  * @brief  _cbDraftBox,草稿箱回调函数
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
		 
			Id    = WM_GetId(pMsg->hWinSrc);      // 控件的ID
			NCode = pMsg->Data.v;                 // 通知代码
				 
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
  * @brief  Mesg_InBox,收件箱处理函数
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

		/* 添加结点到链表 */
	hAPPLinkedList_AddTail(hFrame);
	/* 向ctrl窗口发送消息 */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//WM_SetCallback(hFrame->hAPP,_cbSDViewWin);

	//	TBD 使用回调函数会出现无法移动框架窗口的情况
	_pcbOldMessageWin = WM_SetCallback(hFrame->hAPP, _cbMesgCtrlWin);	//获取旧的回调函数指针

	
	/* 创建窗口按钮 */
	FRAMEWIN_AddCloseButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 0);

	/* 获取框架窗口用户区的句柄 */
	hFrameC = WM_GetClientWindow(hFrame->hAPP);

	FRAMEWIN_SetClientColor(hFrame->hAPP,GUI_WHITE);
	
	hListView = LISTVIEW_CreateEx(0,0, WM_GetWindowSizeX(hFrameC), WM_GetWindowSizeY(hFrameC),hFrameC,WM_CF_SHOW,NULL,GUI_ID_LISTVIEW0);

	hHeader = LISTVIEW_GetHeader(hListView);
	
	HEADER_AddItem(hHeader,WM_GetWindowSizeX(hFrameC)/4,"time",GUI_TA_HCENTER);
	
	HEADER_AddItem(hHeader,WM_GetWindowSizeX(hFrameC)/4,"number",GUI_TA_HCENTER);
	
	HEADER_AddItem(hHeader,WM_GetWindowSizeX(hFrameC)/2,"shortcut",GUI_TA_HCENTER);

}

/**
  * @brief  Mesg_OutBox,发件箱处理函数
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

		/* 添加结点到链表 */
	hAPPLinkedList_AddTail(hFrame);
	/* 向ctrl窗口发送消息 */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//WM_SetCallback(hFrame->hAPP,_cbSDViewWin);

	//	TBD 使用回调函数会出现无法移动框架窗口的情况
	_pcbOldMessageWin = WM_SetCallback(hFrame->hAPP, _cbMesgCtrlWin);	//获取旧的回调函数指针


	/* 创建窗口按钮 */
	FRAMEWIN_AddCloseButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 0);

	/* 获取框架窗口用户区的句柄 */
	hFrameC = WM_GetClientWindow(hFrame->hAPP);

	FRAMEWIN_SetClientColor(hFrame->hAPP,GUI_WHITE);	

	/* 设置信息api的回调函数 */
	WM_SetCallback(hFrameC,_cbOutBox);

	FRAMEWIN_SetClientColor(hFrame->hAPP,GUI_WHITE);
	
	hListView = LISTVIEW_CreateEx(0,0, WM_GetWindowSizeX(hFrameC), WM_GetWindowSizeY(hFrameC),hFrameC,WM_CF_SHOW,NULL,GUI_ID_LISTVIEW0);


  LISTVIEW_AddColumn(hListView, WM_GetWindowSizeX(hFrameC)/4, "Time",    GUI_TA_CENTER);
	LISTVIEW_AddColumn(hListView, WM_GetWindowSizeX(hFrameC)/4, "Number",  GUI_TA_CENTER);
	LISTVIEW_AddColumn(hListView, WM_GetWindowSizeX(hFrameC)/2, "Shortcut",GUI_TA_CENTER);
	
	LISTVIEW_AddRow(hListView,NewMesg);

}

/**
  * @brief  Mesg_DraftBox,草稿箱处理函数
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

		/* 添加结点到链表 */
	hAPPLinkedList_AddTail(hFrame);
	/* 向ctrl窗口发送消息 */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	//WM_SetCallback(hFrame->hAPP,_cbSDViewWin);

	//	TBD 使用回调函数会出现无法移动框架窗口的情况
	_pcbOldMessageWin = WM_SetCallback(hFrame->hAPP, _cbMesgCtrlWin);	//获取旧的回调函数指针

	
	/* 创建窗口按钮 */
	FRAMEWIN_AddCloseButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 0);

	/* 获取框架窗口用户区的句柄 */
	hFrameC = WM_GetClientWindow(hFrame->hAPP);

	/* 设置信息api的回调函数 */
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
  * @brief  _cbMesgWin,短信框回调窗口
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
			* 创建图标控件
			*/
			hIcon = ICONVIEW_CreateEx(20, 0,  WinPara.xSizeWin,  WinPara.ySizeWin, 
															 pMsg->hWin, WM_CF_SHOW | WM_CF_HASTRANS, 
															 0, GUI_ID_ICONVIEW0, 55, 70);
			
			/*
			* 添加图标到图标控件
			*/
			for (i = 0; i < GUI_COUNTOF(_MessageIcon); i++) {    

			ICONVIEW_AddBitmapItem(hIcon, _MessageIcon[i].pBitmap, _MessageIcon[i].pText);
			}		

			/* 设置选定图标的颜色 */
			ICONVIEW_SetBkColor(hIcon, ICONVIEW_CI_SEL, GUI_BLUE | 0xC0000000);
			
			/* 设置图标的字体 */
			ICONVIEW_SetFont(hIcon, &GUI_Font13B_ASCII);
			
			/* 设置初始选择的图标为 -1 (表示尚未选择)*/
			ICONVIEW_SetSel(hIcon,-1);		
		
		
			break;			
		
		case WM_NOTIFY_PARENT:
		 
			Id    = WM_GetId(pMsg->hWinSrc);      // 控件的ID
			NCode = pMsg->Data.v;                 // 通知代码
				 
			switch (Id) {
				case GUI_ID_ICONVIEW0:								// 图标控件
					switch (NCode) {
						case WM_NOTIFICATION_RELEASED:
							Sel   = ICONVIEW_GetSel(pMsg->hWinSrc);
							switch(Sel)
							{
								case 0:
									GUI_Delay(500);								//短暂延时以显示触摸效果
									Mesg_InBox();	
								
									break;
								
								case 1:
									GUI_Delay(500);								//短暂延时以显示触摸效果
									Mesg_OutBox();
								
									break;
								
								case 2:
									GUI_Delay(500);								//短暂延时以显示触摸效果
									Mesg_DraftBox();
								
									break;
								
								default:
									break;	
								
							}//	switch(Sel)
							
							/* 获取ICON的句柄 */
							hIcon = WM_GetDialogItem(hWin, GUI_ID_ICONVIEW0);
						
							/* 设置初始选择的图标为 -1 (表示尚未选择)*/
							ICONVIEW_SetSel(hIcon,-1);	

							break;								
						}		//switch (NCode)					
						
				break;
			}//	switch (Id) 
			
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
			
			GUI_SetColor(GUI_BLACK);
			GUI_FillRect(0,0,WM_GetWindowSizeX(pMsg->hWin),WM_GetWindowSizeY(pMsg->hWin));
		
			break;
		
			default:
			break;
		
	}
	
	
}



/**
  * @brief  WFGUI_Message,短信
  * @param  none
  * @retval none
  */
void WFGUI_Message(void)
{
	HANDLE_LIST *hMessage = hAPPLinkedList_NewNode();

	/* 创建短信窗口 */
	hMessage->hAPP = WM_CreateWindowAsChild(0,
																		0,
																		WinPara.xSizeWin,
																		WinPara.ySizeWin ,
																		WinPara.hWinMain ,
																		WM_CF_SHOW , 
																		_cbMesgWin,
																		0);	
	
	/* 添加结点到链表 */
	hAPPLinkedList_AddTail(hMessage);
	/* 向ctrl窗口发送消息 */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
		/* 初始化并检测模块 */
	if (sim900a_init()!= 0)
	{
		GUI_MessageBox("\r\n No detected SIM900A module! \r\n","error",GUI_MESSAGEBOX_CF_MOVEABLE);
		}
	
}




