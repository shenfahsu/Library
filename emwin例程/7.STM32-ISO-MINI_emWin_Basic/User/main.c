/**
  ******************************************************************************
  * @file    main.c
  * @author  ����
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   emWin��������������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:���� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"

#include "GUI.h"
#include "..\User\lcd\bsp_ili9341_lcd.h"
#include "..\User\usart\bsp_usart1.h"
#include "..\User\SysTick\bsp_SysTick.h"
#include "..\User\led\bsp_led.h"



/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	int i=0;
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
  
  /* ��ʼ����ʱ�� */
	SysTick_Init();
	
	/*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	
	/* ��ʼ��GUI */
	GUI_Init();
	
	/* ��ʼ������ */
	USART1_Config();
	
	/* ���ڵ�����Ϣ */
	printf("\r\n wildfire ISO board emWin test!\r\n");

	
	/* ��ʾ���� */
	GUI_DispStringAt("wildfire ISO board emWin test!",10,10);
	GUI_Delay(100);
	LCD_BK_EN;      //����LCD�����
	
	/* ����2��ʹ�ù̼������IO */
	while (1)
	{
			GUI_DispDecAt(i++,50,50,4);
			if(i>9999)
				i=0;
//		LED1( ON );			  // ��
//    GUI_Delay(1000);
//		LED1( OFF );		  // ��

//		LED2( ON );			  // ��
//    GUI_Delay(1000);
//		LED2( OFF );		  // ��

//		LED3( ON );			  // ��
//    GUI_Delay(1000);
//		LED3( OFF );		  // ��	   
	}

}


/*********************************************END OF FILE**********************/
