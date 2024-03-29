
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ff.h"
#include "DIALOG.h"


static FRESULT scan_files (char* path,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode) ;

#define FILE_NAME_LEN 	50			//文件名长度，如果检测到文件名超过50 则丢弃这个文件 
#define MUSIC_NAME_LEN 	24			//LCD显示的文件名的最大长度
#define _DF1S	0x81


/**
  * @brief  sdCardView_MainTask处理非递归过程，然后调用递归函数scan_files扫描目录
	*					
  * @param  path:初始扫描路径
  * @retval none
  */
 void sdCardView_MainTask(char* path)
{ 
	FATFS fsys;
	char * p_path;									//目录名 指针
	FRESULT fres;										//返回结果
	
	WM_HWIN hTree;	  							//目录树句柄
  TREEVIEW_ITEM_Handle hNode;			//结点句柄
  int xSize  , ySize;	
  //
  // 获取屏幕大小
  //
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  //
  // 设置背景颜色，滑动条大小，字体
  //
  WM_SetDesktopColor(GUI_BLACK);
  SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
  SCROLLBAR_SetDefaultWidth(25);
  SCROLLBAR_SetThumbSizeMin(25);
  TEXT_SetDefaultFont(GUI_FONT_6X8_ASCII);
  //
  // 输出系统信息
  //
  GUI_DrawGradientV(0, 0, xSize - 1, ySize - 1, GUI_BLUE, GUI_BLACK);
  GUI_SetFont(GUI_FONT_20F_ASCII);
  GUI_DispStringHCenterAt("scanning sd Card...", xSize >> 1, ySize / 3);
  GUI_X_Delay(500);
  //
  // 创建目录树
  //
  hTree = TREEVIEW_CreateEx(0, 0, xSize, ySize, WM_HBKWIN, WM_CF_SHOW, TREEVIEW_CF_AUTOSCROLLBAR_H, GUI_ID_TREEVIEW0);
  TREEVIEW_SetAutoScrollV(hTree, 1);
  TREEVIEW_SetSelMode(hTree, TREEVIEW_SELMODE_ROW);	
  //
  // 填充目录树,创建第一个目录结点
  //
  hNode = TREEVIEW_InsertItem(hTree, TREEVIEW_ITEM_TYPE_NODE, 0, 0, "sd Card");
	
  
	f_mount(0,&fsys);								//注册文件系统工作区	
	
	p_path = (char * ) malloc(500* sizeof(char));  //为存储目录名的指针分配空间
	
	strcpy(p_path,path);						//复制目录名到指针
	
	fres = scan_files(p_path,hTree,hNode);			//递归扫描歌曲文件		
	
	free(p_path);										//释放malloc空间
		
	p_path = NULL;
	
  f_mount(0, NULL);								//释放文件系统工作空间			
}
	
/**
  * @brief  scan_files 递归扫描sd卡内的歌曲文件
  * @param  path:初始扫描路径  hTree 目录树 hNode 目录结点
  * @retval result:文件系统的返回值
  */
static FRESULT scan_files (char* path,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode) 
{ 
		
    FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
    FILINFO fno; 
    DIR dir; 
    int i; 
    char *fn; 
		char file_name[FILE_NAME_LEN];	
	
		TREEVIEW_ITEM_Handle hItem;

	
#if _USE_LFN 
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//长文件名支持
    fno.lfname = lfn; 
    fno.lfsize = sizeof(lfn); 
#endif 

    res = f_opendir(&dir, path); //打开目录
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
								hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE,fn,0);						//目录，创建结点
								TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);		//把结点加入到目录树中
							
							  sprintf(&path[i], "/%s", fn); 							//合成完整目录名
                res = scan_files(path,hTree,hItem);					//递归遍历 
                if (res != FR_OK) 
									break; 																		//打开失败，跳出循环
                path[i] = 0; 
            } 
						else 
						{ 
							hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_LEAF,fn,0);						//文件，创建树叶
							TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);		//把树叶添加到目录树
							
							printf("%s/%s\r\n", path, fn);								//输出文件名								

           }//else
        } //for
    } 

    return res; 
} 
