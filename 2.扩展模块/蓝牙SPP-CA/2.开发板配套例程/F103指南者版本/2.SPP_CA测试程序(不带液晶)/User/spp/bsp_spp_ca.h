#ifndef __SPP_CA_H
#define	__SPP_CA_H


#include "stm32f10x.h"
#include "./systick/bsp_SysTick.h"

#define delay_ms 			mdelay
#define SPP_CA_USART   	BLT_USARTx



/* 定义LED连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的LED引脚 */
#define BLT_INT_GPIO_PORT    	GPIOB			              /* GPIO端口 */
#define BLT_INT_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define BLT_INT_GPIO_PIN		  	GPIO_Pin_13			          /* 连接到SPP_CA INT引脚的GPIO */

//IS_SPP_CA_CONNECTED用于检查模块是否处于配对状态
//高电平表示已连接，低电平表示未连接
#define IS_SPP_CA_CONNECTED() 	GPIO_ReadInputDataBit(BLT_INT_GPIO_PORT,BLT_INT_GPIO_PIN	)


/*信息输出*/
#define SPP_CA_DEBUG_ON        0
#define SPP_CA_DEBUG_FUNC_ON    0

#define SPP_CA_INFO(fmt,arg...)           printf("<<-SPP_CA-INFO->> "fmt"\n",##arg)
#define SPP_CA_ERROR(fmt,arg...)          printf("<<-SPP_CA-ERROR->> "fmt"\n",##arg)
#define SPP_CA_DEBUG(fmt,arg...)          do{\
                                          if(SPP_CA_DEBUG_ON)\
                                          printf("<<-SPP_CA-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

#define SPP_CA_DEBUG_FUNC()               do{\
                                         if(SPP_CA_DEBUG_FUNC_ON)\
                                         printf("<<-SPP_CA-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)

	
																		 
																			 

																			 
uint8_t SPP_CA_Init(void);
uint8_t SPP_CA_Send_CMD(char* cmd,uint8_t clean);
uint8_t SPP_CA_Get_CMD(char* cmd,char* respon_string);
void SPP_CA_SendString(char* str);																			 
int get_line(char* line, char* stream ,int max_size);

#endif /* _SPP_CA_H */







