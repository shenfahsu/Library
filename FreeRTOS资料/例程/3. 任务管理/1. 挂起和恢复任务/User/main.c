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

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



/************************************ ������ *********************************/
static TaskHandle_t xHandleTaskLed1 = NULL;



/********************************** ���������� *******************************/
static void vTaskStart( void * pvParameters );
static void vTaskLed1 ( void * pvParameters );
static void vTaskKey  ( void * pvParameters );



/************************************* ������ **********************************/
int main ( void )
{	
	/* ��ʼ�� */
	LED_Init ();	          //��ʼ�� LED
	Key_Initial ();         //��ʼ������

	/* ������ʼ���� */
	xTaskCreate( vTaskStart, "Task Start", 512, NULL, 1, NULL );
	
	/* �������������������FreeRTOSϵͳ */
  vTaskStartScheduler();
	

}



/********************************** ���������� *******************************/
static void vTaskStart( void * pvParameters )                                     //��ʼ����
{
	xTaskCreate( vTaskLed1, "Task Led1", 512, NULL, 1, &xHandleTaskLed1 );          //����Led1����
	xTaskCreate( vTaskKey,  "Task Key",  512, NULL, 2, NULL );                      //����Key����
	
	vTaskDelete( NULL );                                                            //ɾ����ʼ��������
	
}


static void vTaskLed1( void * pvParameters )                  //LED1����
{
	TickType_t xLastWakeTime;
	
	
	while(1)                                                    //ÿ��2s�л�һ������״̬
	{
		macLED1_ON ();			                                      // ��
		vTaskDelayUntil(&xLastWakeTime, 2000 / portTICK_RATE_MS); //��ʱ2000ms
		macLED1_OFF ();		                                        // ��		
		vTaskDelayUntil(&xLastWakeTime, 2000 / portTICK_RATE_MS); //��ʱ2000ms
	}
	
}


static void vTaskKey( void * pvParameters )                                       //��������
{
	uint8_t ucKey1Press = 0, ucKey2Press = 0;
	
	
	while(1)
	{
		if( Key_Scan ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1, & ucKey1Press ) )      //���KEY1������
		{
			
			vTaskSuspend( xHandleTaskLed1 );                                            //����LED1����

		}
		else if( Key_Scan ( macKEY2_GPIO_PORT, macKEY2_GPIO_PIN, 1, & ucKey2Press ) ) //���KEY2������
		{
			
			vTaskResume( xHandleTaskLed1 );                                             //�ָ�LED1����

		}

		vTaskDelay( 20 / portTICK_RATE_MS );                                          //ÿ20msɨ��һ��
		
	}
	
}



/***********************************END OF FILE********************************/






