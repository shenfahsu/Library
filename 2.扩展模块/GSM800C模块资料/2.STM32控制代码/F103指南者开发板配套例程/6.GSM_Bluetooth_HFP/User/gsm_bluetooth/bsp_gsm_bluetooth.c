/**
  ******************************************************************************
  * @file    bsp_gsm_bluetooth.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   GSM模块驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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

/* 请修改电话号码 */
const char usernum[]="10086";

//0表示成功，1表示失败

uint8_t gsm_cmd(char *cmd, char *reply,uint32_t waittime )
{    
		GSM_DEBUG_FUNC();
	
    GSM_CLEAN_RX();                 //清空了接收缓冲区数据

    GSM_TX(cmd);                    //发送命令

	  GSM_DEBUG("Send cmd:%s",cmd);	
	
    if(reply == 0)                      //不需要接收数据
    {
        return GSM_TRUE;
    }
    
    GSM_DELAY(waittime);                 //延时
    
    
    return gsm_cmd_check(reply);    //对接收数据进行处理
}


//0表示成功，1表示失败
uint8_t gsm_cmd_check(char *reply)
{
    uint8_t len;
    uint8_t n;
    uint8_t off;
    char *redata;
    
		GSM_DEBUG_FUNC();

    redata = GSM_RX(len);   //接收数据
   	  
	  *(redata+len) = '\0';
	  GSM_DEBUG("Reply:%s",redata);	

//		GSM_DEBUG_ARRAY((uint8_t *)redata,len);
	
    n = 0;
    off = 0;
    while((n + off)<len)
    {
        if(reply[n] == 0)                 //数据为空或者比较完毕
        {
            return GSM_TRUE;
        }
        
        if(redata[ n + off]== reply[n])
        {
            n++;                //移动到下一个接收数据
        }
        else
        {
            off++;              //进行下一轮匹配
            n=0;                //重来
        }
        //n++;
    }

    if(reply[n]==0)   //刚好匹配完毕
    {
        return GSM_TRUE;
    }
    
    return GSM_FALSE;       //跳出循环表示比较完毕后都没有相同的数据，因此跳出
}

char * gsm_waitask(uint8_t waitask_hook(void))      //等待有数据应答
{
    uint8_t len=0;
    char *redata;
	
		GSM_DEBUG_FUNC();
	
    do{
        redata = GSM_RX(len);   //接收数据
			
			
        if(waitask_hook!=0)
        {
            if(waitask_hook()==GSM_TRUE)           //返回 GSM_TRUE 表示检测到事件，需要退出
            {
                redata = 0;
                return redata;               
            }
        }
    }while(len==0);                 //接收数据为0时一直等待
    				
		GSM_DEBUG_ARRAY((uint8_t *)redata,len);

    
    GSM_DELAY(20);              //延时，确保能接收到全部数据（115200波特率下，每ms能接收11.52个字节）
    return redata;
}

