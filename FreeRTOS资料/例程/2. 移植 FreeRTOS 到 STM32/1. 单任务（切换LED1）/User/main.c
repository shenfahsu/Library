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
	
#include "stm32f10x.h"
#include "bsp_led.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



void vTaskLed1( void * pvParameters )
{
	while(1)
	{
		macLED1_ON ();			  // ��
		vTaskDelay( 5000 );
		macLED1_OFF ();		  // ��		
		vTaskDelay( 5000 );
	}
	
}


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main ( void )
{	
	LED_Init ();	          //��ʼ�� LED

	xTaskCreate( vTaskLed1, "Task Led1", 512, NULL, 1, NULL );
	
  vTaskStartScheduler();
	

}



/*********************************************END OF FILE**********************/












