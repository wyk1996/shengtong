/*******************************************************************************
 *          Copyright (c) 2020-2050, wanzhuangwulian Co., Ltd.
 *                              All Right Reserved.
 * @file
 * @note
 * @brief
 *
 * @author
 * @date
 * @version
 *
 * @Description 通用定义头文件
 *
 * @note History:
 * @note     <author>   <time>    <version >   <desc>
 * @note
 * @warning
 *******************************************************************************/
#ifndef   __COMMON_H__
#define   __COMMON_H__

#include <stdint.h>
#include "os.h"

#define  MIN(a, b)      (((a) < (b)) ? (a) : (b))
typedef enum
{
    TIME_PERIOD_1 = 0,
    TIME_PERIOD_2,
    TIME_PERIOD_3,
    TIME_PERIOD_4,
    TIME_PERIOD_5,
    TIME_PERIOD_6,
    TIME_PERIOD_7,
    TIME_PERIOD_8,
    TIME_PERIOD_9,
    TIME_PERIOD_10,

    TIME_PERIOD_11,
    TIME_PERIOD_12,
    TIME_PERIOD_13,
    TIME_PERIOD_14,
    TIME_PERIOD_15,
    TIME_PERIOD_16,
    TIME_PERIOD_17,
    TIME_PERIOD_18,
    TIME_PERIOD_19,
    TIME_PERIOD_20,

    TIME_PERIOD_21,
    TIME_PERIOD_22,
    TIME_PERIOD_23,
    TIME_PERIOD_24,
    TIME_PERIOD_25,
    TIME_PERIOD_26,
    TIME_PERIOD_27,
    TIME_PERIOD_28,
    TIME_PERIOD_29,
    TIME_PERIOD_30,

    TIME_PERIOD_31,
    TIME_PERIOD_32,
    TIME_PERIOD_33,
    TIME_PERIOD_34,
    TIME_PERIOD_35,
    TIME_PERIOD_36,
    TIME_PERIOD_37,
    TIME_PERIOD_38,
    TIME_PERIOD_39,
    TIME_PERIOD_40,

    TIME_PERIOD_41,
    TIME_PERIOD_42,
    TIME_PERIOD_43,
    TIME_PERIOD_44,
    TIME_PERIOD_45,
    TIME_PERIOD_46,
    TIME_PERIOD_47,
    TIME_PERIOD_48,
    TIME_PERIOD_MAX,
} _TIME_PERIOD;


typedef enum
{
    TIME_QUANTUM_J = 0,	//尖
    TIME_QUANTUM_F,		//峰
    TIME_QUANTUM_P,		//平
    TIME_QUANTUM_G,		//古
    TIME_QPERIOD_5,
    TIME_QPERIOD_6,
    TIME_QPERIOD_7,
    TIME_QPERIOD_8,
    TIME_QPERIOD_9,
    TIME_QPERIOD_10,
    TIME_QPERIOD_11,
    TIME_QPERIOD_12,
    TIME_QUANTUM_MAX,
} _TIME_QUANTUM;

typedef enum
{
    GUN_A = 0,
    GUN_B,
    GUN_UNDEFIN = 0X55,
} _GUN_NUM;

#define GUN_MAX  1
#define   COMMON_INVALID_FLAG   0
#define   COMMON_VALID_FLAG     1

typedef enum
{
    FLASH_ORDER_READ = 0x01, //0x01:读操作
    FLASH_ORDER_WRITE,       //0x02:写操作
} _FLASH_ORDER;


/* 操作系统 tick 是 1ms */
#define CM_TIME_5_MSEC         5
#define CM_TIME_10_MSEC         10
#define CM_TIME_20_MSEC         20
#define CM_TIME_30_MSEC         30
#define CM_TIME_50_MSEC         50
#define CM_TIME_100_MSEC        100ul
#define CM_TIME_250_MSEC        250ul
#define CM_TIME_500_MSEC        500ul
#define CM_TIME_1_SEC           1000ul
#define CM_TIME_2_SEC           2000ul
#define CM_TIME_3_SEC           3000ul
#define CM_TIME_5_SEC           (5  * CM_TIME_1_SEC)
#define CM_TIME_10_SEC          (10 * CM_TIME_1_SEC)
#define CM_TIME_15_SEC          (15 * CM_TIME_1_SEC)
#define CM_TIME_20_SEC          (20 * CM_TIME_1_SEC)
#define CM_TIME_30_SEC          (30 * CM_TIME_1_SEC)
#define CM_TIME_60_SEC          (60 * CM_TIME_1_SEC)
#define CM_TIME_90_SEC          (90 * CM_TIME_1_SEC)

#define CM_TIME_5_MIN           (5 * CM_TIME_60_SEC)
#define CM_TIME_10_MIN           (10 * CM_TIME_60_SEC)
#define CM_TIME_15_MIN          (15 * CM_TIME_60_SEC)
/* 充电任务模块ID */
#define  CM_CHTASK_MODULE_ID     0x01

/* 万桩GPRS模块ID */
#define  CM_WZGPRS_MODULE_ID     0x02

/* TCU任务模块ID */
#define  CM_TCUTASK_MODULE_ID    0x03

/* CARD任务模块ID */
#define  CM_CARD_MODULE_ID       0x04

/* DISP任务模块ID */
#define  CM_DISP_MODULE_ID       0x05

/* 迪文屏幕接收任务模块ID */
#define  CM_DWINRECV_MODULE_ID       0x06

/* 4G接收任务模块ID */
#define  CM_4GRECV_MODULE_ID       0x07

/*4G发送任务模块ID*/
#define  CM_4GSEND_MODULE_ID       0x08

/*4G主任务模块ID*/
#define  CM_4GMAIN_MODULE_ID       0x09

