/**
  ******************************************************************************
  * @file    bsp_gsm_bluetooth.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   GSMģ������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
	*/

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "./usart/bsp_usart.h"
#include "./gsm_bluetooth/bsp_gsm_usart.h"
#include "./gsm_bluetooth/bsp_gsm_bluetooth.h"
#include "ff.h"


uint8_t gsm_bt_mode;

//0��ʾ�ɹ���1��ʾʧ��

uint8_t gsm_cmd(char *cmd, char *reply,uint32_t waittime )
{    
		GSM_DEBUG_FUNC();
	
    GSM_CLEAN_RX();                 //����˽��ջ���������

    GSM_TX(cmd);                    //��������

	  GSM_DEBUG("Send cmd:%s",cmd);	
	
    if(reply == 0)                      //����Ҫ��������
    {
        return GSM_TRUE;
    }
    
    GSM_DELAY(waittime);                 //��ʱ
    
    
    return gsm_cmd_check(reply);    //�Խ������ݽ��д���
}


//0��ʾ�ɹ���1��ʾʧ��
uint8_t gsm_cmd_check(char *reply)
{
    uint8_t len;
    uint8_t n;
    uint8_t off;
    char *redata;
    
		GSM_DEBUG_FUNC();

    redata = GSM_RX(len);   //��������
   	  
	  *(redata+len) = '\0';
	  GSM_DEBUG("Reply:%s",redata);	

//		GSM_DEBUG_ARRAY((uint8_t *)redata,len);
	
    n = 0;
    off = 0;
    while((n + off)<len)
    {
        if(reply[n] == 0)                 //����Ϊ�ջ��߱Ƚ����
        {
            return GSM_TRUE;
        }
        
        if(redata[ n + off]== reply[n])
        {
            n++;                //�ƶ�����һ����������
        }
        else
        {
            off++;              //������һ��ƥ��
            n=0;                //����
        }
        //n++;
    }

    if(reply[n]==0)   //�պ�ƥ�����
    {
        return GSM_TRUE;
    }
    
    return GSM_FALSE;       //����ѭ����ʾ�Ƚ���Ϻ�û����ͬ�����ݣ��������
}

char * gsm_waitask(uint8_t waitask_hook(void))      //�ȴ�������Ӧ��
{
    uint8_t len=0;
    char *redata;
	
		GSM_DEBUG_FUNC();
	
    do{
        redata = GSM_RX(len);   //��������
			
			
        if(waitask_hook!=0)
        {
            if(waitask_hook()==GSM_TRUE)           //���� GSM_TRUE ��ʾ��⵽�¼�����Ҫ�˳�
            {
                redata = 0;
                return redata;               
            }
        }
    }while(len==0);                 //��������Ϊ0ʱһֱ�ȴ�
    				
		GSM_DEBUG_ARRAY((uint8_t *)redata,len);

    
    GSM_DELAY(20);              //��ʱ��ȷ���ܽ��յ�ȫ�����ݣ�115200�������£�ÿms�ܽ���11.52���ֽڣ�
    return redata;
}




/**
 * @brief  ��ѯ����ģ��״̬
 * @retval ��
 */
void get_gsm_bt_status(void)
{
    GSM_CLEAN_RX();
    gsm_cmd("AT+BTSTATUS?\r",0,0);
    GSM_DELAY(1000);
}
/**
 * @brief  ��ѯ����ģ��״̬��
 * @retval ʧ��GSM_FALSE  �ɹ�GSM_TRUE
 */
uint8_t get_gsm_bt_status_num(void)
{
    uint8_t state_num=0;
	char *redata;
    char *redata_off;
    uint8_t len;
   
    
    GSM_CLEAN_RX();
    
    gsm_cmd("AT+BTSTATUS?\r",0,0);
    
    GSM_DELAY(100);
    
    redata = GSM_RX(len);   //��������+BTSTATUS:
    
    //��ȡ��ѯ����״̬�ַ���+BTSTATUS: 5���е�����5
	redata_off=strstr(redata,":");

	//ת������״̬�ַ�����ֵ
	state_num = atoi((char const *)redata_off+1);	
    
    //printf("\r\nstate_num=%d",state_num);
    
    return state_num;//����״̬��
}
/**
 * @brief  �����豸���ܿ���
 * @param  status: ��0 �ر�������Դ
 *                   1 ��������Դ 
 * @retval ʧ��GSM_FALSE  �ɹ�GSM_TRUE
 */
