/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   GSM模块发送短信测试程序，实际测试时请把电话号码修改成要接收短信的手机号
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 ISO-STM32 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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

const char *TESTBUFF="秉火GSM模块TCP数据上传功能测试";


/*
 * 系统软件复位
 */
void Soft_Reset(void)
{
  __set_FAULTMASK(1);   /* 关闭所有中断*/  
  NVIC_SystemReset();   /* 系统复位 */
}

/*
 * 测试发送短信功能，实际测试时请把电话号码修改成要接收短信的手机号
 * 
 */
int main(void)
{
	static uint8_t timecount=1,timestop=0;
    /* 配置USART1 */
    USART1_Config();
    
    /* 初始化系统定时器 */
    SysTick_Init();
    
    printf("\r\n野火WF-SIM900A模块发送短信例程\r\n");
    
    //检测模块响应是否正常
    while(sim900a_init()!= SIM900A_TRUE)
    {
      printf("\r\n模块响应测试不正常！！\r\n");
      printf("\r\n若模块响应测试一直不正常，请检查模块的连接或是否已开启电源开关\r\n");
    }
	
	printf("\n初始化GPRS工作环境\n");
    SIM900A_DELAY(1000);          
	sim900a_gprs_init();//GPRS初始化环境
	printf("\n尝试建立TCP链接\n");
	SIM900A_DELAY(1000);
	
	if(sim900a_gprs_tcp_link(LOCALPORT,SERVERIP,SERVERPORT)!=SIM900A_TRUE)
	{
		printf("\r\nTCP链接失败，请检测正确设置各个模块\r\n");
		SIM900A_DELAY(1000); 
		printf("\nIP链接断开\n");
		SIM900A_DELAY(100);
		sim900a_gprs_link_close();	
		
		printf("\n关闭场景\n");
		SIM900A_DELAY(100);
		sim900a_gprs_shut_close();
		printf("\r\n5s后自动重启\r\n");
		SIM900A_DELAY(5000);  
		Soft_Reset();
	}
	printf("\r\nTCP链接成功，尝试发送数据\n");	
	if(sim900a_gprs_send(TESTBUFF)!=SIM900A_TRUE)
	{
		printf("\r\nTCP发送数据失败，请检测正确设置各个模块\r\n");
		SIM900A_DELAY(1000); 
		printf("\nIP链接断开\n");
		SIM900A_DELAY(100);
		sim900a_gprs_link_close();	
		
		printf("\n关闭场景\n");
		SIM900A_DELAY(100);
		sim900a_gprs_shut_close();
		while(1);
	}
	printf("\n发送数据成功\n");
	
	SIM900A_CLEAN_RX();
	
//	printf("\nIP链接断开\n");
//	SIM900A_DELAY(100);
//	sim900a_gprs_link_close();	
//	
//	printf("\n关闭场景\n");
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
			printf("\nIP链接断开\n");
			SIM900A_DELAY(100);
			sim900a_gprs_link_close();	
			
			printf("\n关闭场景\n");
			SIM900A_DELAY(100);
			sim900a_gprs_shut_close();
			
			SIM900A_DELAY(1000);
			timestop=0xFF;
		}
		timecount++;
		SIM900A_DELAY(10);
	}		
}



