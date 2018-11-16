#ifndef TIME_TEST_H
#define TIME_TEST_H

#include "stm32f10x.h"

#define TIMDelay_ms(ms) TIMDelay_10us(ms*100)

void TIM6_Configuration(void);
void TIMDelay_Decrement(void);
void TIMDelay_10us(__IO u32 nTime);


#endif	/* TIME_TEST_H */
