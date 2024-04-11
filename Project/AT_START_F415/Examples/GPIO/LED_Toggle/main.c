/**
  ******************************************************************************
  * File   : GPIO/LED_Toggle/main.c
  * Version: V1.2.8
  * Date   : 2020-11-27
  * Brief  : Main program body
  ******************************************************************************
  */

#include <stdio.h>
#include "at32f4xx.h"
#include "main.h"
#include "app_cfg.h"
#include <stdlib.h>
#include <includes.h>

extern _m1_card_info m1_card_info;
uint32_t task_cnt[3];
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

OS_ERR timeerr;
OS_ERR ERRSTATE;

OS_TCB   AppTaskCardTCB;
CPU_STK  AppTaskCardStk[APP_CFG_TASK_CARD_STK_SIZE];

OS_TCB   AppTaskChargeTCB;
CPU_STK  AppTaskChargeStk[APP_CFG_TASK_CHARGE_STK_SIZE];


OS_TCB   AppTaskDwinTCB;
CPU_STK  AppTaskDwinStk[APP_CFG_TASK_DWIN_STK_SIZE];

OS_TCB   AppTask4GMainTCB;
CPU_STK  AppTask4GMainStk[APP_CFG_TASK_4GMAIN_STK_SIZE];

OS_TCB   AppTask4GRecvTCB;
CPU_STK  AppTask4GRecvStk[APP_CFG_TASK_4GRECV_STK_SIZE];

OS_TCB   AppTask4GSendTCB;
CPU_STK  AppTask4GSendStk[APP_CFG_TASK_4GSEND_STK_SIZE];

OS_TCB   AppTaskdlt645TCB;
CPU_STK  AppTaskdlt645Stk[APP_CFG_TASK_dlt645_STK_SIZE];

int AppTaskStartT = 0,AppTaskCardT = 0,AppTaskChargeT = 0,AppTaskDwinT = 0,AppTask4GMainT = 0,AppTask4GRecvT = 0,AppTask4GSendT = 0, AppTaskdlt645T = 0;
/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/




/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart          (void     *p_arg);
static  void  AppTaskCreate         (void);

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

SYSTEM_RTCTIME gs_SysTime;
pFunction Jump_To_LoadCode;
uint32_t JumpAddress;
#define LoadCodeAddress    0x8019000  //跳转到烧写代码的位置，通过这个位置执行程序，将应用代码从FLASH里面导出来烧写到应用程序区域
//跳转到烧写代码部分
void  JumpToProgramCode(void)
{
    __set_FAULTMASK(1);     //关闭所有中断后，才可以跳转：如果不关闭，升级直接成死板
    //OSTimeDly(100, OS_OPT_TIME_PERIODIC, &timeerr);  //加延迟确保全部关闭中断

    JumpAddress = *(__IO uint32_t*) (LoadCodeAddress + 4);
    Jump_To_LoadCode = (pFunction) JumpAddress;
    //初始化用户程序的堆栈指针
    __set_MSP(*(__IO uint32_t*) LoadCodeAddress);
    Jump_To_LoadCode();
}




int main(void)
{
    OS_ERR  err;

    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,              /* Create the start task                                */
                 (CPU_CHAR     *)"App Task Start",
                 (OS_TASK_PTR   )AppTaskStart,
                 (void         *)0u,
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,
                 (CPU_STK      *)&AppTaskStartStk[0u],
                 (CPU_STK_SIZE  )AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )0u,
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

    (void)&err;

    return (0u);
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
void IWDG_Config(uint8_t prv, uint16_t rlv)
{
    // 使能 预分频寄存器PR和重装载寄存器RLR可写
    IWDG_KeyRegWrite( IWDG_KeyRegWrite_Enable );

    // 设置预分频器值
    IWDG_SetPrescaler( prv );

    // 设置重装载寄存器值
    IWDG_SetReload( rlv );

    // 把重装载寄存器的值放到计数器中
    IWDG_ReloadCounter();

    // 使能 IWDG
    IWDG_Enable();
}


// 喂狗
void IWDG_Feed(void)
{
    // 把重装载寄存器的值放到计数器中，喂狗，防止IWDG复位
    // 当计数器的值减到0的时候会产生系统复位
    IWDG_ReloadCounter();
}

