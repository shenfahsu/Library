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
#include "croutine.h"



/************************************ 任务句柄 *********************************/



/********************************** 任务函数声明 *******************************/
static void vTaskStart   ( void * pvParameters );

static void vCoRoutineLed(xCoRoutineHandle xHandle, unsigned portBASE_TYPE uxIndex);



/********************************** 内核对象句柄 *******************************/



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
	uint8_t uxIndex;
	
	
	/* 初始化 */
	LED_Init ();	          //初始化 LED
	Key_Initial ();         //初始化按键
	USARTx_Config ();       //初始化 USART1


	printf ( "\r\n合作式任务测试\r\n" );


	/* 创建三个合作式任务 */
	for( uxIndex = 0; uxIndex < 3; uxIndex++ )
	{
		xCoRoutineCreate(vCoRoutineLed,                                   //任务函数
		                 0,                                               //优先级
		                 uxIndex);                                        //任务索引
	} 
	

	vTaskDelete( NULL );                                                //删除起始任务自身
	
	
}


static void vCoRoutineLed(xCoRoutineHandle xHandle, unsigned portBASE_TYPE uxIndex) //合作式任务
{
	const portTickType xTimeToDelay[ 3 ] = { 1000, 1000, 1000 };

	
	crSTART( xHandle );                                  //开始执行合作式任务

	for( ;; )
	{
		/* 通过任务索引来区分合作式任务 */
		switch ( uxIndex )
		{
			case 0:
				macLED1_TOGGLE();
			  break;
			case 1:
				macLED2_TOGGLE();
			  break;
			case 2:
				macLED3_TOGGLE();
			  break;
			default:
				break;
				
		}

		crDELAY(xHandle, xTimeToDelay[uxIndex]);           //对合作式任务进行延时
		
	}

	crEND();                                             //合作式任务执行结束
	
}


void vApplicationIdleHook( void )                      //空闲钩子函数
{
	vCoRoutineSchedule();                                //调度合作式任务
}



/*********************************************END OF FILE**********************/




