/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                  STATISTICS MODULE
*
* File    : OS_STAT.C
* By      : JJL
* Version : V3.03.01
*
* LICENSING TERMS:
* ---------------
*           uC/OS-III is provided in source form for FREE short-term evaluation, for educational use or 
*           for peaceful research.  If you plan or intend to use uC/OS-III in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-III for its use in your 
*           application/product.   We provide ALL the source code for your convenience and to help you 
*           experience uC/OS-III.  The fact that the source is provided does NOT mean that you can use 
*           it commercially without paying a licensing fee.
*
*           Knowledge of the source code may NOT be used to develop a similar product.
*
*           Please help us continue to provide the embedded community with the finest software available.
*           Your honesty is greatly appreciated.
*
*           You can contact us at www.micrium.com, or by phone at +1 (954) 217-2036.
************************************************************************************************************************
*/

#define  MICRIUM_SOURCE
#include <os.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *os_stat__c = "$Id: $";
#endif


#if OS_CFG_STAT_TASK_EN > 0u

/*
************************************************************************************************************************
*                                                   RESET STATISTICS
*
* Description: This function is called by your application to reset the statistics.
*
* Argument(s): p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_NONE
*
* Returns    : none
************************************************************************************************************************
*/

void  OSStatReset (OS_ERR  *p_err)
{
#if (OS_CFG_DBG_EN > 0u)
    OS_TCB      *p_tcb;
#if (OS_MSG_EN > 0u)
    OS_MSG_Q    *p_msg_q;
#endif
#if (OS_CFG_Q_EN > 0u)
    OS_Q        *p_q;
#endif
#endif
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

    CPU_CRITICAL_ENTER();
#if OS_CFG_ISR_POST_DEFERRED_EN > 0u
    OSIntQTaskTimeMax     = (CPU_TS    )0;                  /* Reset the task execution times                         */
    OSIntQNbrEntriesMax   = (OS_OBJ_QTY)0;                  /* Reset the queue maximum number of entries              */
#endif

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageMax = 0u;
    OSStatTaskTimeMax     = (CPU_TS)0;
#endif

    OSTickTaskTimeMax     = (CPU_TS)0;

#if OS_CFG_TMR_EN > 0u
    OSTmrTaskTimeMax      = (CPU_TS)0;
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    OSIntDisTimeMax       = (CPU_TS)0;                      /* Reset the maximum interrupt disable time               */
#endif

#if OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u
    OSSchedLockTimeMax    = (CPU_TS)0;                      /* Reset the maximum scheduler lock time                  */
#endif

#if OS_MSG_EN > 0u
    OSMsgPool.NbrUsedMax  = 0u;
#endif
    CPU_CRITICAL_EXIT();

#if OS_CFG_DBG_EN > 0u
    CPU_CRITICAL_ENTER();
    p_tcb = OSTaskDbgListPtr;
    CPU_CRITICAL_EXIT();
    while (p_tcb != (OS_TCB *)0) {                          /* Reset per-Task statistics                              */
        CPU_CRITICAL_ENTER();

#ifdef CPU_CFG_INT_DIS_MEAS_EN
        p_tcb->IntDisTimeMax    = (CPU_TS      )0;
#endif

#if OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u
        p_tcb->SchedLockTimeMax = (CPU_TS      )0;
#endif

#if OS_CFG_TASK_PROFILE_EN > 0u
#if OS_CFG_TASK_Q_EN > 0u
        p_tcb->MsgQPendTimeMax  = (CPU_TS      )0;
#endif
        p_tcb->SemPendTimeMax   = (CPU_TS      )0;
        p_tcb->CtxSwCtr         = (OS_CTR      )0;
        p_tcb->CPUUsage         = (OS_CPU_USAGE)0;
        p_tcb->CPUUsageMax      = (OS_CPU_USAGE)0;
        p_tcb->CyclesTotal      = (OS_CYCLES   )0;
        p_tcb->CyclesTotalPrev  = (OS_CYCLES   )0;
        p_tcb->CyclesStart      =  OS_TS_GET();
#endif

#if OS_CFG_TASK_Q_EN > 0u
        p_msg_q                 = &p_tcb->MsgQ;
        p_msg_q->NbrEntriesMax  = (OS_MSG_QTY  )0;
#endif
        p_tcb                   = p_tcb->DbgNextPtr;
        CPU_CRITICAL_EXIT();
    }
#endif

#if (OS_CFG_Q_EN > 0u) && (OS_CFG_DBG_EN > 0u)
    CPU_CRITICAL_ENTER();
    p_q = OSQDbgListPtr;
    CPU_CRITICAL_EXIT();
    while (p_q != (OS_Q *)0) {                              /* Reset message queues statistics                        */
        CPU_CRITICAL_ENTER();
        p_msg_q                = &p_q->MsgQ;
        p_msg_q->NbrEntriesMax = (OS_MSG_QTY)0;
        p_q                    = p_q->DbgNextPtr;
        CPU_CRITICAL_EXIT();
    }
#endif

    OS_TickListResetPeak();                                 /* Reset tick wheel statistics                            */

#if OS_CFG_TMR_EN > 0u
    OS_TmrResetPeak();
#endif


   *p_err = OS_ERR_NONE;
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                DETERMINE THE CPU CAPACITY
*
* Description: This function is called by your application to establish CPU usage by first determining how high a 32-bit
*              counter would count to in 1/10 second if no other tasks were to execute during that time.  CPU usage is
*              then determined by a low priority task which keeps track of this 32-bit counter every second but this
*              time, with other tasks running.  CPU usage is determined by:
*
*                                             OS_Stat_IdleCtr
*                 CPU Usage (%) = 100 * (1 - ------------------)
*                                            OS_Stat_IdleCtrMax
*
* Argument(s): p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_NONE
*
* Returns    : none
************************************************************************************************************************
*/

void  OSStatTaskCPUUsageInit (OS_ERR  *p_err)
{
    OS_ERR   err;
    OS_TICK  dly;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL                                 //如果使能了安全检测
    if (p_err == (OS_ERR *)0) {                           //如果 p_err 为空
        OS_SAFETY_CRITICAL_EXCEPTION();                   //执行安全检测异常函数
        return;                                           //返回，停止执行
    }
#endif

#if (OS_CFG_TMR_EN > 0u)                                 //如果使能了软件定时器
    OSTaskSuspend(&OSTmrTaskTCB, &err);                  //挂起软件定时任务
    if (err != OS_ERR_NONE) {                            //如果挂起失败
       *p_err = err;                                     //返回失败原因
        return;                                          //返回，停止执行
    }
#endif

    OSTimeDly((OS_TICK )2,                               //先延时两个节拍，为后面延时同步时钟节拍，增加准确性
              (OS_OPT  )OS_OPT_TIME_DLY,
              (OS_ERR *)&err);
    if (err != OS_ERR_NONE) {                            //如果延时失败
       *p_err = err;                                     //返回失败原因
        return;                                          //返回，停止执行
    }
    CPU_CRITICAL_ENTER();                                //关中断
    OSStatTaskCtr = (OS_TICK)0;                          //清零空闲计数器  
    CPU_CRITICAL_EXIT();                                 //开中断
    /* 根据设置的宏计算统计任务的执行节拍数 */
    dly = (OS_TICK)0;
    if (OSCfg_TickRate_Hz > OSCfg_StatTaskRate_Hz) {
        dly = (OS_TICK)(OSCfg_TickRate_Hz / OSCfg_StatTaskRate_Hz);
    }
    if (dly == (OS_TICK)0) {
        dly =  (OS_TICK)(OSCfg_TickRate_Hz / (OS_RATE_HZ)10);
    }
    /* 延时累加空闲计数器，获取最大空闲计数值 */
    OSTimeDly(dly,                                         
              OS_OPT_TIME_DLY,
              &err);
    
#if (OS_CFG_TMR_EN > 0u)                                 //如果使能了软件定时器 
    OSTaskResume(&OSTmrTaskTCB, &err);                   //恢复软件定时器任务
    if (err != OS_ERR_NONE) {                            //如果恢复失败
       *p_err = err;                                     //返回错误原因
        return;                                          //返回，停止执行
    }
#endif
    /* 如果上面没产生错误 */
    CPU_CRITICAL_ENTER();                                //关中断
    OSStatTaskTimeMax = (CPU_TS)0;                       //

    OSStatTaskCtrMax  = OSStatTaskCtr;                   //存储最大空闲计数值
    OSStatTaskRdy     = OS_STATE_RDY;                    //准备就绪统计任务
    CPU_CRITICAL_EXIT();                                 //开中断
   *p_err             = OS_ERR_NONE;                     //错误类型为“无错误”
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    STATISTICS TASK
*
* Description: This task is internal to uC/OS-III and is used to compute some statistics about the multitasking
*              environment.  Specifically, OS_StatTask() computes the CPU usage.  CPU usage is determined by:
*
*                                                   OSStatTaskCtr
*                 OSStatTaskCPUUsage = 100 * (1 - ------------------)     (units are in %)
*                                                  OSStatTaskCtrMax
*
* Arguments  : p_arg     this pointer is not used at this time.
*
* Returns    : none
*
* Note(s)    : 1) This task runs at a priority level higher than the idle task.
*
*              2) You can disable this task by setting the configuration #define OS_CFG_STAT_TASK_EN to 0.
*
*              3) You MUST have at least a delay of 2/10 seconds to allow for the system to establish the maximum value
*                 for the idle counter.
*
*              4) This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_StatTask (void  *p_arg)                           //统计任务函数
{
#if OS_CFG_DBG_EN > 0u
#if OS_CFG_TASK_PROFILE_EN > 0u
    OS_CPU_USAGE usage;
    OS_CYCLES    cycles_total;
    OS_CYCLES    cycles_div;
    OS_CYCLES    cycles_mult;
    OS_CYCLES    cycles_max;
#endif
    OS_TCB      *p_tcb;
#endif
    OS_TICK      ctr_max;
    OS_TICK      ctr_mult;
    OS_TICK      ctr_div;
    OS_ERR       err;
    OS_TICK      dly;
    CPU_TS       ts_start;
    CPU_TS       ts_end;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

    p_arg = p_arg;                                          //没意义，仅为预防编译器警告
    while (OSStatTaskRdy != DEF_TRUE) {                     //如果统计任务没被允许运行
        OSTimeDly(2u * OSCfg_StatTaskRate_Hz,               //一直延时
                  OS_OPT_TIME_DLY,
                  &err);
    }
    OSStatReset(&err);                                      //如果统计任务已被就绪，复位统计，继续执行
    /* 根据设置的宏计算统计任务的执行节拍数 */
    dly = (OS_TICK)0;                                      
    if (OSCfg_TickRate_Hz > OSCfg_StatTaskRate_Hz) {       
        dly = (OS_TICK)(OSCfg_TickRate_Hz / OSCfg_StatTaskRate_Hz);
    }
    if (dly == (OS_TICK)0) {
        dly =  (OS_TICK)(OSCfg_TickRate_Hz / (OS_RATE_HZ)10);
    }

    while (DEF_ON) {                                       //进入任务体
        ts_start        = OS_TS_GET();                     //获取时间戳
#ifdef  CPU_CFG_INT_DIS_MEAS_EN                            //如果要测量关中断时间
        OSIntDisTimeMax = CPU_IntDisMeasMaxGet();          //获取最大的关中断时间
#endif

        CPU_CRITICAL_ENTER();                              //关中断
        OSStatTaskCtrRun   = OSStatTaskCtr;                //获取上一次延时的空闲计数值 
        OSStatTaskCtr      = (OS_TICK)0;                   //进行下一次延时的空闲计数
        CPU_CRITICAL_EXIT();                               //开中断
        /* 计算CPU使用率 */
        if (OSStatTaskCtrMax > OSStatTaskCtrRun) {         //如果空闲计数值小于最大空闲计数值
            if (OSStatTaskCtrMax < 400000u) {              //这些分类是为了避免计算CPU使用率过程中    
                ctr_mult = 10000u;                         //产生溢出， 就是避免相乘时超出32位寄存器。
                ctr_div  =     1u;
            } else if (OSStatTaskCtrMax <   4000000u) {                             
                ctr_mult =  1000u;
                ctr_div  =    10u;
            } else if (OSStatTaskCtrMax <  40000000u) {                             
                ctr_mult =   100u;
                ctr_div  =   100u;
            } else if (OSStatTaskCtrMax < 400000000u) {                            
                ctr_mult =    10u;
                ctr_div  =  1000u;
            } else {                                                                 
                ctr_mult =     1u;
                ctr_div  = 10000u;
            }
            ctr_max            = OSStatTaskCtrMax / ctr_div; 
            OSStatTaskCPUUsage = (OS_CPU_USAGE)((OS_TICK)10000u - ctr_mult * OSStatTaskCtrRun / ctr_max);
            if (OSStatTaskCPUUsageMax < OSStatTaskCPUUsage) { //更新CPU使用率的最大历史记录
                OSStatTaskCPUUsageMax = OSStatTaskCPUUsage;
            }
        } else {                                           //如果空闲计数值大于或等于最大空闲计数值
            OSStatTaskCPUUsage = (OS_CPU_USAGE)10000u;     //那么CPU使用率为0
        }

        OSStatTaskHook();                                  //用户自定义的钩子函数

         /* 下面计算各个任务的CPU使用率，原理跟计算整体CPU使用率相似 */
#if OS_CFG_DBG_EN > 0u                                     //如果使能了调试代码和变量
#if OS_CFG_TASK_PROFILE_EN > 0u                            //如果使能了任务控制块的简况变量
        cycles_total = (OS_CYCLES)0;

        CPU_CRITICAL_ENTER();                              //关中断
        p_tcb = OSTaskDbgListPtr;                          //获取任务双向调试列表的首个任务
        CPU_CRITICAL_EXIT();                               //开中断
        while (p_tcb != (OS_TCB *)0) {                     //如果该任务非空 
            OS_CRITICAL_ENTER();                           //进入临界段
            p_tcb->CyclesTotalPrev =  p_tcb->CyclesTotal;  //保存任务的运行周期
            p_tcb->CyclesTotal     = (OS_CYCLES)0;         //复位运行周期，为下次运行做准备
            OS_CRITICAL_EXIT();                            //退出临界段

            cycles_total          += p_tcb->CyclesTotalPrev;//所有任务运行周期的总和

            CPU_CRITICAL_ENTER();                          //关中断
            p_tcb                  = p_tcb->DbgNextPtr;    //获取列表的下一个任务，进行下一次循环
            CPU_CRITICAL_EXIT();                           //开中断
        }
#endif

        /* 使用算法计算各个任务的CPU使用率和任务堆栈用量 */
#if OS_CFG_TASK_PROFILE_EN > 0u                            //如果使能了任务控制块的简况变量
                                                            
        if (cycles_total > (OS_CYCLES)0u) {                //如果有任务占用过CPU
            if (cycles_total < 400000u) {                  //这些分类是为了避免计算CPU使用率过程中 
                cycles_mult = 10000u;                      //产生溢出， 就是避免相乘时超出32位寄存器。
                cycles_div  =     1u;
            } else if (cycles_total <   4000000u) {                                 
                cycles_mult =  1000u;
                cycles_div  =    10u;
            } else if (cycles_total <  40000000u) {                                 
                cycles_mult =   100u;
                cycles_div  =   100u;
            } else if (cycles_total < 400000000u) {                                  
                cycles_mult =    10u;
                cycles_div  =  1000u;
            } else {                                                                 
                cycles_mult =     1u;
                cycles_div  = 10000u;
            }
            cycles_max  = cycles_total / cycles_div;
        } else {                                           //如果没有任务占用过CPU
            cycles_mult = 0u;
            cycles_max  = 1u;
        }
#endif
        CPU_CRITICAL_ENTER();                              //关中断
        p_tcb = OSTaskDbgListPtr;                          //获取任务双向调试列表的首个任务
        CPU_CRITICAL_EXIT();                               //开中断
        while (p_tcb != (OS_TCB *)0) {                     //如果该任务非空
#if OS_CFG_TASK_PROFILE_EN > 0u                            //如果使能了任务控制块的简况变量
            usage = (OS_CPU_USAGE)(cycles_mult * p_tcb->CyclesTotalPrev / cycles_max); //计算任务的CPU使用率
            if (usage > 10000u) {                          //任务的CPU使用率为100%
                usage = 10000u;
            }
            p_tcb->CPUUsage = usage;                       //保存任务的CPU使用率
            if (p_tcb->CPUUsageMax < usage) {              //更新任务的最大CPU使用率的历史记录
                p_tcb->CPUUsageMax = usage;
            }
#endif
        /* 堆栈检测 */
#if OS_CFG_STAT_TASK_STK_CHK_EN > 0u                       //如果使能了任务堆栈检测
            OSTaskStkChk( p_tcb,                           //计算被激活任务的堆栈用量
                         &p_tcb->StkFree,
                         &p_tcb->StkUsed,
                         &err);
#endif

            CPU_CRITICAL_ENTER();                          //关中断
            p_tcb = p_tcb->DbgNextPtr;                     //获取列表的下一个任务，进行下一次循环
            CPU_CRITICAL_EXIT();                           //开中断
        }
#endif

        if (OSStatResetFlag == DEF_TRUE) {                 //如果需要复位统计
            OSStatResetFlag  = DEF_FALSE;
            OSStatReset(&err);                             //复位统计
        }

        ts_end = OS_TS_GET() - ts_start;                   //计算统计任务的执行时间
        if (OSStatTaskTimeMax < ts_end) {                  //更新统计任务的最大执行时间的历史记录
            OSStatTaskTimeMax = ts_end;
        }

        OSTimeDly(dly,                                     //按照先前计算的执行节拍数延时
                  OS_OPT_TIME_DLY,
                  &err);
    }
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                              INITIALIZE THE STATISTICS
*
* Description: This function is called by OSInit() to initialize the statistic task.
*
* Argument(s): p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_STK_INVALID       If you specified a NULL stack pointer during configuration
*                            OS_ERR_STK_SIZE_INVALID  If you didn't specify a large enough stack.
*                            OS_ERR_PRIO_INVALID      If you specified a priority for the statistic task equal to or
*                                                     lower (i.e. higher number) than the idle task.
*                            OS_ERR_xxx               An error code returned by OSTaskCreate()
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_StatTaskInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

    OSStatTaskCtr    = (OS_TICK)0;
    OSStatTaskCtrRun = (OS_TICK)0;
    OSStatTaskCtrMax = (OS_TICK)0;
    OSStatTaskRdy    = OS_STATE_NOT_RDY;                    /* Statistic task is not ready                            */
    OSStatResetFlag  = DEF_FALSE;

                                                            /* ---------------- CREATE THE STAT TASK ---------------- */
    if (OSCfg_StatTaskStkBasePtr == (CPU_STK *)0) {
       *p_err = OS_ERR_STAT_STK_INVALID;
        return;
    }

    if (OSCfg_StatTaskStkSize < OSCfg_StkSizeMin) {
       *p_err = OS_ERR_STAT_STK_SIZE_INVALID;
        return;
    }

    if (OSCfg_StatTaskPrio >= (OS_CFG_PRIO_MAX - 1u)) {
       *p_err = OS_ERR_STAT_PRIO_INVALID;
        return;
    }

    OSTaskCreate((OS_TCB     *)&OSStatTaskTCB,
                 (CPU_CHAR   *)((void *)"uC/OS-III Stat Task"),
                 (OS_TASK_PTR )OS_StatTask,
                 (void       *)0,
                 (OS_PRIO     )OSCfg_StatTaskPrio,
                 (CPU_STK    *)OSCfg_StatTaskStkBasePtr,
                 (CPU_STK_SIZE)OSCfg_StatTaskStkLimit,
                 (CPU_STK_SIZE)OSCfg_StatTaskStkSize,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)p_err);
}

#endif
