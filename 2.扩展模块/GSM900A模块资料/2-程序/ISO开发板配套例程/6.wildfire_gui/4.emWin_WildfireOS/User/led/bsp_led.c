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
  * 实验平台:野火 iSO STM32 开发板 
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

		/*开启GPIOB和GPIOF的外设时钟*/
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOF, ENABLE); 

		/*选择要控制的GPIOB引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化GPIOB0*/
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
		
		/*选择要控制的GPIOF引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;

		/*调用库函数，初始化GPIOF7*/
		GPIO_Init(GPIOF, &GPIO_InitStructure);
		
		/*选择要控制的GPIOF引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;

		/*调用库函数，初始化GPIOF7*/
		GPIO_Init(GPIOF, &GPIO_InitStructure);			  

		/* 关闭所有led灯	*/
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
		
		/* 关闭所有led灯	*/
		GPIO_SetBits(GPIOF, GPIO_Pin_7|GPIO_Pin_8);	 
}


/**
* @brief  LED周期反转，给systick周期调用
* @param  None
* @retval None
*/ 
void LED_Toggle(void)
{
  static uint32_t LED_ticks = 0;
    
  if ( LED_ticks++ > 100 )
  {
    LED_ticks = 0;
    
    /* toggle LED1..4 each 100ms */
    LED1_TOGGLE;
    LED2_TOGGLE;
    LED3_TOGGLE;
	}
}
/*********************************************END OF FILE**********************/
