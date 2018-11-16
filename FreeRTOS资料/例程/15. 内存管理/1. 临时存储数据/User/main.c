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

#include <string.h>



/************************************ 任务句柄 *********************************/



/********************************** 任务函数声明 *******************************/
static void vTaskStart( void * pvParameters );



/********************************** 内核对象句柄 *******************************/



/********************************** 全局变量声明 *********************************/



/************************************* 主函数 **********************************/
int main ( void )
{	
	/* 创建起始任务 */
	xTaskCreate( vTaskStart, "Task Start", 512, "Hello, welcome to 秉火科技.", 1, NULL );

	
	/* 启动任务调度器，运行FreeRTOS系统 */
  vTaskStartScheduler();
	

}



/********************************** 任务函数定义 *******************************/
static void vTaskStart( void * pvParameters )                       //定义起始任务函数
{
	char * pMallocMem;
	
	
	/* 初始化 */
	LED_Init ();	          //初始化 LED
	Key_Initial ();         //初始化按键
	USARTx_Config ();       //初始化 USART1


	printf ( "\r\n内存管理测试\r\n" );

  
	/* 测试动态申请和释放内存块 */
	pMallocMem = (char *)pvPortMalloc(50);                             //申请动态内存块
	
	printf( "%s\r\n", ( char * ) pvParameters );                       //打印任务函数参数	
	
	strcpy( pMallocMem, pvParameters );                                //拷贝任务函数参数到动态内存块
	
	printf( "%s\r\n", pMallocMem );                                    //打印动态内存块里的字符串
	
	vPortFree(pMallocMem);                                             //如果动态内存块不再使用，应该释放它
	

	vTaskDelete( NULL );                                               //删除起始任务自身
	
	
}



/*********************************************END OF FILE**********************/



