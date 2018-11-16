#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"
#include "W5500_conf.h"
/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
#define LED_ON    0
#define LED_OFF   1

#define LED1_PIN                  GPIO_Pin_5                 
#define LED1_GPIO_PORT            GPIOB                     
#define LED1_GPIO_CLK             RCC_APB2Periph_GPIOB

#define LED2_PIN                  GPIO_Pin_0                 
#define LED2_GPIO_PORT            GPIOB                      
#define LED2_GPIO_CLK             RCC_APB2Periph_GPIOB

#define LED3_PIN                  GPIO_Pin_1                  
#define LED3_GPIO_PORT            GPIOB                       
#define LED3_GPIO_CLK             RCC_APB2Periph_GPIOB

/* ���κ꣬��������������һ��ʹ�� */
#define LED1(a)	if (a)	\
					GPIO_SetBits(LED1_GPIO_PORT,LED1_PIN);\
					else		\
					GPIO_ResetBits(LED1_GPIO_PORT,LED1_PIN)

#define LED2(a)	if (a)	\
					GPIO_SetBits(LED2_GPIO_PORT,LED2_PIN);\
					else		\
					GPIO_ResetBits(LED2_GPIO_PORT,LED2_PIN)

#define LED3(a)	if (a)	\
					GPIO_SetBits(LED3_GPIO_PORT,LED3_PIN);\
					else		\
					GPIO_ResetBits(LED3_GPIO_PORT,LED3_PIN)


/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)			   {p->BSRRL=i;}			  //����Ϊ�ߵ�ƽ		
#define digitalLo(p,i)			   {p->BSRRH=i;}				//����͵�ƽ
#define digitalToggle(p,i)		 {p->ODR ^=i;}			//�����ת״̬


/* �������IO�ĺ� */
#define LED1_TOGGLE    digitalToggle(LED1_GPIO_PORT,LED1_PIN)
#define LED1_OFF       digitalHi(LED1_GPIO_PORT,LED1_PIN)
#define LED1_ON        digitalLo(LED1_GPIO_PORT,LED1_PIN)

#define LED2_TOGGLE    digitalToggle(LED2_GPIO_PORT,LED2_PIN)
#define LED2_OFF       digitalHi(LED2_GPIO_PORT,LED2_PIN)
#define LED2_ON        digitalLo(LED2_GPIO_PORT,LED2_PIN)

#define LED3_TOGGLE    digitalToggle(LED3_GPIO_PORT,LED3_PIN)
#define LED3_OFF       digitalHi(LED3_GPIO_PORT,LED3_PIN)
#define LED3_ON        digitalLo(LED3_GPIO_PORT,LED3_PIN)



//LED1 - R
//LED2 - G
//LED3 - B
                                       //  R   G   B
#define  LED_BLACK             0X0     //  0   0   0
#define  LED_RED               0X1     //  0   0   1
#define  LED_GREEN             0X2     //  0   1   0
#define  LED_YELLOW            0X3     //  0   1   1
#define  LED_BLUE              0X4     //  1   0   0
#define  LED_MAGENTA           0X5     //  1   0   1
#define  LED_CYAN              0X6     //  1   1   0
#define  LED_WHITE             0X7     //  1   1   1

#define IS_RGB111_ALL_PERIPH(PERIPH) (((PERIPH) == LED_BLACK)   || \
																			((PERIPH) == LED_RED)     || \
																			((PERIPH) == LED_GREEN)   || \
																			((PERIPH) == LED_YELLOW)  || \
																			((PERIPH) == LED_BLUE)    || \
																			((PERIPH) == LED_MAGENTA) || \
																			((PERIPH) == LED_CYAN)    || \
																			((PERIPH) == LED_WHITE)

void LED_GPIO_Config(void);
void LED_RGB111(uint8_t rgb111);

#endif /* __LED_H */
