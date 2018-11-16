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
	
/************************************* 头文件 **********************************/
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



/************************************ 任务句柄 *********************************/
static TaskHandle_t xHandleTaskLed1 = NULL;



/********************************** 任务函数声明 *******************************/
static void vTaskStart( void * pvParameters );
static void vTaskLed1 ( void * pvParameters );
static void vTaskKey  ( void * pvParameters );



/************************************* 主函数 **********************************/
int main ( void )
{	
	/* 初始化 */
	LED_Init ();	          //初始化 LED
	Key_Initial ();         //初始化按键

	/* 创建起始任务 */
	xTaskCreate( vTaskStart, "Task Start", 512, NULL, 1, NULL );
	
	/* 启动任务调度器，运行FreeRTOS系统 */
  vTaskStartScheduler();
	

}



/********************************** 任务函数定义 *******************************/
static void vTaskStart( void * pvParameters )                                     //起始任务
{
	xTaskCreate( vTaskLed1, "Task Led1", 512, NULL, 1, &xHandleTaskLed1 );          //创建Led1任务
	xTaskCreate( vTaskKey,  "Task Key",  512, NULL, 2, NULL );                      //创建Key任务
	
	vTaskDelete( NULL );                                                            //删除起始任务自身
	
}


static void vTaskLed1( void * pvParameters )                  //LED1任务
{
	TickType_t xLastWakeTime;
	
	
	while(1)                                                    //每隔2s切换一次亮灭状态
	{
		macLED1_ON ();			                                      // 亮
		vTaskDelayUntil(&xLastWakeTime, 2000 / portTICK_RATE_MS); //延时2000ms
		macLED1_OFF ();		                                        // 灭		
		vTaskDelayUntil(&xLastWakeTime, 2000 / portTICK_RATE_MS); //延时2000ms
	}
	
}


static void vTaskKey( void * pvParameters )                                       //按键任务
{
	uint8_t ucKey1Press = 0, ucKey2Press = 0;
	
	
	while(1)
	{
		if( Key_Scan ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1, & ucKey1Press ) )      //如果KEY1被单击
		{
			
			vTaskSuspend( xHandleTaskLed1 );                                            //挂起LED1任务

		}
		else if( Key_Scan ( macKEY2_GPIO_PORT, macKEY2_GPIO_PIN, 1, & ucKey2Press ) ) //如果KEY2被单击
		{
			
			vTaskResume( xHandleTaskLed1 );                                             //恢复LED1任务

		}

		vTaskDelay( 20 / portTICK_RATE_MS );                                          //每20ms扫描一次
		
	}
	
}



/***********************************END OF FILE********************************/






