
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_exti.h" 
#include "MicroStepDriver.h" 
#include "bsp_usart.h"
#include "MSD_test.h" 
 
/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
    //��ʼ��LED
    LED_GPIO_Config();
    //��ʼ������
    EXTI_Key_Config();
    //��ʼ������
    USART_Config();
	//���������ʼ��
	MSD_Init();
    //��ӡ��������
    ShowHelp();
    
    while(1) 
    {
        //����������
        DealSerialData();
    }
                          
}
/*********************************************END OF FILE**********************/

