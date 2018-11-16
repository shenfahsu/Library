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

OS_MUTEX mutex;                         //声明互斥信号量


uint8_t ucValue [ 2 ] = { 0x00, 0x00 };


/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;      //任务控制块

static  OS_TCB   AppTaskWriteTCB;
static  OS_TCB   AppTaskReadTCB;


/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //任务堆栈

static  CPU_STK  AppTaskWriteStk [ APP_TASK_WRITE_STK_SIZE ];
static  CPU_STK  AppTaskReadStk [ APP_TASK_READ_STK_SIZE ];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);               //任务函数声明

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


    OSInit(&err);                                                           //初始化 uC/OS-III

	  /* 创建起始任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                            //任务控制块地址
                 (CPU_CHAR   *)"App Task Start",                            //任务名称
                 (OS_TASK_PTR ) AppTaskStart,                               //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_START_PRIO,                        //任务的优先级
                 (CPU_STK    *)&AppTaskStartStk[0],                         //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,               //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,                    //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型

    OSStart(&err);                                                          //启动多任务管理（交由uC/OS-III控制）

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

    BSP_Init();                                                 //板级初始化
    CPU_Init();                                                 //初始化 CPU 组件（时间戳、关中断时间测量和主机名）

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //获取 CPU 内核时钟频率（SysTick 工作时钟）
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //根据用户设定的时钟节拍频率计算 SysTick 定时器的计数值
    OS_CPU_SysTickInit(cnts);                                   //调用 SysTick 初始化函数，设置定时器计数值和启动定时器

    Mem_Init();                                                 //初始化内存管理组件（堆内存池和内存池表）

#if OS_CFG_STAT_TASK_EN > 0u                                    //如果使能（默认使能）了统计任务
    OSStatTaskCPUUsageInit(&err);                               //计算没有应用任务（只有空闲任务）运行时 CPU 的（最大）
#endif                                                          //容量（决定 OS_Stat_IdleCtrMax 的值，为后面计算 CPU 
                                                                //使用率使用）。
    CPU_IntDisMeasMaxCurReset();                                //复位（清零）当前最大关中断时间

    
		/* 创建互斥信号量 mutex */
    OSMutexCreate ((OS_MUTEX  *)&mutex,           //指向信号量变量的指针
                   (CPU_CHAR  *)"Mutex For Test", //信号量的名字
                   (OS_ERR    *)&err);            //错误类型
							 

		/* 创建 AppTaskWrite 任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskWriteTCB,                            //任务控制块地址
                 (CPU_CHAR   *)"App Task Write",                            //任务名称
                 (OS_TASK_PTR ) AppTaskWrite,                               //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_WRITE_PRIO,                        //任务的优先级
                 (CPU_STK    *)&AppTaskWriteStk[0],                         //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_WRITE_STK_SIZE / 10,               //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_WRITE_STK_SIZE,                    //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型

		/* 创建 AppTaskRead 任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskReadTCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task Read",                             //任务名称
                 (OS_TASK_PTR ) AppTaskRead,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_READ_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskReadStk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_READ_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_READ_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
    
		OSTaskDel ( & AppTaskStartTCB, & err );                     //删除起始任务本身，该任务不再运行
		
		
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

					 
	while (DEF_TRUE) {                                   //任务体
		OSMutexPend ((OS_MUTEX  *)&mutex,                  //申请互斥信号量 mutex
								 (OS_TICK    )0,                       //无期限等待
								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果不能申请到信号量就堵塞任务
								 (CPU_TS    *)0,                       //不想获得时间戳
								 (OS_ERR    *)&err);                   //返回错误类型		
		
		ucValue [ 0 ] ++;
		
		OSTimeDly ( 100, OS_OPT_TIME_DLY, & err );        //延时100个时钟节拍（100ms）
		
		ucValue [ 1 ] ++;
		
		OSMutexPost ((OS_MUTEX  *)&mutex,                  //释放互斥信号量 mutex
								 (OS_OPT     )OS_OPT_POST_NONE,        //进行任务调度
								 (OS_ERR    *)&err);                   //返回错误类型		
		
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
	CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
									//定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
									// SR（临界段关中断只需保存SR），开中断时将该值还原。
	(void)p_arg;

					 
	while (DEF_TRUE) {                                   //任务体
		OSMutexPend ((OS_MUTEX  *)&mutex,                  //申请互斥信号量 mutex
								 (OS_TICK    )0,                       //无期限等待
								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果申请不到就堵塞任务
								 (CPU_TS    *)0,                       //不想获得时间戳
								 (OS_ERR    *)&err);                   //返回错误类型		
		
		if ( ucValue [ 0 ] == ucValue [ 1 ] )
		{ 
			OS_CRITICAL_ENTER();                             //进入临界段
			
			printf ( "\r\nSUCCESSFUL\r\n" );
			
			OS_CRITICAL_EXIT();
			
		}
		else
		{
			OS_CRITICAL_ENTER();                             //进入临界段
			
			printf ( "\r\nFAIL\r\n" );
			
			OS_CRITICAL_EXIT();
			
		}

		OSMutexPost ((OS_MUTEX  *)&mutex,                 //释放互斥信号量 mutex
								 (OS_OPT     )OS_OPT_POST_NONE,       //进行任务调度
								 (OS_ERR    *)&err);                  //返回错误类型		
		
		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );          //延时1000个时钟节拍（1s）
				
	}
	
}











