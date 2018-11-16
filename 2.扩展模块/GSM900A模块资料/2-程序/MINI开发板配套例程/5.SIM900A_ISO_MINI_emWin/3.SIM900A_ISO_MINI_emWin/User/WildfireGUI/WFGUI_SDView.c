/**
  ******************************************************************************
  * @file    WFGUI_SDView.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   SD卡文件浏览器
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

#include "WFGUI_Common.h"
#include "WFGUI_SDView.h"
#include "WFGUI_TextReader.h"
#include "WFGUI_ImageReader.h"


static FRESULT scan_files (char* path,char*file_name,FIL *hFile,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode,FILE_TYPE fileType,int *fileNum) ;
static void OpenFileProcess(int sel_num,char *record_file);



static WM_CALLBACK*     _pcbOldSDViewWin = NULL;


/**
  * @brief  对话框用到的小工具资源表
	*					
  * @param  none
  * @retval none
  */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Message",    0,      20,  90, 200, 100, FRAMEWIN_CF_MOVEABLE },
  { TEXT_CreateIndirect,     "Do you sure to open the file?",     	GUI_ID_TEXT0,    10,  10, 180,  80 },
  { BUTTON_CreateIndirect,   "Yes",   	GUI_ID_YES,     10, 50,  60,  18 },
  { BUTTON_CreateIndirect,   "No", 			GUI_ID_NO,  120, 50,  60,  18 },
};

/*********************************************************************
*
*       对话框回调函数
*/
static void _cbDlgWin(WM_MESSAGE * pMsg) {
  int NCode, Id;
  WM_HWIN hDlg, hItem;
  hDlg = pMsg->hWin;
  switch (pMsg->MsgId) {
    
    case WM_INIT_DIALOG:									        //初始化对话框
			hItem = WM_GetDialogItem(hDlg, GUI_ID_TEXT0);
			TEXT_SetFont(hItem,GUI_FONT_16_ASCII);	    //设置text的字体
      break;
    case WM_NOTIFY_PARENT:
      Id    = WM_GetId(pMsg->hWinSrc);            //获取控件ID
      NCode = pMsg->Data.v;                       //获取消息
      switch (NCode) {

      case WM_NOTIFICATION_RELEASED:              //控件被释放
        if (Id == GUI_ID_YES) {                   // Yes Button     
					GUI_EndDialog(hDlg, 0);					        //结束对话框，返回0
        }
        if (Id == GUI_ID_NO) {        		        // No Button
          GUI_EndDialog(hDlg, 1);					        //结束对话框，返回1
        }
        break;
      }
      break;
    default:
      WM_DefaultProc(pMsg);
  }
}




/**
  * @brief  _cbSDViewWin framewin框架的回调函数
	*					
  * @param  none
  * @retval none
  */
