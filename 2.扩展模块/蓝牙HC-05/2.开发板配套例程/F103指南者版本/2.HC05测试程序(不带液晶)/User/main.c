#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./usart/bsp_usart_blt.h"
#include "./systick/bsp_SysTick.h"
#include "./hc05/bsp_hc05.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 
#include <string.h>
#include <stdlib.h>


unsigned int Task_Delay[NumOfTask]; 
BLTDev bltDevList;

char sendData[1024];
char linebuff[1024];

int main(void)
{	
	char* redata;
	uint16_t len;
	
	static uint8_t hc05_role=1;
	unsigned long count;
	
	char hc05_mode[10]="SLAVE";
	char hc05_name[30]="HC05_SLAVE";
	char hc05_nameCMD[40];
	
  //��ʼ��systick
	SysTick_Init();
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;

	USART_Config();
	
	LED_GPIO_Config();
	Key_GPIO_Config();
	
	
	HC05_INFO("**********HC05ģ��ʵ��************");
	
	
	if(HC05_Init() == 0)
		HC05_INFO("HC05ģ����������");
	else
	{
		HC05_ERROR("HC05ģ���ⲻ����������ģ���뿪��������ӣ�Ȼ��λ���������²��ԡ�");
		while(1);
	}


	/*��λ���ָ�Ĭ��״̬*/
	HC05_Send_CMD("AT+RESET\r\n",1);	
	delay_ms(800);
	
	HC05_Send_CMD("AT+ORGL\r\n",1);
	delay_ms(200);

	
	/*�������������ʾ��Ĭ�ϲ���ʾ��
	 *��bsp_hc05.h�ļ���HC05_DEBUG_ON ������Ϊ1��
	 *����ͨ�����ڵ������ֽ��յ�����Ϣ*/	
	
	//��ȡ����汾��
	HC05_Send_CMD("AT+VERSION?\r\n",1);
	
	//��ȡģ��������ַ
	HC05_Send_CMD("AT+ADDR?\r\n",1);
	
	//���ò�ѯ���ڲ���
	HC05_Send_CMD("AT+UART?\r\n",1);
	
	//����/��ѯ������ģʽ
	HC05_Send_CMD("AT+CMODE?\r\n",1);
	
	//��ȡ����ģ�鹤��״̬
	HC05_Send_CMD("AT+STATE?\r\n",1);	

	//����/��ѯ��ģ���ɫ
	HC05_Send_CMD("AT+ROLE=1\r\n",1);
	
	/*��ʼ��SPP�淶*/
	//��ʼ��SPP�淶��
	HC05_Send_CMD("AT+INIT\r\n",1);
	HC05_Send_CMD("AT+CLASS=0\r\n",1);
	HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
	
	/*����ģ������*/
	sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
	HC05_Send_CMD(hc05_nameCMD,1);

	HC05_INFO("��ģ������Ϊ:%s ,ģ����׼��������",hc05_name);
	
	
	while(!IS_HC05_CONNECTED())
	{
				//��������ģ�飬����������
				//if(Task_Delay[2]==0) 
				//{
					if(hc05_role == 1)	//��ģʽ
					{
						HC05_INFO("����ɨ�������豸...");
						
						linkHC05();
						
						Task_Delay[2]=3000; //��ֵÿ1ms���1������0�ſ������½����������ִ�е�������3s

					}
					else	//��ģʽ
					{
							HC05_Send_CMD("AT+INQ\r\n",1);//ģ���ڲ�ѯ״̬���������ױ������豸������
							delay_ms(5000);
							//HC05_Send_CMD("AT+INQC\r\n",1);//�жϲ�ѯ����ֹ��ѯ�Ľ�����Ŵ���͸���Ľ���

							//Task_Delay[2]=2000; //��ֵÿ1ms���1������0�ſ������½����������ִ�е�������2s
							printf("�������С���");
					}
					

	}

	printf("��Գɹ�");
		
	while(1)
	{

			//���Ӻ�ÿ��һ��ʱ������ջ�����
		if(Task_Delay[0]==0 && IS_HC05_CONNECTED())  
		{
				uint16_t linelen;

				/*��ȡ����*/
				redata = get_rebuff(&len); 
				linelen = get_line(linebuff,redata,len);
			
				/*��������Ƿ��и���*/
				if(linelen<200 && linelen != 0)
				{
					
					if(strcmp(redata,"AT+LED1=ON")==0)
					{
						LED1_ON;						
						HC05_SendString("+LED1:ON\r\nOK\r\n");

					}
					else if(strcmp(redata,"AT+LED1=OFF")==0)
					{
						LED1_OFF;
						HC05_SendString("+LED1:OFF\r\nOK\r\n");

					}
					else
					{
						HC05_INFO("receive:\r\n%s",redata);
					}
					
					/*�������ݺ���ս�������ģ�����ݵĻ�����*/
					clean_rebuff();
					
				}
			Task_Delay[0]=500;//��ֵÿ1ms���1������0�ſ������½����������ִ�е�������500ms
		}
	}
}
		
