#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./usart/bsp_usart_blt.h"
#include "./systick/bsp_SysTick.h"
#include "./hc05/bsp_hc05.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 
#include <string.h>
#include <stdlib.h>


unsigned int Task_Delay[NumOfTask]; 
BLTDev bltDevList;

char sendData[1024];
char linebuff[1024];

int main(void)
{	
	char* redata;
	uint16_t len;
	
	static uint8_t hc05_role=1;
	unsigned long count;
	
	char hc05_mode[10]="SLAVE";
	char hc05_name[30]="HC05_SLAVE";
	char hc05_nameCMD[40];
	
  //初始化systick
	SysTick_Init();
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;

	USART_Config();
	
	LED_GPIO_Config();
	Key_GPIO_Config();
	
	
	HC05_INFO("**********HC05模块实验************");
	
	
	if(HC05_Init() == 0)
		HC05_INFO("HC05模块检测正常。");
	else
	{
		HC05_ERROR("HC05模块检测不正常，请检查模块与开发板的连接，然后复位开发板重新测试。");
		while(1);
	}


	/*复位、恢复默认状态*/
	HC05_Send_CMD("AT+RESET\r\n",1);	
	delay_ms(800);
	
	HC05_Send_CMD("AT+ORGL\r\n",1);
	delay_ms(200);

	
	/*各种命令测试演示，默认不显示。
	 *在bsp_hc05.h文件把HC05_DEBUG_ON 宏设置为1，
	 *即可通过串口调试助手接收调试信息*/	
	
	//获取软件版本号
	HC05_Send_CMD("AT+VERSION?\r\n",1);
	
	//获取模块蓝牙地址
	HC05_Send_CMD("AT+ADDR?\r\n",1);
	
	//设置查询串口参数
	HC05_Send_CMD("AT+UART?\r\n",1);
	
	//设置/查询—连接模式
	HC05_Send_CMD("AT+CMODE?\r\n",1);
	
	//获取蓝牙模块工作状态
	HC05_Send_CMD("AT+STATE?\r\n",1);	

	//设置/查询—模块角色
	HC05_Send_CMD("AT+ROLE=1\r\n",1);
	
	/*初始化SPP规范*/
	//初始化SPP规范库
	HC05_Send_CMD("AT+INIT\r\n",1);
	HC05_Send_CMD("AT+CLASS=0\r\n",1);
	HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
	
	/*设置模块名字*/
	sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
	HC05_Send_CMD(hc05_nameCMD,1);

	HC05_INFO("本模块名字为:%s ,模块已准备就绪。",hc05_name);
	
	
	while(!IS_HC05_CONNECTED())
	{
				//搜索蓝牙模块，并进行连接
				//if(Task_Delay[2]==0) 
				//{
					if(hc05_role == 1)	//主模式
					{
						HC05_INFO("正在扫描蓝牙设备...");
						
						linkHC05();
						
						Task_Delay[2]=3000; //此值每1ms会减1，减到0才可以重新进来这里，所以执行的周期是3s

					}
					else	//从模式
					{
							HC05_Send_CMD("AT+INQ\r\n",1);//模块在查询状态，才能容易被其它设备搜索到
							delay_ms(5000);
							//HC05_Send_CMD("AT+INQC\r\n",1);//中断查询，防止查询的结果干扰串口透传的接收

							//Task_Delay[2]=2000; //此值每1ms会减1，减到0才可以重新进来这里，所以执行的周期是2s
							printf("被搜索中……");
					}
					

	}

	printf("配对成功");
		
	while(1)
	{

			//连接后每隔一段时间检查接收缓冲区
		if(Task_Delay[0]==0 && IS_HC05_CONNECTED())  
		{
				uint16_t linelen;

				/*获取数据*/
				redata = get_rebuff(&len); 
				linelen = get_line(linebuff,redata,len);
			
				/*检查数据是否有更新*/
				if(linelen<200 && linelen != 0)
				{
					
					if(strcmp(redata,"AT+LED1=ON")==0)
					{
						LED1_ON;						
						HC05_SendString("+LED1:ON\r\nOK\r\n");

					}
					else if(strcmp(redata,"AT+LED1=OFF")==0)
					{
						LED1_OFF;
						HC05_SendString("+LED1:OFF\r\nOK\r\n");

					}
					else
					{
						HC05_INFO("receive:\r\n%s",redata);
					}
					
					/*处理数据后，清空接收蓝牙模块数据的缓冲区*/
					clean_rebuff();
					
				}
			Task_Delay[0]=500;//此值每1ms会减1，减到0才可以重新进来这里，所以执行的周期是500ms
		}
	}
}
		
//		//连接后每隔一段时间通过蓝牙模块发送字符串
//		if(Task_Delay[1]==0 && IS_HC05_CONNECTED())
//		{
//			static uint8_t testdata=0;
//		
//			sprintf(sendData,"<%s> send data test,testdata=%d\r\n",hc05_name,testdata++);
//			HC05_SendString(sendData);			

//			Task_Delay[1]=5000;//此值每1ms会减1，减到0才可以重新进来这里，所以执行的周期是5000ms

//		}		
		
		//如果KEY1被单击，切换master-slave模式
//		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
//		{
//			hc05_role=!hc05_role;
//			if(hc05_role == 0)
//			{						
//					HC05_Send_CMD("AT+RESET\r\n",1);
//					delay_ms(800);

//					if(HC05_Send_CMD("AT+ROLE=0\r\n",1) == 0)	
//					{				
//						delay_ms(100);
//						
//						sprintf(hc05_mode,"SLAVE");
//						HC05_INFO("hc05_mode  = %s",hc05_mode);	

//						sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
//						sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
//						
//						if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
//							HC05_INFO("设备名字被更改为：%s",hc05_name);
//						else
//							HC05_ERROR("更改名字失败");
//						
//						HC05_Send_CMD("AT+INIT\r\n",1);
//						HC05_Send_CMD("AT+CLASS=0\r\n",1);
//						HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
//						
//						//清空蓝牙设备列表
//						bltDevList.num = 0;
//					}
//					


//			}
//			else
//			{
//				HC05_Send_CMD("AT+RESET\r\n",1);
//				delay_ms(800);
//				
//				if(HC05_Send_CMD("AT+ROLE=1\r\n",1) == 0)	
//				{
//					delay_ms(100);
//					
//					sprintf(hc05_mode,"MASTER");
//					HC05_INFO("HC05 mode  = %s",hc05_mode);
//						
//					sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
//					sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);	
//					
//					if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
//						HC05_INFO("设备名字被更改为：%s",hc05_name);
//					else
//						HC05_ERROR("更改名字失败");
//						
//					HC05_Send_CMD("AT+INIT\r\n",1);
//					HC05_Send_CMD("AT+CLASS=0\r\n",1);
//					HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);	
//					
//					//清空蓝牙设备列表
//					bltDevList.num = 0;

//				}					

//			}

//		}
		
//		//如果KEY2被单击，随机生成一个名字
//		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
//		{
//				/*生成随机数，给名字随机编号*/
//				get_tick_count(&count);
//				srand(count);
//			
//				sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
//				sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);

//				if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
//					HC05_INFO("设备名字被更改为：%s",hc05_name);
//				else
//					HC05_ERROR("更改名字失败");
//			}
//		
//	
//	}
//}

/*********************************************END OF FILE**********************/
