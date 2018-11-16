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
#include "croutine.h"



/************************************ ������ *********************************/



/********************************** ���������� *******************************/
static void vTaskStart   ( void * pvParameters );

static void vCoRoutineLed(xCoRoutineHandle xHandle, unsigned portBASE_TYPE uxIndex);



/********************************** �ں˶����� *******************************/



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
	uint8_t uxIndex;
	
	
	/* ��ʼ�� */
	LED_Init ();	          //��ʼ�� LED
	Key_Initial ();         //��ʼ������
	USARTx_Config ();       //��ʼ�� USART1


	printf ( "\r\n����ʽ�������\r\n" );


	/* ������������ʽ���� */
	for( uxIndex = 0; uxIndex < 3; uxIndex++ )
	{
		xCoRoutineCreate(vCoRoutineLed,                                   //������
		                 0,                                               //���ȼ�
		                 uxIndex);                                        //��������
	} 
	

	vTaskDelete( NULL );                                                //ɾ����ʼ��������
	
	
}


static void vCoRoutineLed(xCoRoutineHandle xHandle, unsigned portBASE_TYPE uxIndex) //����ʽ����
{
	const portTickType xTimeToDelay[ 3 ] = { 1000, 1000, 1000 };

	
	crSTART( xHandle );                                  //��ʼִ�к���ʽ����

	for( ;; )
	{
		/* ͨ���������������ֺ���ʽ���� */
		switch ( uxIndex )
		{
			case 0:
				macLED1_TOGGLE();
			  break;
			case 1:
				macLED2_TOGGLE();
			  break;
			case 2:
				macLED3_TOGGLE();
			  break;
			default:
				break;
				
		}

		crDELAY(xHandle, xTimeToDelay[uxIndex]);           //�Ժ���ʽ���������ʱ
		
	}

	crEND();                                             //����ʽ����ִ�н���
	
}


void vApplicationIdleHook( void )                      //���й��Ӻ���
{
	vCoRoutineSchedule();                                //���Ⱥ���ʽ����
}



/*********************************************END OF FILE**********************/