//		//���Ӻ�ÿ��һ��ʱ��ͨ������ģ�鷢���ַ���
//		if(Task_Delay[1]==0 && IS_HC05_CONNECTED())
//		{
//			static uint8_t testdata=0;
//		
//			sprintf(sendData,"<%s> send data test,testdata=%d\r\n",hc05_name,testdata++);
//			HC05_SendString(sendData);			

//			Task_Delay[1]=5000;//��ֵÿ1ms���1������0�ſ������½����������ִ�е�������5000ms

//		}		
		
		//���KEY1���������л�master-slaveģʽ
//		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
//		{
//			hc05_role=!hc05_role;
//			if(hc05_role == 0)
//			{						
//					HC05_Send_CMD("AT+RESET\r\n",1);
//					delay_ms(800);

//					if(HC05_Send_CMD("AT+ROLE=0\r\n",1) == 0)	
//					{				
//						delay_ms(100);
//						
//						sprintf(hc05_mode,"SLAVE");
//						HC05_INFO("hc05_mode  = %s",hc05_mode);	

//						sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
//						sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
//						
//						if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
//							HC05_INFO("�豸���ֱ�����Ϊ��%s",hc05_name);
//						else
//							HC05_ERROR("��������ʧ��");
//						
//						HC05_Send_CMD("AT+INIT\r\n",1);
//						HC05_Send_CMD("AT+CLASS=0\r\n",1);
//						HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
//						
//						//��������豸�б�
//						bltDevList.num = 0;
//					}
//					


//			}
//			else
//			{
//				HC05_Send_CMD("AT+RESET\r\n",1);
//				delay_ms(800);
//				
//				if(HC05_Send_CMD("AT+ROLE=1\r\n",1) == 0)	
//				{
//					delay_ms(100);
//					
//					sprintf(hc05_mode,"MASTER");
//					HC05_INFO("HC05 mode  = %s",hc05_mode);
//						
//					sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
//					sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);	
//					
//					if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
//						HC05_INFO("�豸���ֱ�����Ϊ��%s",hc05_name);
//					else
//						HC05_ERROR("��������ʧ��");
//						
//					HC05_Send_CMD("AT+INIT\r\n",1);
//					HC05_Send_CMD("AT+CLASS=0\r\n",1);
//					HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);	
//					
//					//��������豸�б�
//					bltDevList.num = 0;

//				}					

//			}

//		}
		
//		//���KEY2���������������һ������
//		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
//		{
//				/*�����������������������*/
//				get_tick_count(&count);
//				srand(count);
//			
//				sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
//				sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);

//				if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
//					HC05_INFO("�豸���ֱ�����Ϊ��%s",hc05_name);
//				else
//					HC05_ERROR("��������ʧ��");
//			}
//		
//	
//	}
//}

/*********************************************END OF FILE**********************/
