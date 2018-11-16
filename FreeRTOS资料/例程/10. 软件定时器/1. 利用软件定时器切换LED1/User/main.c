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
#include "bsp_key.h"
#include "bsp_usart1.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "timers.h"



/************************************ ������ *********************************/



/********************************** ���������� *******************************/
static void vTaskStart( void * pvParameters );
static void vTaskTmr  ( void * pvParameters );



/********************************** �ں˶����� *******************************/
static TimerHandle_t xTimers[3] = {NULL};



/********************************** ȫ�ֱ������� *********************************/



/************************************* ������ **********************************/
int main ( void )
{	
	/* ������ʼ���� */
	xTaskCreate( vTaskStart, "Task Start", 512, NULL, 1, NULL );
	
	/* �������������������FreeRTOSϵͳ */
  vTaskStartScheduler();
	

}



/********************************** ���������� *******************************/
static void vTaskStart( void * pvParameters )                       //��ʼ����
{
	/* ��ʼ�� */
	LED_Init ();	          //��ʼ�� LED
	Key_Initial ();         //��ʼ������
	USARTx_Config ();       //��ʼ�� USART1
	
	
	printf ( "\r\n�����ʱ������\r\n" );
	

	xTaskCreate( vTaskTmr,  "Task Tmr", 512, NULL, 1, NULL );         //����Tmr����
	
	
	vTaskDelete( NULL );                                               //ɾ����ʼ��������
	
	
}


static void vTimerCallback( xTimerHandle pxTimer )
{
	uint32_t ulTimerID;
	
	
	ulTimerID = (uint32_t) pvTimerGetTimerID( pxTimer );              //��ȡ�����ʱ����ID
	
	switch ( ulTimerID )
	{
		case 0:
			macLED1_TOGGLE ();                                             //�л�LED1������״̬
			break;
		case 1:
			macLED2_TOGGLE ();                                             //�л�LED1������״̬
			break;
		case 2:
			macLED3_TOGGLE ();                                             //�л�LED1������״̬
			break;	
    default:
      break;			
	}
	
	
}

static void vTaskTmr( void * pvParameters )                            //Led3����
{
	uint8_t i;
	
	
	for( i=0; i<3; i++ )
	{
	/* ���������ʱ�� */
	xTimers[i] = xTimerCreate("Timer",                                    //������ʱ��
									          (i+1)*1000/portTICK_RATE_MS,                //��ʱ�ĵδ���
									          pdTRUE,                                     //������
									          (void * const)i,                            //��ʱ��ID
									          vTimerCallback);                            //�ص�����
	}
	
	for( i=0; i<3; i++ )
	{
	/* ���������ʱ�� */								 
	xTimerStart(xTimers[i],                                                //�����ʱ���ľ��
	            portMAX_DELAY);									                           //����޷����������Ķ����δ���
	}  	

	
	vTaskDelete( NULL );                                                   //ɾ����ʱ����������
	
		
}



/*********************************************END OF FILE**********************/




