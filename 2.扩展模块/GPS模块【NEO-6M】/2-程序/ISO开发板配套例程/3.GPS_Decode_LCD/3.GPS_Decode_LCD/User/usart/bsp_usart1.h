#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>


#define __DEBUG 	//¿ªÆô´®¿Úµ÷ÊÔ

#ifdef __DEBUG

#define DEBUG(format,...) printf("FILE: "__FILE__", LINE: %d: "format"\n", __LINE__, ##__VA_ARGS__)

#else

#define DEBUG(format,...)

#endif



void USART1_Config(void);
//int fputc(int ch, FILE *f);
//void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...);

#endif /* __USART1_H */
