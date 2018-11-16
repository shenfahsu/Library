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
#include "./key/bsp_key.h"
#include "./usart/bsp_usart.h"
#include "./gsm_bluetooth/bsp_gsm_usart.h"
#include "./gsm_bluetooth/bsp_gsm_bluetooth.h"
#include "ff.h"

phoneData phone_status;
uint8_t gsm_bt_mode;

/* ���޸ĵ绰���� */
const char usernum[]="10086";

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

//��ʼ�������ģ��
//0��ʾ�ɹ���1��ʾʧ��
uint8_t gsm_init(void)
{
	char *redata;
	uint8_t len;
	
	GSM_DEBUG_FUNC();  

	GSM_CLEAN_RX();                 //����˽��ջ���������
	GSM_USART_Config();					//��ʼ������
	
   
	if(gsm_cmd("AT+CGMM\r","OK", 100) != GSM_TRUE)
	{
			return GSM_FALSE;
	}
	
	redata = GSM_RX(len);   //��������

	if(len == 0)
	{
			return GSM_FALSE;
	}
	//���������GSM900A��GSM800A��GSM800C
	if (strstr(redata,"SIMCOM_GSM900A") != 0)
	{
		return GSM_TRUE;
	}
	else if(strstr(redata,"SIMCOM_SIM800") != 0)
	{
		return GSM_TRUE;
	}
	else
		return GSM_FALSE;

}

/**
 * @brief  ��ѯ�Ѿ�������ӵ��ֻ�״̬
 * @retval <service> 0����������� ��1�����������
 * @retval <call>    0����ͨ�� ��1����ͨ��
 * @retval <call_setup> 0��ͨ�����������߱��ܾ� ��1�����磬2��ȥ�磬3��ȥ������
 * @retval <held> 0���ޱ���ͨ�� ��1��ͨ�������ֻ��߼���ͱ���ͨ���л���2��ͨ�������֣��޼����ͨ��
 * @retval <signal> 0��5�����ź�ǿ��
 * @retval <roam> 0������ ��1������
 * @retval <battchg> 0��5��ص���
 */
void get_phone_status(void)
{
    char *redata;
    char *redata_off;
    uint8_t len;
    
    GSM_CLEAN_RX();
    gsm_cmd("AT+BTCIND?\r",0,0);
    GSM_DELAY(200);
    redata = GSM_RX(len);   //��������+BTCIND:
    redata_off = strstr(redata,":");
    redata_off = strtok(redata_off,",");
    phone_status.service = atoi(strtok(NULL,","));
    phone_status.call = atoi(strtok(NULL,","));
    phone_status.call_setup = atoi(strtok(NULL,","));
    phone_status.held = atoi(strtok(NULL,","));
    phone_status.signal = atoi(strtok(NULL,","));
    phone_status.roam = atoi(strtok(NULL,","));
    phone_status.battchg = atoi(strtok(NULL,"\r"));
       
//    printf("\r\nservice=%d,\ncall=%d,\ncall_setup=%d,\nheld=%d,\nsignal=%d,\nroam=%d,\nbattchg=%d",
//            phone_status.service,
//            phone_status.call,
//            phone_status.call_setup,
//            phone_status.held,
//            phone_status.signal,
//            phone_status.roam,
//            phone_status.battchg
//          );
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
            get_phone_status();
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
        get_phone_status();
        if(phone_status.service)
            printf("\r\n�ֻ����������");
        else      
            printf("\r\n�ֻ����������"); 
        printf("\r\n�ֻ�����ǿ��Ϊ:%d����Χ<1��5>��",phone_status.signal);
        printf("\r\n�ֻ���ص���Ϊ:%d����Χ<1��5>��",phone_status.battchg); 
        printf("\r\nԤ��绰Ϊ:%s������1������߽����绰������2�Ҷϵ绰",usernum);
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
            get_phone_status();
            if(phone_status.service)
                printf("\r\n�ֻ����������");
            else      
                printf("\r\n�ֻ����������"); 
            printf("\r\n�ֻ�����ǿ��Ϊ:%d����Χ<1��5>��",phone_status.signal);
            printf("\r\n�ֻ���ص���Ϊ:%d����Χ<1��5>��",phone_status.battchg); 
            printf("\r\nԤ��绰Ϊ:%s������1������߽����绰������2�Ҷϵ绰",usernum); 
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
 * @brief  ��������ģ��������������ܣ���HFP
 * @retval ��
 */
void gsm_bt_hfp_test(void)
{
    char callcmd[50];
    static uint8_t bt_call_mode=0;
    static uint8_t callflag=0;
    if(gsm_bt_mode==SPP)
    {
       printf("\r\n����ģʽΪ��������ͨ�ţ����ظ�����"); 
    }
    else if(gsm_bt_mode==HFP)
    {
        if(gsm_cmd_check("BTRING") == GSM_TRUE)//�е绰���������ģ����Ե��ֻ�
        {   
            
            if(!callflag)
                printf("\r\n�е绰���룬����1����������2�ܽ�");
            callflag = 1;
            if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)//����1�����绰
            {
                if(gsm_cmd("AT+BTATA\r","OK",1000) == GSM_TRUE)//�����绰
                {
                    bt_call_mode=1;
                    printf("\r\n��ͨ�绰������ʹ�ð���2�Ҷϵ绰");
                }
            }
        }
        else
        {
            if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)//����1����绰
            {
                sprintf((char *)&callcmd,"AT+BTATD=%s;\r",usernum);
                if(gsm_cmd((char *)&callcmd,"OK",1000) == GSM_TRUE)//����绰
                {
                    bt_call_mode=1;
                    printf("\r\n���ڲ���绰:%s",usernum);
                } 
            }
        }
        if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)//����2�Ҷϵ绰
        {
            if(bt_call_mode == 1)//����ͨ��
            {
                if(gsm_cmd("AT+BTATH\r","OK",1000) == GSM_TRUE)//�Ҷϵ绰
                {   
                    if(!callflag)//����ǲ���绰��ȥ��Ҫ��������Ҷ�����
                        gsm_cmd("AT+BTATH\r","OK",1000);
                    
                    callflag = 0;//����绰�����־
                    printf("\r\n�Ҷϵ绰");
                }
            }
        }
      
    }

}
/*---------------------------------------------------------------------*/
