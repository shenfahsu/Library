/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   ����FreeRTOS
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



/************************************ ������ *********************************/
static TaskHandle_t xHandleTaskWait = NULL;



/********************************** ���������� *******************************/
static void vTaskStart( void * pvParameters );
static void vTaskWait ( void * pvParameters );
static void vTaskPost ( void * pvParameters );



/************************************* ������ **********************************/
int main ( void )
{	
	/* ������ʼ���� */
	xTaskCreate( vTaskStart, "Task Start", 512, NULL, 1, NULL );
	
	/* �������������������FreeRTOSϵͳ */
  vTaskStartScheduler();
	

}



/********************************** ���������� *******************************/
static void vTaskStart( void * pvParameters )   //������ʼ����
{
	/* ��ʼ�� */
	LED_Init ();	          //��ʼ�� LED
	Key_Initial ();         //��ʼ������
	USARTx_Config();        //��ʼ�� USART1��115200-N-1

	
	printf( "\r\n������Ϣ����\r\n" );
	
	
	xTaskCreate( vTaskWait, "Task Wait", 512, NULL, 1, &xHandleTaskWait );          //����Wait����
	xTaskCreate( vTaskPost, "Task Post", 512, NULL, 1, NULL );                      //����Post����
	
	vTaskDelete( NULL );                                                            //ɾ����ʼ��������
	
}


static void vTaskWait( void * pvParameters )     //�������������Ϣ����
{
	BaseType_t xResult;
	uint32_t   ulNotifiedValue;

	
	while(1)                                                  
	{
		xResult = xTaskNotifyWait(0x00000000,       //����ִ��ǰ����������Ϣ���ݵ�����λ
						                  0xFFFFFFFF,       //����ִ�к����������Ϣ���ݵ�����λ
						                  &ulNotifiedValue, //����������Ϣ����
						                  portMAX_DELAY);   //�ȵ���������Ϣ����Ϊֹ
		
		if(xResult == pdPASS)
		{
			printf("���յ���������Ϣ����Ϊ�� %d\r\n", ulNotifiedValue);
		}
		else
		{
			macLED1_TOGGLE();                         //��תLED1������״̬
		}		
		

	}
	
}


static void vTaskPost( void * pvParameters )     //���巢��������Ϣ����
{
	uint32_t ucCount = 0;
	
	
	while(1)
	{
		 /* ����������Ϣ */
		xTaskNotify(xHandleTaskWait,                 //Ŀ�������������
					      ucCount++,                       //������Ϣ����
      		      eSetValueWithOverwrite);         //���Ŀ������û�м�ʱ���գ��ϴε����ݻᱻ����

		vTaskDelay( 2000 / portTICK_RATE_MS );       //ÿ2s����һ��������Ϣ
		
	}
	
}



/*********************************************END OF FILE**********************/