uint8_t gsm_bt_power(uint8_t status)
{
    uint8_t state_num=0;
    
    state_num = get_gsm_bt_status_num();

    if(status)//��������Դ
    {
        if(state_num == 0)//�������ڳ�ʼ��״̬
        {
            if(gsm_cmd("AT+BTPOWER=1\r","OK",1000) != GSM_TRUE)
            return GSM_FALSE;
        }
        else
        {
            printf("\r\n����ģ���Դ�Ѿ���");
        }
        
    }
    else if(status == 0)//�ر�������Դ
    {
        if(state_num == 5)//����״̬�ſ��Թرյ�Դ
        {
            if(gsm_cmd("AT+BTPOWER=0\r","OK",1000) != GSM_TRUE)
            return GSM_FALSE;
        }
        else if(state_num == 0)//�������ڳ�ʼ��״̬
        printf("\r\n����ģ���Դ�Ѿ��ر�");
    }
    return GSM_TRUE;
}
/**
 * @brief  ����������Ӳ���
 * @param  status: ������ӷ�ʽ��0����������������ģ�����
 *                               1��������������ģ�鷢������� 
 * @retval ʧ��GSM_FALSE  �ɹ�GSM_TRUE
 */
uint8_t gsm_bt_mode_config(uint8_t status)
{
    char *redata;
    uint8_t len;
    uint32_t timeout=0xFFFF;
    
    gsm_bt_power(1); //������ģ���Դ
    
    if(status)//����������ģ�鷢�������
    {
        get_gsm_bt_status();    //��ȡGSM����ģ��״̬
        redata = GSM_RX(len);   //���շ���״̬
        if(strstr(redata,"P:")!=NULL)//�Ѿ���������豸�������ͬһ�������豸����ɾ����ԣ����ܱ�������
        {
            printf("\r\n�Ѿ���������豸�������ͬһ�������豸����ɾ����ԣ����ܱ�������");
            gsm_cmd("AT+BTUNPAIR=0\r","OK",1000);//ɾ�������豸
            return GSM_FALSE;
        }
        while(get_gsm_bt_status_num() != 5 && timeout--);//�ȴ�����ģ�鴦�ڿ���״̬
        if(timeout==0)
        {  
            printf("\r\n�ȴ���ʱ������GSMģ���Ƿ���������");
            return GSM_FALSE;
        }
        printf("\r\n����ģ�鴦�ڿ���״̬����������������ģ�鷢�������");
        
        printf("\r\n��������������������\r\n��������ģ��������Ϊ�ɼ�");
        
        if(gsm_cmd("AT+BTSCAN=1,10\r","OK",1000) != GSM_TRUE)//������Χ�����豸
            return GSM_FALSE;
        
        GSM_DELAY(1000);        
        do
        {   
            redata = GSM_RX(len);   //���շ���״̬
        }while(strstr(redata,"+BTSCAN: 0") == NULL);//�ж��Ƿ������������豸
        printf("\r\n�������豸");        
        GSM_DELAY(1000);
        do
        {   
            if(gsm_cmd("AT+BTPAIR=0,1\r","OK",1000) != GSM_TRUE)//�������ɨ�赽�ĵ�1�������豸
                return GSM_FALSE;

        }while(get_gsm_bt_status_num() != 9);//�ж��Ƿ�����������������豸        
        printf("\r\n������������豸�������豸��ȷ�����"); 
        
        do
        {   
            if(gsm_cmd("AT+BTPAIR=1,1\r","OK",1000) != GSM_TRUE)//����ȷ�Ϸ�ʽ��Ӧ���
                return GSM_FALSE;
                get_gsm_bt_status();
                redata = GSM_RX(len);   //���շ���״̬

        }while(strstr(redata,"P:")==NULL);//�ж��Ƿ���Գɹ�
        printf("\r\n��Գɹ�");
        printf("\r\n�ȴ����������豸����������");        
         do
        {
            if(get_gsm_bt_status_num() == 25)//��������Ե������豸����������
            {
                printf("\r\n�������ӡ���������");
                if(gsm_cmd("AT+BTACPT=1\r","OK",1000) != GSM_TRUE)//������Ե������豸����������
                    return GSM_FALSE;                
            
                get_gsm_bt_status();
                redata = GSM_RX(len);   //���շ���״̬
            }
        }while(strstr(redata,"C:")==NULL);//�ж��Ƿ����ӳɹ�
        printf("\r\n���ӳɹ�");
        redata = strstr(redata,"C:");
        if(strstr(redata,"HFP")!=NULL)//�ж��Ƿ�Ϊ���������豸
        {
            gsm_bt_mode = HFP;
            printf("\r\nGSM����ģ��ʵ��������������");
        }                
        else if(strstr(redata,"SPP")!=NULL)//�ж��Ƿ�Ϊ���������豸
        {
            gsm_bt_mode = SPP;
            printf("\r\nGSM����ģ��ʵ���������ڹ���");
        }
        
        redata = strstr(redata,":");//��ȡ�豸����Ϣ            
        printf("\r\n�豸���%s",strtok(redata,","));
        printf("\r\n�豸����:%s",strtok(NULL,","));
        printf("\r\n�����ַ:%s",strtok(NULL,","));
        
    }
    else if(status==0)//������������ģ���������
    {
        get_gsm_bt_status();    //��ȡGSM����ģ��״̬
        redata = GSM_RX(len);   //���շ���״̬
        if(strstr(redata,"P:")!=NULL)//�Ѿ�������豸����ɾ��
        {
            gsm_cmd("AT+BTUNPAIR=0\r","OK",1000);//ɾ�������豸
        } 
    
        while(get_gsm_bt_status_num() != 5 && timeout--);//�ȴ�����ģ�鴦�ڿ���״̬
        if(timeout==0)
        {  
            printf("\r\n�ȴ���ʱ������GSMģ���Ƿ���������");
            return GSM_FALSE;
        }
        printf("\r\n����ģ�鴦�ڿ���״̬���ȴ���������ģ�鷢�����");
        
        do
        {
            if(get_gsm_bt_status_num() == 19)//�������ʱ�ȴ����ȷ��
            {
                printf("\r\n������ԡ�����������\r\n�����ֻ���ȷ�����");
                if(gsm_cmd("AT+BTPAIR=1,1\r","OK",1000) != GSM_TRUE)//��Ӧ���������豸���������
                    return GSM_FALSE;                
            
                get_gsm_bt_status();
                redata = GSM_RX(len);   //���շ���״̬
            }

        }while(strstr(redata,"P:")==NULL);//�ж��Ƿ���Գɹ�
        printf("\r\n��Գɹ�");
        
        get_gsm_bt_status();    //��ȡGSM����ģ��״̬
        redata = GSM_RX(len);   //���շ���״̬
        if(strstr(redata,"C:")==NULL)//����Ե�δ����
        {
            printf("\r\n�豸����Գɹ���δ���ӣ�������");
            do
            {
                if(get_gsm_bt_status_num() == 25)//��������Ե������豸����������
                {
                    printf("\r\n�������ӡ���������");
                    if(gsm_cmd("AT+BTACPT=1\r","OK",1000) != GSM_TRUE)//������Ե������豸����������
                        return GSM_FALSE;                
                
                    get_gsm_bt_status();
                    redata = GSM_RX(len);   //���շ���״̬
                }
            }while(strstr(redata,"C:")==NULL);//�ж��Ƿ����ӳɹ�

        }

        if(strstr(redata,"C:")!=NULL)//�Ѿ����ӵ������豸
        {   
            printf("\r\n���ӳɹ�");
            redata = strstr(redata,"C:");
            if(strstr(redata,"HFP")!=NULL)
            {
                gsm_bt_mode = HFP;
                printf("\r\nGSM����ģ��ʵ��������������");
            }                
            else if(strstr(redata,"SPP")!=NULL)//
            {
                gsm_bt_mode = SPP;
                printf("\r\nGSM����ģ��ʵ���������ڹ���");
            }
            
            redata = strstr(redata,":");             
            printf("\r\n�豸���%s",strtok(redata,","));
            printf("\r\n�豸����:%s",strtok(NULL,","));
            printf("\r\n�����ַ:%s",strtok(NULL,","));

        }
        else
        {
           printf("\r\n�����豸����ʧ�ܣ�����������"); 
           return GSM_FALSE;
        }
    }
    GSM_CLEAN_RX();
    return GSM_TRUE; 
}
/**
 * @brief  ��������ģ����������ڹ��ܣ���SPP
 * @retval ��
 */
