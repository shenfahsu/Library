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
#include "bsp_usart1.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



/************************************ �ں˶��� *********************************/
static xQueueHandle xQueue;



/********************************** ���������� *******************************/
static void vTaskStart  ( void * pvParameters );
static void vTaskSender ( void * pvParameters );
static void vTaskReceive( void * pvParameters );



/********************************** ���������� *******************************/
static void vTaskStart( void * pvParameters )                            //��ʼ����
{
	LED_Init ();	      //��ʼ�� LED
	USARTx_Config ();   //��ʼ�� USART1
	
	
	printf("Test Queue");
	
	
	/* ������Ϣ���� */
	xQueue = xQueueCreate(5, sizeof(uint32_t) );
	
	if(xQueue != NULL)                                                      //�����Ϣ���д����ɹ�
	{
		/* ���������������� */
		xTaskCreate( vTaskSender, "Task Sender1", 512, ( void *)100, 1, NULL);
		xTaskCreate( vTaskSender, "Task Sender2", 512, ( void *)200, 1, NULL);
		
		/* ����һ���������� */
		xTaskCreate( vTaskReceive, "Task Receiver", 512, NULL, 2, NULL );
		
	}	
	else                                                                      //�����Ϣ���д���ʧ��
	{
		while(1);
	}
	
	
	vTaskDelete( NULL );                                                      //ɾ����ʼ��������
	
	
}


static void vTaskSender( void * pvParameters )
{
	uint32_t lValueToSend;
	portBASE_TYPE xStatus;
	
	
	lValueToSend = ( uint32_t ) pvParameters;                  //��ȡ��������ʱ����Ĳ���
	
	
	while(1)
	{
		xStatus = xQueueSendToBack( xQueue, &lValueToSend, 0 );  //��������ȴ���Ϣ���е���Ϣ
		
		if( xStatus != pdPASS )                                  //���������Ϣʧ��
		{
		  printf("���ݲ��ܷ��͵���Ϣ����.\r\n");
		}
		
		taskYIELD();                                             //����ʱ��Ƭ����CPU�ø�ͬ���ȼ�����������
		
	}
	
	
}



static void vTaskReceive( void * pvParameters )
{
	uint32_t lReceivedValue;
	portBASE_TYPE xStatus;
	
	
	while(1)
	{
		xStatus = xQueueReceive( xQueue, &lReceivedValue, portMAX_DELAY );//û�г�ʱ���Ƶȴ���Ϣ
		
		if(xStatus==pdPASS)                                               //����ɹ����մ���Ϣ
		{
			printf( "���յ������ǣ� %d\r\n", lReceivedValue );               //��ӡ��Ϣ����
		}
		else                                                              //���û�н��յ���Ϣ
		{
			printf("û�н��յ���Ϣ.\r\n");
		}
	}
	
	
}



/************************************* ������ **********************************/
int main ( void )
{	
	xTaskCreate( vTaskStart, "Task Start", 512, NULL, 1, NULL );          //������ʼ����
	
	
	vTaskStartScheduler();                                                 //��������������ʼ����FreeRTOSϵͳ

	
}



/*********************************************END OF FILE**********************/



