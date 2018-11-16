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



/************************************ 任务句柄 *********************************/



/********************************** 任务函数声明 *******************************/
static void vTaskStart( void * pvParameters );
static void vTaskKey  ( void * pvParameters );
static void vTaskLed3 ( void * pvParameters );



/********************************** 内核对象句柄 *******************************/
static EventGroupHandle_t xCreatedEventGroup = NULL;



/************************************ 全局变量声明 *********************************/



/************************************* 主函数 **********************************/
int main ( void )
{	
	/* 创建起始任务 */
	xTaskCreate( vTaskStart, "Task Start", 512, NULL, 1, NULL );
	
	/* 启动任务调度器，运行FreeRTOS系统 */
  vTaskStartScheduler();
	

}



/********************************** 任务函数定义 *******************************/
static void vTaskStart( void * pvParameters )                            //起始任务
{
	/* 初始化 */
	LED_Init ();	          //初始化 LED
	Key_Initial ();         //初始化按键
	USARTx_Config ();       //初始化 USART1
	
	
	printf ( "\r\n互斥信号量测试\r\n" );
	
		
	/* 创建互斥信号量 */
	xCreatedEventGroup = xEventGroupCreate();

	
	xTaskCreate( vTaskKey,  "Task Key",  512, NULL, 1, NULL );             //创建Key任务
	xTaskCreate( vTaskLed3, "Task Led3", 512, NULL, 1, NULL );             //创建Led3任务
	
	
	vTaskDelete( NULL );                                                   //删除起始任务自身
	
	
}


static void vTaskKey( void * pvParameters )                              //按键任务
{
	while(1)
	{
		if( Key_ReadStatus ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1 ) == 1 ) //如果KEY1被按下
		{		
			macLED1_ON ();                                                     //点亮LED1
			
			xEventGroupSetBits(xCreatedEventGroup, 0x01);                      //置位事件标志组的BIT0

		}
		else                                                                 //如果KEY1被释放
		{		
			macLED1_OFF ();                                                    //熄灭LED1
			
			xEventGroupClearBits(xCreatedEventGroup, 0x01);                    //清零事件标志组的BIT0

		}		
		
		if( Key_ReadStatus ( macKEY2_GPIO_PORT, macKEY2_GPIO_PIN, 1 ) == 1 ) //如果KEY2被按下
		{		
			macLED2_ON ();                                                     //点亮LED2
			
			xEventGroupSetBits(xCreatedEventGroup, 0x02);                      //置位事件标志组的BIT1

		}
		else                                                                 //如果KEY2被释放
		{		
			macLED2_OFF ();                                                    //熄灭LED2
			
			xEventGroupClearBits(xCreatedEventGroup, 0x02);                    //清零事件标志组的BIT1

		}	
		
	}
	
}


static void vTaskLed3( void * pvParameters )                             //Led3任务
{
	while(1)
	{
		/* 等待事件标志组 */
		xEventGroupWaitBits(xCreatedEventGroup,                              //事件标志组对象
							          0x03,                                            //等待BIT0和BIT1
							          pdTRUE,                                          //事件发生后把该两位均清零
							          pdTRUE,                                          //等待两位均被置位
							          portMAX_DELAY); 	                               //无期限等待

		macLED3_ON ();                                                        //点亮LED3
				
	}
		
}



/*********************************************END OF FILE**********************/


//xEventGroupClearBits