void gsm_bt_spp_test(void)
{
    static uint32_t timecount=0;
    static uint8_t sendcount=0;
    char end= 0x1A;//������
    char cmdbuff[256];
    char *redata;
    uint8_t  len;
    if(gsm_bt_mode==SPP)
    {
        redata = GSM_RX(len);   //�����������ڵ�����
        if(strstr(redata,"+BTSPPDATA:")!=NULL)
        {
            if(strstr(redata,"\r\n")!=NULL)
            {
                printf("\r\n���յ����ݣ�");
                redata = strstr(redata,":"); 
                printf("\r\n�豸���%s",strtok(redata,","));
                printf("\r\n���ݳ���:%s",strtok(NULL,","));
                printf("\r\n��������:%s",strtok(NULL,"\r"));
                GSM_CLEAN_RX();
            }
        }
        timecount++;
        if(timecount>=0x50000)//���ڷ�������
        {   
            timecount = 0;
            gsm_cmd("AT+BTSPPSEND\r",">",100);//ͨ���������ڷ�������
            sprintf((char *)cmdbuff,"BT SPP Mode test %d \r\n",sendcount);
            sendcount++;
            gsm_cmd((char *)cmdbuff,0,0);//��������
            gsm_cmd(&end,0,100); //���ͽ�����
            GSM_CLEAN_RX();
        }
    }
    else if(gsm_bt_mode==SPP)
    {
        printf("����ģʽΪ�������������ܽ�����������ͨ�ţ����ظ�����");
    }

}
/*---------------------------------------------------------------------*/
