/**
******************************************************************************
* @file    			main.c
* @author  			WIZnet Software Team
* @version 			V1.0
* @date    			2015-02-14
* @brief   			��3.5.0�汾�⽨�Ĺ���ģ��
* @attention  	ʵ��ƽ̨������ iSO-V3 STM32 ������ + ����W5500���������
*
*               Ĭ��ʹ�ñ��𿪷����SPI2�ӿ�
*							
*               �������ԣ��뱣֤W5500��IP�����PC����ͬһ�����ڣ��Ҳ���ͻ
*               ����������߸�PC��ֱ����������PC���������ӵ�ַIPΪ��̬IP
*
* ʵ��ƽ̨:���� iSO-V3 STM32 ������ 
* ��̳    :http://www.firebbs.cn
* �Ա�    :https://fire-stm32.taobao.com
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

/*app����ͷ�ļ�*/
#include "dns.h"
#include "smtp.h"

extern  uint8 mail_send_ok;
int main(void)
{ 	
	systick_init(72);										/*��ʼ��Systick����ʱ��*/
	USART1_Config();										/*��ʼ������ͨ��:115200@8-n-1*/
	i2c_CfgGpio();											/*��ʼ��eeprom*/

	printf("  Ұ����������� �����ʼ�� Demo V1.0 \r\n");		

	gpio_for_w5500_config();						        /*��ʼ��MCU�������*/
	reset_w5500();											/*Ӳ��λW5500*/
	set_w5500_mac();										/*����MAC��ַ*/
	set_w5500_ip();											/*����IP��ַ*/
	
	socket_buf_init(txsize, rxsize);		/*��ʼ��8��Socket�ķ��ͽ��ջ����С*/
	
  printf(" Ĭ�Ͻ���smtp.126.com��IP \r\n");
	printf(" ����ǰ����ռ��������ַ��Ϊ�Լ������� \r\n");
	printf(" ����������Ϊ:%s \r\n",from);
	printf(" �ռ�������Ϊ:%s \r\n",to);
	printf(" Ӧ�ó���ִ���С��� \r\n"); 

	mail_message();	
	while(1)//ѭ��ִ�еĺ��� 
	{
		do_dns();/*����126����ķ���������smtp.126.com*/
		do_smtp(); /*ִ���ʼ������Ӻ���*/
		if(mail_send_ok)
		while(1);
	}
} 


