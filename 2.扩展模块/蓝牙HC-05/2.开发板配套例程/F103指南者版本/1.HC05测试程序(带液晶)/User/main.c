/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   HC05����ģ����Գ���
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
#include "./hc05/bsp_hc05.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 
#include "./lcd/bsp_ili9341_lcd.h"
#include <string.h>
#include <stdlib.h>



unsigned int Task_Delay[NumOfTask]; 



BLTDev bltDevList;


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
	
	static uint8_t hc05_role=0;
	unsigned long count;
	
	char hc05_mode[10]="SLAVE";
	char hc05_name[30]="HC05_SLAVE";
	char hc05_nameCMD[40];
	char disp_buff[200];
	
	 //��ʼ��systick
	SysTick_Init();
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;

	
	ILI9341_Init();
	
	ILI9341_GramScan ( 6 );
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
	
	USART_Config();
	
	LED_GPIO_Config();
	Key_GPIO_Config();
	
	HC05_INFO("**********HC05ģ��ʵ��************");
	ILI9341_DispString_EN ( 40, 20, "HC05 BlueTooth Demo" );

	
	if(HC05_Init() == 0)
	{
		HC05_INFO("HC05ģ����������");
		ILI9341_DispString_EN ( 40, 40, "HC05 module detected!" );

	}
	else
	{
		HC05_ERROR("HC05ģ���ⲻ����������ģ���뿪��������ӣ�Ȼ��λ���������²��ԡ�");
		ILI9341_DispString_EN ( 20, 40, "No HC05 module detected!"  );
		ILI9341_DispString_EN ( 5, 60, "Please check the hardware connection and reset the system." );

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
	
	HC05_Send_CMD("AT+VERSION?\r\n",1);
	
	HC05_Send_CMD("AT+ADDR?\r\n",1);
	
	HC05_Send_CMD("AT+UART?\r\n",1);
	
	HC05_Send_CMD("AT+CMODE?\r\n",1);
	
	HC05_Send_CMD("AT+STATE?\r\n",1);	

	HC05_Send_CMD("AT+ROLE=0\r\n",1);
	
	/*��ʼ��SPP�淶*/
	HC05_Send_CMD("AT+INIT\r\n",1);
	HC05_Send_CMD("AT+CLASS=0\r\n",1);
	HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
	
	/*����ģ������*/
	sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
	HC05_Send_CMD(hc05_nameCMD,1);

	HC05_INFO("��ģ������Ϊ:%s ,ģ����׼��������",hc05_name);
	
	sprintf(disp_buff,"Device name:%s",hc05_name);
	ILI9341_DispString_EN( 20, 60, disp_buff );



	while(1)
	{

		//��������ģ�飬����������
		if(Task_Delay[2]==0 && !IS_HC05_CONNECTED() ) 
		{
			if(hc05_role == 1)	//��ģʽ
			{
				static uint8_t color=0;
				
				HC05_INFO("����ɨ�������豸...");		
				
				if(color==0)	
				{
					ILI9341_DispString_EN ( 5, 80,"Scaning bluetooth device..." );
					color=1;
				}
				else
				{
					LCD_SetColors(BLUE,BLACK);

					ILI9341_DispString_EN ( 5, 80,"Scaning bluetooth device..." );
					color=0;
				}				

				/*��������ģ�飬����������*/
				linkHC05();
				
				Task_Delay[2]=3000; //��ֵÿ1ms���1������0�ſ������½����������ִ�е�������3s

			}
			else	//��ģʽ
			{
					HC05_Send_CMD("AT+INQ\r\n",1);//ģ���ڲ�ѯ״̬���������ױ������豸������
					delay_ms(1000);
					HC05_Send_CMD("AT+INQC\r\n",1);//�жϲ�ѯ����ֹ��ѯ�Ľ�����Ŵ���͸���Ľ���

					ILI9341_Clear(0,80,240,240);
						
					Task_Delay[2]=2000; //��ֵÿ1ms���1������0�ſ������½����������ִ�е�������2s

			}
			
		}				
		
			//���Ӻ�ÿ��һ��ʱ������ջ�����
		if(Task_Delay[0]==0 && IS_HC05_CONNECTED())  
		{
				uint16_t linelen;
			
			
				LCD_SetColors(YELLOW,BLACK);
			
				ILI9341_Clear(0,80,240,20);
				ILI9341_DispString_EN( 5, 80,"Bluetooth connected!"  );


				/*��ȡ����*/
				redata = get_rebuff(&len); 
				linelen = get_line(linebuff,redata,len);
			
				/*��������Ƿ��и���*/
				if(linelen<200 && linelen != 0)
				{
					
					LCD_SetColors(YELLOW,BLACK);

					if(strcmp(redata,"AT+LED1=ON")==0)
					{
						LED1_ON;						
						HC05_SendString("+LED1:ON\r\nOK\r\n");	
						
						ILI9341_Clear(0,100,240,20);						
						ILI9341_DispString_EN ( 5, 100, "receive cmd: AT+LED1=ON" );
					}
					else if(strcmp(redata,"AT+LED1=OFF")==0)
					{
						LED1_OFF;
						HC05_SendString("+LED1:OFF\r\nOK\r\n");
						
						ILI9341_Clear(0,100,240,20);
						ILI9341_DispString_EN ( 5, 100, "receive cmd: AT+LED1=OFF" );
					}
					else
					{
						/*����ֻ��ʾ��ʾ���е����ݣ��������ʾ���������ݣ���ֱ��ʹ��redata����*/
						HC05_INFO("receive:\r\n%s",linebuff);
						
						ILI9341_Clear(0,120,240,200);
						
						LCD_SetColors(RED,BLACK);

						ILI9341_DispString_EN( 5, 120,"receive data:" );
						
						LCD_SetColors(YELLOW,BLACK);
						ILI9341_DispString_EN( 5, 140,linebuff );

					}
					
					/*�������ݺ���ս�������ģ�����ݵĻ�����*/
					clean_rebuff();
					
				}
			Task_Delay[0]=500;//��ֵÿ1ms���1������0�ſ������½����������ִ�е�������500ms
		}
		
		//���Ӻ�ÿ��һ��ʱ��ͨ������ģ�鷢���ַ���
		if(Task_Delay[1]==0 && IS_HC05_CONNECTED())
		{
			static uint8_t testdata=0;
		
			sprintf(sendData,"<%s> send data test,testdata=%d\r\n",hc05_name,testdata++);
			HC05_SendString(sendData);			

			Task_Delay[1]=5000;//��ֵÿ1ms���1������0�ſ������½����������ִ�е�������5000ms

		}		
		
		//���KEY1���������л�master-slaveģʽ
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			hc05_role=!hc05_role;
			if(hc05_role == 0)
			{						
					HC05_Send_CMD("AT+RESET\r\n",1);
					delay_ms(800);

					if(HC05_Send_CMD("AT+ROLE=0\r\n",1) == 0)	
					{				
						delay_ms(100);
						
						sprintf(hc05_mode,"SLAVE");
						HC05_INFO("hc05_mode  = %s",hc05_mode);	

						sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
						sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
						
						if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
						{
							HC05_INFO("�豸���ֱ�����Ϊ��%s",hc05_name);
							
							sprintf(disp_buff,"Device name: %s",hc05_name);
							
							LCD_SetColors(RED,BLACK);

							ILI9341_Clear(0,60,240,20);
							ILI9341_DispString_EN( 5, 60,disp_buff );
						}
						else
						{							
							HC05_ERROR("��������ʧ��");
							
							LCD_SetColors(BLUE,BLACK);
							ILI9341_Clear(0,60,240,20);
							ILI9341_DispString_EN( 5, 60,"Rename fail!" );
						}
						
						HC05_Send_CMD("AT+INIT\r\n",1);
						HC05_Send_CMD("AT+CLASS=0\r\n",1);
						HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
						
						//��������豸�б�
						bltDevList.num = 0;
					}
					


			}
			else
			{
				HC05_Send_CMD("AT+RESET\r\n",1);
				delay_ms(800);
				
				if(HC05_Send_CMD("AT+ROLE=1\r\n",1) == 0)	
				{
					delay_ms(100);
					
					sprintf(hc05_mode,"MASTER");
					HC05_INFO("HC05 mode  = %s",hc05_mode);
						
					sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
					sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);	
					
					if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
					{
						HC05_INFO("�豸���ֱ�����Ϊ��%s",hc05_name);
						sprintf(disp_buff,"Device name: %s",hc05_name);
						
						LCD_SetColors(RED,BLACK);
						ILI9341_Clear(0,60,240,20);
						ILI9341_DispString_EN( 5, 60,disp_buff );

					}
					else
					{
						HC05_ERROR("��������ʧ��");
						
						LCD_SetColors(BLUE,BLACK);
						ILI9341_Clear(0,60,240,20);
						ILI9341_DispString_EN( 5, 60,"Rename fail!" );
					}
						
					HC05_Send_CMD("AT+INIT\r\n",1);
					HC05_Send_CMD("AT+CLASS=0\r\n",1);
					HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);	
					
					//��������豸�б�
					bltDevList.num = 0;

				}					

			}

		}
		
		//���KEY2���������������һ������
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
				/*�����������������������*/
				get_tick_count(&count);
				srand(count);
			
				sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
				sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
				
				if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
				{
					HC05_INFO("�豸���ֱ�����Ϊ��%s",hc05_name);
					sprintf(disp_buff,"Device name: %s",hc05_name);
					
					LCD_SetColors(RED,BLACK);
					ILI9341_Clear(0,60,240,20);
					ILI9341_DispString_EN( 5, 60,disp_buff );

				}
				else
				{
					HC05_ERROR("��������ʧ��");
					
					LCD_SetColors(BLUE,BLACK);
					ILI9341_Clear(0,60,240,20);
					ILI9341_DispString_EN( 5, 60,"Rename fail!"  );
				}

			}
		
	
	}
}

/*********************************************END OF FILE**********************/

