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
static void vTaskStart   ( void * pvParameters );
static void vTaskKey     ( void * pvParameters ); 
static void vTaskSender1 ( void * pvParameters );
static void vTaskSender2 ( void * pvParameters );
static void vTaskPend    ( void * pvParameters );



/********************************** �ں˶����� *******************************/
static SemaphoreHandle_t xSemaphore = NULL;
static xQueueHandle      xQueue1    = NULL;
static xQueueHandle      xQueue2    = NULL;
static xQueueSetHandle   xQueueSet  = NULL;



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


	printf ( "\r\n�ȴ�����ں˶������\r\n" );


	/* ������ֵ�ź��� */
	xSemaphore = xSemaphoreCreateBinary();                                    //�����ź���

	/* ����������Ϣ���� */
	xQueue1 = xQueueCreate( 10, sizeof(uint32_t) );	                          //��Ϣ����1
	xQueue2 = xQueueCreate( 10, sizeof(uint32_t) );                           //��Ϣ����2

	/* ����һ���ں˶��󼯺� */
	xQueueSet = xQueueCreateSet( 1 );                                       

	
	/* ����ں˶��󵽶��󼯺� xQueueSet */
  xQueueAddToSet(xSemaphore, xQueueSet);
	xQueueAddToSet(xQueue1,    xQueueSet);
	xQueueAddToSet(xQueue2,    xQueueSet);


  /* �������� */
	xTaskCreate( vTaskPend,    "Task MsgPro",  512, NULL,         1, NULL );  //�����ȴ���������
	xTaskCreate( vTaskKey,     "Task Key",     512, NULL,         2, NULL );  //������������
	xTaskCreate( vTaskSender1, "Task Sender1", 512, ( void *)100, 2, NULL );  //����������Ϣ����1����
	xTaskCreate( vTaskSender2, "Task Sender2", 512, ( void *)200, 2, NULL );  //����������Ϣ����2����


	vTaskDelete( NULL );                                                      //ɾ����ʼ��������
	
	
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


static void vTaskSender1( void * pvParameters )              //������Ϣ����1����
{
	uint32_t lValueToSend;
	portBASE_TYPE xStatus;
	
	
	lValueToSend = ( uint32_t ) pvParameters;                  //��ȡ��������ʱ����Ĳ���
	
	
	while(1)
	{
		/* ������Ϣ����Ϣ���� xQueue1 */
		xStatus = xQueueSendToBack( xQueue1,                      //���͵���Ϣ����xQueue1
		                            &lValueToSend, 
		                            0 ); 
		
		if( xStatus != pdPASS )                                   //���������Ϣʧ��
		{
		  printf("�������ݵ�����xQueue1ʧ��.\r\n");
		}
                          
	  vTaskDelay( 2000 / portTICK_RATE_MS );                     //ÿ2s����һ��	
	
	}
	
	
}


static void vTaskSender2( void * pvParameters )               //������Ϣ����2����
{
	uint32_t lValueToSend;
	portBASE_TYPE xStatus;
	
	
	lValueToSend = ( uint32_t ) pvParameters;                   //��ȡ��������ʱ����Ĳ���
	
	
	while(1)
	{
		/* ������Ϣ����Ϣ���� xQueue2 */
		xStatus = xQueueSendToBack( xQueue2,                       //���͵���Ϣ����xQueue2
		                            &lValueToSend, 
		                            0 ); 
		
		if( xStatus != pdPASS )                                    //���������Ϣʧ��
		{
		  printf("�������ݵ�����xQueue2ʧ��.\r\n");
		}
		
                                      
	  vTaskDelay( 2000 / portTICK_RATE_MS );                                          //ÿ2s����һ��	
	
	}
	
	
}


static void vTaskPend(void *pvParameters)
{
	QueueSetMemberHandle_t xActivatedMember;
	uint32_t ulQueueMsgValue;
	uint8_t  ucQueueMsgValue;

	
	while(1)
	{
	    /* �ȴ�����ں˶��� */
			xActivatedMember = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);

			/* �жϽ����ĸ��ں˶��� */
			if(xActivatedMember == xQueue1)                                    //������յ��Ķ�����xQueue1
			{
		      /* ����xQueue1����Ϣ */
					xQueueReceive(xActivatedMember, &ulQueueMsgValue, 0);
					printf("���յ���Ϣ����1���յ���Ϣ����Ϊ %d\r\n", ulQueueMsgValue);
			}
			else if(xActivatedMember == xQueue2)                               //������յ��Ķ�����xQueue2
			{
		      /* ����xQueue2����Ϣ */
					xQueueReceive(xActivatedMember, &ucQueueMsgValue, 0);
					printf("���յ���Ϣ����2���յ���Ϣ����Ϊ %d\r\n", ucQueueMsgValue);  
			}
			else if(xActivatedMember == xSemaphore)                             //������յ��Ķ�����xSemaphore
			{
					/* �����ź��� */	
					if( xSemaphoreTake(xActivatedMember, 0) == pdPASS )
					  macLED1_TOGGLE();                                             //��תLED1������״̬              
			}

	}
	
}



/*********************************************END OF FILE**********************/




