 /******************** (C) COPYRIGHT 2011 野火嵌入式开发工作室 ********************
 * 文件名  ：tempad.c
 * 描述    ：DMA方式读取ADC值应用函数库         
 * 实验平台：野火STM32开发板
 * 硬件连接：无	   （温度传感器在内部和ADCx_IN16输入通道相连接）
 *        
 * 库版本  ：ST3.5.0
 *
 * 作者    ：wildfire team 
 * 论坛    ：http://www.firebbs.cn
 * 淘宝    ：https://fire-stm32.taobao.com
**********************************************************************************/
#include "bsp_internal_temp.h"
						

/*
 * 函数名：Temp_ADC1_Init
 * 描述  ：无
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void Temp_ADC1_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	
		/* Enable ADC1 and GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* ADC1 configuration */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                        	//独立工作模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;			                    			//多通道
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			            				//连续转换
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;       	//由软件触发启动
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;											//采集数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 1;				                    					//仅一个通道转换
  ADC_Init(ADC1, &ADC_InitStructure);
	
  	/*配置ADC时钟，为PCLK2的8分频，即9MHz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 

  //设置采样通道IN16, 设置采样时间
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);	

  //使能温度传感器和内部参考电压   
  ADC_TempSensorVrefintCmd(ENABLE); 
  
  /* 使能ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* 使能ADC1校准寄存器  */   
  ADC_ResetCalibration(ADC1);			                              
  /* 等待至复位校准完成 */
  while(ADC_GetResetCalibrationStatus(ADC1));                         

  /* 开始ADC校准 */
  ADC_StartCalibration(ADC1);				                        
  /* 等待至校准完成 */
  while(ADC_GetCalibrationStatus(ADC1));	  
     
  /* 开始ADC转换 */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


/*
 * 函数名：Get_ChipTemperature 获取芯片温度数据
 * 描述  ：无
 * 输入  ：无
 * 输出  ：芯片温度
 * 调用  ：外部调用
 */
float Get_ChipTemperature(void)
{
	uint16_t adc_value;
	float temperature;
	
	/* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	/* 等待至转换完成 */
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);
	
	/* 读取adc原始数据 */
	adc_value = ADC_GetConversionValue (ADC1);
	
	/* 根据手册的公式转换成温度 */
	temperature = (float)(V25-adc_value)/AVG_SLOPE+25;
	
	return temperature;

}
