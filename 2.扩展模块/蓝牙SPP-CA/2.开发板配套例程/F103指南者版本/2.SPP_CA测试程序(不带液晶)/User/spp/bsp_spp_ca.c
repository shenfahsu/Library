/**
  ******************************************************************************
  * @file    bsp_hc05.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   SPP_CA驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火 指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./spp/bsp_spp_ca.h"
#include "./usart/bsp_usart_blt.h"
#include <string.h>
#include <stdio.h>


 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
static void SPP_CA_GPIO_Config(void)
{		
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启GPIO时钟*/
		RCC_APB2PeriphClockCmd( BLT_INT_GPIO_CLK, ENABLE); 

		GPIO_InitStructure.GPIO_Pin = BLT_INT_GPIO_PIN;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(BLT_INT_GPIO_PORT, &GPIO_InitStructure);	
	
}


				   
	
 /**
  * @brief  向SPP_CA模块发送命令并检查OK。只适用于具有OK应答的命令
	* @param  cmd:命令的完整字符串，需要加\r\n。
	* @param	clean 命令结束后是否清除接收缓冲区，1 清除，0 不清除
	* @template  复位命令：	SPP_CA_Send_CMD("AT+RESET\r\n",1);	
  * @retval 0,设置成功;其他,设置失败.	
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
  * @brief  向SPP_CA模块发送获取信息的命令
	* @param  respon_string:存储接收到的响应
	* @note 	SPP_CA模块的获取信息命令的正常返回都不带OK：
						如 ：AT+VERSION
						返回：+VERSION=+BOLUTEK Firmware V2.2, Bluetooth V2.1
  * @retval 0,设置成功;其他,设置失败.	
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
							//删除命令字符串的回车字符
							p = strstr(cmd_temp,"\r\n");
							if(p != NULL)
								*p = '\0';		

							//组成的check_parten用于检查命令返回的信息
							//cmd_temp地址+2是为了去除命令前的“AT”，响应不带“AT”字符
							sprintf(check_parten,"%s=",cmd_temp+2);
							
							SPP_CA_DEBUG("check patern :%s",check_parten);
						
							//正常的命令响应以check_parten开头，以“\r\n”结尾
							if(strstr(redata,check_parten) && strstr(redata,"\r\n"))				
							{
								//以接收完整的命令响应，拷贝、退出
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
  * @brief  使用SPP_CA透传字符串数据
	* @param  str,要传输的字符串
  * @retval 无
  */
void SPP_CA_SendString(char* str)
{
	
		Usart_SendString(SPP_CA_USART,(uint8_t *)str);

}



 /**
  * @brief  初始化GPIO及检测SPP_CA模块
  * @param  无
  * @retval SPP_CA状态，0 正常，非0异常
  */
uint8_t SPP_CA_Init(void)
{

	SPP_CA_GPIO_Config();

	BLT_USART_Config();
	
	return SPP_CA_Send_CMD("AT\r\n",1);
}





 /**
  * @brief  把接收到的字符串转化成16进制形式的数字变量(主要用于转化蓝牙地址)
	* @param  纯粹的数字字符串
  * @retval 转化后的数字变量	
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
  * @brief  在str中，跳过它前面的prefix字符串,
						如str为"abcdefg",prefix为"abc"，则调用本函数后返回指向"defg"的指针
	* @param  str,原字符串
	* @param  str_length，字符串长度
	* @param 	prefix，要跳过的字符串
  * @retval 跳过prefix后的字符串指针
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
  * @brief  从stream中获取一行字符串到line中
	* @param  line,存储获得行的字符串数组
	* @param  stream，原字符串数据流
	* @param 	max_size，stream的大小 
  * @retval line的长度，若stream中没有‘\0’，'\r'，'\n'，则返回0
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
  * @brief  向SPP_CA写入命令，不检查模块的响应
	* @param  command ，要发送的命令
	* @param  arg，命令参数，为0时不带参数，若command也为0时，发送"AT"命令
  * @retval 无
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







