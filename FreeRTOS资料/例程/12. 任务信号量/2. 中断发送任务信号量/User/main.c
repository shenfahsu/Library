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
#include "bsp_exti.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



/************************************ 任务句柄 *********************************/
TaskHandle_t xHandleTaskLed1 = NULL;



/********************************** 任务函数声明 *******************************/
static void vTaskStart( void * pvParameters );
static void vTaskLed1 ( void * pvParameters );



/************************************* 主函数 **********************************/
int main ( void )
{	
	/* 创建起始任务 */
	xTaskCreate( vTaskStart, "Task Start", 512, NULL, 1, NULL );
	
	/* 启动任务调度器，运行FreeRTOS系统 */
  vTaskStartScheduler();
	

}



/********************************** 任务函数定义 *******************************/
static void vTaskStart( void * pvParameters )                                     //起始任务
{
	/* 初始化 */
	EXTI_Pxy_Config();      //初始化按键
	LED_Init ();	          //初始化 LED
	
	
	xTaskCreate( vTaskLed1, "Task Led1", 512, NULL, 1, &xHandleTaskLed1 );          //创建Led1任务
	
	vTaskDelete( NULL );                                                            //删除起始任务自身
	
}


static void vTaskLed1( void * pvParameters )     //LED1任务
{
	while(1)                                                  
	{
		/* 等待一个二值信号量 */
	  ulTaskNotifyTake( pdTRUE,                    //接收到信号量后将其清零
						          portMAX_DELAY );           //无期限等待
		
		macLED1_TOGGLE();                            //反转LED1的亮灭状态

	}
	
}



/*********************************************END OF FILE**********************/




