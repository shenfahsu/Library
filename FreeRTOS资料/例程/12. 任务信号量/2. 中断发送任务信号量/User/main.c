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
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
	
/************************************* ͷ�ļ� **********************************/
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_exti.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



/************************************ ������ *********************************/
TaskHandle_t xHandleTaskLed1 = NULL;



/********************************** ���������� *******************************/
static void vTaskStart( void * pvParameters );
static void vTaskLed1 ( void * pvParameters );



/************************************* ������ **********************************/
int main ( void )
{	
	/* ������ʼ���� */
	xTaskCreate( vTaskStart, "Task Start", 512, NULL, 1, NULL );
	
	/* �������������������FreeRTOSϵͳ */
  vTaskStartScheduler();
	

}



/********************************** ���������� *******************************/
static void vTaskStart( void * pvParameters )                                     //��ʼ����
{
	/* ��ʼ�� */
	EXTI_Pxy_Config();      //��ʼ������
	LED_Init ();	          //��ʼ�� LED
	
	
	xTaskCreate( vTaskLed1, "Task Led1", 512, NULL, 1, &xHandleTaskLed1 );          //����Led1����
	
	vTaskDelete( NULL );                                                            //ɾ����ʼ��������
	
}


static void vTaskLed1( void * pvParameters )     //LED1����
{
	while(1)                                                  
	{
		/* �ȴ�һ����ֵ�ź��� */
	  ulTaskNotifyTake( pdTRUE,                    //���յ��ź�����������
						          portMAX_DELAY );           //�����޵ȴ�
		
		macLED1_TOGGLE();                            //��תLED1������״̬

	}
	
}



/*********************************************END OF FILE**********************/