/*4G主任务模块ID*/
#define  CM_4GUARTRECV_MODULE_ID    0x0A

/*485接收*/
#define  CM_485USRTRECV_MODULE_ID    0x0B
#define  CM_UNDEFINE_ID       		0xaa  //无需关心源
/* 获取两个数据之间的较小值 */
#define  CM_DATA_GET_MIN(a,b)  (((a) <= (b)) ? (a) : (b))

/* 获取两个数据之间的较大值 */
#define  CM_DATA_GET_MAX(a,b)  (((a) >= (b)) ? (a) : (b))

typedef struct
{
    /* 消息源头的功能模块ID */
    uint32_t     uiSrcMoudleId;
    /* 消息目标的功能模块ID */
    uint32_t     uiDestMoudleId;
    /* 消息码 */
    uint32_t     uiMsgCode;
    /* 消息特殊参数 */
    uint32_t     uiMsgVar;
    /* 消息有效载荷的长度 */
    uint32_t     uiLoadLen;
    /* 指向有效消息的缓冲区 */
    uint8_t      *pucLoad;
} MQ_MSG_T;

enum
{
    WZGPRS_TO_TCU_NETSTATE,         /* 网络状态 */
    WZGPRS_TO_TCU_IMEI,             /* imei */
    WZGPRS_TO_TCU_IMSI,             /* imsi */
    WZGPRS_TO_TCU_CSQ,              /* CSQ */
    WZGPRS_TO_TCU_DATA,             /* 万桩GPRS模块发送数据给TCU模块 */
    TCU_TO_WZGPRS_DATA,             /* TCU需要发送数据给模块 */
    TCU_TO_WZGPRS_FILEDOWN,	        /* 文件下载格式 */
};


enum
{
    CH_TO_TCU_REALTIMEDATA,        /* 实时数据 */
    CH_TO_TCU_CHCTL_ACK,           /* 充电控制应答 */
    CH_TO_TCU_RECORD,              /* 充电记录 */
    TCU_TO_CH_CHCLT,               /* 充电控制 */
    CARD_TO_TCU_INFO,              /* 卡信息 */
} ;

enum
{
    CH_TO_DIP_STARTSUCCESS,     		/* 启动成功 */
    CH_TO_DIP_STARTFAIL,              	/* 启动失败 */
    CH_TO_DIP_STOP,              		/* 停止充电 */
} ;

enum
{
    WZGPRS_TO_HTTP_START_ERASE ,    /* 启动擦除 */
    WZGPRS_TO_HTTP_DOWN_FILE       /* 下载文件 */
} ;

//4g部分
#define  	APP_START_ACK		0	//开始充电应答
#define  	APP_ATIV_STOP		1	//主动停止充电
#define  	APP_STOP_ACK		3	//停止充电应答
#define  	APP_STOP_BILL		17	//停止结算
#define 	APP_RATE_ACK		5   //费率设置应答
#define 	APP_STE_DEVS		6	//查询设备状态
#define 	APP_STE_BILL		7	//查询记录
#define 	APP_STE_RATE		8	//查询费率
#define 	APP_STE_TIME		9	//校准时间
#define     APP_CARDVIN_CHARGE	10	//刷卡vin启动
#define     APP_CARD_INFO		11	//卡鉴权
#define     APP_VIN_INFO		12	//Vin鉴权
#define		APP_CARD_WL			13	//卡白名单
#define		APP_VIN_WL			14	//VIN白名单
#define		APP_VINCARD_RES		15	//白名单清空
#define		APP_OFFLINE_ACK		16	//离线应答
#define     APP_SJDATA_QUERY	18  //查询实时数据
#define		APP_QUERY_RATE		19	//查询费率
#define		APP_RATE_MODE		20	//计费模型
#define    APP_UPDADA_BALANCE   21  //更新余额











typedef enum
{
    INPUT_YX1_DOOR = 0,
    INPUT_YX2,
    INPUT_YX3_EM,			//急停
    INPUT_YX_MAX,
} _IO_INPINT_STATE;
uint8_t mq_service_bind(uint32_t uiModuleId,const char *sMqName);
uint8_t mq_service_recv_msg(OS_Q * pq, MQ_MSG_T *pstMsg, uint8_t *pucMsgBuf, uint32_t uiBufSize, uint32_t rtTimeout);
uint8_t mq_service_xxx_send_msg_to_chtask(uint32_t uiSrcMoudleId, uint32_t uiMsgCode,uint32_t uiMsgVar, uint32_t uiLoadLen, uint8_t *pucLoad); 										  ;
uint8_t mq_service_card_send_disp(uint32_t  uiMsgCode ,uint32_t uiMsgVar ,uint32_t uiLoadLen ,uint8_t  *pucLoad);
uint8_t mq_service_dwinrecv_send_disp(uint32_t  uiMsgCode ,uint32_t uiMsgVar ,uint32_t uiLoadLen ,uint8_t  *pucLoad);
uint8_t mq_service_ch_send_dip(uint32_t  uiMsgCode ,uint32_t uiMsgVar ,uint32_t uiLoadLen ,uint8_t  *pucLoad);
uint8_t mq_service_send_to_4gsend(uint32_t  uiMsgCode ,uint32_t uiMsgVar ,uint32_t uiLoadLen ,uint8_t  *pucLoad);

uint8_t mq_service_4GUart_send_recv(uint32_t  uiMsgCode ,uint32_t uiMsgVar ,uint32_t uiLoadLen ,uint8_t  *pucLoad);
uint8_t mq_service_485Uart_send_recv(uint32_t  uiMsgCode ,uint32_t uiMsgVar ,uint32_t uiLoadLen ,uint8_t  *pucLoad);
#endif


