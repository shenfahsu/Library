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
static void vTaskStart   ( void * pvParameters );
static void vTaskKey     ( void * pvParameters ); 
static void vTaskSender1 ( void * pvParameters );
static void vTaskSender2 ( void * pvParameters );
static void vTaskPend    ( void * pvParameters );



/********************************** 内核对象句柄 *******************************/
static SemaphoreHandle_t xSemaphore = NULL;
static xQueueHandle      xQueue1    = NULL;
static xQueueHandle      xQueue2    = NULL;
static xQueueSetHandle   xQueueSet  = NULL;



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


	printf ( "\r\n等待多个内核对象测试\r\n" );


	/* 创建二值信号量 */
	xSemaphore = xSemaphoreCreateBinary();                                    //按键信号量

	/* 创建两个消息队列 */
	xQueue1 = xQueueCreate( 10, sizeof(uint32_t) );	                          //消息队列1
	xQueue2 = xQueueCreate( 10, sizeof(uint32_t) );                           //消息队列2

	/* 创建一个内核对象集合 */
	xQueueSet = xQueueCreateSet( 1 );                                       

	
	/* 添加内核对象到对象集合 xQueueSet */
  xQueueAddToSet(xSemaphore, xQueueSet);
	xQueueAddToSet(xQueue1,    xQueueSet);
	xQueueAddToSet(xQueue2,    xQueueSet);


  /* 创建任务 */
	xTaskCreate( vTaskPend,    "Task MsgPro",  512, NULL,         1, NULL );  //创建等待对象任务
	xTaskCreate( vTaskKey,     "Task Key",     512, NULL,         2, NULL );  //创建按键任务
	xTaskCreate( vTaskSender1, "Task Sender1", 512, ( void *)100, 2, NULL );  //创建发送消息队列1任务
	xTaskCreate( vTaskSender2, "Task Sender2", 512, ( void *)200, 2, NULL );  //创建发送消息队列2任务


	vTaskDelete( NULL );                                                      //删除起始任务自身
	
	
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


static void vTaskSender1( void * pvParameters )              //发送消息队列1任务
{
	uint32_t lValueToSend;
	portBASE_TYPE xStatus;
	
	
	lValueToSend = ( uint32_t ) pvParameters;                  //获取创建任务时传入的参数
	
	
	while(1)
	{
		/* 发送消息到消息队列 xQueue1 */
		xStatus = xQueueSendToBack( xQueue1,                      //发送到消息队列xQueue1
		                            &lValueToSend, 
		                            0 ); 
		
		if( xStatus != pdPASS )                                   //如果发送消息失败
		{
		  printf("发送数据到队列xQueue1失败.\r\n");
		}
                          
	  vTaskDelay( 2000 / portTICK_RATE_MS );                     //每2s发送一次	
	
	}
	
	
}


static void vTaskSender2( void * pvParameters )               //发送消息队列2任务
{
	uint32_t lValueToSend;
	portBASE_TYPE xStatus;
	
	
	lValueToSend = ( uint32_t ) pvParameters;                   //获取创建任务时传入的参数
	
	
	while(1)
	{
		/* 发送消息到消息队列 xQueue2 */
		xStatus = xQueueSendToBack( xQueue2,                       //发送到消息队列xQueue2
		                            &lValueToSend, 
		                            0 ); 
		
		if( xStatus != pdPASS )                                    //如果发送消息失败
		{
		  printf("发送数据到队列xQueue2失败.\r\n");
		}
		
                                      
	  vTaskDelay( 2000 / portTICK_RATE_MS );                                          //每2s发送一次	
	
	}
	
	
}


static void vTaskPend(void *pvParameters)
{
	QueueSetMemberHandle_t xActivatedMember;
	uint32_t ulQueueMsgValue;
	uint8_t  ucQueueMsgValue;

	
	while(1)
	{
	    /* 等待多个内核对象 */
			xActivatedMember = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);

			/* 判断接收哪个内核对象 */
			if(xActivatedMember == xQueue1)                                    //如果接收到的对象是xQueue1
			{
		      /* 接收xQueue1的消息 */
					xQueueReceive(xActivatedMember, &ulQueueMsgValue, 0);
					printf("接收到消息队列1接收到消息内容为 %d\r\n", ulQueueMsgValue);
			}
			else if(xActivatedMember == xQueue2)                               //如果接收到的对象是xQueue2
			{
		      /* 接收xQueue2的消息 */
					xQueueReceive(xActivatedMember, &ucQueueMsgValue, 0);
					printf("接收到消息队列2接收到消息内容为 %d\r\n", ucQueueMsgValue);  
			}
			else if(xActivatedMember == xSemaphore)                             //如果接收到的对象是xSemaphore
			{
					/* 接收信号量 */	
					if( xSemaphoreTake(xActivatedMember, 0) == pdPASS )
					  macLED1_TOGGLE();                                             //反转LED1的亮灭状态              
			}

	}
	
}



/*********************************************END OF FILE**********************/




