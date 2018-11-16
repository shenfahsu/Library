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

#include <string.h>



/************************************ ������ *********************************/



/********************************** ���������� *******************************/
static void vTaskStart( void * pvParameters );



/********************************** �ں˶����� *******************************/



/********************************** ȫ�ֱ������� *********************************/



/************************************* ������ **********************************/
int main ( void )
{	
	/* ������ʼ���� */
	xTaskCreate( vTaskStart, "Task Start", 512, "Hello, welcome to ����Ƽ�.", 1, NULL );

	
	/* �������������������FreeRTOSϵͳ */
  vTaskStartScheduler();
	

}



/********************************** ���������� *******************************/
static void vTaskStart( void * pvParameters )                       //������ʼ������
{
	char * pMallocMem;
	
	
	/* ��ʼ�� */
	LED_Init ();	          //��ʼ�� LED
	Key_Initial ();         //��ʼ������
	USARTx_Config ();       //��ʼ�� USART1


	printf ( "\r\n�ڴ�������\r\n" );

  
	/* ���Զ�̬������ͷ��ڴ�� */
	pMallocMem = (char *)pvPortMalloc(50);                             //���붯̬�ڴ��
	
	printf( "%s\r\n", ( char * ) pvParameters );                       //��ӡ����������	
	
	strcpy( pMallocMem, pvParameters );                                //������������������̬�ڴ��
	
	printf( "%s\r\n", pMallocMem );                                    //��ӡ��̬�ڴ������ַ���
	
	vPortFree(pMallocMem);                                             //�����̬�ڴ�鲻��ʹ�ã�Ӧ���ͷ���
	

	vTaskDelete( NULL );                                               //ɾ����ʼ��������
	
	
}



/*********************************************END OF FILE**********************/