/* UNIQUE_ID[31: 0] */
uint32_t Unique_ID1;
/* UNIQUE_ID[63:32] */
uint32_t Unique_ID2;
/* UNIQUE_ID[95:63] */
uint32_t Unique_ID3;
extern _m1_card_info m1_card_info;		 //M1卡相关信息
#define sw32(A) ((((uint32_t)(A) & 0xff000000) >> 24) | (((uint32_t)(A) & 0x00ff0000) >> 8) | (((uint32_t)(A) & 0x0000ff00) << 8) | (((uint32_t)(A) & 0x000000ff) << 24))
#define SW32(A)	sw32(((uint32_t)A))
uint8_t  reststae = 0;   //0表示表示未复位 1表示复位了   只要解决复位了仍然继续充电
uint8_t OnlyMCUID[12] = {0};//唯一一个MCUID号  就是在绑定时用到
static  void  AppTaskStart (void *p_arg)
{
    uint32_t i = 0;
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;
    int temp;
    (void)p_arg;
    BSP_Init();                                                 /* Initialize BSP functions                             */

#if(USE_645==0)
    UART_Print_Init(115200); //打印串口初始化 只能二选一  //PB10 PB11  === 打印串口初始化（只能在前面初始化，后面初始化会死机）
#endif
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts         = cpu_clk_freq                                 /* Determine nbr SysTick increments                     */
                   / (CPU_INT32U)OSCfg_TickRate_Hz;

    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

//    Mem_Init();                                                 /* Initialize memory managment module                   */
//    Math_Init();                                                /* Initialize mathematical module                       */


#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif


#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
    App_SerialInit();                                           /* Initialize Serial communication for application ...  */
#endif

    Spi_Flash_Io_Init();
    FLASH_Init();
    ertc();   			//RTC初始化
    reststae = 1;
    printf("System Reset  System Reset  System Reset.....................\r\n");

    //Delay_init();  //AT board自带的一个延时 能单独用 但不能在ucos里用  所以需要下面这个适配操作系统的延迟函数
    delay_init(144);   //  毫秒级延时  delay_m(500);
    Mcu_Io_Init();
    TIM3_Int_Init(19999,14399);   //定时器3初始化，1ms，主要处理串口中断
    Init_HLW8112();		//计量芯片初始化
    TIM4_Int_Init(9,143);   //定时器4  ADC采集 1kHZ  周期=1ms   100点=0.01ms

    AppTaskCreate ();
    /* 获取MCU唯一ID */
    Unique_ID1 = *(uint32_t *)(0x1FFFF7E8);
    Unique_ID1 = SW32(Unique_ID1);
    Unique_ID2 = *(uint32_t *)(0x1FFFF7EC);
    Unique_ID2 = SW32(Unique_ID2);
    Unique_ID3 = *(uint32_t *)(0x1FFFF7F0);
    Unique_ID3 = SW32(Unique_ID3);

    printf("MCU Id:%08x %08x %08x\n",Unique_ID1,Unique_ID2,Unique_ID3);


    memcpy(&OnlyMCUID[0],&Unique_ID1, 4);
    memcpy(&OnlyMCUID[4],&Unique_ID2, 4);
    memcpy(&OnlyMCUID[8],&Unique_ID3, 4);
    for(i = 0; i<12; i++)
    {
        if(OnlyMCUID[i] == 0x00)   //标准比较函数遇到0就结束
        {
            OnlyMCUID[i] = 0xAA;
        }
    }





//    memcpy(&m1_card_info.MCUID[0],&Unique_ID1, 4);
//    memcpy(&m1_card_info.MCUID[4],&Unique_ID2, 4);
//    memcpy(&m1_card_info.MCUID[8],&Unique_ID3, 4);
//    for(i = 0; i<12; i++)
//    {
//        if(m1_card_info.MCUID[i] == 0x00)   //标准比较函数遇到0就结束
//        {
//            m1_card_info.MCUID[i] = 0xAA;
//        }
//    }

#if(doguser)
    IWDG_Config(IWDG_Psc_256 ,625*10);	  //20s   625*5修改成625*3  看门狗
#endif

    while (1)
    {
        IWDG_ReloadCounter();			//喂狗
        task_cnt[0]++;
        OSTimeDly(100, OS_OPT_TIME_PERIODIC, &err);

        ERTC_TimeShow();
        gs_SysTime.ucDay = ERTC_DateStructure.ERTC_Date;
        gs_SysTime.ucYear = ERTC_DateStructure.ERTC_Year + 100;
        gs_SysTime.ucMonth = ERTC_DateStructure.ERTC_Month;
        gs_SysTime.ucHour = ERTC_TimeStructure.ERTC_Hours;
        gs_SysTime.ucMin = ERTC_TimeStructure.ERTC_Minutes;
        gs_SysTime.ucSec = ERTC_TimeStructure.ERTC_Seconds;

        temp = (AppTaskStartTCB.StkUsed * 100)/(AppTaskStartTCB.StkUsed + AppTaskStartTCB.StkFree);
        if(AppTaskStartT < temp)
        {
            AppTaskStartT = temp;
        }
        temp= (AppTaskCardTCB.StkUsed * 100)/(AppTaskCardTCB.StkUsed + AppTaskCardTCB.StkFree);
        if(AppTaskCardT < temp)
        {
            AppTaskCardT = temp;
        }
        temp= (AppTaskChargeTCB.StkUsed * 100)/(AppTaskChargeTCB.StkUsed + AppTaskChargeTCB.StkFree);
        if(AppTaskChargeT < temp)
        {
            AppTaskChargeT = temp;
        }
        temp= (AppTaskDwinTCB.StkUsed * 100)/(AppTaskDwinTCB.StkUsed + AppTaskDwinTCB.StkFree);
        if(AppTaskDwinT < temp)
        {
            AppTaskDwinT = temp;
        }
        temp= (AppTask4GMainTCB.StkUsed * 100)/(AppTask4GMainTCB.StkUsed + AppTask4GMainTCB.StkFree);
        if(AppTask4GMainT < temp)
        {
            AppTask4GMainT = temp;
        }
        temp= (AppTask4GRecvTCB.StkUsed * 100)/(AppTask4GRecvTCB.StkUsed + AppTask4GRecvTCB.StkFree);
        if(AppTask4GRecvT < temp)
        {
            AppTask4GRecvT = temp;
        }
        temp= (AppTask4GSendTCB.StkUsed * 100)/(AppTask4GSendTCB.StkUsed + AppTask4GSendTCB.StkFree);
        if(AppTask4GSendT < temp)
        {
            AppTask4GSendT = temp;
        }
        temp= (AppTaskdlt645TCB.StkUsed * 100)/(AppTaskdlt645TCB.StkUsed + AppTaskdlt645TCB.StkFree);

        if(AppTaskdlt645T < temp)
        {
            AppTaskdlt645T = temp;
        }
    }
}


