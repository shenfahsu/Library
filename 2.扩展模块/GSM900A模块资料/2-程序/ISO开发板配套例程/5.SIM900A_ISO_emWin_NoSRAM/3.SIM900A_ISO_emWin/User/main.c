/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "GUI.h"
#include "diskio.h"
#include "ff.h"

#include "bsp_led.h"
#include "bsp_touch.h"
#include "bsp_SysTick.h"
#include "bsp_usart1.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_fsmc_sram.h"
#include "bsp_rtc.h"
#include "bsp_exti.h"

#include "WFGUI_DeskTop.h"
#include "WFGUI_Common.h"

extern void Touch_MainTask(void);

/*ʱ��ṹ��*/
struct rtc_time systmtime;


void BSP_Init(void)
{
	
	static FATFS fsys;
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
	
	/* ��ʼ������ */
	Touch_Init();
	
	/* ��ʼ����ʱ�� */
	SysTick_Init();
	
	/* ���� FSMC Bank1 NOR/SRAM3 */
 // FSMC_SRAM_Init();
	
	/*��ʼ��sd��*/
	disk_initialize(0);
	
	f_mount(0,&fsys);								//ע���ļ�ϵͳ������	// TBD ����sd�����

	/*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	
	/* ��ʼ��GUI */
	GUI_Init();

	/* ��ʼ������*/
	USART1_Config();
	
	/* ����RTC���ж����ȼ� */
	RTC_NVIC_Config();

	RTC_CheckAndConfig(&systmtime);
	
	EXTI_Key_Config();
	
	DEBUG("\r\n wildfire ISO board emWin test \r\n");

}


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	
	BSP_Init();
	
	GUI_Delay (20);
	
	#if 0
	/* ����У׼demo */
	Touch_MainTask();
	
	#else	

	WFGUI_MainTask();	
	
	while(1)
				{

    GUI_Delay(50); //������gui delay�ڼ��Զ��ػ棬����Ҫ��gui_delay
  }
	#endif

}


/*********************************************END OF FILE**********************/
