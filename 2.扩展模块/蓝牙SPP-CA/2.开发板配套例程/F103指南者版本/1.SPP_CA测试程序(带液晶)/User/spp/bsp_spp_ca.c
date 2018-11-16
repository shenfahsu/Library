/**
  ******************************************************************************
  * @file    bsp_hc05.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   SPP_CA����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:���� ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./spp/bsp_spp_ca.h"
#include "./usart/bsp_usart_blt.h"
#include <string.h>
#include <stdio.h>


 /**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
static void SPP_CA_GPIO_Config(void)
{		
		GPIO_InitTypeDef GPIO_InitStructure;

		/*����GPIOʱ��*/
		RCC_APB2PeriphClockCmd( BLT_INT_GPIO_CLK, ENABLE); 

		GPIO_InitStructure.GPIO_Pin = BLT_INT_GPIO_PIN;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(BLT_INT_GPIO_PORT, &GPIO_InitStructure);	
	
}


				   
	
 /**
  * @brief  ��SPP_CAģ�鷢��������OK��ֻ�����ھ���OKӦ�������
	* @param  cmd:����������ַ�������Ҫ��\r\n��
	* @param	clean ����������Ƿ�������ջ�������1 �����0 �����
	* @template  ��λ���	SPP_CA_Send_CMD("AT+RESET\r\n",1);	
  * @retval 0,���óɹ�;����,����ʧ��.	
  */
uint8_t SPP_CA_Send_CMD(char* cmd,uint8_t clean)
{	 		 
	uint8_t retry=5;
	uint8_t i,result=1;
	
	while(retry--)
	{
		delay_ms(10);
		Usart_SendString(SPP_CA_USART,(uint8_t *)cmd);
		
			for(i=0;i<20;i++)
			{ 
				uint16_t len;
				char * redata;
				
				delay_ms(10);
				
				redata = get_rebuff(&len); 
				if(len>0)
				{
					if(redata[0]!=0)
					{
						SPP_CA_DEBUG("send CMD: %s",cmd);

						SPP_CA_DEBUG("receive %s",redata);
					}
					if(strstr(redata,"OK"))				
					{
						
						if(clean==1)
							clean_rebuff();
						return 0;
					}
					else
					{
						//clean_rebuff();
					}
				}
				else
				{					
					delay_ms(100);
				}		
			}
			SPP_CA_DEBUG("SPP_CA send CMD fail %d times",retry);
		}
	
	SPP_CA_DEBUG("SPP_CA send CMD fail ");
		
	if(clean==1)
		clean_rebuff();

	return result ;

}





/**
  * @brief  ��SPP_CAģ�鷢�ͻ�ȡ��Ϣ������
	* @param  respon_string:�洢���յ�����Ӧ
	* @note 	SPP_CAģ��Ļ�ȡ��Ϣ������������ض�����OK��
						�� ��AT+VERSION
						���أ�+VERSION=+BOLUTEK Firmware V2.2, Bluetooth V2.1
  * @retval 0,���óɹ�;����,����ʧ��.	
  */
uint8_t SPP_CA_Get_CMD(char* cmd,char* respon_string)
{	 		 
	uint8_t retry=5;
	uint8_t i,result=1;
	char check_parten[30]; 
	
	char *p;	
	char cmd_temp[30];
	
	strcpy(cmd_temp,cmd);
	
	while(retry--)
	{
		delay_ms(10);
		Usart_SendString(SPP_CA_USART,(uint8_t *)cmd);
		
			for(i=0;i<20;i++)
			{ 
				uint16_t len;
				char * redata;
				
				delay_ms(10);
				
				redata = get_rebuff(&len); 
				if(len>0 )
				{
					if(redata[0] != 0)
					{
						SPP_CA_DEBUG("send CMD: %s\r\n",cmd);

						SPP_CA_DEBUG("receive %s",redata);
					}
					{			
							//ɾ�������ַ����Ļس��ַ�
							p = strstr(cmd_temp,"\r\n");
							if(p != NULL)
								*p = '\0';		

							//��ɵ�check_parten���ڼ������ص���Ϣ
							//cmd_temp��ַ+2��Ϊ��ȥ������ǰ�ġ�AT������Ӧ������AT���ַ�
							sprintf(check_parten,"%s=",cmd_temp+2);
							
							SPP_CA_DEBUG("check patern :%s",check_parten);
						
							//������������Ӧ��check_parten��ͷ���ԡ�\r\n����β
							if(strstr(redata,check_parten) && strstr(redata,"\r\n"))				
							{
								//�Խ���������������Ӧ���������˳�
								strcpy(respon_string,redata);
								clean_rebuff();
								return 0;
							}
							else
							{
								//clean_rebuff();
							}
					 }
				}
				else
				{					
					delay_ms(100);
				}		
			}
			SPP_CA_DEBUG("SPP_CA send CMD fail %d times",retry);
		}
	
	SPP_CA_DEBUG("SPP_CA send CMD fail ");
		
	clean_rebuff();

	return result ;

}
		

 /**
  * @brief  ʹ��SPP_CA͸���ַ�������
	* @param  str,Ҫ������ַ���
  * @retval ��
  */
