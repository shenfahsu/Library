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
static void vTaskKey1 ( void * pvParameters );
static void vTaskKey2 ( void * pvParameters );



/********************************** �ں˶����� *******************************/
static SemaphoreHandle_t  xSemaphore = NULL;



/************************************ ȫ�ֱ������� *********************************/



/************************************* ������ **********************************/
int main ( void )
{	
	/* ������ʼ���� */
	xTaskCreate( vTaskStart, "Task Start", 512, NULL, 1, NULL );
	
	/* �������������������FreeRTOSϵͳ */
  vTaskStartScheduler();
	

}



/********************************** ���������� *******************************/
static void vTaskStart( void * pvParameters )                                    //��ʼ����
{
	/* ��ʼ�� */
	LED_Init ();	          //��ʼ�� LED
	Key_Initial ();         //��ʼ������
	USARTx_Config ();       //��ʼ�� USART1
	
	
	printf ( "\r\n�����ź���ģ��ͣ�����������\r\n" );
	
		
	/* ���������ź��� */
	xSemaphore = xSemaphoreCreateCounting(5,                                       //�ܹ�5��ͣ��λ
	                                      5);                                      //Ŀǰ5��ͣ��λ������

	
	xTaskCreate( vTaskKey1, "Task Key1", 512, NULL, 1, NULL );                      //����Write����
	xTaskCreate( vTaskKey2, "Task Key2", 512, NULL, 1, NULL );                      //����Read����
	
	
	vTaskDelete( NULL );                                                            //ɾ����ʼ��������
	
	
}


static void vTaskKey1( void * pvParameters )                                       //��������
{
	BaseType_t xResult;
	
	uint8_t ucKey1Press = 0;
	
	
	while(1)
	{
		if( Key_Scan ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1, & ucKey1Press ) )      //���KEY1������
		{
		/* �ȴ�һ�������ź��� */
		xResult = xSemaphoreTake( xSemaphore,                                         //�ȴ����ռ����ź��� xSemaphore
		                          0);                                                 //���ȴ�
			
		taskENTER_CRITICAL();
			
		if ( xResult == pdPASS )                      
			printf ( "\r\nKEY1���������ɹ����뵽ͣ��λ��\r\n" );
		else
			printf ( "\r\nKEY1��������������˼������ͣ����������\r\n" );			
			
		taskEXIT_CRITICAL();
		

		}

		vTaskDelay( 20 / portTICK_RATE_MS );                                          //ÿ20msɨ��һ��
		
	}
	
}


static void vTaskKey2( void * pvParameters )                                       //��������
{
	BaseType_t xResult;
	
	uint8_t ucKey2Press = 0;
	
	
	while(1)
	{
		if( Key_Scan ( macKEY2_GPIO_PORT, macKEY2_GPIO_PIN, 1, & ucKey2Press ) )       //���KEY2������
		{
      /* �ͷ�һ�������ź��� */
			xResult = xSemaphoreGive( xSemaphore );                                      //���������ź��� xSemaphore
			
			taskENTER_CRITICAL();
				
			if ( xResult == pdPASS ) 
				printf ( "\r\nKEY2���������ͷ�1��ͣ��λ��\r\n" );	
		  else
			  printf ( "\r\nKEY2�������������޳�λ�����ͷţ�\r\n" );					
				
			taskEXIT_CRITICAL();
			

		}

		vTaskDelay( 20 / portTICK_RATE_MS );                                          //ÿ20msɨ��һ��
		
	}
	
}



/*********************************************END OF FILE**********************/


