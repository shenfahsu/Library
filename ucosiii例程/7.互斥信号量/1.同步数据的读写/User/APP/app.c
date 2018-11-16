/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

OS_MUTEX mutex;                         //���������ź���


uint8_t ucValue [ 2 ] = { 0x00, 0x00 };


/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;      //������ƿ�

static  OS_TCB   AppTaskWriteTCB;
static  OS_TCB   AppTaskReadTCB;


/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //�����ջ

static  CPU_STK  AppTaskWriteStk [ APP_TASK_WRITE_STK_SIZE ];
static  CPU_STK  AppTaskReadStk [ APP_TASK_READ_STK_SIZE ];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);               //����������

static  void  AppTaskWrite  ( void * p_arg );
static  void  AppTaskRead   ( void * p_arg );


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  err;


    OSInit(&err);                                                           //��ʼ�� uC/OS-III

	  /* ������ʼ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                            //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Start",                            //��������
                 (OS_TASK_PTR ) AppTaskStart,                               //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_START_PRIO,                        //��������ȼ�
                 (CPU_STK    *)&AppTaskStartStk[0],                         //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,               //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,                    //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

    OSStart(&err);                                                          //�����������������uC/OS-III���ƣ�

}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;


    (void)p_arg;

    BSP_Init();                                                 //�弶��ʼ��
    CPU_Init();                                                 //��ʼ�� CPU �����ʱ��������ж�ʱ���������������

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //��ȡ CPU �ں�ʱ��Ƶ�ʣ�SysTick ����ʱ�ӣ�
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //�����û��趨��ʱ�ӽ���Ƶ�ʼ��� SysTick ��ʱ���ļ���ֵ
    OS_CPU_SysTickInit(cnts);                                   //���� SysTick ��ʼ�����������ö�ʱ������ֵ��������ʱ��

    Mem_Init();                                                 //��ʼ���ڴ������������ڴ�غ��ڴ�ر�

#if OS_CFG_STAT_TASK_EN > 0u                                    //���ʹ�ܣ�Ĭ��ʹ�ܣ���ͳ������
    OSStatTaskCPUUsageInit(&err);                               //����û��Ӧ������ֻ�п�����������ʱ CPU �ģ����
#endif                                                          //���������� OS_Stat_IdleCtrMax ��ֵ��Ϊ������� CPU 
                                                                //ʹ����ʹ�ã���
    CPU_IntDisMeasMaxCurReset();                                //��λ�����㣩��ǰ�����ж�ʱ��

    
		/* ���������ź��� mutex */
    OSMutexCreate ((OS_MUTEX  *)&mutex,           //ָ���ź���������ָ��
                   (CPU_CHAR  *)"Mutex For Test", //�ź���������
                   (OS_ERR    *)&err);            //��������
							 

		/* ���� AppTaskWrite ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskWriteTCB,                            //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Write",                            //��������
                 (OS_TASK_PTR ) AppTaskWrite,                               //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_WRITE_PRIO,                        //��������ȼ�
                 (CPU_STK    *)&AppTaskWriteStk[0],                         //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_WRITE_STK_SIZE / 10,               //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_WRITE_STK_SIZE,                    //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

		/* ���� AppTaskRead ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskReadTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Read",                             //��������
                 (OS_TASK_PTR ) AppTaskRead,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_READ_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskReadStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_READ_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_READ_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
    
		OSTaskDel ( & AppTaskStartTCB, & err );                     //ɾ����ʼ������������������
		
		
}


/*
*********************************************************************************************************
*                                          WRITE TASK
*********************************************************************************************************
*/
static  void  AppTaskWrite ( void * p_arg )
{
	OS_ERR      err;

	
	(void)p_arg;

					 
	while (DEF_TRUE) {                                   //������
		OSMutexPend ((OS_MUTEX  *)&mutex,                  //���뻥���ź��� mutex
								 (OS_TICK    )0,                       //�����޵ȴ�
								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //����������뵽�ź����Ͷ�������
								 (CPU_TS    *)0,                       //������ʱ���
								 (OS_ERR    *)&err);                   //���ش�������		
		
		ucValue [ 0 ] ++;
		
		OSTimeDly ( 100, OS_OPT_TIME_DLY, & err );        //��ʱ100��ʱ�ӽ��ģ�100ms��
		
		ucValue [ 1 ] ++;
		
		OSMutexPost ((OS_MUTEX  *)&mutex,                  //�ͷŻ����ź��� mutex
								 (OS_OPT     )OS_OPT_POST_NONE,        //�����������
								 (OS_ERR    *)&err);                   //���ش�������		
		
	}

}


/*
*********************************************************************************************************
*                                          READ TASK
*********************************************************************************************************
*/
static  void  AppTaskRead ( void * p_arg )
{
	OS_ERR      err;
	CPU_SR_ALLOC(); //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú�������
									//����һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ���
									// SR���ٽ�ι��ж�ֻ�豣��SR�������ж�ʱ����ֵ��ԭ��
	(void)p_arg;

					 
	while (DEF_TRUE) {                                   //������
		OSMutexPend ((OS_MUTEX  *)&mutex,                  //���뻥���ź��� mutex
								 (OS_TICK    )0,                       //�����޵ȴ�
								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //������벻���Ͷ�������
								 (CPU_TS    *)0,                       //������ʱ���
								 (OS_ERR    *)&err);                   //���ش�������		
		
		if ( ucValue [ 0 ] == ucValue [ 1 ] )
		{ 
			OS_CRITICAL_ENTER();                             //�����ٽ��
			
			printf ( "\r\nSUCCESSFUL\r\n" );
			
			OS_CRITICAL_EXIT();
			
		}
		else
		{
			OS_CRITICAL_ENTER();                             //�����ٽ��
			
			printf ( "\r\nFAIL\r\n" );
			
			OS_CRITICAL_EXIT();
			
		}

		OSMutexPost ((OS_MUTEX  *)&mutex,                 //�ͷŻ����ź��� mutex
								 (OS_OPT     )OS_OPT_POST_NONE,       //�����������
								 (OS_ERR    *)&err);                  //���ش�������		
		
		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );          //��ʱ1000��ʱ�ӽ��ģ�1s��
				
	}
	
}











