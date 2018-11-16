/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   SPP_CA蓝牙模块测试程序
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火 指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
	char* redata;
	uint16_t len;
	
	unsigned long count;
	
	char spp_ca_name[30]="BT_FIRE";
	char spp_ca_nameCMD[40];
	char respon_string[100]="";
	
	  //初始化systick
	SysTick_Init();
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;

	USART_Config();
	
	LED_GPIO_Config();
	Key_GPIO_Config();
	
	SPP_CA_INFO("**********SPP_CA模块实验************");
	
	if(SPP_CA_Init() == 0)
	{
		SPP_CA_INFO("SPP_CA模块检测正常。");
	}
	else if	( IS_SPP_CA_CONNECTED() ) //若已经与其它模块处于配对状态，是不会响应AT命令的。
	{
		SPP_CA_ERROR("SPP_CA模块已处于连接状态，请先断开它与其它蓝牙的连接或重新上电，再测试。");
		while(1);
	}	
	else
	{
		SPP_CA_ERROR("SPP_CA模块检测不正常，请检查模块与开发板的连接，然后复位开发板重新测试。");
		while(1);
	}


	/*复位、恢复默认状态*/
	SPP_CA_Send_CMD("AT+RESET\r\n",1);	
	delay_ms(800);
	
	SPP_CA_Send_CMD("AT+DEFAULT\r\n",1);
	delay_ms(200);

	
	/*各种命令测试演示，默认不显示。
	 即可通过串口调试助手接收调试信息*/	
	
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
	
	//设置命令使用示例，把名字设置成spp_ca_name的值,使用其它设备扫描时，
	//会获取得该值加上“-A”的字符串
	
	/*设置模块名字,注意使用这一类设置命令时，不要加"="号*/
	sprintf(spp_ca_nameCMD,"AT+NAME%s\r\n",spp_ca_name);
	SPP_CA_Send_CMD(spp_ca_nameCMD,1);
	SPP_CA_INFO("本模块名字为:%s-A ,模块已准备就绪。",spp_ca_name);
	
	SPP_CA_Get_CMD("AT+NAME\r\n",respon_string);
	SPP_CA_INFO("%s",respon_string);

	while(1)
	{

	  //每隔一段时间检查连接状态
		if(Task_Delay[2]==0 && !IS_SPP_CA_CONNECTED() ) 
		{	
			//模块未连接，打印提示信息
			Task_Delay[2]=5000; //此值每1ms会减1，减到0才可以重新进来这里，所以执行的周期是5s
			SPP_CA_INFO("模块已准备就绪，等待连接...");
		}				
		
			//连接后每隔一段时间检查接收缓冲区
		if(Task_Delay[0]==0 && IS_SPP_CA_CONNECTED())  
		{
				uint16_t linelen;

				/*获取数据*/
				redata = get_rebuff(&len); 
				linelen = get_line(linebuff,redata,len);
			
				/*检查数据是否有更新*/
				if(linelen<200 && linelen != 0)
				{
					//接收到这样的字符串则点灯，可以自行添加其它命令。
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
					
					/*处理数据后，清空接收蓝牙模块数据的缓冲区*/
					clean_rebuff();
					
				}
			Task_Delay[0]=500;//此值每1ms会减1，减到0才可以重新进来这里，所以执行的周期是500ms
		}
		
		//连接后每隔一段时间通过蓝牙模块发送字符串
		if(Task_Delay[1]==0 && IS_SPP_CA_CONNECTED())
		{
			static uint8_t testdata=0;
		
			sprintf(sendData,"<%s> send data test,testdata=%d\r\n",spp_ca_name,testdata++);
			SPP_CA_SendString(sendData);			

			Task_Delay[1]=5000;//此值每1ms会减1，减到0才可以重新进来这里，所以执行的周期是5000ms

		}		
		
				
		//如果KEY2被单击，随机生成一个名字
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
				/*生成随机数，给名字随机编号*/
				get_tick_count(&count);
				srand(count);
			
				sprintf(spp_ca_name,"BT_FIRE_%d",(uint8_t)rand());
				sprintf(spp_ca_nameCMD,"AT+NAME%s\r\n",spp_ca_name);

				if(SPP_CA_Send_CMD(spp_ca_nameCMD,1) == 0)
					SPP_CA_INFO("设备名字被更改为：%s",spp_ca_name);
				else
					SPP_CA_ERROR("更改名字失败");

			}
		
	
	}
}

/*********************************************END OF FILE**********************/
