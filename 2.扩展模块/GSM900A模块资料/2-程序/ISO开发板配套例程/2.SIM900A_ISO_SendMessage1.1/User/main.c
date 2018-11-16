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


/* 接收短信的电话号码 */
const char num[]="112";

/*
 * 测试发送短信功能，实际测试时请把电话号码修改成要接收短信的手机号
 * 
 */
int main(void)
{
	uint8_t newmessadd=0,IsRead=0;
//	static uint8_t timecount=0;
	char namenum[20*4]={0},str[512]={0},gbkstr[256]={0},namegbk[20]={0};
     /* 配置USART1 */
    USART1_Config();

    /* 配置USART2 */
    USART2_Config();
    
    /* 初始化系统定时器 */
    SysTick_Init();
    
    printf("\r\n野火WF-SIM900A模块发送短信例程\r\n");
    
    //检测模块响应是否正常
    while(sim900a_cmd("AT\r","OK",1000) != SIM900A_TRUE)
    {
      printf("\r\n模块响应测试不正常！！\r\n");
      printf("\r\n若模块响应测试一直不正常，请检查模块的连接或是否已开启电源开关\r\n");
    }
    
    printf("\r\n通过了模块响应测试，5秒后开始发送短信测试... \r\n");
    
    /* 延时5秒再发送命令到模块 */
    SIM900A_DELAY(5000);    
    
    //发送短信
		
    sim900a_sms((char *)num,"hellow world");
    printf("\r\n英文短信已发送至：%s，为方便测试，请在程序中修改接收短信的手机号码",num);
    
    //至少间隔5s再发第二条短信
   
    SIM900A_DELAY(5000); 
    //中英文短信，实际测试时请把电话号码修改成要接收短信的手机号
    sim900a_sms((char *)num,"野火WF-SIM900A模块短信测试");		
	printf("\r\n中英文短信已发送至：%s，为方便测试，请在程序中修改接收短信的手机号码",num);
	SIM900A_DELAY(5000);
	SIM900A_DELAY(5000);	
	printf("\r\n接收短信测试：通过串口1打印新短信信息\r\n");
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
				printf("新短信:\n发件人:%s\n内容:%s\n\r",namenum,gbkstr);
			}
		}
		
	}
}



