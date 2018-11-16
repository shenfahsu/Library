/**
  ******************************************************************************
  * @file    WFGUI_TextReader.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   文本浏览器
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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
  * @brief  _cbTextReaderWin framewin框架的回调函数
	*					
  * @param  none
  * @retval none
  */
static void _cbTextReaderWin(WM_MESSAGE * pMsg)
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
	
	if (_pcbOldSDViewWin) {
			(*_pcbOldSDViewWin)(pMsg);	//调用旧的回调函数处理部分未处理过程
		}	
	
}


/**
  * @brief  文本阅读器
  * @param  要打开的文件名  
  * @retval 无
  */
void TextReader(char *file_name )
{	
	FIL hFile;
	unsigned int rw_num;			//已读或已写的字节数
	FRESULT fres;							//返回结果
	char* read_buffer; 

	WM_HWIN hFrameC;
	WM_HWIN hMultiEdit;
	
	HANDLE_LIST *hFrame = hAPPLinkedList_NewNode();

	
	read_buffer = (char * ) malloc(300* sizeof(char));  					//为存储目录名的指针分配空间
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

	/* 创建窗口按钮 */
	FRAMEWIN_AddCloseButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 0);
	FRAMEWIN_AddMaxButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 1);
	FRAMEWIN_AddMinButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 2);
	
	
	/* 添加结点到链表 */
	hAPPLinkedList_AddTail(hFrame);
	
  /* 向ctrl窗口发送消息 */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	_pcbOldSDViewWin = WM_SetCallback(hFrame->hAPP, _cbTextReaderWin);	//获取旧的回调函数指针

	
	FRAMEWIN_SetText(hFrame->hAPP,file_name);
	
	/* 获取框架窗口用户区的句柄 */
	hFrameC = WM_GetClientWindow(hFrame->hAPP);
	
	DEBUG("file_name =%s",file_name);	
	
	fres = f_open (&hFile, file_name, FA_READ ); 				                //打开要阅读的文件
  
  if(fres != FR_OK)
      return ;
	
  /* 创建文本阅读控件 */
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

  //TBD 这样显示文本，在读取大文件时会出错
	while(hFile.fptr!=hFile.fsize) 		                                  //如果指针指向了文件尾，表示数据全部读完
	{
		fres = f_read(&hFile, read_buffer, 300, &rw_num);
		if(rw_num<300)
			read_buffer[rw_num]='\0';		                                    //到了文件尾加上结束符
		
		MULTIEDIT_AddText(hMultiEdit,read_buffer);		
	}
  
	fres = f_close (&hFile);	
	
	DEBUG("%s",read_buffer);
	
	free(read_buffer);								//释放malloc空间		
	read_buffer = NULL;



}




/**
  * @brief  WFGUI_TextReader,WFGUI textReader 主函数
  * @param  none
  * @retval none
  */
void WFGUI_TextReader(void)
{

  /* 扫描文本文件，生成目录树 */
	Fill_TreeView(TEXTFILE,TEXT_LIST_PATH); 

}










