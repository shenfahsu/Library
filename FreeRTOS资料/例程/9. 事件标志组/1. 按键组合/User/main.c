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



/************************************ ������ *********************************/



/********************************** ���������� *******************************/
static void vTaskStart( void * pvParameters );
static void vTaskKey  ( void * pvParameters );
static void vTaskLed3 ( void * pvParameters );



/********************************** �ں˶����� *******************************/
static EventGroupHandle_t xCreatedEventGroup = NULL;



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
static void vTaskStart( void * pvParameters )                            //��ʼ����
{
	/* ��ʼ�� */
	LED_Init ();	          //��ʼ�� LED
	Key_Initial ();         //��ʼ������
	USARTx_Config ();       //��ʼ�� USART1
	
	
	printf ( "\r\n�����ź�������\r\n" );
	
		
	/* ���������ź��� */
	xCreatedEventGroup = xEventGroupCreate();

	
	xTaskCreate( vTaskKey,  "Task Key",  512, NULL, 1, NULL );             //����Key����
	xTaskCreate( vTaskLed3, "Task Led3", 512, NULL, 1, NULL );             //����Led3����
	
	
	vTaskDelete( NULL );                                                   //ɾ����ʼ��������
	
	
}


static void vTaskKey( void * pvParameters )                              //��������
{
	while(1)
	{
		if( Key_ReadStatus ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1 ) == 1 ) //���KEY1������
		{		
			macLED1_ON ();                                                     //����LED1
			
			xEventGroupSetBits(xCreatedEventGroup, 0x01);                      //��λ�¼���־���BIT0

		}
		else                                                                 //���KEY1���ͷ�
		{		
			macLED1_OFF ();                                                    //Ϩ��LED1
			
			xEventGroupClearBits(xCreatedEventGroup, 0x01);                    //�����¼���־���BIT0

		}		
		
		if( Key_ReadStatus ( macKEY2_GPIO_PORT, macKEY2_GPIO_PIN, 1 ) == 1 ) //���KEY2������
		{		
			macLED2_ON ();                                                     //����LED2
			
			xEventGroupSetBits(xCreatedEventGroup, 0x02);                      //��λ�¼���־���BIT1

		}
		else                                                                 //���KEY2���ͷ�
		{		
			macLED2_OFF ();                                                    //Ϩ��LED2
			
			xEventGroupClearBits(xCreatedEventGroup, 0x02);                    //�����¼���־���BIT1

		}	
		
	}
	
}


static void vTaskLed3( void * pvParameters )                             //Led3����
{
	while(1)
	{
		/* �ȴ��¼���־�� */
		xEventGroupWaitBits(xCreatedEventGroup,                              //�¼���־�����
							          0x03,                                            //�ȴ�BIT0��BIT1
							          pdTRUE,                                          //�¼�������Ѹ���λ������
							          pdTRUE,                                          //�ȴ���λ������λ
							          portMAX_DELAY); 	                               //�����޵ȴ�

		macLED3_ON ();                                                        //����LED3
				
	}
		
}



/*********************************************END OF FILE**********************/


//xEventGroupClearBits