void SPP_CA_SendString(char* str)
{
	
		Usart_SendString(SPP_CA_USART,(uint8_t *)str);

}



 /**
  * @brief  ��ʼ��GPIO�����SPP_CAģ��
  * @param  ��
  * @retval SPP_CA״̬��0 ��������0�쳣
  */
uint8_t SPP_CA_Init(void)
{

	SPP_CA_GPIO_Config();

	BLT_USART_Config();
	
	return SPP_CA_Send_CMD("AT\r\n",1);
}





 /**
  * @brief  �ѽ��յ����ַ���ת����16������ʽ�����ֱ���(��Ҫ����ת��������ַ)
	* @param  ����������ַ���
  * @retval ת��������ֱ���	
  */
unsigned long htoul(const char *str)
{


  long result = 0;

  if (!str)
    return 0;

  while (*str)
  {
    uint8_t value;

    if (*str >= 'a' && *str <= 'f')
      value = (*str - 'a') + 10;
    else if (*str >= 'A' && *str <= 'F')
      value = (*str - 'A') + 10;
    else if (*str >= '0' && *str <= '9')
      value = *str - '0';
    else
      break;

    result = (result * 16) + value;
    ++str;
  }

  return result;
}


 /**
  * @brief  ��str�У�������ǰ���prefix�ַ���,
						��strΪ"abcdefg",prefixΪ"abc"������ñ������󷵻�ָ��"defg"��ָ��
	* @param  str,ԭ�ַ���
	* @param  str_length���ַ�������
	* @param 	prefix��Ҫ�������ַ���
  * @retval ����prefix����ַ���ָ��
  */
char *skipPrefix(char *str, size_t str_length, const char *prefix)
{

  uint16_t prefix_length = strlen(prefix);

  if (!str || str_length == 0 || !prefix)
    return 0;

  if (str_length >= prefix_length && strncmp(str, prefix, prefix_length) == 0)
    return str + prefix_length;

  return 0;
}

 /**
  * @brief  ��stream�л�ȡһ���ַ�����line��
	* @param  line,�洢����е��ַ�������
	* @param  stream��ԭ�ַ���������
	* @param 	max_size��stream�Ĵ�С 
  * @retval line�ĳ��ȣ���stream��û�С�\0����'\r'��'\n'���򷵻�0
  */
int get_line(char* line, char* stream ,int max_size)  
{  
		char *p;	
    int len = 0;  
		p=stream;
    while( *p != '\0' && len < max_size ){  
        line[len++] = *p;  
				p++;
        if('\n' == *p || '\r'==*p)  
            break;  
    }
	
		if(*p != '\0' && *p != '\n' && *p != '\r')
			return 0;
  
      
    line[len] = '\0';  
    return len;  
} 


 /**
  * @brief  ��SPP_CAд����������ģ�����Ӧ
	* @param  command ��Ҫ���͵�����
	* @param  arg�����������Ϊ0ʱ������������commandҲΪ0ʱ������"AT"����
  * @retval ��
  */
void writeCommand(const char *command, const char *arg)
{
		char str_buf[50];

		delay_ms(10);

		if (arg && arg[0] != 0)
			sprintf(str_buf,"AT+%s%s\r\n",command,arg);
		else if (command && command[0] != 0)
		{
			sprintf(str_buf,"AT+%s\r\n",command);
		}
		else
			sprintf(str_buf,"AT\r\n");

		SPP_CA_DEBUG("CMD send:%s",str_buf);
		
		Usart_SendString(SPP_CA_USART,(uint8_t *)str_buf);

}


 
 /*****************************END OF FILE***********************************/