static  void  AppTaskCreate (void)
{
    OS_ERR      err;


    /*****************卡任务*****************/
    OSTaskCreate((OS_TCB       *)&AppTaskCardTCB,
                 (CPU_CHAR     *)"App Card",
                 (OS_TASK_PTR   )AppTaskCard,
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_CARD_PRIO,
                 (CPU_STK      *)&AppTaskCardStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_CARD_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_CARD_STK_SIZE,
                 (OS_MSG_QTY    )20,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);


    /*****************充电任务*****************/
    OSTaskCreate((OS_TCB       *)&AppTaskChargeTCB,
                 (CPU_CHAR     *)"App Charge",
                 (OS_TASK_PTR   )AppTaskCharge,
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_CHARGE_PRIO,
                 (CPU_STK      *)&AppTaskChargeStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_CHARGE_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_CHARGE_STK_SIZE,
                 (OS_MSG_QTY    )20,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    /*****************显示任务*****************/
    OSTaskCreate((OS_TCB       *)&AppTaskDwinTCB,
                 (CPU_CHAR     *)"App Dwin",
                 (OS_TASK_PTR   )AppTaskDwin,
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_DWIN_PRIO,
                 (CPU_STK      *)&AppTaskDwinStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_DWIN_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_DWIN_STK_SIZE,
                 (OS_MSG_QTY    )20,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);


    /*****************4G主任务*****************/
    OSTaskCreate((OS_TCB       *)&AppTask4GMainTCB,
                 (CPU_CHAR     *)"App 4GMain",
                 (OS_TASK_PTR   )AppTask4GMain,
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_4GMAIN_PRIO,
                 (CPU_STK      *)&AppTask4GMainStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_4GMAIN_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_4GMAIN_STK_SIZE,
                 (OS_MSG_QTY    )20,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);


    /*****************4G接收任务*****************/
    OSTaskCreate((OS_TCB       *)&AppTask4GRecvTCB,
                 (CPU_CHAR     *)"App 4GRecv",
                 (OS_TASK_PTR   )AppTask4GRecv,
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_4GRECV_PRIO,
                 (CPU_STK      *)&AppTask4GRecvStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_4GRECV_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_4GRECV_STK_SIZE,
                 (OS_MSG_QTY    )20,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);


    /*****************4G发送任务*****************/
    OSTaskCreate((OS_TCB       *)&AppTask4GSendTCB,
                 (CPU_CHAR     *)"App 4GSend",
                 (OS_TASK_PTR   )AppTask4GSend,
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_4GSEND_PRIO,
                 (CPU_STK      *)&AppTask4GSendStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_4GSEND_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_4GSEND_STK_SIZE,
                 (OS_MSG_QTY    )20,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    /*****************485串口3任务*****************/
#if(USE_645 ==1)
    OSTaskCreate((OS_TCB       *)&AppTaskdlt645TCB,
                 (CPU_CHAR     *)"App dlt645",
                 (OS_TASK_PTR   )AppTaskdlt645,
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_dlt645_PRIO,
                 (CPU_STK      *)&AppTaskdlt645Stk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_dlt645_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_dlt645_STK_SIZE,
                 (OS_MSG_QTY    )20,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
#endif
}
