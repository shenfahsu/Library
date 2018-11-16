/**
  ******************************************************************************
  * @file    OLED_I2C.c
  * @author  fire
  * @version V1.0
  * @date    2014-xx-xx
  * @brief   128*64�����OLED��ʾ���Թ��̣���������SD1306����IICͨ�ŷ�ʽ��ʾ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� ISO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
	*
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "OLED_I2C.h"
#include "delay.h"

extern const unsigned char BMP1[];

int main(void)
{
	unsigned char i;

	
	DelayInit();
	I2C_Configuration();
	OLED_Init();
	
	while(1)
	{
		OLED_Fill(0xFF);//ȫ������
		DelayS(2);
		OLED_Fill(0x00);//ȫ����
		DelayS(2);
		for(i=0;i<4;i++)
		{
			OLED_ShowCN(22+i*16,0,i);									//������ʾ����
		}
		DelayS(2);
		OLED_ShowStr(0,3,(unsigned char*)"Wildfire Tech",1);				//����6*8�ַ�
		OLED_ShowStr(0,4,(unsigned char*)"Hello wildfire",2);				//����8*16�ַ�
		DelayS(200);
		OLED_CLS();//����
		OLED_OFF();//����OLED����
		DelayS(2);
		OLED_ON();//����OLED���ߺ���
		OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP1);//����BMPλͼ��ʾ
		DelayS(20);
	}
}
