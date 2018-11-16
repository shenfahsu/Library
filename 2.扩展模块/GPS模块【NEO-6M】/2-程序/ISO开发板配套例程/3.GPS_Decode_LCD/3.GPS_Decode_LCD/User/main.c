/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   STM32 GPS Һ����ʾ���Գ���
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
#include "bsp_led.h"
#include "GUI.h"
#include "diskio.h"
#include "bsp_touch.h"
#include "bsp_SysTick.h"
#include "bsp_usart1.h"
#include "bsp_sdio_sdcard.h"
#include "gps_config.h"


extern void Touch_MainTask(void);
extern void Fatfs_MainTask(void);

extern void nmea_decode_test(void);



/**
  * @brief  GPS Һ����ʾ���Գ���
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
	
	/* ��ʼ������ */
	Touch_Init();
	
	/* ��ʼ����ʱ�� */
	SysTick_Init();
	
	/*��ʼ��sd��*/
	disk_initialize(0);  
	
	/*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	
	/* ��ʼ��GUI */
	GUI_Init();
	
	/* ��ʼ������*/
	USART1_Config();
  
  /* ��ʼ��GPSģ��ʹ�õĽӿ� */
  GPS_Config();
	
	DEBUG("\r\n Wildfire ISO GPS module test \r\n");
	
	GUI_Delay (20);
	
	#if 0
	/* ����У׼demo */
	Touch_MainTask();
	#else
  
  /* GPS������� */
  nmea_decode_test();  
  
	#endif

}


/*********************************************END OF FILE**********************/
