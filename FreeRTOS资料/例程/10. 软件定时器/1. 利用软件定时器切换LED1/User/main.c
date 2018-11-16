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
#include "bsp_usart1.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "timers.h"



/************************************ 任务句柄 *********************************/



/********************************** 任务函数声明 *******************************/
static void vTaskStart( void * pvParameters );
static void vTaskTmr  ( void * pvParameters );



/********************************** 内核对象句柄 *******************************/
static TimerHandle_t xTimers[3] = {NULL};



/********************************** 全局变量声明 *********************************/



/************************************* 主函数 **********************************/
int main ( void )
{	
	/* 创建起始任务 */
	xTaskCreate( vTaskStart, "Task Start", 512, NULL, 1, NULL );
	
	/* 启动任务调度器，运行FreeRTOS系统 */
  vTaskStartScheduler();
	

}



/********************************** 任务函数定义 *******************************/
static void vTaskStart( void * pvParameters )                       //起始任务
{
	/* 初始化 */
	LED_Init ();	          //初始化 LED
	Key_Initial ();         //初始化按键
	USARTx_Config ();       //初始化 USART1
	
	
	printf ( "\r\n软件定时器测试\r\n" );
	

	xTaskCreate( vTaskTmr,  "Task Tmr", 512, NULL, 1, NULL );         //创建Tmr任务
	
	
	vTaskDelete( NULL );                                               //删除起始任务自身
	
	
}


static void vTimerCallback( xTimerHandle pxTimer )
{
	uint32_t ulTimerID;
	
	
	ulTimerID = (uint32_t) pvTimerGetTimerID( pxTimer );              //获取软件定时器的ID
	
	switch ( ulTimerID )
	{
		case 0:
			macLED1_TOGGLE ();                                             //切换LED1的亮灭状态
			break;
		case 1:
			macLED2_TOGGLE ();                                             //切换LED1的亮灭状态
			break;
		case 2:
			macLED3_TOGGLE ();                                             //切换LED1的亮灭状态
			break;	
    default:
      break;			
	}
	
	
}

static void vTaskTmr( void * pvParameters )                            //Led3任务
{
	uint8_t i;
	
	
	for( i=0; i<3; i++ )
	{
	/* 创建软件定时器 */
	xTimers[i] = xTimerCreate("Timer",                                    //命名定时器
									          (i+1)*1000/portTICK_RATE_MS,                //定时的滴答数
									          pdTRUE,                                     //周期性
									          (void * const)i,                            //定时器ID
									          vTimerCallback);                            //回调函数
	}
	
	for( i=0; i<3; i++ )
	{
	/* 启动软件定时器 */								 
	xTimerStart(xTimers[i],                                                //软件定时器的句柄
	            portMAX_DELAY);									                           //如果无法立即启动的堵塞滴答数
	}  	

	
	vTaskDelete( NULL );                                                   //删除定时器任务自身
	
		
}



/*********************************************END OF FILE**********************/




