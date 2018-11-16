 /******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���  ��tempad.c
 * ����    ��DMA��ʽ��ȡADCֵӦ�ú�����         
 * ʵ��ƽ̨��Ұ��STM32������
 * Ӳ�����ӣ���	   ���¶ȴ��������ڲ���ADCx_IN16����ͨ�������ӣ�
 *        
 * ��汾  ��ST3.5.0
 *
 * ����    ��wildfire team 
 * ��̳    ��http://www.firebbs.cn
 * �Ա�    ��https://fire-stm32.taobao.com
**********************************************************************************/
#include "bsp_internal_temp.h"
						

/*
 * ��������Temp_ADC1_Init
 * ����  ����
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void Temp_ADC1_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	
		/* Enable ADC1 and GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* ADC1 configuration */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                        	//��������ģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;			                    			//��ͨ��
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			            				//����ת��
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;       	//�������������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;											//�ɼ������Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 1;				                    					//��һ��ͨ��ת��
  ADC_Init(ADC1, &ADC_InitStructure);
	
  	/*����ADCʱ�ӣ�ΪPCLK2��8��Ƶ����9MHz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 

  //���ò���ͨ��IN16, ���ò���ʱ��
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);	

  //ʹ���¶ȴ��������ڲ��ο���ѹ   
  ADC_TempSensorVrefintCmd(ENABLE); 
  
  /* ʹ��ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* ʹ��ADC1У׼�Ĵ���  */   
  ADC_ResetCalibration(ADC1);			                              
  /* �ȴ�����λУ׼��� */
  while(ADC_GetResetCalibrationStatus(ADC1));                         

  /* ��ʼADCУ׼ */
  ADC_StartCalibration(ADC1);				                        
  /* �ȴ���У׼��� */
  while(ADC_GetCalibrationStatus(ADC1));	  
     
  /* ��ʼADCת�� */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


/*
 * ��������Get_ChipTemperature ��ȡоƬ�¶�����
 * ����  ����
 * ����  ����
 * ���  ��оƬ�¶�
 * ����  ���ⲿ����
 */
float Get_ChipTemperature(void)
{
	uint16_t adc_value;
	float temperature;
	
	/* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	/* �ȴ���ת����� */
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);
	
	/* ��ȡadcԭʼ���� */
	adc_value = ADC_GetConversionValue (ADC1);
	
	/* �����ֲ�Ĺ�ʽת�����¶� */
	temperature = (float)(V25-adc_value)/AVG_SLOPE+25;
	
	return temperature;

}
