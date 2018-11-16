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
static void vTaskWrite( void * pvParameters );
static void vTaskRead ( void * pvParameters );



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
	
	
	printf ( "\r\n互斥信号量测试\r\n" );
	
		
	/* 创建互斥信号量 */
	xSemaphore = xSemaphoreCreateMutex();

	
	xTaskCreate( vTaskWrite, "Task Write", 512, NULL, 1, NULL );                      //创建Write任务
	xTaskCreate( vTaskRead,  "Task Read",  512, NULL, 1, NULL );                      //创建Read任务
	
	
	vTaskDelete( NULL );                                                             //删除起始任务自身
	
	
}


static void vTaskWrite( void * pvParameters )     //写任务
{
	while(1)
	{
		xSemaphoreTake( xSemaphore, portMAX_DELAY );  //获取互斥信号量 xSemaphore
		
		ucValue [ 0 ] ++;
		
		vTaskDelay( 100 / portTICK_RATE_MS ); 
		
		ucValue [ 1 ] ++;
		
		xSemaphoreGive( xSemaphore );                 //给出互斥信号量 xSemaphore
		
		taskYIELD();                                  //放弃剩余时间片
		
	}
}


static void vTaskRead( void * pvParameters )     //读任务
{
	while(1)
	{
		xSemaphoreTake( xSemaphore, portMAX_DELAY ); //获取互斥信号量 xSemaphore
		
		if ( ucValue [ 0 ] == ucValue [ 1 ] )
		{ 			
			printf ( "\r\nSuccessful\r\n" );
			
		}
		else
		{

			printf ( "\r\nFail\r\n" );

			
		}
		
		xSemaphoreGive( xSemaphore );                //给出互斥信号量 xSemaphore
		
		vTaskDelay( 1000 / portTICK_RATE_MS );       //每1s读一次
		
	}
}



/*********************************************END OF FILE**********************/


