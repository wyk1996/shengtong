#ifndef __DWIN_COM_PRO_H
#define __DWIN_COM_PRO_H

#include <stdint.h>
#include "common.h"
#include "DispShowStatus.h"
#include "DispKey.h"
#include "app_cfg.h"


//版本号：13A30
//第一位：单/双/三相     1=单枪   2=双枪   3=3相
//第二位：年份3   2023=3     2024=4
//第三位：月份   3=3月    A=10月  （16进制）
//第四位和第五位：日期   21=21号   30=30号


#define DISP_NET    	1   //网络版本 
#define DISP_CARD       2  //刷卡正常版本
#define DISP_CARD_mode  3  //刷卡预约版本


#define Normal_mode    	  1   //刷卡单机正常模式 
#define Reservation_mode  2	  //刷卡单机预约模式 

#define DWIN_MAX_READ_LEN 200  //读数据的最大数据长度
#define DWIN_MAX_WRITE_LEN 50  //写数据的最大数据长度

// port setting
#define DWIN_DEFAULT_RESPONSE_TIMEOUT 500   //500ms
#define MAX_DEVICE_NAME_LEN 10              //最大设备名长度

#define RECODE_DISPOSE1(a)       	((a) == (0) ? (1000): (a))


typedef struct siji
{
    uint8_t NUM; //条数
    uint32_t addr[50]; //地址
} _siji_t;


extern _siji_t sijit123;



//DWIN 环境结构体
typedef struct dwin
{
    uint8_t addr[6];    //从机地址
    uint8_t debug;      //调试标志
    int (*write)(uint8_t *buf, uint16_t len);     //底层写函数
    int (*read) (uint8_t *msg, uint16_t len);     //底层读函数
    void *port_data;                                            //移植层拓展接口
} dwin_t;


typedef enum {
    DIP_STATE_NORMAL = 0,
    DIP_STATE_SYS,
    DIP_CARD_SHOW
} _DIS_SYS_STATE;
typedef struct
{
    uint16_t CountDown;						//记录倒计时时间
    _DIS_SYS_STATE CurSysState;						//0 非配置状态   1配置状态	2系统升级
} _DISP_CONTROL; //显示控制结构体


//枚举
__packed typedef struct
{
//	uint32_t			uiSharpTimePrices[4];										//尖时段电价,精确到五位小数.如果某一费率值为0，那么费率段中就不会有该费率段序号
//	uint32_t			uiPeakTimePrices;										//峰时段电价
//	uint32_t			uiFlatTimePrices;										//平时段电价
//	uint32_t			uiValleyTimePrices;										//谷时段电价
//	uint32_t			uifwSharpTimePrices;									//服务费尖时段电价,精确到五位小数.如果某一费率值为0，那么费率段中就不会有该费率段序号
//	uint32_t			uifwPeakTimePrices;										//服务费服务费峰时段电价
//	uint32_t			uifwFlatTimePrices;										//服务费平时段电价
//	uint32_t			uifwValleyTimePrices;									//服务费谷时段电价
    uint32_t			Prices[4];												//尖峰平谷电价
    uint32_t			fwPrices[4];											//尖峰平谷服务费
    uint8_t				ucSegNum[48];											//时段费率号.每半小时一段，共48段.0：尖费率 1：峰费率 2：平费率 3：谷费率

    //安培云快充分段计费使用
    uint8_t TimeQuantumNum;			//时间段个数
} RATE_T;

//显示界面系统配置信息
__packed typedef struct
{
    uint8_t	DevNum[16];				//桩编号
    uint8_t IP[4];			    //IP
    uint16_t Port;		    //端口
    uint8_t NetNum;			   //网络个数
    uint8_t GunNum;			   //充电枪个数
    uint8_t standaloneornet; //单机或者网络
    uint8_t energymeter; //1板内电表     0外部电表
    uint16_t cardtype;  //用于存储发卡，选择卡类型
    uint32_t admincode2;  //存储管理员二级密码
    uint32_t Companycode; //公司代码（区分某一个小区）
    uint8_t XYSelece; //存储协议选择的第几个（用于判断是不是第一次烧程序）
} _DIS_SYS_CONFIG_INFO;


//记录查询页面测试
typedef struct
{
    //=======hycsh start
    uint32_t CardNum; //卡号4字节
    uint8_t Gunnum[10];	//接口号，A或B枪
    uint32_t Record_balance;   //扣款后卡内余额
    //=======hycsh end

    uint32_t  chargepwer;			//充电电量 0.01
    uint32_t  chargemoney;			//消费金额 0.01元
    uint8_t	SerialNum[20];			//交易流水号
    uint8_t	  BillStopRes[10];		//停止原因
    uint8_t	  StartTime[20];		//开始充电时间
    uint8_t	  StopTime[20];			//结束充电时间
    uint8_t   ChargeType[10];		//充电方式
    uint8_t   BillState[10];		//结算状态
    uint32_t  After_balance;  //消费后卡内余额（必须写在后面这个地方，因为兼容之前旧版本信息）
} _RECORD_INFO;


