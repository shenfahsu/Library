/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   GSMģ��������������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:���� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
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
 * GSMģ����������
 * 
 */
int main(void)
{
    char end= 0x1A;//������
     /* ����USART */
    USART_Config();

    /* ����USART */
    GSM_USART_Config();
    
    /* ��ʼ��ϵͳ��ʱ�� */
    SysTick_Init();
    
    printf("\r\n����BH-GSMģ���������ڲ�������\r\n");

    //ģ���쳣�˳�ʱ��������ڷ������ݣ�����һ�ν�������ȷ��ģ������ATģʽ
    gsm_cmd(&end,0,1000);
    //���ģ����Ӧ�Ƿ�����
    while(gsm_cmd("AT\r","OK",1000) != GSM_TRUE)
    {
      printf("\r\nģ����Ӧ���Բ���������\r\n");
      printf("\r\n��ģ����Ӧ����һֱ������������ģ������ӻ��Ƿ��ѿ�����Դ����\r\n");
    }
    
    printf("\r\nͨ����ģ����Ӧ����\r\n");
    
    gsm_bt_mode_config(0);//�ֻ�������������ģ��
	
  while(1)
	{
        gsm_bt_hfp_test();//�������ڹ��ܲ���
	}
}


