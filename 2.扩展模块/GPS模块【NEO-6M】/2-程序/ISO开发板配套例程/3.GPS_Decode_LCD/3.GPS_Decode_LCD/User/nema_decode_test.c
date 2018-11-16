/**
  ******************************************************************************
  * @file    nmea_decode_test.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试NEMA解码库
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "stm32f10x.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_usart1.h"	
#include "bsp_usart2.h"
#include "ff.h"
#include "nmea/nmea.h"
#include "gps_config.h"
#include "GUI.h"

  

#ifdef __GPS_LOG_FILE             //对SD卡上的gpslog.txt文件进行解码；（需要在sd卡上存放gpslog.txt文件）

FATFS fs;
FIL log_file;
FRESULT res; 
UINT br, bw;            					/* File R/W count */

/**
  * @brief  nmea_decode_test 解码GPS文件信息
  * @param  无
  * @retval 无
  */
void nmea_decode_test(void)
{

    nmeaINFO info;          //GPS解码后得到的信息
    nmeaPARSER parser;      //解码时使用的数据结构  
    
    nmeaTIME beiJingTime;    //北京时间 

    char buff[2048];
    char str_buff[100];
  
    /* 给屏幕刷上背景颜色 */
    GUI_SetBkColor(GUI_BLACK);
  
    /* 设置字体格式 */
		GUI_SetFont(&GUI_Font8x16);
  
  	GUI_DispStringAt (" Wildfire STM32-ISO",0,20);
  
    GUI_DispStringAt (" WF-NEO-6M GPS module",0,40); 
  
    	/* 注册盘符 */
		f_mount(0,&fs);	

    /* 打开记录有GPS信息的文件 */
    res = f_open(&log_file,"0:gpslog.txt", FA_OPEN_EXISTING|FA_READ);

    if(!(res == FR_OK))
    {
        printf("\r\n打开gpslog.txt文件失败，请检查SD卡的根目录是否存放了gpslog.txt文件!\r\n");
        return ;      
    }
    

    /* 设置用于输出调试信息的函数 */
    nmea_property()->trace_func = &trace;
    nmea_property()->error_func = &error;

    /* 初始化GPS数据结构 */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

    while(!f_eof(&log_file))
    {
      
        f_read(&log_file, &buff[0], 100, &br);

        /* 进行nmea格式解码 */
        nmea_parse(&parser, &buff[0], br, &info);
      
        /* 对解码后的时间进行转换，转换成北京时间 */
        GMTconvert(&info.utc,&beiJingTime,8,1);
        
        /* 输出解码得到的信息 */
        printf("\r\n时间%d,%d,%d,%d,%d,%d\r\n", beiJingTime.year+1900, beiJingTime.mon+1,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
        printf("\r\n纬度：%f,经度%f\r\n",info.lat,info.lon);
        printf("\r\n正在使用的卫星：%d,可见卫星：%d",info.satinfo.inuse,info.satinfo.inview);
      
        /* 显示时间日期 */
        sprintf(str_buff," Date:%d/%d/%d Time:%d:%d:%d", beiJingTime.year+1900, beiJingTime.mon+1,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
        GUI_DispStringAt (str_buff,0,60); 
      
        /* 纬度 经度*/
        sprintf(str_buff," latitude :%f \r\n longitude :%f", info.lat,info.lon);
        GUI_DispStringAt (str_buff,0,80); 
        
        /* 正在使用的卫星 可见的卫星*/
        sprintf(str_buff," Satellite in use :%d \r\n Satellite in view :%d", info.satinfo.inuse,info.satinfo.inview);
        GUI_DispStringAt (str_buff,0,120); 
        
        /* 海拔高度 */
        sprintf(str_buff," Altitude:%f m", info.elv);
        GUI_DispStringAt (str_buff,0,160); 
        
        /* 速度 */
        sprintf(str_buff," speed:%f km/h", info.speed);
        GUI_DispStringAt (str_buff,0,180); 
        
        /* 航向 */
        sprintf(str_buff," Track angle:%f deg", info.direction);
        GUI_DispStringAt (str_buff,0,200); 

	
	}

    f_lseek(&log_file, f_size(&log_file));

    /* 释放GPS数据结构 */
    nmea_parser_destroy(&parser);
  
    /* 关闭文件 */
    f_close(&log_file);
    
   
}


#else        //对GPS模块传回的信息进行解码

/**
  * @brief  nmea_decode_test 解码GPS模块信息
  * @param  无
  * @retval 无
  */
int nmea_decode_test(void)
{

    nmeaINFO info;          //GPS解码后得到的信息
    nmeaPARSER parser;      //解码时使用的数据结构  
    uint8_t new_parse=0;    //是否有新的解码数据标志
  
 
    nmeaTIME beiJingTime;    //北京时间 
  
    char str_buff[100];
  
    /* 给屏幕刷上背景颜色 */
    GUI_SetBkColor(GUI_BLACK);
  
    /* 设置字体格式 */
		GUI_SetFont(&GUI_Font8x16);
    
    /* 设置前景颜色（字体颜色）*/
		GUI_SetColor(GUI_RED);
  
  	GUI_DispStringAt (" Wildfire STM32-ISO",40,20);
  
    GUI_DispStringAt (" WF-NEO-6M GPS module",30,40);

    /* 设置用于输出调试信息的函数 */
    nmea_property()->trace_func = &trace;
    nmea_property()->error_func = &error;

    /* 初始化GPS数据结构 */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

    while(1)
    {
      if(GPS_HalfTransferEnd)     /* 接收到GPS_RBUFF_SIZE一半的数据 */
      {
        /* 进行nmea格式解码 */
        nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);
        
        GPS_HalfTransferEnd = 0;   //清空标志位
        new_parse = 1;             //设置解码消息标志 
      }
      else if(GPS_TransferEnd)    /* 接收到另一半数据 */
      {

        nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);
       
        GPS_TransferEnd = 0;
        new_parse =1;
      }
      
      if(new_parse )                //有新的解码消息   
      {    
        /* 对解码后的时间进行转换，转换成北京时间 */
        GMTconvert(&info.utc,&beiJingTime,8,1);
        
        
        /* 串口输出 */
        /* 输出解码得到的信息 */
        printf("\r\n\r\n时间%d,%d,%d,%d,%d,%d", beiJingTime.year+1900, beiJingTime.mon+1,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
        printf("\r\n纬度：%f （度分.秒）,经度：%f （度分.秒）",info.lat,info.lon);
        printf("\r\n正在使用的卫星：%d,可见卫星：%d",info.satinfo.inuse,info.satinfo.inview);
        printf("\r\n海拔高度：%f 米 ", info.elv);
        printf("\r\n速度：%f km/h ", info.speed);
        printf("\r\n航向：%f 度", info.direction);
        
        
        /* 液晶输出 */
        
        /* 设置前景颜色（字体颜色）*/
        GUI_SetColor(GUI_BLUE);
        
        GUI_DispStringAt (" GPS Info:",0,80);

        /* 设置前景颜色（字体颜色）*/
        GUI_SetColor(GUI_WHITE);
        
        /* 显示时间日期 */
        sprintf(str_buff," Date:%d/%d/%d Time:%d:%d:%d", beiJingTime.year+1900, beiJingTime.mon+1,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
        GUI_DispStringAt (str_buff,0,100); 
      
        /* 纬度 经度*/
        sprintf(str_buff," latitude :%f \r\n longitude :%f", info.lat,info.lon);
        GUI_DispStringAt (str_buff,0,120); 
        
        /* 正在使用的卫星 可见的卫星*/
        sprintf(str_buff," Satellite in use :%d \r\n Satellite in view :%d", info.satinfo.inuse,info.satinfo.inview);
        GUI_DispStringAt (str_buff,0,160); 
        
        /* 海拔高度 */
        sprintf(str_buff," Altitude:%f m", info.elv);
        GUI_DispStringAt (str_buff,0,200); 
        
        /* 速度 */
        sprintf(str_buff," speed:%f km/h", info.speed);
        GUI_DispStringAt (str_buff,0,220); 
        
        /* 航向 */
        sprintf(str_buff," Track angle:%f deg", info.direction);
        GUI_DispStringAt (str_buff,0,240); 
        
               
        new_parse = 0;
      }
	
	}

    /* 释放GPS数据结构 */
    // nmea_parser_destroy(&parser);

    
    //  return 0;
}

#endif






/**************************************************end of file****************************************/