static void _cbSDViewWin(WM_MESSAGE * pMsg)
{
	HANDLE_LIST	*appNode;
	
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
  * @brief  _cbSDViewWinC framewin用户窗口的回调函数
	*					
  * @param  none
  * @retval none
  */
static void _cbSDViewWinC(WM_MESSAGE * pMsg)
{
	TREEVIEW_ITEM_INFO ItemInfo;
	TREEVIEW_ITEM_Handle hNode;
	
  switch (pMsg->MsgId) {
		
		case WM_NOTIFY_PARENT:	 //通知代码
			
			switch(pMsg->Data.v)	//通知代码类型
			{
				case WM_NOTIFICATION_RELEASED:	//释放了
					DEBUG("\r\n release");
				
				/* 查看选中了哪个项目 */
				hNode = TREEVIEW_GetSel(pMsg->hWinSrc);		
				
        /* 获取该项目的信息 */
				TREEVIEW_ITEM_GetInfo(hNode,&ItemInfo);
				
				if(ItemInfo.IsNode == 0)        //点击的是目录树的叶子（即文件）
				{
					char *record_file;
					DEBUG("\r\n leaf num =%ld",hNode);					

          /* 打开文件 */
          FRAMEWIN_GetUserData( WM_GetParent(pMsg->hWin),&record_file,sizeof(char *));
          OpenFileProcess(hNode,record_file);
				}
				else										        //结点
				{					
					DEBUG("\r\n node num =%ld",hNode);
				}
				break;				
				
			}	
			
		
    default:		
      WM_DefaultProc(pMsg);
  }

}



/**
  * @brief  OpenFileProcess打开文件	// TBD jpg显示不正常 
	*					
  * @param  none
  * @retval none
  */
static void OpenFileProcess(int sel_num,char* record_file)
{
	char* file_name;
	FIL hFile;
	FRESULT fres;							//返回结果
	unsigned int rw_num;			//已读或已写的字节数

	
	/* 执行对话框，确定是否要打开文件，返回0表示确定 */
	if(	GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbDlgWin, WinPara.hWinMain, 0, 0) == 0)
	{
			//打开文件
		
			file_name 	= (char * ) malloc(FILE_NAME_LEN* sizeof(char));  //为存储目录名的指针分配空间
			
			fres = f_open (&hFile, record_file, FA_READ ); 		            //打开创建索引文件
    
       if(fres != FR_OK)
          return ;
       
			fres = f_lseek (&hFile, sel_num*FILE_NAME_LEN);				        //根据索引值查找将要打开文件的路径
			fres = f_read(&hFile, file_name, FILE_NAME_LEN, &rw_num);
			fres = f_close (&hFile);
			
			if(strstr(file_name,".txt")||strstr(file_name,".TXT")
					||strstr(file_name,".c")||strstr(file_name,".c")
						||strstr(file_name,".cpp")||strstr(file_name,".CPP")
								||strstr(file_name,".h")||strstr(file_name,".h"))//判断是否可以使用txt格式打开
			{
				//	TBD file_name 是malloc来的，作为形参传入函数，小心出错
				TextReader(file_name);
				
			}
			else if(strstr(file_name,".bmp")||strstr(file_name,".BMP")||
								strstr(file_name,".jpg")||strstr(file_name,".JPG")||
									strstr(file_name,".png")||strstr(file_name,".PNG")||
										strstr(file_name,".gif")||strstr(file_name,".GIF"))
			{
				/* 显示图片 */
				ImageReader_FrameWin(file_name);
			}
			else if(strstr(file_name,".mp3")||strstr(file_name,".MP3")||strstr(file_name,".wav")||strstr(file_name,".WAV"))
			{
				GUI_MessageBox("Sorry,do not support open the file!","Sorry",GUI_MESSAGEBOX_CF_MOVEABLE);
			}
			else
			{				
				GUI_MessageBox("Sorry,do not support open the file!","Sorry",GUI_MESSAGEBOX_CF_MOVEABLE);

				}

			
			free(file_name);								//释放malloc空间		
			file_name = NULL;
				
		}	

}


/**
  * @brief  scan_files 递归扫描sd卡内的文件
  * @param  path:初始扫描路径 file_name：指向用来存储文件名的一段空间 hFile:用于记录文件路径的文件指针 hTree 目录树 hNode 目录结点
	*					hTree == NULL &&	hNode == NULL 的话，不创建目录树			
  * @retval result:文件系统的返回值
  */
