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
static void vTaskKey1 ( void * pvParameters );
static void vTaskKey2 ( void * pvParameters );



/********************************** 内核对象句柄 *******************************/
static SemaphoreHandle_t  xSemaphore = NULL;



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
static void vTaskStart( void * pvParameters )                                    //起始任务
{
	/* 初始化 */
	LED_Init ();	          //初始化 LED
	Key_Initial ();         //初始化按键
	USARTx_Config ();       //初始化 USART1
	
	
	printf ( "\r\n计数信号量模拟停车场管理测试\r\n" );
	
		
	/* 创建计数信号量 */
	xSemaphore = xSemaphoreCreateCounting(5,                                       //总共5个停车位
	                                      5);                                      //目前5个停车位均可用

	
	xTaskCreate( vTaskKey1, "Task Key1", 512, NULL, 1, NULL );                      //创建Write任务
	xTaskCreate( vTaskKey2, "Task Key2", 512, NULL, 1, NULL );                      //创建Read任务
	
	
	vTaskDelete( NULL );                                                            //删除起始任务自身
	
	
}


static void vTaskKey1( void * pvParameters )                                       //按键任务
{
	BaseType_t xResult;
	
	uint8_t ucKey1Press = 0;
	
	
	while(1)
	{
		if( Key_Scan ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1, & ucKey1Press ) )      //如果KEY1被单击
		{
		/* 等待一个计数信号量 */
		xResult = xSemaphoreTake( xSemaphore,                                         //等待接收计数信号量 xSemaphore
		                          0);                                                 //不等待
			
		taskENTER_CRITICAL();
			
		if ( xResult == pdPASS )                      
			printf ( "\r\nKEY1被单击：成功申请到停车位。\r\n" );
		else
			printf ( "\r\nKEY1被单击：不好意思，现在停车场已满！\r\n" );			
			
		taskEXIT_CRITICAL();
		

		}

		vTaskDelay( 20 / portTICK_RATE_MS );                                          //每20ms扫描一次
		
	}
	
}


static void vTaskKey2( void * pvParameters )                                       //按键任务
{
	BaseType_t xResult;
	
	uint8_t ucKey2Press = 0;
	
	
	while(1)
	{
		if( Key_Scan ( macKEY2_GPIO_PORT, macKEY2_GPIO_PIN, 1, & ucKey2Press ) )       //如果KEY2被单击
		{
      /* 释放一个计数信号量 */
			xResult = xSemaphoreGive( xSemaphore );                                      //给出计数信号量 xSemaphore
			
			taskENTER_CRITICAL();
				
			if ( xResult == pdPASS ) 
				printf ( "\r\nKEY2被单击：释放1个停车位。\r\n" );	
		  else
			  printf ( "\r\nKEY2被单击：但已无车位可以释放！\r\n" );					
				
			taskEXIT_CRITICAL();
			

		}

		vTaskDelay( 20 / portTICK_RATE_MS );                                          //每20ms扫描一次
		
	}
	
}



/*********************************************END OF FILE**********************/


