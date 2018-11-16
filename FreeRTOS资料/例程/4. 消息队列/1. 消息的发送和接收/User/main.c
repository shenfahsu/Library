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
#include "bsp_usart1.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



/************************************ 内核对象 *********************************/
static xQueueHandle xQueue;



/********************************** 任务函数声明 *******************************/
static void vTaskStart  ( void * pvParameters );
static void vTaskSender ( void * pvParameters );
static void vTaskReceive( void * pvParameters );



/********************************** 任务函数定义 *******************************/
static void vTaskStart( void * pvParameters )                            //起始任务
{
	LED_Init ();	      //初始化 LED
	USARTx_Config ();   //初始化 USART1
	
	
	printf("Test Queue");
	
	
	/* 创建消息队列 */
	xQueue = xQueueCreate(5, sizeof(uint32_t) );
	
	if(xQueue != NULL)                                                      //如果消息队列创建成功
	{
		/* 创建两个发送任务 */
		xTaskCreate( vTaskSender, "Task Sender1", 512, ( void *)100, 1, NULL);
		xTaskCreate( vTaskSender, "Task Sender2", 512, ( void *)200, 1, NULL);
		
		/* 创建一个接收任务 */
		xTaskCreate( vTaskReceive, "Task Receiver", 512, NULL, 2, NULL );
		
	}	
	else                                                                      //如果消息队列创建失败
	{
		while(1);
	}
	
	
	vTaskDelete( NULL );                                                      //删除起始任务自身
	
	
}


static void vTaskSender( void * pvParameters )
{
	uint32_t lValueToSend;
	portBASE_TYPE xStatus;
	
	
	lValueToSend = ( uint32_t ) pvParameters;                  //获取创建任务时传入的参数
	
	
	while(1)
	{
		xStatus = xQueueSendToBack( xQueue, &lValueToSend, 0 );  //堵塞任务等待消息队列的消息
		
		if( xStatus != pdPASS )                                  //如果发送消息失败
		{
		  printf("数据不能发送到消息队列.\r\n");
		}
		
		taskYIELD();                                             //放弃时间片，把CPU让给同优先级的其他任务
		
	}
	
	
}



static void vTaskReceive( void * pvParameters )
{
	uint32_t lReceivedValue;
	portBASE_TYPE xStatus;
	
	
	while(1)
	{
		xStatus = xQueueReceive( xQueue, &lReceivedValue, portMAX_DELAY );//没有超时限制等待消息
		
		if(xStatus==pdPASS)                                               //如果成功接收大到消息
		{
			printf( "接收的数据是： %d\r\n", lReceivedValue );               //打印消息内容
		}
		else                                                              //如果没有接收到消息
		{
			printf("没有接收到消息.\r\n");
		}
	}
	
	
}



/************************************* 主函数 **********************************/
int main ( void )
{	
	xTaskCreate( vTaskStart, "Task Start", 512, NULL, 1, NULL );          //创建起始任务
	
	
	vTaskStartScheduler();                                                 //启动调度器，开始运行FreeRTOS系统

	
}



/*********************************************END OF FILE**********************/