static FRESULT scan_files (char* path,char* file_name,FIL *hFile,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode,FILE_TYPE fileType,int *fileNum) 
{ 
		
    FRESULT res; 		          //部分在递归过程被修改的变量，不用全局变量	
    FILINFO fno; 
		unsigned int rw_num;			//已读或已写的字节数

    DIR dir; 
    int i; 
    char *fn; 
	
		TREEVIEW_ITEM_Handle hItem;

	
#if _USE_LFN 
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//长文件名支持
    fno.lfname = lfn; 
    fno.lfsize = sizeof(lfn); 
#endif 

    res = f_opendir(&dir, path);                            //打开目录
    if (res == FR_OK) 
			{ 
        i = strlen(path); 
        for (;;) 
				{ 
            res = f_readdir(&dir, &fno); 										//读取目录下的内容
            if (res != FR_OK || fno.fname[0] == 0) break; 	//为空时表示所有项目读取完毕，跳出
#if _USE_LFN 
            fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
            fn = fno.fname; 
#endif 
            if (*fn == '.') continue; 											//点表示当前目录，跳过			
            if (fno.fattrib & AM_DIR) 
						{ 																							//目录，递归读取
							
							  if(hTree != NULL &&	hNode != NULL)
								{
									hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE,fn,0);						//目录，创建结点
									TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);		//把结点加入到目录树中
								}
								
							  sprintf(&path[i], "/%s", fn); 							//合成完整目录名
                res = scan_files(path,file_name,hFile,hTree,hItem,fileType,fileNum);		//递归遍历 
                if (res != FR_OK) 
									break; 																		//打开失败，跳出循环
                path[i] = 0; 
            } 
						else 																														//是文件
						{ 
						
							if(fileType == TEXTFILE )
							{
								if(!(strstr(fn,".txt")||strstr(fn,".TXT")
											||strstr(fn,".c")||strstr(fn,".c")
												||strstr(fn,".cpp")||strstr(fn,".CPP")
														||strstr(fn,".h")||strstr(fn,".h")))						//判断如果不是txt文件，跳出本函数
															{
																	return res;
																}
								
							}
							else if(fileType ==IMAGEFILE)
							{
								if(!(strstr(fn,".bmp")||strstr(fn,".BMP")||
												strstr(fn,".jpg")||strstr(fn,".JPG")||
													strstr(fn,".gif")||strstr(fn,".GIF")||
														strstr(fn,".png")||strstr(fn,".PNG")))					//判断如果不是Image文件，跳出本函数
														{
															return res;																					
														}
							
							}
							
							
							/* 是符合要求的文件 */
							
							/* 根据要求是否创建目录树 */
							if(hTree != NULL &&	hNode != NULL)																			//创建目录树
							{
								hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_LEAF,fn,0);						//文件，创建树叶
								TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);		//把树叶添加到目录树
						
							
								if (strlen(path)+strlen(fn)<FILE_NAME_LEN)
								{
									sprintf(file_name, "%s/%s", path,fn); 	
									
                  //存储文件名到filelist(含路径)										
									res = f_lseek (hFile, hItem*FILE_NAME_LEN);  
									res = f_write (hFile, file_name, FILE_NAME_LEN, &rw_num);						
								}			
							}
							else																																		//不创建目录树
							{
								sprintf(file_name, "%s/%s", path,fn); 	
								//DEBUG("\r\n file_name = %s \r\n",file_name);
                
                //存储文件名到filelist(含路径)										
								res = f_lseek (hFile, (*fileNum)*FILE_NAME_LEN);  
								res = f_write (hFile, file_name, FILE_NAME_LEN, &rw_num);						
							
								(*fileNum)++;	                                                        //记录文件数目

							}
           }//else
        } //for
    } 

    return res; 
} 


/**
  * @brief  Fill_FileList处理非递归过程，然后调用递归函数scan_files扫描目录
	*					
  * @param  path:初始扫描路径
  * @retval none
  */
