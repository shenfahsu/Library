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



/************************************ ������ *********************************/




/********************************** ���������� *******************************/
static void vTaskStart( void * pvParameters );
static void vTaskLed1 ( void * pvParameters );
static void vTaskKey  ( void * pvParameters );



/********************************** �ں˶����� *******************************/
static SemaphoreHandle_t  xSemaphore = NULL;



/************************************ ȫ�ֱ������� *********************************/
uint8_t ucValue [ 2 ] = { 0x00, 0x00 };



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
	LED_Init ();	          //��ʼ�� LED
	Key_Initial ();         //��ʼ������
	USARTx_Config ();       //��ʼ�� USART1
	
	
	printf ( "\r\n�����ź�������\r\n" );
	
		
	/* ���������ź������ź�������ֵΪ0 */
	xSemaphore = xSemaphoreCreateCounting(1, 0);

	
	xTaskCreate( vTaskLed1, "Task Led1", 512, NULL, 1, NULL );                      //����Led1����
	xTaskCreate( vTaskKey,  "Task Key",  512, NULL, 1, NULL );                      //����Key����
	
	
	vTaskDelete( NULL );                                                            //ɾ����ʼ��������
	
	
}


static void vTaskLed1( void * pvParameters )     //LED1����
{
	while(1)                                                  
	{
		/* �ȴ�һ�������ź��� */
		xSemaphoreTake( xSemaphore,                  //�ȴ����ռ����ź��� xSemaphore
		                portMAX_DELAY);              //�����޵ȴ�
		
		macLED1_TOGGLE();                            //��תLED1������״̬

	}
	
}


static void vTaskKey( void * pvParameters )                                       //��������
{
	uint8_t ucKey1Press = 0;
	
	
	while(1)
	{
		if( Key_Scan ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1, & ucKey1Press ) )      //���KEY1������
		{
      /* ����һ�������ź��� */
			xSemaphoreGive( xSemaphore );                                               //���������ź��� xSemaphore

		}

		vTaskDelay( 20 / portTICK_RATE_MS );                                          //ÿ20msɨ��һ��
		
	}
	
}



/*********************************************END OF FILE**********************/