//初始化并检测模块
//0表示成功，1表示失败
uint8_t gsm_init(void)
{
	char *redata;
	uint8_t len;
	
	GSM_DEBUG_FUNC();  

	GSM_CLEAN_RX();                 //清空了接收缓冲区数据
	GSM_USART_Config();					//初始化串口
	
   
	if(gsm_cmd("AT+CGMM\r","OK", 100) != GSM_TRUE)
	{
			return GSM_FALSE;
	}
	
	redata = GSM_RX(len);   //接收数据

	if(len == 0)
	{
			return GSM_FALSE;
	}
	//本程序兼容GSM900A、GSM800A、GSM800C
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
 * @brief  查询已经配对连接的手机状态
 * @retval <service> 0：无网络服务 ，1：有网络服务
 * @retval <call>    0：无通话 ，1：有通话
 * @retval <call_setup> 0：通话被接听或者被拒绝 ，1：来电，2：去电，3：去电响铃
 * @retval <held> 0：无保持通话 ，1：通话被保持或者激活和保持通话切换，2：通话被保持，无激活的通话
 * @retval <signal> 0—5网络信号强度
 * @retval <roam> 0：本地 ，1：漫游
 * @retval <battchg> 0—5电池电量
 */
void get_phone_status(void)
{
    char *redata;
    char *redata_off;
    uint8_t len;
    
    GSM_CLEAN_RX();
    gsm_cmd("AT+BTCIND?\r",0,0);
    GSM_DELAY(200);
    redata = GSM_RX(len);   //接收数据+BTCIND:
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
 * @brief  查询蓝牙模块状态
 * @retval 无
 */
void get_gsm_bt_status(void)
{
    GSM_CLEAN_RX();
    gsm_cmd("AT+BTSTATUS?\r",0,0);
    GSM_DELAY(1000);
}
/**
 * @brief  查询蓝牙模块状态码
 * @retval 失败GSM_FALSE  成功GSM_TRUE
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
    
    redata = GSM_RX(len);   //接收数据+BTSTATUS:
    
    //提取查询蓝牙状态字符“+BTSTATUS: 5”中的数字5
	redata_off=strstr(redata,":");

	//转换蓝牙状态字符串的值
	state_num = atoi((char const *)redata_off+1);	
    
    //printf("\r\nstate_num=%d",state_num);
    
    return state_num;//返回状态码
}
/**
 * @brief  蓝牙设备功能开关
 * @param  status: ：0 关闭蓝牙电源
 *                   1 打开蓝牙电源 
 * @retval 失败GSM_FALSE  成功GSM_TRUE
 */
uint8_t gsm_bt_power(uint8_t status)
{
    uint8_t state_num=0;
    
    state_num = get_gsm_bt_status_num();

    if(status)//打开蓝牙电源
    {
        if(state_num == 0)//蓝牙处于初始化状态
        {
            if(gsm_cmd("AT+BTPOWER=1\r","OK",1000) != GSM_TRUE)
            return GSM_FALSE;
        }
        else
        {
            printf("\r\n蓝牙模块电源已经打开");
        }
        
    }
    else if(status == 0)//关闭蓝牙电源
    {
        if(state_num == 5)//空闲状态才可以关闭电源
        {
            if(gsm_cmd("AT+BTPOWER=0\r","OK",1000) != GSM_TRUE)
            return GSM_FALSE;
        }
        else if(state_num == 0)//蓝牙处于初始化状态
        printf("\r\n蓝牙模块电源已经关闭");
    }
    return GSM_TRUE;
}
/**
 * @brief  蓝牙配对连接测试
 * @param  status: 配对连接方式：0被动接受其他蓝牙模块配对
 *                               1主动给其他蓝牙模块发配对请求 
 * @retval 失败GSM_FALSE  成功GSM_TRUE
 */
uint8_t gsm_bt_mode_config(uint8_t status)
{
    char *redata;
    uint8_t len;
    uint32_t timeout=0xFFFF;
    
    gsm_bt_power(1); //打开蓝牙模块电源
    
    if(status)//给其他蓝牙模块发配对请求
    {
        get_gsm_bt_status();    //获取GSM蓝牙模块状态
        redata = GSM_RX(len);   //接收返回状态
        if(strstr(redata,"P:")!=NULL)//已经存在配对设备，如果是同一个蓝牙设备请先删除配对，才能被搜索到
        {
            printf("\r\n已经存在配对设备，如果是同一个蓝牙设备请先删除配对，才能被搜索到");
            gsm_cmd("AT+BTUNPAIR=0\r","OK",1000);//删除蓝牙设备
            return GSM_FALSE;
        }
        while(get_gsm_bt_status_num() != 5 && timeout--);//等待蓝牙模块处于空闲状态
        if(timeout==0)
        {  
            printf("\r\n等待超时，请检查GSM模块是否正常工作");
            return GSM_FALSE;
        }
        printf("\r\n蓝牙模块处于空闲状态，可以向其他蓝牙模块发配对请求");
        
        printf("\r\n正在搜索。。。。。。\r\n其他蓝牙模块请设置为可见");
        
        if(gsm_cmd("AT+BTSCAN=1,10\r","OK",1000) != GSM_TRUE)//搜索周围蓝牙设备
            return GSM_FALSE;
        
        GSM_DELAY(1000);        
        do
        {   
            redata = GSM_RX(len);   //接收返回状态
        }while(strstr(redata,"+BTSCAN: 0") == NULL);//判断是否搜索到蓝牙设备
        printf("\r\n搜索到设备");        
        GSM_DELAY(1000);
        do
        {   
            if(gsm_cmd("AT+BTPAIR=0,1\r","OK",1000) != GSM_TRUE)//主动配对扫描到的第1个蓝牙设备
                return GSM_FALSE;

        }while(get_gsm_bt_status_num() != 9);//判断是否正在主动配对蓝牙设备        
        printf("\r\n正在主动配对设备，蓝牙设备请确认配对"); 
        
        do
        {   
            if(gsm_cmd("AT+BTPAIR=1,1\r","OK",1000) != GSM_TRUE)//数字确认方式响应配对
                return GSM_FALSE;
                get_gsm_bt_status();
                redata = GSM_RX(len);   //接收返回状态

        }while(strstr(redata,"P:")==NULL);//判断是否配对成功
        printf("\r\n配对成功");
        printf("\r\n等待其他蓝牙设备的连接请求");        
         do
        {
            if(get_gsm_bt_status_num() == 25)//接受已配对的蓝牙设备的连接请求
            {
                printf("\r\n接受连接。。。。。");
                if(gsm_cmd("AT+BTACPT=1\r","OK",1000) != GSM_TRUE)//接受配对的蓝牙设备的连接请求
                    return GSM_FALSE;                
            
                get_gsm_bt_status();
                redata = GSM_RX(len);   //接收返回状态
            }
        }while(strstr(redata,"C:")==NULL);//判断是否连接成功
        printf("\r\n连接成功");
        redata = strstr(redata,"C:");
        if(strstr(redata,"HFP")!=NULL)//判断是否为蓝牙耳机设备
        {
            gsm_bt_mode = HFP;
            get_phone_status();
            printf("\r\nGSM蓝牙模块实现蓝牙耳机功能");
        }                
        else if(strstr(redata,"SPP")!=NULL)//判断是否为蓝牙串口设备
        {
            gsm_bt_mode = SPP;
            printf("\r\nGSM蓝牙模块实现蓝牙串口功能");
        }
        
        redata = strstr(redata,":");//获取设备的信息            
        printf("\r\n设备序号%s",strtok(redata,","));
        printf("\r\n设备名称:%s",strtok(NULL,","));
        printf("\r\n物理地址:%s",strtok(NULL,","));
        get_phone_status();
        if(phone_status.service)
            printf("\r\n手机有网络服务");
        else      
            printf("\r\n手机无网络服务"); 
        printf("\r\n手机网络强度为:%d（范围<1—5>）",phone_status.signal);
        printf("\r\n手机电池电量为:%d（范围<1—5>）",phone_status.battchg); 
        printf("\r\n预设电话为:%s，按键1拨打或者接听电话，按键2挂断电话",usernum);
    }
    else if(status==0)//接受其他蓝牙模块配对连接
    {
        get_gsm_bt_status();    //获取GSM蓝牙模块状态
        redata = GSM_RX(len);   //接收返回状态
        if(strstr(redata,"P:")!=NULL)//已经有配对设备，先删除
        {
            gsm_cmd("AT+BTUNPAIR=0\r","OK",1000);//删除蓝牙设备
        } 
    
        while(get_gsm_bt_status_num() != 5 && timeout--);//等待蓝牙模块处于空闲状态
        if(timeout==0)
        {  
            printf("\r\n等待超时，请检查GSM模块是否正常工作");
            return GSM_FALSE;
        }
        printf("\r\n蓝牙模块处于空闲状态，等待其他蓝牙模块发起配对");
        
        do
        {
            if(get_gsm_bt_status_num() == 19)//被动配对时等待配对确认
            {
                printf("\r\n正在配对。。。。。。\r\n请在手机端确认配对");
                if(gsm_cmd("AT+BTPAIR=1,1\r","OK",1000) != GSM_TRUE)//响应其他蓝牙设备的配对请求
                    return GSM_FALSE;                
            
                get_gsm_bt_status();
                redata = GSM_RX(len);   //接收返回状态
            }

        }while(strstr(redata,"P:")==NULL);//判断是否配对成功
        printf("\r\n配对成功");
        
        get_gsm_bt_status();    //获取GSM蓝牙模块状态
        redata = GSM_RX(len);   //接收返回状态
        if(strstr(redata,"C:")==NULL)//已配对但未连接
        {
            printf("\r\n设备已配对成功但未连接，请连接");
            do
            {
                if(get_gsm_bt_status_num() == 25)//接受已配对的蓝牙设备的连接请求
                {
                    printf("\r\n接受连接。。。。。");
                    if(gsm_cmd("AT+BTACPT=1\r","OK",1000) != GSM_TRUE)//接受配对的蓝牙设备的连接请求
                        return GSM_FALSE;                
                
                    get_gsm_bt_status();
                    redata = GSM_RX(len);   //接收返回状态
                }
            }while(strstr(redata,"C:")==NULL);//判断是否连接成功

        }

        if(strstr(redata,"C:")!=NULL)//已经连接的蓝牙设备
        {   
            printf("\r\n连接成功");
            redata = strstr(redata,"C:");
            if(strstr(redata,"HFP")!=NULL)
            {
                gsm_bt_mode = HFP;
                printf("\r\nGSM蓝牙模块实现蓝牙耳机功能");
            }                
            else if(strstr(redata,"SPP")!=NULL)//
            {
                gsm_bt_mode = SPP;
                printf("\r\nGSM蓝牙模块实现蓝牙串口功能");
            }
            
            redata = strstr(redata,":");             
            printf("\r\n设备序号%s",strtok(redata,","));
            printf("\r\n设备名称:%s",strtok(NULL,","));
            printf("\r\n物理地址:%s",strtok(NULL,","));
            get_phone_status();
            if(phone_status.service)
                printf("\r\n手机有网络服务");
            else      
                printf("\r\n手机无网络服务"); 
            printf("\r\n手机网络强度为:%d（范围<1—5>）",phone_status.signal);
            printf("\r\n手机电池电量为:%d（范围<1—5>）",phone_status.battchg); 
            printf("\r\n预设电话为:%s，按键1拨打或者接听电话，按键2挂断电话",usernum); 
        }
        else
        {
           printf("\r\n蓝牙设备连接失败，请重新连接"); 
           return GSM_FALSE;
        }
    }
    GSM_CLEAN_RX();
    return GSM_TRUE; 
}

/**
 * @brief  测试蓝牙模块的蓝牙耳机功能，即HFP
 * @retval 无
 */
void gsm_bt_hfp_test(void)
{
    char callcmd[50];
    static uint8_t bt_call_mode=0;
    static uint8_t callflag=0;
    if(gsm_bt_mode==SPP)
    {
       printf("\r\n连接模式为蓝牙串口通信，请重复操作"); 
    }
    else if(gsm_bt_mode==HFP)
    {
        if(gsm_cmd_check("BTRING") == GSM_TRUE)//有电话打进跟蓝牙模块配对的手机
        {   
            
            if(!callflag)
                printf("\r\n有电话呼入，按键1接听，按键2拒接");
            callflag = 1;
            if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)//按键1接听电话
            {
                if(gsm_cmd("AT+BTATA\r","OK",1000) == GSM_TRUE)//接听电话
                {
                    bt_call_mode=1;
                    printf("\r\n接通电话，可以使用按键2挂断电话");
                }
            }
        }
        else
        {
            if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)//按键1拨打电话
            {
                sprintf((char *)&callcmd,"AT+BTATD=%s;\r",usernum);
                if(gsm_cmd((char *)&callcmd,"OK",1000) == GSM_TRUE)//拨打电话
                {
                    bt_call_mode=1;
                    printf("\r\n正在拨打电话:%s",usernum);
                } 
            }
        }
        if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)//按键2挂断电话
        {
            if(bt_call_mode == 1)//正在通话
            {
                if(gsm_cmd("AT+BTATH\r","OK",1000) == GSM_TRUE)//挂断电话
                {   
                    if(!callflag)//如果是拨打电话出去需要发送两侧挂断命令
                        gsm_cmd("AT+BTATH\r","OK",1000);
                    
                    callflag = 0;//清除电话接入标志
                    printf("\r\n挂断电话");
                }
            }
        }
      
    }

}
/*---------------------------------------------------------------------*/