//======================hycsh显示记录信息30，31页面对应的结构体[1]
typedef struct
{
    uint32_t Record_balance;   //扣款后卡内余额
    uint8_t  Record_BillState[10];  //结算状态
    uint8_t	 Record_SerialNum[20]; //交易流水号
    uint32_t Record_chargepwer; //充电电量
    uint8_t	 Record_BillStopRes[10]; //停止原因

    uint32_t CardNum; //卡号4字节
    uint8_t  Record_ChargeType[10];	//充电方式
    uint8_t Gunnum[10];	//接口号，A或B枪
    uint32_t Record_chargemoney; //消费金额
    uint32_t After_balance;   //消费后卡内余额
    uint8_t	 Record_StartTime[20];		//开始充电时间
    uint8_t	 Record_StopTime[20];			//结束充电时间
} _Record_displayinfo;


typedef struct
{
    uint32_t RecodeCurNum;			   //交易记录条数
    uint16_t CurReadRecodeNun;		//当前读到的交易记录的偏移条数（查询使用）
    uint16_t NextReadRecodeNun;		//下一条读到的交易记录的偏移条数（查询使用）
    uint16_t UpReadRecodeNun;			//上一条读到的交易记录的偏移条数（查询使用）
    uint16_t CurNun;					   //当前记录页码
    _RECORD_INFO CurRecode;	//存放当前记录
    _RECORD_INFO NextRecode;	//存放下一条记录
    _RECORD_INFO UpRecode;	//存放上一条记录
} _RECODE_CONTROL;

extern _RECODE_CONTROL RECODECONTROL;
extern _Record_displayinfo Recorddisplay_info;
extern _Record_displayinfo contrast_CardNum;
extern _RECORD_INFO	RecordInfo;		//显示交易记录信息
extern _RECORD_INFO  SaveRecordinfo; //hycsh存储交易记录




extern dwin_t dwin;
extern _DISP_CONTROL DispControl; 	//显示控制结构体
extern _DIS_SYS_CONFIG_INFO DisSysConfigInfo;	//显示界面系统配置信息


//对外提供环境声明

extern void send_ch_ctl_msg(uint8_t ucCtl,uint8_t ucChStartMode,uint8_t ucChMode,uint32_t uiStopParam);
extern void send_update_rate(char *pstRate);
uint8_t Munu27_ShowSysInfo(uint8_t entrn);


uint8_t WriterFmBill(_GUN_NUM gun,uint8_t billflag);
uint8_t ReadFmBill(void);
uint8_t APP_GetNetOFFLineRecodeNum(void);
uint8_t APP_SetNetOFFLineRecodeNum(uint8_t num);
uint8_t APP_RWNetOFFLineRecode(uint16_t count,_FLASH_ORDER RWChoose,uint8_t  * pdata);
uint8_t APP_RWCardWl(_FLASH_ORDER RWChoose,uint8_t  * pdata,uint16_t len);
uint8_t APP_RWNetFSOFFLineRecode(uint16_t count,_FLASH_ORDER RWChoose,uint8_t  * pdata);
uint8_t *APP_GetBillInfo(_GUN_NUM gun);

uint8_t Munu13_ShowSysInfo(void);
uint8_t Disp_Showsettime(ST_Menu *pMenu);
uint8_t APP_SelectCurChargeRecode(void);
uint32_t  APP_GetRecodeCurNum(void);
uint8_t APP_SelectNextNChargeRecode(uint8_t num);
uint8_t APP_SelectUpNChargeRecode(uint8_t num);
uint8_t APP_ClearRecodeInfo(void);


uint8_t Recordqueryinfo_WR(uint16_t count,_FLASH_ORDER RWChoose,uint8_t * pdata);
//static uint8_t DispShow_Recode(_RECORD_INFO * precode,ST_Menu *pMenu);
uint8_t APP_SelectCurChargeRecode(void);
uint8_t APP_SelectNextNChargeRecode(uint8_t num);
uint8_t APP_SelectUpNChargeRecode(uint8_t num);

uint8_t  APPTransactionrecord(STOP_REASON Stop_reason,_SHOW_NUM Charg_mode,_SHOW_NUM GUNnum,uint32_t CurNum);
//uint8_t  APPTransactionrecord(_SHOW_NUM Stop_reason,_SHOW_NUM Charg_mode,_SHOW_NUM GUNnum,uint32_t CurNum);//存储记录数据
uint8_t  Unlock_settlementrecord(_SHOW_NUM BILL_status,uint32_t CurNum);//查找覆盖
uint8_t  Unlock_Transactionrecord(_SHOW_NUM BILL_status,uint32_t CurNum);
uint8_t StoreRecodeCurNum(void);//存储当前的条数
uint8_t Clear_record(void);//清除当前的记录条数
extern uint8_t  show_Notcalculated(void); //停止时，未结算页面,停留到结算界面
extern uint8_t Clear_flag(void);

#if(WLCARD_STATE)
uint8_t* APP_GetCARDWL(void);
#endif

uint8_t Set_judge_rete_info(uint32_t price);

uint8_t Auto_charging(void);
#endif
