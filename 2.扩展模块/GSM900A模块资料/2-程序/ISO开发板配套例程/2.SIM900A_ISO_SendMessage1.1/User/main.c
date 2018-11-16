/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   GSMģ�鷢�Ͷ��Ų��Գ���ʵ�ʲ���ʱ��ѵ绰�����޸ĳ�Ҫ���ն��ŵ��ֻ���
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


/* ���ն��ŵĵ绰���� */
const char num[]="112";

/*
 * ���Է��Ͷ��Ź��ܣ�ʵ�ʲ���ʱ��ѵ绰�����޸ĳ�Ҫ���ն��ŵ��ֻ���
 * 
 */
int main(void)
{
	uint8_t newmessadd=0,IsRead=0;
//	static uint8_t timecount=0;
	char namenum[20*4]={0},str[512]={0},gbkstr[256]={0},namegbk[20]={0};
     /* ����USART1 */
    USART1_Config();

    /* ����USART2 */
    USART2_Config();
    
    /* ��ʼ��ϵͳ��ʱ�� */
    SysTick_Init();
    
    printf("\r\nҰ��WF-SIM900Aģ�鷢�Ͷ�������\r\n");
    
    //���ģ����Ӧ�Ƿ�����
    while(sim900a_cmd("AT\r","OK",1000) != SIM900A_TRUE)
    {
      printf("\r\nģ����Ӧ���Բ���������\r\n");
      printf("\r\n��ģ����Ӧ����һֱ������������ģ������ӻ��Ƿ��ѿ�����Դ����\r\n");
    }
    
    printf("\r\nͨ����ģ����Ӧ���ԣ�5���ʼ���Ͷ��Ų���... \r\n");
    
    /* ��ʱ5���ٷ������ģ�� */
    SIM900A_DELAY(5000);    
    
    //���Ͷ���
		
    sim900a_sms((char *)num,"hellow world");
    printf("\r\nӢ�Ķ����ѷ�������%s��Ϊ������ԣ����ڳ������޸Ľ��ն��ŵ��ֻ�����",num);
    
    //���ټ��5s�ٷ��ڶ�������
   
    SIM900A_DELAY(5000); 
    //��Ӣ�Ķ��ţ�ʵ�ʲ���ʱ��ѵ绰�����޸ĳ�Ҫ���ն��ŵ��ֻ���
    sim900a_sms((char *)num,"Ұ��WF-SIM900Aģ����Ų���");		
	printf("\r\n��Ӣ�Ķ����ѷ�������%s��Ϊ������ԣ����ڳ������޸Ľ��ն��ŵ��ֻ�����",num);
	SIM900A_DELAY(5000);
	SIM900A_DELAY(5000);	
	printf("\r\n���ն��Ų��ԣ�ͨ������1��ӡ�¶�����Ϣ\r\n");
    while(1)
	{
		SIM900A_DELAY(1000);
		newmessadd=IsReceiveMS();	
		if(newmessadd)
		{			
			IsRead=readmessage(newmessadd,namenum,str);				
			 
			//printf("newmessadd=%d,IsRead:%d\n",newmessadd,IsRead);
			if(IsRead)
			{
				//hexuni2gbk(namenum,namegbk);	
				hexuni2gbk(str,gbkstr);						
				printf("�¶���:\n������:%s\n����:%s\n\r",namenum,gbkstr);
			}
		}
		
	}
}



