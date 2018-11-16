/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   GSMģ��绰���Ų��Գ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� ISO-STM32 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
	*/
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_sdfs_app.h"
#include "bsp_SysTick.h"
#include "sim900a.h"
#include <string.h>
#include "bsp_key.h" 

char    testbuff[100];
uint8_t len;
char *  rebuff;

/*
 * ���Ե绰���Ź���
 * 
 */
int main(void)
{
	static uint8_t timecount=0;
	char num[20]={0};
    /* ����USART1 */
    USART1_Config();
    Key_GPIO_Config();
    /* ��ʼ��ϵͳ��ʱ�� */
    SysTick_Init();
    
    printf("\r\nҰ��WF-SIM900Aģ�鲦������\r\n");          
    
    
    //���ģ����Ӧ�Ƿ�����
    while(sim900a_init()!= SIM900A_TRUE)
    {
      printf("\r\nģ����Ӧ���Բ���������\r\n");
      printf("\r\n��ģ����Ӧ����һֱ������������ģ������ӻ��Ƿ��ѿ�����Դ����\r\n");
    }
    
    printf("\r\nͨ����ģ����Ӧ���ԣ�5���ʼ���Ų���...\r\n");
    
    /* ��ʱ3���ٷ������ģ�� */
    SIM900A_DELAY(5000);
    
    //����绰
  
    sim900a_call("112");                      //����112�绰����
    rebuff = sim900a_waitask(0);
    if(strstr(rebuff,"ATD") != NULL)           //��ӦOK����ʾģ���������յ�����
    {
      printf("\r\n���ں���\r\n");
      SIM900A_CLEAN_RX();                     //������ջ�����
      rebuff = sim900a_waitask(0);            //���µȴ���Ϣ
     
      if(strstr(rebuff,"NO CARRIER") != NULL) //��ӦNO CARRIER,ͨ������
       {
         printf("\r\nͨ������\r\n");
       }
      else if(strstr(rebuff,"NO ANSWER") != NULL)   //��ӦNO ANSWER�����˽���
      {
        printf("\r\n�㲦��ĵ绰��ʱ���˽��������Ժ��ٲ�\r\n");
      }  
    }
	
	SIM900A_CLEAN_RX();                     //������ջ�����
	
    while(1)
	{
		if(timecount>=100)
		{
			if(IsRing(num)== SIM900A_TRUE)
			{
				printf("Ringing:\nFrom:%s\n�밴��KEY2�������߰���KEY1�Ҷ�\n\r",num);	
			}
			timecount=0;
		}
		if(Key_Scan(GPIOC,GPIO_Pin_13,0) == KEY_ON)
		{
			SIM900A_HANGON();
		}
		if(Key_Scan(GPIOA,GPIO_Pin_0,1) == KEY_ON)
		{
			SIM900A_HANGOFF();
		}	
		timecount++;
		SIM900A_DELAY(10);
	}
	
}



