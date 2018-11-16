#ifndef __DS18B20_H
#define	__DS18B20_H

#include "stm32f10x.h"
#include "bsp_SysTick.h"

#define HIGH  1
#define LOW   0

#define DS18B20_CLK     RCC_APB2Periph_GPIOB
#define DS18B20_PIN     GPIO_Pin_10                  
#define DS18B20_PORT		GPIOB 

#define DS18B20_DATATIMEOUT             ((uint32_t)0x000FFFFF)


//���κ꣬��������������һ��ʹ��,����ߵ�ƽ��͵�ƽ
#define DS18B20_DATA_OUT(a)	if (a)	\
					GPIO_SetBits(GPIOB,GPIO_Pin_10);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_10)
 //��ȡ���ŵĵ�ƽ
#define  DS18B20_DATA_IN()	   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)

typedef struct
{
	uint8_t  humi_int;		//ʪ�ȵ���������
	uint8_t  humi_deci;	 	//ʪ�ȵ�С������
	uint8_t  temp_int;	 	//�¶ȵ���������
	uint8_t  temp_deci;	 	//�¶ȵ�С������
	uint8_t  check_sum;	 	//У���
		                 
}DS18B20_Data_TypeDef;

uint8_t DS18B20_Init(void);
float DS18B20_Get_Temp(void);
#endif /* __DS18B20_H */







