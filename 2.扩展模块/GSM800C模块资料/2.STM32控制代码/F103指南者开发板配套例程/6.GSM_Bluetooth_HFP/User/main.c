/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   GSM模块蓝牙耳机功能
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
	*/
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./gsm_bluetooth/bsp_gsm_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./gsm_bluetooth/bsp_gsm_bluetooth.h"
#include "./key/bsp_key.h" 
#include "bsp_sdfs_app.h"
#include <string.h>


/*
 * GSM模块蓝牙耳机
 * 
 */
int main(void)
{
    char end= 0x1A;//结束符
     /* 配置USART */
    USART_Config();

    /* 配置USART */
    GSM_USART_Config();
    
    /* 初始化系统定时器 */
    SysTick_Init();
    
    printf("\r\n秉火BH-GSM模块蓝牙串口测试例程\r\n");

    //模块异常退出时，如果正在发送数据，输入一次结束符，确保模块是在AT模式
    gsm_cmd(&end,0,1000);
    //检测模块响应是否正常
    while(gsm_cmd("AT\r","OK",1000) != GSM_TRUE)
    {
      printf("\r\n模块响应测试不正常！！\r\n");
      printf("\r\n若模块响应测试一直不正常，请检查模块的连接或是否已开启电源开关\r\n");
    }
    
    printf("\r\n通过了模块响应测试\r\n");
    
    gsm_bt_mode_config(0);//手机主动连接蓝牙模块
	
  while(1)
	{
        gsm_bt_hfp_test();//蓝牙串口功能测试
	}
}


