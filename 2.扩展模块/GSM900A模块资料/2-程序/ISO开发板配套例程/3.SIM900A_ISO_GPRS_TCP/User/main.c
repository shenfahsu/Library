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

#define		LOCALPORT	"2000"

#define		SERVERIP	"116.18.42.163"
#define		SERVERPORT	"8234"

const char *TESTBUFF="����GSMģ��TCP�����ϴ����ܲ���";


/*
 * ϵͳ�����λ
 */
void Soft_Reset(void)
{
  __set_FAULTMASK(1);   /* �ر������ж�*/  
  NVIC_SystemReset();   /* ϵͳ��λ */
}

/*
 * ���Է��Ͷ��Ź��ܣ�ʵ�ʲ���ʱ��ѵ绰�����޸ĳ�Ҫ���ն��ŵ��ֻ���
 * 
 */
int main(void)
{
	static uint8_t timecount=1,timestop=0;
    /* ����USART1 */
    USART1_Config();
    
    /* ��ʼ��ϵͳ��ʱ�� */
    SysTick_Init();
    
    printf("\r\nҰ��WF-SIM900Aģ�鷢�Ͷ�������\r\n");
    
    //���ģ����Ӧ�Ƿ�����
    while(sim900a_init()!= SIM900A_TRUE)
    {
      printf("\r\nģ����Ӧ���Բ���������\r\n");
      printf("\r\n��ģ����Ӧ����һֱ������������ģ������ӻ��Ƿ��ѿ�����Դ����\r\n");
    }
	
	printf("\n��ʼ��GPRS��������\n");
    SIM900A_DELAY(1000);          
	sim900a_gprs_init();//GPRS��ʼ������
	printf("\n���Խ���TCP����\n");
	SIM900A_DELAY(1000);
	
	if(sim900a_gprs_tcp_link(LOCALPORT,SERVERIP,SERVERPORT)!=SIM900A_TRUE)
	{
		printf("\r\nTCP����ʧ�ܣ�������ȷ���ø���ģ��\r\n");
		SIM900A_DELAY(1000); 
		printf("\nIP���ӶϿ�\n");
		SIM900A_DELAY(100);
		sim900a_gprs_link_close();	
		
		printf("\n�رճ���\n");
		SIM900A_DELAY(100);
		sim900a_gprs_shut_close();
		printf("\r\n5s���Զ�����\r\n");
		SIM900A_DELAY(5000);  
		Soft_Reset();
	}
	printf("\r\nTCP���ӳɹ������Է�������\n");	
	if(sim900a_gprs_send(TESTBUFF)!=SIM900A_TRUE)
	{
		printf("\r\nTCP��������ʧ�ܣ�������ȷ���ø���ģ��\r\n");
		SIM900A_DELAY(1000); 
		printf("\nIP���ӶϿ�\n");
		SIM900A_DELAY(100);
		sim900a_gprs_link_close();	
		
		printf("\n�رճ���\n");
		SIM900A_DELAY(100);
		sim900a_gprs_shut_close();
		while(1);
	}
	printf("\n�������ݳɹ�\n");
	
	SIM900A_CLEAN_RX();
	
//	printf("\nIP���ӶϿ�\n");
//	SIM900A_DELAY(100);
//	sim900a_gprs_link_close();	
//	
//	printf("\n�رճ���\n");
//	SIM900A_DELAY(100);
//	sim900a_gprs_shut_close();
    while(1)
	{
		if((timecount>=50)&& (timestop!=0xFF))
		{
			
			if(PostGPRS()!=SIM900A_TRUE)
				timestop++;
			else
				timestop=0;
			timecount=0;
		}
		if(timestop==120)//60s
		{
			printf("\nIP���ӶϿ�\n");
			SIM900A_DELAY(100);
			sim900a_gprs_link_close();	
			
			printf("\n�رճ���\n");
			SIM900A_DELAY(100);
			sim900a_gprs_shut_close();
			
			SIM900A_DELAY(1000);
			timestop=0xFF;
		}
		timecount++;
		SIM900A_DELAY(10);
	}		
}