void Fill_FileList(char* path,char* record_file,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode,FILE_TYPE fileType,int *fileNum)
{
	char * p_path;									//目录名 指针
	char * file_name;								//用于存储的目录文件名，
	FIL		hFile;										//文件句柄	
	FRESULT fres;										//返回结果
	
	
	fres = f_unlink(record_file);//删除旧的filelist		// TBD 增加自建目录

	p_path = (char * ) malloc(PATH_LEN* sizeof(char));  //为存储目录名的指针分配空间
	file_name = (char * ) malloc(FILE_NAME_LEN* sizeof(char));  //为存储目录名的指针分配空间
	
	fres = f_open (&hFile, record_file, FA_READ|FA_WRITE|FA_CREATE_ALWAYS ); //打开创建索引文件
     if(fres != FR_OK)
      return ;

	strcpy(p_path,path);						//复制目录名到指针
	
	fres = scan_files(p_path,file_name,&hFile,hTree,hNode,fileType,fileNum);			//递归扫描文件		
	
	fres = f_close (&hFile);					//关闭索引文件		

	free(p_path);										//释放malloc空间		
	p_path = NULL;
	
	free(file_name);								//释放malloc空间		
	file_name = NULL;

	

}
/**
  * @brief  Fill_TreeView创建目录树
	*					
  * @param  fileType:文件类型
  * @retval none
  */
 void Fill_TreeView(FILE_TYPE fileType ,char* record_file)
{ 

	//WM_HWIN hFrame;									//sdview窗口句柄
  WM_HWIN hFrameC;								//子窗口句柄
	
	WM_HWIN hTree;	  							//目录树句柄
  TREEVIEW_ITEM_Handle hNode;			//结点句柄
	
	HANDLE_LIST *hFrame = hAPPLinkedList_NewNode();

	

  //
  // 输出系统信息
  //


	hFrame->hAPP = FRAMEWIN_CreateUser(0,0,WinPara.xSizeWin,WinPara.ySizeWin,WinPara.hWinMain,WM_CF_SHOW,FRAMEWIN_CF_ACTIVE|FRAMEWIN_CF_MOVEABLE,GUI_ID_FRAMEWIN0,"SD View",0,50);

	FRAMEWIN_SetResizeable(hFrame->hAPP,1);
	/* 创建窗口按钮 */
  FRAMEWIN_AddCloseButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 0);
  FRAMEWIN_AddMaxButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 1);
  FRAMEWIN_AddMinButton(hFrame->hAPP, FRAMEWIN_BUTTON_RIGHT, 2);
	
	/* 把app句柄插入链表 */
	/* 添加结点到链表 */
	hAPPLinkedList_AddTail(hFrame);
	/* 向ctrl窗口发送消息 */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//WM_SetCallback(hFrame,_cbSDViewWin);

	//	TBD 使用回调函数会出现无法移动框架窗口的情况
	_pcbOldSDViewWin = WM_SetCallback(hFrame->hAPP, _cbSDViewWin);	//获取旧的回调函数指针
	
	/* 获取框架窗口用户区的句柄 */
	hFrameC = WM_GetClientWindow(hFrame->hAPP);

  //
  // 创建目录树
  //
  hTree = TREEVIEW_CreateEx(0, 0, WM_GetWindowSizeX(hFrameC),WM_GetWindowSizeY(hFrameC), hFrameC, WM_CF_SHOW, TREEVIEW_CF_AUTOSCROLLBAR_H, GUI_ID_TREEVIEW0);
  TREEVIEW_SetAutoScrollV(hTree, 1);								//使用自动滑动条
  TREEVIEW_SetSelMode(hTree, TREEVIEW_SELMODE_ROW);

	WM_SetCallback(hFrameC,_cbSDViewWinC); //设置目录树的父窗口(背景窗口)的回调函数
  
	// 填充目录树,创建第一个目录结点
  //
  hNode = TREEVIEW_InsertItem(hTree, TREEVIEW_ITEM_TYPE_NODE, 0, 0, "sd Card");
	
	/* 扫描sd卡，填充目录树*/
	Fill_FileList("0:",record_file,hTree,hNode,fileType,NULL);
	
	/* 设置recordfile，在回调函数中使用到 */
	FRAMEWIN_SetUserData(hFrame->hAPP,&record_file ,sizeof(char*));	

}

/**
  * @brief  WFGUI_SDView SD文件浏览器主函数
	*					
  * @param  fileType:文件类型
  * @retval none
  */
void WFGUI_SDView(void )
{

	Fill_TreeView(ALLFILE,FILE_LIST_PATH);
	
}


	


