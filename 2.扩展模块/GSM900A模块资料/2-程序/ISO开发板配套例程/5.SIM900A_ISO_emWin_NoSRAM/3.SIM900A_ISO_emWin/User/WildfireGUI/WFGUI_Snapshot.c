/**
  ******************************************************************************
  * @file    WFGUI_Snapshot.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   截图相关代码
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

#include "DIALOG.h"
#include "MESSAGEBOX.h "
#include "TEXT.h"
#include "bsp_led.h"
#include "ff.h"


#include "WFGUI_Snapshot.h"



//截图的存储路径
#define SNAPSHOT_PATH 	"0:WF_OS/Snapshot/test2.bmp"


/**
  * @brief  _WriteByte2File 作为屏幕截图的回调函数
  * @param  none
  * @retval none
  */
static void _WriteByte2File(U8 Data, void * p) {

		UINT rwb;
		
    /* 点亮LED，表示正在截图 */
		LED1_ON;            
  
		/* 写入数据 */
		f_write((FIL *)p,&Data,1,&rwb);	
  
    /* 关闭LED，表示截图完成 */
		LED1_OFF;
}


/**
  * @brief  WFGUI_Snapshot 对屏幕截图
  * @param  none
  * @retval none
  */
void WFGUI_Snapshot(void)
{

	FIL hFile;
	FRESULT res;
	char fpath[40];

	/* 根据当前时间创建截图文件名 */
	sprintf(fpath,"0:WF_OS/Snapshot/%04d%02d%02d_%02d%02d%02d.bmp",
					systmtime.tm_year,
          systmtime.tm_mon,
          systmtime.tm_mday,
          systmtime.tm_hour,
          systmtime.tm_min,
          systmtime.tm_sec ); 
	
	/* 打开文件 */
	res = f_open(&hFile,fpath,FA_CREATE_ALWAYS|FA_WRITE);
	if(res == FR_OK)
	{	
		/* 输出文件 */
		GUI_BMP_Serialize(_WriteByte2File, &hFile);
		
		/* 关闭文件 */
		f_close(&hFile);
		
		LED1_OFF;		
		
		/* 调用消息框显示信息 */
		MESSAGEBOX_Create("Snapshot succeed!","Snapshot",0);			

	}
	else
	{
		LED1_OFF;
		MESSAGEBOX_Create("Snapshot failed!","Snapshot",0);

	}
}



