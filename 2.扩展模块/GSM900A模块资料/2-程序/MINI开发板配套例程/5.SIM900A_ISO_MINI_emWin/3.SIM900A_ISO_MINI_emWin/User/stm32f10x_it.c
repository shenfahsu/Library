/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_ov7725.h"
#include "bsp_led.h"

#include "GUI.h"
#include "WFGUI_Common.h"
#include "DIALOG.h"


extern __IO int32_t OS_TimeMS;
extern void LED_Toggle(void);
extern uint32_t TimeDisplay;

extern void TIMDelay_Decrement(void);
void bsp_USART2_IRQHandler(void);



// ����ͷ-����ov7725
extern u8 Ov7725_vsync;




/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	
	printf("\r\n hardfalut err  \r\n");
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	static uint8_t count_time=0;
  OS_TimeMS ++;
	
	count_time++;
	if(count_time == 10)
	{
		GUI_TOUCH_Exec();			//ÿ10ms����һ�Σ��������ô�������
		count_time =0;
	}	
  
	//LED_Toggle();						//ÿ100ms��תһ��led
	
	
}


/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */
void TIM6_IRQHandler(void)
{

	if ( TIM_GetITStatus(TIM6 , TIM_IT_Update) != RESET ) 
	{	
		TIMDelay_Decrement();

		TIM_ClearITPendingBit(TIM6 , TIM_FLAG_Update);  		 
	}		 	
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @brief  This function handles RTC interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{

	  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	  {
	    /* Clear the RTC Second interrupt */
	    RTC_ClearITPendingBit(RTC_IT_SEC);
	
	    /* Enable time update */
	    TimeDisplay = 1;
        
			/* �жϴ����Ƿ���Ч */
			if(WM_IsWindow(WinPara.hWinStatus))
			{
				/*	���� */				
				WM_SendMessageNoPara(WinPara.hWinStatus,MY_MESSAGE_RTC);
			}
			if(WM_IsWindow(WinPara.hWinTime))
			{			
				WM_SendMessageNoPara(WinPara.hWinTime,MY_MESSAGE_RTC);
			}
			
	    /* Wait until last write operation on RTC registers has finished */
	    RTC_WaitForLastTask();
	  }
}

/* ����1�����ж� */
/* ����1�����ж� */
void USART1_IRQHandler(void)
{
   uint8_t rec_cmd;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{     
			rec_cmd = USART_ReceiveData(USART1);
      //printf("%c \r\n", rec_cmd);
      if(rec_cmd == '\r' )  // �س�
      {
        //Soft_Reset();
      }
	}	 
}

/*
*����2�жϣ�����SIM900Aģ��
*/
void USART2_IRQHandler(void)
{
    bsp_USART2_IRQHandler();
}
/*
 * ��������SDIO_IRQHandler
 * ����  ����SDIO_ITConfig(���������������sdio�ж�	��
 *		     ���ݴ������ʱ�����ж�
 * ����  ����		 
 * ���  ����
 */
void SDIO_IRQHandler(void) 
{
  /* Process All SDIO Interrupt Sources */
  SD_ProcessIRQSrc();
}


void SD_SDIO_DMA_IRQHANDLER(void)
{
  /* Process DMA2 Stream3 or DMA2 Stream6 Interrupt Sources */
  SD_ProcessDMAIRQ();
}


/* ov7725 ���ж� ������� */
//void EXTI0_IRQHandler(void)
//{
//    if ( EXTI_GetITStatus(EXTI_Line0) != RESET ) 	//���EXTI_Line0��·�ϵ��ж������Ƿ��͵���NVIC 
//    {
//        if( Ov7725_vsync == 0 )
//        {
//            FIFO_WRST_L(); 	                      //����ʹFIFOд(����from����ͷ)ָ�븴λ
//            FIFO_WE_H();	                        //����ʹFIFOд����
//            
//            Ov7725_vsync = 1;	   	
//            FIFO_WE_H();                          //ʹFIFOд����
//            FIFO_WRST_H();                        //����ʹFIFOд(����from����ͷ)ָ���˶�
//        }
//        else if( Ov7725_vsync == 1 )
//        {
//            FIFO_WE_L();                          //����ʹFIFOд��ͣ
//            Ov7725_vsync = 2;
//					
//						/* ������Ϣ��ʼ��ʾ���� */	
//					
//        } 

//					
//        EXTI_ClearITPendingBit(EXTI_Line0);		    //���EXTI_Line0��·�����־λ        
//    }    
//}


void EXTI0_IRQHandler(void)
{
	
	if(EXTI_GetITStatus(EXTI_Line0) != RESET) //ȷ���Ƿ������EXTI Line�ж�
	{		
		
		if(WM_IsWindow(WinPara.hWinMain))
		{		
			WM_SendMessageNoPara(WinPara.hWinMain ,MY_MESSAGE_SNAPSHOT);
		}

		EXTI_ClearITPendingBit(EXTI_Line0);     //����жϱ�־λ
	}  
}


//void EXTI13_IRQnHandler(void)
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line13) != RESET) //ȷ���Ƿ������EXTI Line�ж�
	{			
		EXTI_ClearITPendingBit(EXTI_Line13);     //����жϱ�־λ
	}  
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
