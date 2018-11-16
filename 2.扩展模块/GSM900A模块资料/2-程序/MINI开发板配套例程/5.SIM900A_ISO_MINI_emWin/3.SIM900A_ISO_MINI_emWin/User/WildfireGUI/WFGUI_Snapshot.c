/**
  ******************************************************************************
  * @file    WFGUI_Snapshot.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ��ͼ��ش���
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

#include "DIALOG.h"
#include "MESSAGEBOX.h "
#include "TEXT.h"
#include "bsp_led.h"
#include "ff.h"


#include "WFGUI_Snapshot.h"



//��ͼ�Ĵ洢·��
#define SNAPSHOT_PATH 	"0:WF_OS/Snapshot/test2.bmp"


/**
  * @brief  _WriteByte2File ��Ϊ��Ļ��ͼ�Ļص�����
  * @param  none
  * @retval none
  */
static void _WriteByte2File(U8 Data, void * p) {

		UINT rwb;
		
    /* ����LED����ʾ���ڽ�ͼ */
		LED1_ON;            
  
		/* д������ */
		f_write((FIL *)p,&Data,1,&rwb);	
  
    /* �ر�LED����ʾ��ͼ��� */
		LED1_OFF;
}


/**
  * @brief  WFGUI_Snapshot ����Ļ��ͼ
  * @param  none
  * @retval none
  */
void WFGUI_Snapshot(void)
{

	FIL hFile;
	FRESULT res;
	char fpath[40];

	/* ���ݵ�ǰʱ�䴴����ͼ�ļ��� */
	sprintf(fpath,"0:WF_OS/Snapshot/%04d%02d%02d_%02d%02d%02d.bmp",
					systmtime.tm_year,
          systmtime.tm_mon,
          systmtime.tm_mday,
          systmtime.tm_hour,
          systmtime.tm_min,
          systmtime.tm_sec ); 
	
	/* ���ļ� */
	res = f_open(&hFile,fpath,FA_CREATE_ALWAYS|FA_WRITE);
	if(res == FR_OK)
	{	
		/* ����ļ� */
		GUI_BMP_Serialize(_WriteByte2File, &hFile);
		
		/* �ر��ļ� */
		f_close(&hFile);
		
		LED1_OFF;		
		
		/* ������Ϣ����ʾ��Ϣ */
		MESSAGEBOX_Create("Snapshot succeed!","Snapshot",0);			

	}
	else
	{
		LED1_OFF;
		MESSAGEBOX_Create("Snapshot failed!","Snapshot",0);

	}
}



