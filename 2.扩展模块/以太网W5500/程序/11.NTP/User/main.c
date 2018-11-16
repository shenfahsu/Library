/**
******************************************************************************
* @file    			main.c
* @author  			WIZnet Software Team
* @version 			V1.0
* @date    			2015-02-14
* @brief   			用3.5.0版本库建的工程模板
* @attention  	实验平台：秉火 iSO-V3 STM32 开发板 + 秉火W5500网络适配板
*
*               默认使用秉火开发板的SPI2接口
*							
*               内网测试，请保证W5500的IP与测试PC机在同一网段内，且不冲突
*               如果是用网线跟PC机直连，请设置PC机本地连接地址IP为静态IP
*
* 实验平台:秉火 iSO-V3 STM32 开发板 
* 论坛    :http://www.firebbs.cn
* 淘宝    :https://fire-stm32.taobao.com
******************************************************************************
*/ 
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_i2c_ee.h"
#include "bsp_i2c_gpio.h"
#include "bsp_led.h"

#include "w5500.h"
#include "W5500_conf.h"
#include "socket.h"
#include "utility.h"

/*app函数头文件*/
#include "dhcp.h"
#include "ntp.h"

int main(void)
{ 	
	systick_init(72);										/*初始化Systick工作时钟*/
	USART1_Config();										/*初始化串口通信:115200@8-n-1*/
	i2c_CfgGpio();											/*初始化eeprom*/

	printf(" 野火网络适配版 NTP demo V1.0 \r\n");	

	gpio_for_w5500_config();						        /*初始化MCU相关引脚*/
	reset_w5500();											/*硬复位W5500*/
	set_w5500_mac();										/*配置MAC地址*/

	
	socket_buf_init(txsize, rxsize);		/*初始化8个Socket的发送接收缓存大小*/

  printf(" NTP服务器IP为:%d.%d.%d.%d\r\n",ntp_server_ip[0],ntp_server_ip[1],ntp_server_ip[2],ntp_server_ip[3]);
	printf(" NTP服务器端口为:%d \r\n",ntp_port);
	
  ntp_client_init();                  /*NTP初始化*/ 
  
	while(1)
	{ 	
		do_dhcp();                        /*从DHCP服务器获取并配置IP地址*/
		if(dhcp_ok==1)										/*成功获取IP地址*/
		{
			do_ntp_client();
		} 
	}
} 


