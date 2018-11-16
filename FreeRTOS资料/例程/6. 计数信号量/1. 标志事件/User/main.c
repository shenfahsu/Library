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



/************************************ 任务句柄 *********************************/




/********************************** 任务函数声明 *******************************/
static void vTaskStart( void * pvParameters );
static void vTaskLed1 ( void * pvParameters );
static void vTaskKey  ( void * pvParameters );



/********************************** 内核对象句柄 *******************************/
static SemaphoreHandle_t  xSemaphore = NULL;



/************************************ 全局变量声明 *********************************/
uint8_t ucValue [ 2 ] = { 0x00, 0x00 };



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
	LED_Init ();	          //初始化 LED
	Key_Initial ();         //初始化按键
	USARTx_Config ();       //初始化 USART1
	
	
	printf ( "\r\n计数信号量测试\r\n" );
	
		
	/* 创建计数信号量，信号量计数值为0 */
	xSemaphore = xSemaphoreCreateCounting(1, 0);

	
	xTaskCreate( vTaskLed1, "Task Led1", 512, NULL, 1, NULL );                      //创建Led1任务
	xTaskCreate( vTaskKey,  "Task Key",  512, NULL, 1, NULL );                      //创建Key任务
	
	
	vTaskDelete( NULL );                                                            //删除起始任务自身
	
	
}


static void vTaskLed1( void * pvParameters )     //LED1任务
{
	while(1)                                                  
	{
		/* 等待一个计数信号量 */
		xSemaphoreTake( xSemaphore,                  //等待接收计数信号量 xSemaphore
		                portMAX_DELAY);              //无期限等待
		
		macLED1_TOGGLE();                            //反转LED1的亮灭状态

	}
	
}


static void vTaskKey( void * pvParameters )                                       //按键任务
{
	uint8_t ucKey1Press = 0;
	
	
	while(1)
	{
		if( Key_Scan ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1, & ucKey1Press ) )      //如果KEY1被单击
		{
      /* 给出一个计数信号量 */
			xSemaphoreGive( xSemaphore );                                               //给出计数信号量 xSemaphore

		}

		vTaskDelay( 20 / portTICK_RATE_MS );                                          //每20ms扫描一次
		
	}
	
}



/*********************************************END OF FILE**********************/


