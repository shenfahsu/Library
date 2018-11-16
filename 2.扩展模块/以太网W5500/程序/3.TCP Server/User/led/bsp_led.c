/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   led应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO-MINI STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_led.h"   

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void LED_GPIO_Config(void)
{		
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启LED的外设时钟*/
	RCC_APB2PeriphClockCmd( LED1_GPIO_CLK|LED2_GPIO_CLK|LED3_GPIO_CLK, ENABLE); 

	/*选择要控制的GPIOB引脚*/															   
	GPIO_InitStructure.GPIO_Pin = LED1_PIN;	

	/*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*设置引脚速率为50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*调用库函数，初始化GPIOB0*/
	GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);	

	/*选择要控制的引脚*/															   
	GPIO_InitStructure.GPIO_Pin = LED2_PIN;

	GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED3_PIN;

	GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);	
	  

	/* 关闭所有led灯	*/
	GPIO_SetBits(LED1_GPIO_PORT, LED1_PIN);
	GPIO_SetBits(LED2_GPIO_PORT, LED2_PIN);	
	GPIO_SetBits(LED3_GPIO_PORT, LED3_PIN);	
}
void LED_RGB111(uint8_t rgb111)
{
	assert_param(IS_RGB111_ALL_PERIPH(rgb111));
	if(rgb111&0x1)
		GPIO_ResetBits(LED1_GPIO_PORT,LED1_PIN);
	else
		GPIO_SetBits(LED1_GPIO_PORT,LED1_PIN);
	if(rgb111&0x2)
		GPIO_ResetBits(LED2_GPIO_PORT,LED2_PIN);
	else
		GPIO_SetBits(LED2_GPIO_PORT,LED2_PIN);
	if(rgb111&0x4)
		GPIO_ResetBits(LED3_GPIO_PORT,LED3_PIN);
	else
		GPIO_SetBits(LED3_GPIO_PORT,LED3_PIN);
}
/*********************************************END OF FILE**********************/

