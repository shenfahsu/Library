/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   测试FreeRTOS
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



/************************************ 任务句柄 *********************************/
static TaskHandle_t xHandleTaskWait = NULL;



/********************************** 任务函数声明 *******************************/
static void vTaskStart( void * pvParameters );
static void vTaskWait ( void * pvParameters );
static void vTaskPost ( void * pvParameters );



/************************************* 主函数 **********************************/
int main ( void )
{	
	/* 创建起始任务 */
	xTaskCreate( vTaskStart, "Task Start", 512, NULL, 1, NULL );
	
	/* 启动任务调度器，运行FreeRTOS系统 */
  vTaskStartScheduler();
	

}



/********************************** 任务函数定义 *******************************/
static void vTaskStart( void * pvParameters )   //定义起始任务
{
	/* 初始化 */
	LED_Init ();	          //初始化 LED
	Key_Initial ();         //初始化按键
	USARTx_Config();        //初始化 USART1，115200-N-1

	
	printf( "\r\n任务消息测试\r\n" );
	
	
	xTaskCreate( vTaskWait, "Task Wait", 512, NULL, 1, &xHandleTaskWait );          //创建Wait任务
	xTaskCreate( vTaskPost, "Task Post", 512, NULL, 1, NULL );                      //创建Post任务
	
	vTaskDelete( NULL );                                                            //删除起始任务自身
	
}


static void vTaskWait( void * pvParameters )     //定义接收任务消息任务
{
	BaseType_t xResult;
	uint32_t   ulNotifiedValue;

	
	while(1)                                                  
	{
		xResult = xTaskNotifyWait(0x00000000,       //函数执行前保留任务消息数据的所有位
						                  0xFFFFFFFF,       //函数执行后清除任务消息数据的所有位
						                  &ulNotifiedValue, //接收任务消息数据
						                  portMAX_DELAY);   //等到有任务消息到来为止
		
		if(xResult == pdPASS)
		{
			printf("接收到的任务消息数据为： %d\r\n", ulNotifiedValue);
		}
		else
		{
			macLED1_TOGGLE();                         //反转LED1的亮灭状态
		}		
		

	}
	
}


static void vTaskPost( void * pvParameters )     //定义发送任务消息任务
{
	uint32_t ucCount = 0;
	
	
	while(1)
	{
		 /* 发送任务消息 */
		xTaskNotify(xHandleTaskWait,                 //目标任务的任务句柄
					      ucCount++,                       //任务消息数据
      		      eSetValueWithOverwrite);         //如果目标任务没有及时接收，上次的数据会被覆盖

		vTaskDelay( 2000 / portTICK_RATE_MS );       //每2s发送一次任务消息
		
	}
	
}



/*********************************************END OF FILE**********************/




