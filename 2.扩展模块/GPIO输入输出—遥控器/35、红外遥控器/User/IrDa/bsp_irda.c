#include "bsp_irda.h"
#include "bsp_SysTick.h"
#include "bsp_led.h"



static void                          IrDa_EXTI_NVIC_Configuration                            ( void );
static void                          IrDa_EXTI_Config                                        ( void );



uint32_t frame_data=0;    /* 一帧数据缓存 */
uint8_t  frame_cnt=0;
uint8_t  frame_flag=0;    /* 一帧数据接收完成标志 */



/* 初始化红外接收头1838用到的IO */
void IrDa_Init ( void )
{
  IrDa_EXTI_Config ();
	
}


 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void IrDa_EXTI_NVIC_Configuration ( void )
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig (  macNVIC_PriorityGroup_x );
  
  /* 配置P[A|B|C|D|E]11为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = macIrDa_EXTI_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  配置 PB11 为线中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
static void IrDa_EXTI_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline clock and AFIO clock */
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_AFIO, ENABLE );
	
	macIrDa_EXTI_SCK_APBxClock_FUN ( macIrDa_EXTI_GPIO_CLK, ENABLE );
												
	/* config the NVIC */
	IrDa_EXTI_NVIC_Configuration();

	/* EXTI line gpio config */	
  GPIO_InitStructure.GPIO_Pin = macIrDa_EXTI_GPIO_PIN;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 
  GPIO_Init ( macIrDa_EXTI_GPIO_PORT, &GPIO_InitStructure );

	/* EXTI line mode config */
  GPIO_EXTILineConfig ( macIrDa_EXTI_SOURCE_PORT, macIrDa_EXTI_SOURCE_PIN ); 
  EXTI_InitStructure.EXTI_Line = macIrDa_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init ( & EXTI_InitStructure ); 
}


/* 获取高电平的时间 */
uint8_t IrDa_Get_Pulse_Time(void)
{
  uint8_t time = 0;
  while( macIrDa_DATA_IN() )
  {
    time ++;
    Delay_us(2);     // 延时 20us
    if(time == 250)
      return time;   // 超时溢出   
  }
  return time;
}

/*
 * 帧数据有4个字节，第一个字节是遥控的ID，第二个字节是第一个字节的反码
 * 第三个数据是遥控的真正的键值，第四个字节是第三个字节的反码
 */
uint8_t IrDa_Process(void)
{
  uint8_t first_byte, sec_byte, tir_byte, fou_byte;  
  
  first_byte = frame_data >> 24;
  sec_byte = (frame_data>>16) & 0xff;
  tir_byte = frame_data >> 8;
  fou_byte = frame_data;
  
  /* 记得清标志位 */
  frame_flag = 0;
  
  if( (first_byte==(uint8_t)~sec_byte) && (first_byte==IRDA_ID) )
  {
    if( tir_byte == (u8)~fou_byte )
      return tir_byte;
  }
  
  return 0;   /* 错误返回 */
}

/*********************************************END OF FILE**********************/
