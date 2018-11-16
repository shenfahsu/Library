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
static void vTaskWrite( void * pvParameters );
static void vTaskRead ( void * pvParameters );



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
	
		
	/* ���������ź��� */
	xSemaphore = xSemaphoreCreateMutex();

	
	xTaskCreate( vTaskWrite, "Task Write", 512, NULL, 1, NULL );                      //����Write����
	xTaskCreate( vTaskRead,  "Task Read",  512, NULL, 1, NULL );                      //����Read����
	
	
	vTaskDelete( NULL );                                                             //ɾ����ʼ��������
	
	
}


static void vTaskWrite( void * pvParameters )     //д����
{
	while(1)
	{
		xSemaphoreTake( xSemaphore, portMAX_DELAY );  //��ȡ�����ź��� xSemaphore
		
		ucValue [ 0 ] ++;
		
		vTaskDelay( 100 / portTICK_RATE_MS ); 
		
		ucValue [ 1 ] ++;
		
		xSemaphoreGive( xSemaphore );                 //���������ź��� xSemaphore
		
		taskYIELD();                                  //����ʣ��ʱ��Ƭ
		
	}
}


static void vTaskRead( void * pvParameters )     //������
{
	while(1)
	{
		xSemaphoreTake( xSemaphore, portMAX_DELAY ); //��ȡ�����ź��� xSemaphore
		
		if ( ucValue [ 0 ] == ucValue [ 1 ] )
		{ 			
			printf ( "\r\nSuccessful\r\n" );
			
		}
		else
		{

			printf ( "\r\nFail\r\n" );

			
		}
		
		xSemaphoreGive( xSemaphore );                //���������ź��� xSemaphore
		
		vTaskDelay( 1000 / portTICK_RATE_MS );       //ÿ1s��һ��
		
	}
}



/*********************************************END OF FILE**********************/


