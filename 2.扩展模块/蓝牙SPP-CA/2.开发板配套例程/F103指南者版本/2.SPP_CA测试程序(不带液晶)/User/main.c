/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   SPP_CA����ģ����Գ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:���� ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
 
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./usart/bsp_usart_blt.h"
#include "./systick/bsp_SysTick.h"
#include "./spp/bsp_spp_ca.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 
#include <string.h>
#include <stdlib.h>


unsigned int Task_Delay[NumOfTask]; 



char sendData[1024];
char linebuff[1024];



/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
	char* redata;
	uint16_t len;
	
	unsigned long count;
	
	char spp_ca_name[30]="BT_FIRE";
	char spp_ca_nameCMD[40];
	char respon_string[100]="";
	
	  //��ʼ��systick
	SysTick_Init();
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;

	USART_Config();
	
	LED_GPIO_Config();
	Key_GPIO_Config();
	
	SPP_CA_INFO("**********SPP_CAģ��ʵ��************");
	
	if(SPP_CA_Init() == 0)
	{
		SPP_CA_INFO("SPP_CAģ����������");
	}
	else if	( IS_SPP_CA_CONNECTED() ) //���Ѿ�������ģ�鴦�����״̬���ǲ�����ӦAT����ġ�
	{
		SPP_CA_ERROR("SPP_CAģ���Ѵ�������״̬�����ȶϿ������������������ӻ������ϵ磬�ٲ��ԡ�");
		while(1);
	}	
	else
	{
		SPP_CA_ERROR("SPP_CAģ���ⲻ����������ģ���뿪��������ӣ�Ȼ��λ���������²��ԡ�");
		while(1);
	}


	/*��λ���ָ�Ĭ��״̬*/
	SPP_CA_Send_CMD("AT+RESET\r\n",1);	
	delay_ms(800);
	
	SPP_CA_Send_CMD("AT+DEFAULT\r\n",1);
	delay_ms(200);

	
	/*�������������ʾ��Ĭ�ϲ���ʾ��
	 ����ͨ�����ڵ������ֽ��յ�����Ϣ*/	
	
	SPP_CA_Get_CMD("AT+VERSION\r\n",respon_string);
	SPP_CA_INFO("%s",respon_string);

	SPP_CA_Get_CMD("AT+LADDR\r\n",respon_string);
	SPP_CA_INFO("%s",respon_string);
	
	SPP_CA_Get_CMD("AT+NAME\r\n",respon_string);
	SPP_CA_INFO("%s",respon_string);

	SPP_CA_Get_CMD("AT+BAUD\r\n",respon_string);
	SPP_CA_INFO("%s",respon_string);

	SPP_CA_Get_CMD("AT+PIN\r\n",respon_string);
	SPP_CA_INFO("%s",respon_string);
	
	SPP_CA_Get_CMD("AT+ROLE\r\n",respon_string);
	SPP_CA_INFO("%s",respon_string);
	
	//��������ʹ��ʾ�������������ó�spp_ca_name��ֵ,ʹ�������豸ɨ��ʱ��
	//���ȡ�ø�ֵ���ϡ�-A�����ַ���
	
	/*����ģ������,ע��ʹ����һ����������ʱ����Ҫ��"="��*/
	sprintf(spp_ca_nameCMD,"AT+NAME%s\r\n",spp_ca_name);
	SPP_CA_Send_CMD(spp_ca_nameCMD,1);
	SPP_CA_INFO("��ģ������Ϊ:%s-A ,ģ����׼��������",spp_ca_name);
	
	SPP_CA_Get_CMD("AT+NAME\r\n",respon_string);
	SPP_CA_INFO("%s",respon_string);

	while(1)
	{

	  //ÿ��һ��ʱ��������״̬
		if(Task_Delay[2]==0 && !IS_SPP_CA_CONNECTED() ) 
		{	
			//ģ��δ���ӣ���ӡ��ʾ��Ϣ
			Task_Delay[2]=5000; //��ֵÿ1ms���1������0�ſ������½����������ִ�е�������5s
			SPP_CA_INFO("ģ����׼���������ȴ�����...");
		}				
		
			//���Ӻ�ÿ��һ��ʱ������ջ�����
		if(Task_Delay[0]==0 && IS_SPP_CA_CONNECTED())  
		{
				uint16_t linelen;

				/*��ȡ����*/
				redata = get_rebuff(&len); 
				linelen = get_line(linebuff,redata,len);
			
				/*��������Ƿ��и���*/
				if(linelen<200 && linelen != 0)
				{
					//���յ��������ַ������ƣ�������������������
					if(strcmp(redata,"AT+LED1=ON")==0)
					{
						LED1_ON;						
						SPP_CA_SendString("+LED1:ON\r\nOK\r\n");

					}
					else if(strcmp(redata,"AT+LED1=OFF")==0)
					{
						LED1_OFF;
						SPP_CA_SendString("+LED1:OFF\r\nOK\r\n");

					}
					else
					{
						SPP_CA_INFO("receive:\r\n%s",redata);
					}
					
					/*�������ݺ���ս�������ģ�����ݵĻ�����*/
					clean_rebuff();
					
				}
			Task_Delay[0]=500;//��ֵÿ1ms���1������0�ſ������½����������ִ�е�������500ms
		}
		
		//���Ӻ�ÿ��һ��ʱ��ͨ������ģ�鷢���ַ���
		if(Task_Delay[1]==0 && IS_SPP_CA_CONNECTED())
		{
			static uint8_t testdata=0;
		
			sprintf(sendData,"<%s> send data test,testdata=%d\r\n",spp_ca_name,testdata++);
			SPP_CA_SendString(sendData);			

			Task_Delay[1]=5000;//��ֵÿ1ms���1������0�ſ������½����������ִ�е�������5000ms

		}		
		
				
		//���KEY2���������������һ������
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
				/*�����������������������*/
				get_tick_count(&count);
				srand(count);
			
				sprintf(spp_ca_name,"BT_FIRE_%d",(uint8_t)rand());
				sprintf(spp_ca_nameCMD,"AT+NAME%s\r\n",spp_ca_name);

				if(SPP_CA_Send_CMD(spp_ca_nameCMD,1) == 0)
					SPP_CA_INFO("�豸���ֱ�����Ϊ��%s",spp_ca_name);
				else
					SPP_CA_ERROR("��������ʧ��");

			}
		
	
	}
}

/*********************************************END OF FILE**********************/
