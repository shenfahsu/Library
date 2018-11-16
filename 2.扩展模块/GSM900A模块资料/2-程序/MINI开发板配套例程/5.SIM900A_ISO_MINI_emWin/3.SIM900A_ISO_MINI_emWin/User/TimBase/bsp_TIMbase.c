/**
  ******************************************************************************
  * @file    bsp_TimBase.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   TIM2 1ms 定时应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "bsp_TiMbase.h" 


static __IO u32 TIMDelay;


/// TIM2中断优先级配置
static void TIM6_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * TIM_Period / Auto Reload Register(ARR) = 1000   TIM_Prescaler--71 
 * 中断周期为 = 1/(72MHZ /72) * 10 = 10us
 *
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIM_Period(ARR) --> 中断 且TIMxCNT重置为0重新计数 
 */
void TIM6_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		
		/* 设置TIM6CLK 为 72MHZ */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6 , ENABLE);
	
	/* 自动重装载寄存器周期的值(计数值) */
    TIM_TimeBaseStructure.TIM_Period=10;
	
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
	  /* 时钟预分频数为72 */
    TIM_TimeBaseStructure.TIM_Prescaler= 71;
	
		/* 对外部时钟进行采样的时钟分频,这里没有用到 */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	
    TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
		
		TIM6_NVIC_Configuration();
		
		/* 默认不开启定时器 */
		TIM_Cmd(TIM6, DISABLE);
    
}


/**
  * @brief   us延时程序,10us为一个单位
  * @param  
  *		@arg nTime: Delay_us( 1 ) 则实现的延时为 1 * us 
  * @retval  无
  */
void TIMDelay_10us(__IO u32 nTime)
{ 
	TIMDelay = nTime;	

	/* TIM2 重新开时钟，开始计时 */
	TIM_Cmd(TIM6, ENABLE);

	while( TIMDelay !=0);
	
	/* TIM2 关闭，禁止计时 */
	TIM_Cmd(TIM6, DISABLE);
}

/**
  * @brief  获取节拍程序
  * @param  无
  * @retval 无
  * @attention  在 TIM 中断函数 TIM6_IRQHandler()调用
  */
void TIMDelay_Decrement(void)
{
	if (TIMDelay != 0x00)
	{ 
		TIMDelay--;
	}
}
/*********************************************END OF FILE**********************/
