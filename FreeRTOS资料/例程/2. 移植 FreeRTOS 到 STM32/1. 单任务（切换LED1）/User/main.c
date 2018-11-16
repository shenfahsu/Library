/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
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
		macLED1_ON ();			  // 亮
		vTaskDelay( 5000 );
		macLED1_OFF ();		  // 灭		
		vTaskDelay( 5000 );
	}
	
}


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main ( void )
{	
	LED_Init ();	          //初始化 LED

	xTaskCreate( vTaskLed1, "Task Led1", 512, NULL, 1, NULL );
	
  vTaskStartScheduler();
	

}



/*********************************************END OF FILE**********************/












