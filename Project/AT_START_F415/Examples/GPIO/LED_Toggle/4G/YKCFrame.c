/*****************************************Copyright(C)******************************************
*******************************************杭州汇誉*********************************************
*---------------------------------------------------------------------------------------
* FileName			: GPRSMain.c
* Author			:
* Date First Issued	:
* Version			:
* Description		:
*----------------------------------------历史版本信息-------------------------------------------
* History			:
* //2010		    : V
* Description		:
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/
/* Includes-----------------------------------------------------------------------------------*/
#include "4GMain.h"
#include "4GRecv.h"
#include <string.h>
#include "dwin_com_pro.h"
#include "chtask.h"
#include "flashdispos.h"
#include "dlt645_port.h"
#include "main.h"
/* Private define-----------------------------------------------------------------------------*/
#if NET_YX_SELCT == XY_YKC
#define YKC_SEND_FRAME_LEN   3
#define YKC_RECV_FRAME_LEN	12
_4G_START_TYPE YKCStartType[GUN_MAX] = { _4G_APP_START};					//YKCC 0表示App启动 1表示刷卡启动
extern _m1_card_info m1_card_info;		 //M1卡相关信息
uint8_t checkYKConline = 0;  //用请求费率的方式来检测YKC是否在线。  0= 初始化   1=可以在空闲状态下正常的请求   2=发送过第一次了


#define YKC_SEND_CMD_01   		0x01			//充电桩主动注册信息
#define YKC_RECV_CMD_02 		0x02			//pc返回确认信息

#define YKC_SEND_CMD_03   		0x03			//心跳发送
#define YKC_RECV_CMD_04 		0x04			//心跳应答

#define YKC_SEND_CMD_05   		0x05			//计费模型验证请求
#define YKC_RECV_CMD_06 		0x06			//计费模型验证请求应答


#define YKC_SEND_CMD_09   		0x09			//计费模型请求
#define YKC_RECV_CMD_0A 		0x0A			//计费模型请求应答

#define YKC_SEND_CMD_31   		0x31			//发送卡鉴权
#define YKC_RECV_CMD_32 		0x32			//平台确认  下发启动

#define YKC_SEND_CMD_33   		0x33			//启动充电应答
#define YKC_RECV_CMD_34 		0x34			//启动充电

#define YKC_SEND_CMD_35   		0x35			//停止充电应答
#define YKC_RECV_CMD_36 		0x36			//停止充电

#define YKC_SEND_CMD_3B 		0x3B			//上传交易记录

#define YKC_SEND_CMD_13   		0x13			//上传实时监测数据
#define YKC_RECV_CMD_12 		0x12			//读取试试监控数据

#define YKC_SEND_CMD_55   		0x55			//对时应答
#define YKC_RECV_CMD_56 		0x56			//对时

#define YKC_SEND_CMD_57   		0x57			//费率应答
#define YKC_RECV_CMD_58 		0x58			//费率设置

#define YKC_SEND_CMD_41   		0x41			//余额更新
#define YKC_RECV_CMD_42 		0x42			//余额更新应答


#define YKC_SEND_CMD_15   		0x15			//充电握手
#define YKC_SEND_CMD_17   		0x17			//参数配置
#define YKC_SEND_CMD_19 		0x19			//结束充电
#define YKC_SEND_CMD_1B			0x1B			//错误报文
#define YKC_SEND_CMD_1D			0x1D			//充电阶段BMS中止
#define YKC_SEND_CMD_21			0x21			//充电阶段充电机中止
#define YKC_SEND_CMD_23			0x23			//充电过程BMS需求与充电机输出
#define YKC_SEND_CMD_25			0x25			//充电过程BMS信息

#define YKC_RECV_CMD_40 		0x40			//交易记录确认




uint8_t YCKDevNum[7] = {0};     //云快充桩编号位7个字节


//注册帧
__packed typedef struct
{
    uint8_t DevNun[7];    //BCD  不足7位补充0
    uint8_t Devtype;		//0 表示直流桩，1 表示交流桩
    uint8_t GunNum;		//总共枪数量
    uint8_t Vsion;		//通信版本
    uint8_t ProVersions[8];	//程序版本
    uint8_t NetState;		//0 SIM卡  1 LAN     2 WAN   3其他
    uint8_t SIM[10];		//SIM卡号
    uint8_t Operator;     //运营商 0x00 移动0x02 电信0x03 联通0x04 其他
} _YKC_SEND_CMD01;

//心跳
__packed typedef struct
{
    uint8_t DevNun[7];    //BCD  不足7位补充0
    uint8_t GunNum;		//枪号，从1开始
    uint8_t GunState;		//枪状态
} _YKC_SEND_CMD03;


//发送卡鉴权
__packed typedef struct
{
    uint8_t DevNun[7];     //BCD  不足7位补充0
    uint8_t GunNum;		    //枪号，从1开始
    uint8_t Startmode;		//启动方式 0x01 表示通过刷卡启动充电  0x03 表示vin码启动充电
    uint8_t password;    //是否需要密码 0x00 不需要 0x01 需要
    uint8_t card_NUM[8];  //账号或者物理卡号  不足8位补 0，具体见示例
    uint8_t Input_password[16];	//输入密码  对用户输入的密码进行16位MD5加密，采用小写上传
    uint8_t VIN[17];		//启动方式为vin码启动充电时上送,其他方式置零( ASCII码)，VIN码需要反序上送
} _YKC_SEND_CMD31;


//启动应答
__packed typedef struct
{
    uint8_t SerialNum[16];	//交易流水号
    uint8_t DevNun[7];    //BCD  不足7位补充0
    uint8_t GunNum;		//枪号，从1开始
    uint8_t StartState;		//启动是否成功 0失败  1成功
    uint8_t FailReason;	//停止原因  0x00无    0x01设备编号不匹配    0x02枪已在充电   0x03设备故障     0x04设备离线     0x05未插枪
} _YKC_SEND_CMD33;


//发送实时数据   周期上传。待机5分钟 充电15秒
__packed typedef struct
{
    uint8_t SerialNum[16];	//交易流水号
    uint8_t DevNun[7];    //BCD  不足7位补充0
    uint8_t GunNum;		//枪号，从1开始
    uint8_t State;		//状态  0x00：离线 0x01：故障 0x02：空闲 0x03：充电
    uint8_t GunReturn;	//枪是否归位   0没归位  1是  2未知
    uint8_t GunState;		//是否插枪		0未插枪  1插枪  需做到变位上送
    uint16_t OutVol;		//输出电压  0.1
    uint16_t OutCur;		//输出电流	0.1
    uint8_t GunT;			//枪线维度	偏移 -50
    uint8_t GunNb[8];	     //枪线编码	没有置0
    uint8_t Soc;
    uint8_t BatteryT;		//电池组最高温度
    uint16_t ChargeTime;	//充电时间  min
    uint16_t RTime;	//剩余充电时间  min
    uint32_t ChargeKwh;	//充电读数   精确到小数点4位
    uint32_t res;			//计损充电读数
    uint32_t ChargeMoney;	//已充金额	精确到小数点4位
//Bit1：急停按钮动作故障；Bit2：无可用整流模块；Bit3：出风口温度过高；Bit4：交流防雷故障；Bit5：交直流模块 DC20 通信中断；Bit6：绝缘检测模块 FC08 通信中断；
//Bit7：电度表通信中断；Bit8：读卡器通信中断；Bit9：RC10 通信中断；Bit10：风扇调速板故障；Bit11：直流熔断器故障；Bit12：高压接触器故障；Bit13：门打开；
    uint8_t Fail[2];
} _YKC_SEND_CMD13;

//充电握手  主动上报一次  BRM
__packed typedef struct
{
    uint8_t SerialNum[16];	//交易流水号
    uint8_t DevNun[7];    //BCD  不足7位补充0
    uint8_t GunNum;		//枪号，从1开始

    uint8_t  BMSVersion[3];        //BMS通信协议版本
    uint8_t  BatteryType;          //电池类型
    uint16_t RatedCapacity;        //整车动力蓄电池系统额定容量 /Ah;0.1Ah/位
    uint16_t RatedVol;             //整车动力蓄电池系统额定电压/V;0.1V/
    uint8_t  BatteryFactory[4];    //电池生产厂商名称
    uint8_t  BatterySerialNum[4];  //电池组序号
    uint8_t  BatteryProduceYear;   //电池组生产年1年/bit  1985--2235
    uint8_t  BatteryProduceMonth;  //电池组生产月1月/bit
    uint8_t  BatteryProduceDay;    //电池组生产日1日/bit
    uint8_t  BatteryChargedFreq[3];//电池组充电次数
    uint8_t  BatteryRightFlag;     //电池组产权标示
    uint8_t  defaultbyte;          //预留字节
    uint8_t  VIN[17];              //车辆识别码
    uint8_t  BMSSoftwareVer[8];    //BMS软件版本号（新国标内容）
} _YKC_SEND_CMD15;


//参数配置，主动上报一次  BCP
__packed typedef struct
{
    uint8_t SerialNum[16];	//交易流水号
    uint8_t DevNun[7];    //BCD  不足7位补充0
    uint8_t GunNum;		//枪号，从1开始

    uint16_t UnitBatteryMaxVol;    //单体动力蓄电池最高允许充电电压
    uint16_t MaxCurrent;           //最高允许充电电流
    uint16_t BatteryWholeEnergy;   //动力蓄电池标称总能量
    uint16_t MaxVoltage;           //最高允许充电总电压
    uint8_t  MaxTemprature;        //最高允许温度
    uint16_t BatterySOC;           //整车蓄电池荷电状态:SOC
    uint16_t BatteryPresentVol;    //整车动力蓄电池当前电池电压,0.1V/位
    uint16_t DevHVol;				//0.1设备最高电压
    uint16_t DevLVol;				//0.1设备最低电压
    uint16_t DevHCur;				//0.1最高电流  	-400A
    uint16_t DevLCur;				//0.1最低电流	-400A
} _YKC_SEND_CMD17;


//结束充电，主动上报一次 BSD
__packed typedef struct
{
    uint8_t SerialNum[16];	//交易流水号
    uint8_t DevNun[7];    //BCD  不足7位补充0
    uint8_t GunNum;		//枪号，从1开始

    uint8_t   EndChargeSOC;          //中止荷电状态SOC%
    uint16_t  UnitBatteryminVol;     //动力蓄电池单体最低电压
    uint16_t  UnitBatteryMaxVol;     //动力蓄电池单体最高电压
    uint8_t   BatteryMinTemp;        //动力蓄电池最低温度
    uint8_t   BatteryMaxTemp;        //动力蓄电池最高温度

    uint16_t  ChargeTime;				//充电时间 min
    uint16_t	ChargeKwh;				//充电输出能量 0.1
    uint8_t 	DevNum[4];				//电桩充电机编号
} _YKC_SEND_CMD19;


//错误报文  BEM CEM 只上报一次
__packed typedef struct
{
    uint8_t SerialNum[16];	//交易流水号
    uint8_t DevNun[7];    //BCD  不足7位补充0
    uint8_t GunNum;		//枪号，从1开始
    uint8_t BEM[4];
    uint8_t CEM[4];
} _YKC_SEND_CMD1B;

//BMS 终止  主动上报  BST
__packed typedef struct
{
    uint8_t SerialNum[16];	//交易流水号
    uint8_t DevNun[7];    //BCD  不足7位补充0
    uint8_t GunNum;		//枪号，从1开始
    uint8_t BST[4];
} _YKC_SEND_CMD1D;

//用户 终止  主动上报  CST
__packed typedef struct
{
    uint8_t SerialNum[16];	//交易流水号
    uint8_t DevNun[7];    //BCD  不足7位补充0
    uint8_t GunNum;		//枪号，从1开始
    uint8_t CST[4];
} _YKC_SEND_CMD21;


//15S上报一次
__packed typedef struct
{
    uint8_t SerialNum[16];	//交易流水号
    uint8_t DevNun[7];    //BCD  不足7位补充0
    uint8_t GunNum;		//枪号，从1开始

    //_BMS_BCL_CONTEXT BCL;
    uint16_t DemandVol;      //电压需求V
    uint16_t DemandCur;      //电流需求A
    uint8_t  ChargeMode;     //充电模式  01：恒压充电   02：恒流充电


    //_BMS_BCS_CONTEXT BCS;
    uint16_t ChargeVolMeasureVal;      //充电电压测量值
    uint16_t ChargeCurMeasureVal;      //充电电流测量值
    uint16_t MaxUnitVolandNum;         //最高单体动力蓄电池电压以及组号
    uint8_t  SOC;                      //当前荷电状态%
    uint16_t RemainderTime;            //剩余充电时间min

    uint16_t OutVol;		//输出电压   0.1
    uint16_t OutCur;		//输出电流  0.1 -400A偏移量
    uint16_t ChargeTime;	//累计充电时间
} _YKC_SEND_CMD23;

//15S上报一次  BSM
__packed typedef struct
{
    uint8_t SerialNum[16];	//交易流水号
    uint8_t DevNun[7];    //BCD  不足7位补充0
    uint8_t GunNum;		//枪号，从1开始

    //_BMS_BSM_CONTEXT BSM;
    uint8_t BSM[7];

} _YKC_SEND_CMD25;

//停止充电应答
__packed typedef struct
{
    uint8_t DevNun[7];    //BCD  不足7位补充0
    uint8_t GunNum;		//枪号，从1开始

    uint8_t StopResul;	//停止结果  0失败 1成功
    uint8_t Fail;			//失败原因    0x00 无
} _YKC_SEND_CMD35;


__packed typedef struct {
    //CP56格式
    uint16_t Second;  //0.001秒
    uint8_t	Minute;
    uint8_t	Hour;
    uint8_t	Day;
    uint8_t	Month;
    uint8_t	Year;
} YCK_TIME;

__packed typedef struct {
    uint32_t Pric;			//精确到小数点5位
    uint32_t PowerKwh;		//电量 精确到小数点4位
    uint32_t JSKwh;			//计损电量 精确到小数点4位
    uint32_t Money;			//尖金额 精确到小数点4位
} _BILL_FS_INFO;

//发送交易记录
__packed typedef struct
{
    uint8_t SerialNum[16];	//交易流水号
    uint8_t DevNun[7];    //BCD  不足7位补充0
    uint8_t GunNum;		//枪号，从1开始

    YCK_TIME	StartTime;				//开始时间  CP56Time2a格式
    YCK_TIME StopTime;				//结束时间  CP56Time2a格式

    _BILL_FS_INFO BillFsInfo[4];	//分时电量信息

    uint32_t 		ChargeStartKwh; 		//总起示值  精确到小数点后4位
    uint8_t      	ChargeStartKwhRes;		//因为是5字节补充

    uint32_t 		ChargeStopKwh; 			//总止示值  精确到小数点后4位
    uint8_t      	ChargeStopKwhRes;		//因为是5字节补充

    uint32_t 		ChargeAllKwh; 			//总电量  精确到小数点后4位
    uint32_t		JSAllKwh;				//计损总电量  精确到小数点后4位
    uint32_t 		ChargeMoney;			//充电总金额 电费+服务费  精确到小数点后4位
    uint8_t		CarVin[17];				//电动汽车唯一标识
    uint8_t		ChargeType;				// 1 app启动  2卡启动   3离线卡启动			5VIN码启动
    YCK_TIME 		JYTime;				//交易时间
    uint8_t StopReason;					//停止原因
    uint8_t CardNum[8];					//物理卡号
} _YKC_SEND_CMD3B;

__packed typedef struct {
    uint8_t DevNun[7];   	 	//BCD  不足7位补充0
    YCK_TIME	CurTime;
} _YKC_SEND_CMD55;  //校时应答


//下发费率 cmd = 0x0a
__packed typedef struct {
    uint32_t 	Fric;			//费率     5位小数
    uint32_t	ServiceFric;	//服务费	5位小数
} _PRC_INFO;
__packed typedef struct {
    uint8_t DevNun[7];   	 	//BCD  不足7位补充0
    uint8_t PrcNum[2];		//计费模型编号  固定值： 01 00
    _PRC_INFO PrcInfo[4];	//尖峰平古费率服务费
    uint8_t Calculate;		//计损比例 目前全部为0
    uint8_t			FricNum[48];	//费率号
} _YKC_RECV_CMD0A;


//==启动充电鉴权结果
__packed typedef struct {
    uint8_t SerialNum[16];	//交易流水号
    uint8_t DevNun[7];   	 	//BCD  不足7位补充0
    uint8_t GunNum;			//枪号 从1开始
    uint8_t  CardNum1[8];		//逻辑卡号
    uint32_t Money;			//余额  保留2位小数
    uint8_t Success_flag;  //0x00 失败 0x01 成功
    uint8_t fail;		  //失败原因  0x01 账户不存在  0x02 账户冻结  0x03 账户余额不足  0x04 该卡存在未结账记录  0x05 桩停用
} _YKC_RECV_CMD32;

__packed typedef struct {
    uint8_t SerialNum[16];	//交易流水号
    uint8_t DevNun[7];   	 	//BCD  不足7位补充0
    uint8_t GunNum;			//枪号 从1开始
    uint8_t CardNum1[8];		//逻辑卡号
    uint8_t CardNum2[8];		//物理卡号
    uint32_t Money;			//余额  保留2位小数
} _YKC_RECV_CMD34;


__packed typedef struct {
    uint8_t DevNun[7];   	 	//BCD  不足7位补充0
    YCK_TIME	CurTime;
} _YKC_RECV_CMD56;


__packed typedef struct {
    uint8_t DevNun[7];   	 	//BCD  不足7位补充0
    uint8_t PrcNum[2];		//计费模型编号  固定值： 01 00
    _PRC_INFO PrcInfo[4];	//尖峰平古费率服务费
    uint8_t Calculate;		//计损比例 目前全部为0
    uint8_t			FricNum[48];	//费率号
} _YKC_RECV_CMD58;


__packed typedef struct {
    uint8_t DevNun[7];   	 	//BCD  不足7位补充0
    uint8_t GunNum;		//枪号，从1开始
    uint8_t  CardNum[8];	//卡号
    uint32_t Balance;		//余额 保留2位小数
} _YKC_RECV_CMD42;

/*extern _PRICE_SET PriceSet;							//电价设置
extern _PRICE_SET A_PriceSet;							//电价设置
extern _PRICE_SET B_PriceSet;							//电价设置 */
//uint8_t IfRecvBill = 0;							     //是否收到了电价设置，收到了置1
uint8_t IfAPPStop[GUN_MAX] = {0};								//APP是否主动停止    0未主动停止  1主动停止


//发送数据
_YKC_SEND_CMD01 YKCSendCmd1;			//注册帧
_YKC_SEND_CMD03 YKCSendCmd3[GUN_MAX];	//发送枪心跳，（心跳中有各个枪的状态）
_YKC_SEND_CMD31 YKCSendCmd31[GUN_MAX];	//发送卡鉴权
_YKC_SEND_CMD33 YKCSendCmd33[GUN_MAX];	//启动应答
_YKC_SEND_CMD13 YKCSendCmd13[GUN_MAX];	//发送实时数据
_YKC_SEND_CMD15 YKCSendCmd15[GUN_MAX];	//发送充电握手  BRM
_YKC_SEND_CMD17 YKCSendCmd17[GUN_MAX];  //发送参数配置	BCP
_YKC_SEND_CMD19 YKCSendCmd19[GUN_MAX];	//充电结束      BSD
_YKC_SEND_CMD1B  YKCSendCmd1B[GUN_MAX];	//错误报文   BEM CEM
_YKC_SEND_CMD1D YKCSendCmd1D[GUN_MAX];	//bms终止   BST
_YKC_SEND_CMD21 YKCSendCmd21[GUN_MAX];	//设备终止  CST
_YKC_SEND_CMD23 YKCSendCmd23[GUN_MAX];	//充电中实时数据
_YKC_SEND_CMD25 YKCSendCmd25[GUN_MAX];	//BSM  15秒上报一次
_YKC_SEND_CMD35 YKCSendCmd35[GUN_MAX];	//停止应答
_YKC_SEND_CMD3B YKCSendCmd3B[GUN_MAX];	//上传订单
_YKC_SEND_CMD55 YKCSendCmd55;			//校时应答

/**************************************接收数据*************************/
_YKC_RECV_CMD0A YKCRecvCmd0A;			//接收倒费率
_YKC_RECV_CMD32	YKCRecvCmd32[GUN_MAX];			//接收卡鉴权结果
_YKC_RECV_CMD34	YKCRecvCmd34[GUN_MAX];			//开关机指令
_YKC_RECV_CMD56 YKCRecvCmd56;				//校时放回
_YKC_RECV_CMD58 YKCRecvCmd58;				//费率设置
_YKC_RECV_CMD42 YKCRecvCmd42[GUN_MAX];   //更新余额


uint8_t   YKC_SendRegisterCmd1(void); 	//云快充送注册帧
uint8_t   YKC_SendHearGunACmd3(void);		//发送心跳A
uint8_t   YKC_SendHearGunBCmd3(void);		//发送心跳B
uint8_t YKC_SendPriodSJDataGunACmd13(void);  	//发送A枪实时数据
uint8_t YKC_SendPriodSJDataGunBCmd13(void);  	//发送B枪实时数据
uint8_t YKC_SendSJDataGunACmd23(void);  	//发送A枪bms实时数据
uint8_t YKC_SendSJDataGunBCmd23(void);  	//发送B枪bms实时数据


_YKC_RECV_CMD0A YKCGunBill[GUN_MAX];			//枪费率


extern CH_TASK_T stChTcb;
extern _dlt645_info dlt645_info;
extern SYSTEM_RTCTIME gs_SysTime;
extern RATE_T	           stChRate;                 /* 充电费率  */
extern RATE_T	           stChRateA;                 /* 充电费率  */

_4G_SEND_TABLE YKCSendTable[YKC_SEND_FRAME_LEN] = {
    {0,    0,    CM_TIME_10_SEC, 	YKC_SendRegisterCmd1		},  //发送注册帧

    {0,    0,    CM_TIME_10_SEC, 	YKC_SendHearGunACmd3		},  //发送A枪心跳状态

    {0,    0,    CM_TIME_15_SEC, 	YKC_SendPriodSJDataGunACmd13		},  //发送A卡枪实时数据   上送充电枪实时数据，周期上送时，待机 5 分钟、充电 15 秒

    // {0,    0,    CM_TIME_15_SEC, 	YKC_SendPriodSJDataGunBCmd13		},  //发送A卡枪实时数据   上送充电枪实时数据，周期上送时，待机 5 分钟、充电 15 秒

//    {0,    0,    CM_TIME_15_SEC, 	YKC_SendSJDataGunACmd23		},  //充电 15 秒

};


uint32_t Balance[GUN_MAX]; //余额
uint8_t   YKC_RecvRegisterAck(uint8_t *pdata,uint16_t len);
uint8_t   YKC_RecvHeartAck(uint8_t *pdata,uint16_t len);
uint8_t   YKC_RecvPrimodelAck(uint8_t *pdata,uint16_t len);
uint8_t YKC_RecvPricAck(uint8_t *pdata,uint16_t len);
uint8_t YKC_RecvCard(uint8_t *pdata,uint16_t len);
uint8_t YKC_RecvStart(uint8_t *pdata,uint16_t len);
uint8_t YKC_RecvStop(uint8_t *pdata,uint16_t len);
uint8_t YKC_RecvReadData(uint8_t *pdata,uint16_t len);
static uint8_t   YKC_RecvRecordAck(uint8_t *pdata,uint16_t len);
static uint8_t   YKC_RecvTime(uint8_t *pdata,uint16_t len);
static uint8_t   YKC_RecvPricSet(uint8_t *pdata,uint16_t len);
static uint8_t   YKC_RecvBalance(uint8_t *pdata,uint16_t len);
_4G_RECV_TABLE YKCRecvTable[YKC_RECV_FRAME_LEN] = {

    {YKC_RECV_CMD_02			,	YKC_RecvRegisterAck		}, 		//pc返回确认信息

    {YKC_RECV_CMD_04			,	YKC_RecvHeartAck		}, 		//pc返回确认信息

    {YKC_RECV_CMD_06			,	YKC_RecvPrimodelAck		}, 		//计费模型应答

    {YKC_RECV_CMD_0A			,	YKC_RecvPricAck			}, 		//费率接受

    {YKC_RECV_CMD_32			,	YKC_RecvCard			}, 		//卡鉴权接收结果

    {YKC_RECV_CMD_34			,	YKC_RecvStart			}, 		//接收到启动命令

    {YKC_RECV_CMD_36			,	YKC_RecvStop			}, 		//接收到停止充电

    {YKC_RECV_CMD_12			,	YKC_RecvReadData		}, 		//读取实时数据

    {YKC_RECV_CMD_40			,	YKC_RecvRecordAck		}, 		//交易记录返回

    {YKC_RECV_CMD_56			,	YKC_RecvTime			}, 		//接收到校时

    {YKC_RECV_CMD_58			,	YKC_RecvPricSet			}, 		//费率设置

    {YKC_RECV_CMD_42			,	YKC_RecvBalance			}, 		//更新余额
};


uint8_t gabyCRCHi[] =
{
    0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,
    0x80,0x41,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
    0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,0x01,0xc0,
    0x80,0x41,0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
    0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x00,0xc1,
    0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,0x80,0x41,
    0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x00,0xc1,
    0x81,0x40,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
    0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,
    0x80,0x41,0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,
    0x01,0xc0,0x80,0x41,0x01,0xc0,0x80,0x41,0x00,0xc1,
    0x81,0x40,0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
    0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,
    0x80,0x41,0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,
    0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,0x01,0xc0,
    0x80,0x41,0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
    0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,
    0x80,0x41,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
    0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,0x01,0xc0,
    0x80,0x41,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
    0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,0x01,0xc0,
    0x80,0x41,0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,
    0x01,0xc0,0x80,0x41,0x01,0xc0,0x80,0x41,0x00,0xc1,
    0x81,0x40,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
    0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,
    0x80,0x41,0x00,0xc1,0x81,0x40
};

uint8_t gabyCRCLo[] =
{
    0x00,0xc0,0xc1,0x01,0xc3,0x03,0x02,0xc2,0xc6,0x06,
    0x07,0xc7,0x05,0xc5,0xc4,0x04,0xcc,0x0c,0x0d,0xcd,
    0x0f,0xcf,0xce,0x0e,0x0a,0xca,0xcb,0x0b,0xc9,0x09,
    0x08,0xc8,0xd8,0x18,0x19,0xd9,0x1b,0xdb,0xda,0x1a,
    0x1e,0xde,0xdf,0x1f,0xdd,0x1d,0x1c,0xdc,0x14,0xd4,
    0xd5,0x15,0xd7,0x17,0x16,0xd6,0xd2,0x12,0x13,0xd3,
    0x11,0xd1,0xd0,0x10,0xf0,0x30,0x31,0xf1,0x33,0xf3,
    0xf2,0x32,0x36,0xf6,0xf7,0x37,0xf5,0x35,0x34,0xf4,
    0x3c,0xfc,0xfd,0x3d,0xff,0x3f,0x3e,0xfe,0xfa,0x3a,
    0x3b,0xfb,0x39,0xf9,0xf8,0x38,0x28,0xe8,0xe9,0x29,
    0xeb,0x2b,0x2a,0xea,0xee,0x2e,0x2f,0xef,0x2d,0xed,
    0xec,0x2c,0xe4,0x24,0x25,0xe5,0x27,0xe7,0xe6,0x26,
    0x22,0xe2,0xe3,0x23,0xe1,0x21,0x20,0xe0,0xa0,0x60,
    0x61,0xa1,0x63,0xa3,0xa2,0x62,0x66,0xa6,0xa7,0x67,
    0xa5,0x65,0x64,0xa4,0x6c,0xac,0xad,0x6d,0xaf,0x6f,
    0x6e,0xae,0xaa,0x6a,0x6b,0xab,0x69,0xa9,0xa8,0x68,
    0x78,0xb8,0xb9,0x79,0xbb,0x7b,0x7a,0xba,0xbe,0x7e,
    0x7f,0xbf,0x7d,0xbd,0xbc,0x7c,0xb4,0x74,0x75,0xb5,
    0x77,0xb7,0xb6,0x76,0x72,0xb2,0xb3,0x73,0xb1,0x71,
    0x70,0xb0,0x50,0x90,0x91,0x51,0x93,0x53,0x52,0x92,
    0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,0x9c,0x5c,
    0x5d,0x9d,0x5f,0x9f,0x9e,0x5e,0x5a,0x9a,0x9b,0x5b,
    0x99,0x59,0x58,0x98,0x88,0x48,0x49,0x89,0x4b,0x8b,
    0x8a,0x4a,0x4e,0x8e,0x8f,0x4f,0x8d,0x4d,0x4c,0x8c,
    0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,
    0x43,0x83,0x41,0x81,0x80,0x40
};
/*****************************************************************************
* Function     : get_crc_Data
* Description  : 累加和
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日
*****************************************************************************/
static uint16_t ykc_get_crc_Data(uint8_t * pData, uint8_t len)
{
    uint8_t byCRCHi = 0xff;
    uint8_t byCRCLo = 0xff;
    uint8_t byIdx;
    uint16_t crc = 0;
    while(len--)
    {
        byIdx = byCRCHi ^(*(pData++));
        byCRCHi = byCRCLo ^ gabyCRCHi[byIdx];
        byCRCLo = gabyCRCLo[byIdx];
    }
    crc = byCRCHi;
    crc <<= 8;
    crc += byCRCLo;
    return crc;

}


/*****************************************************************************
* Function     : APP_GetYKCNetMoney
* Description  :获取账户余额
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日
******************************************************************************/
uint32_t APP_GetYKCNetMoney(uint8_t gun)
{
    if(gun >= GUN_MAX)
    {
        return 0;
    }
    return Balance[gun];
}

/*****************************************************************************
* Function     : APP_GetYKCBatchNum
* Description  : 获取交易流水号
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日
******************************************************************************/
uint8_t *  APP_GetYKCBatchNum(uint8_t gun)
{
    static uint8_t buf[16];		//交易流水号位16个字节，ASICC 因此取后16个数字

    if(gun >= GUN_MAX)
    {
        return NULL;
    }
    memcpy(buf,&YKCRecvCmd34[gun].SerialNum,16);		//订单号的后16位唯一
    return buf;
}


/*****************************************************************************
* Function     : HFQG_RecvFrameDispose
* Description  : 合肥乾古接收帧处理
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日
*****************************************************************************/
uint8_t   YKC_SendFrameDispose(void)
{
    uint8_t i;

    for(i = 0; i < YKC_SEND_FRAME_LEN; i++)
    {
        if(YKCSendTable[i].cycletime == 0)
        {
            continue;
        }
        YKCSendTable[i].curtime = OSTimeGet(&timeerr);
        if((YKCSendTable[i].curtime >= YKCSendTable[i].lasttime) ? ((YKCSendTable[i].curtime - YKCSendTable[i].lasttime) >= YKCSendTable[i].cycletime) : \
                ((YKCSendTable[i].curtime + (0xFFFFFFFFu - YKCSendTable[i].lasttime)) >= YKCSendTable[i].cycletime))
        {
            YKCSendTable[i].lasttime = YKCSendTable[i].curtime;
            if(YKCSendTable[i].Sendfunc != NULL)
            {
                YKCSendTable[i].Sendfunc();
            }
        }

    }
    return TRUE;
}



/*****************************************************************************
* Function     : YKCFreamSend
* Description  : 合肥乾古帧发送
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日
*****************************************************************************/
uint8_t   YKCFreamSend(uint8_t cmd,uint8_t *pdata, uint16_t len)
{
    uint16_t crc,i;
    uint8_t Sendbuf[200];
    if((pdata == NULL) || (!len)  || (len > 190))
    {
        return FALSE;
    }
    //1字节帧投
    Sendbuf[0] = 0x68;
    //1字节数据长度
    Sendbuf[1] = len+4;
    //2字节序列号
    Sendbuf[2] = 0x00;
    Sendbuf[3] = 0x00;
    //1个字节加密标志
    Sendbuf[4] = 0x00;
    //命令
    Sendbuf[5] = cmd;

    memcpy(&Sendbuf[6],pdata,len);
    //2字节crc




    crc = ykc_get_crc_Data(&Sendbuf[2],4+len);

    Sendbuf[7+len] = crc & 0x00ff;
    Sendbuf[6+len] = (crc  >> 8)& 0x00ff;
    ModuleSIM7600_SendData(0, Sendbuf,(8+len)); //发送数据

#if(USE_645 == 0)   //打印YKC发送信息
    printf("tx len:%d,tx data:",8+len);
    for(i = 0; i < 8+len; i++)
    {
        printf("%02x ",Sendbuf[i]);
    }
    printf("\r\n");
#endif

    OSTimeDly(500, OS_OPT_TIME_PERIODIC, &timeerr);
    return TRUE;
}

/*******************************周期性发送数据*******************************/
/*****************************************************************************
* Function     : YKC_SendRegister
* Description  : 云快充
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日完成
*****************************************************************************/
uint8_t   YKC_SendRegisterCmd1(void)
{
    static uint32_t i = 0;
    uint8_t * pdevnum = DisSysConfigInfo.DevNum; //设备号 (原始为ASCII)
    if(APP_GetAppRegisterState(LINK_NUM) == STATE_OK)	//显示已经注册成功了
    {
        return FALSE;
    }
    memset(&YKCSendCmd1,0,sizeof(_YKC_SEND_CMD01));
    YCKDevNum[0] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
    YCKDevNum[1] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
    YCKDevNum[2] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
    YCKDevNum[3] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
    YCKDevNum[4] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
    YCKDevNum[5] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
    YCKDevNum[6] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
    memcpy(YKCSendCmd1.DevNun,YCKDevNum,sizeof(YCKDevNum));
    YKCSendCmd1.Devtype = 0x01;   //桩类型 0表示直流桩 1表示交流桩
    YKCSendCmd1.GunNum = DisSysConfigInfo.GunNum;
    YKCSendCmd1.Vsion = 0x10;		//通信版本  V1.6
    YKCSendCmd1.NetState = 0; 		//SIM
    YKCSendCmd1.Operator = 0;		//移动

    memcpy(&YKCSendCmd1.ProVersions,dwin_showversion,sizeof(YKCSendCmd1.ProVersions));  //程序版本号 复制到屏幕上面的版本号
    return YKCFreamSend(YKC_SEND_CMD_01,(uint8_t*)&YKCSendCmd1,sizeof(YKCSendCmd1));
}


//心跳
/*****************************************************************************
* Function     : YKC_SendHearGunACmd3
* Description  : 云快充发送A枪心跳状态
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日   完成
*****************************************************************************/
uint8_t   YKC_SendHearGunACmd3(void)
{
    if(APP_GetAppRegisterState(LINK_NUM) != STATE_OK)
    {
        return FALSE;		//注册未成功，无需发送
    }
    memset(&YKCSendCmd3[GUN_A],0,sizeof(_YKC_SEND_CMD03));
    memcpy(YKCSendCmd3[GUN_A].DevNun,YCKDevNum,sizeof(YCKDevNum));
    YKCSendCmd3[GUN_A].GunNum = 1;
    if(stChTcb.ucState != CHARGER_FAULT)
    {
        YKCSendCmd3[GUN_A].GunState  = 0x00;   //正常
    }
    else
    {
        YKCSendCmd3[GUN_A].GunState  = 0x01;	//故障
    }
    return YKCFreamSend(YKC_SEND_CMD_03,(uint8_t*)&YKCSendCmd3[GUN_A],sizeof(_YKC_SEND_CMD03));
}



/*****************************************************************************
* Function     : YKC_SendSJDataGunACmd13
* Description  : 云快充发送实时数据    上送充电枪实时数据，周期上送时，待机 5 分钟、充电 15 秒
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日   完成
*****************************************************************************/
uint8_t   YKC_SendSJDataGunACmd13(void)
{

    if(APP_GetAppRegisterState(LINK_NUM) != STATE_OK)
    {
        return FALSE;		//注册未成功，无需发送
    }



    memset(&YKCSendCmd13[GUN_A],0,sizeof(_YKC_SEND_CMD13));
    memcpy(YKCSendCmd13[GUN_A].DevNun,YCKDevNum,sizeof(YCKDevNum));
    YKCSendCmd13[GUN_A].GunNum = 1;
    if(stChTcb.ucState == CHARGING)
    {
        YKCSendCmd13[GUN_A].State = 3;
    }
    else if(stChTcb.ucState == CHARGER_FAULT)
    {
        YKCSendCmd13[GUN_A].State = 1; //故障
    } else
    {
        YKCSendCmd13[GUN_A].State = 2; //待机
    }

    if(stChTcb.stGunStat.ucCpStat != CP_12V)
    {
        YKCSendCmd13[GUN_A].GunState = 1;    //插枪
    }
    else
    {
        YKCSendCmd13[GUN_A].GunState = 0;
    }
    if(stChTcb.ucState == CHARGING)
    {
        memcpy(YKCSendCmd13[GUN_A].SerialNum,YKCRecvCmd34[GUN_A].SerialNum,16);	//订单号
        if(DisSysConfigInfo.energymeter == 1)
        {
            YKCSendCmd13[GUN_A].OutVol = stChTcb.stHLW8112.usVolt;			//电压		//输出电压  0.1
            YKCSendCmd13[GUN_A].OutCur = stChTcb.stHLW8112.usCurrent/10;		//输出电流	0.1
        }
        else
        {
            YKCSendCmd13[GUN_A].OutVol = dlt645_info.out_vol;			//电压		//输出电压  0.1
            YKCSendCmd13[GUN_A].OutCur = dlt645_info.out_cur/100;		//输出电流	0.1
        }
        YKCSendCmd13[GUN_A].ChargeTime = (time(NULL) - stChTcb.stCh.timestart) /60;	//充电时间  min
        YKCSendCmd13[GUN_A].ChargeKwh =  stChTcb.stCh.uiChargeEnergy * 10;	//充电读数   精确到小数点4位
        YKCSendCmd13[GUN_A].ChargeMoney =stChTcb.stCh.uiAllEnergy;	//已充金额	精确到小数点4位
    }
    if(stChTcb.stIOFault.ucFrist == INPUT_YX2)
    {
        YKCSendCmd13[GUN_A].Fail[0] = 0x01;
    }
    return YKCFreamSend(YKC_SEND_CMD_13,(uint8_t*)&YKCSendCmd13[GUN_A],sizeof(_YKC_SEND_CMD13));
}


/*****************************************************************************
* Function     : YKC_SendSJDataGunACmd13
* Description  : 云快充发送实时数据    上送充电枪实时数据，周期上送时，待机 5 分钟、充电 15 秒
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日   完成
*****************************************************************************/
uint8_t   YKC_SendPriodSJDataGunACmd13(void)
{
    static uint8_t count = 0;


    if(APP_GetAppRegisterState(LINK_NUM) != STATE_OK)
    {
        return FALSE;		//注册未成功，无需发送
    }

    if(stChTcb.ucState != CHARGING)  //未再充电中5分钟发送一次
    {
        //枪状态改变需要立马发送
        if(++count < CM_TIME_5_MIN/CM_TIME_15_SEC)
        {
            return FALSE;
        }
    }
    count = 0;



    memset(&YKCSendCmd13[GUN_A],0,sizeof(_YKC_SEND_CMD13));
    memcpy(YKCSendCmd13[GUN_A].DevNun,YCKDevNum,sizeof(YCKDevNum));
    YKCSendCmd13[GUN_A].GunNum = 1;
    if(stChTcb.ucState == CHARGING)
    {
        YKCSendCmd13[GUN_A].State = 3;
    }
    else if(stChTcb.ucState == CHARGER_FAULT)
    {
        YKCSendCmd13[GUN_A].State = 1; //故障
    } else
    {
        YKCSendCmd13[GUN_A].State = 2; //待机
    }

    if(stChTcb.stGunStat.ucCpStat != CP_12V)
    {
        YKCSendCmd13[GUN_A].GunState = 1;    //插枪
    }
    else
    {
        YKCSendCmd13[GUN_A].GunState = 0;
    }
    if(stChTcb.ucState == CHARGING)
    {
        memcpy(YKCSendCmd13[GUN_A].SerialNum,YKCRecvCmd34[GUN_A].SerialNum,16);	//订单号
        if(DisSysConfigInfo.energymeter == 1)
        {
            YKCSendCmd13[GUN_A].OutVol = stChTcb.stHLW8112.usVolt;			//电压		//输出电压  0.1
            YKCSendCmd13[GUN_A].OutCur = stChTcb.stHLW8112.usCurrent/10;		//输出电流	0.1
        }
        else
        {
            YKCSendCmd13[GUN_A].OutVol = dlt645_info.out_vol*10;		//电压		//输出电压  0.1
            YKCSendCmd13[GUN_A].OutCur = dlt645_info.out_cur*10;		//输出电流	0.1
//            YKCSendCmd13[GUN_A].OutVol = dlt645_info.out_vol;			//电压		//输出电压  0.1
//            YKCSendCmd13[GUN_A].OutCur = dlt645_info.out_cur/100;;		//输出电流	0.1
        }
        YKCSendCmd13[GUN_A].ChargeTime =(time(NULL) - stChTcb.stCh.timestart) /60;	//充电时间  min
        YKCSendCmd13[GUN_A].ChargeKwh =  stChTcb.stCh.uiChargeEnergy * 10;	//充电读数   精确到小数点4位
        YKCSendCmd13[GUN_A].ChargeMoney =stChTcb.stCh.uiAllEnergy;	//已充金额	精确到小数点4位
    }
    if(stChTcb.stIOFault.ucFrist == INPUT_YX2)
    {
        YKCSendCmd13[GUN_A].Fail[0] = 0x01;
    }
    return YKCFreamSend(YKC_SEND_CMD_13,(uint8_t*)&YKCSendCmd13[GUN_A],sizeof(_YKC_SEND_CMD13));
}



///*****************************************************************************
//* Function     : YKC_SendSJDataGunACmd23
//* Description  : 云快充发送实时数据 充电 15 秒
//* Input        : void *pdata
//* Output       : None
//* Return       :
//* Note(s)      :
//* Contributor  : 2018年7月27日   完成
//*****************************************************************************/
//uint8_t   YKC_SendSJDataGunACmd23(void)
//{
//    if(APP_GetAppRegisterState(LINK_NUM) != STATE_OK)
//    {
//    	return RT_FALSE;		//注册未成功，无需发送
//    }

//    if(stChTcb.ucState != CHARGING)  //未再充电中5分钟发送一次
//    {
//    	//枪状态改变需要立马发送
//    	return RT_FALSE;
//    }

//    memset(&YKCSendCmd23[GUN_A],0,sizeof(_YKC_SEND_CMD23));
//    memcpy(YKCSendCmd23[GUN_A].DevNun,YCKDevNum,sizeof(YCKDevNum));
//    memcpy(YKCSendCmd23[GUN_A].SerialNum,YKCRecvCmd34[GUN_A].SerialNum,16);	//订单号
//    YKCSendCmd23[GUN_A].GunNum = 1;


//	if(DisSysConfigInfo.energymeter == 1)
//	{
//		YKCSendCmd23[GUN_A].OutVol = stChTcb.stRn8209.usVolt;			//电压		//输出电压  0.1
//		YKCSendCmd23[GUN_A].OutCur = stChTcb.stRn8209.usCurrent/10;		//输出电流	0.1
//	}
//	else
//	{
//		YKCSendCmd23[GUN_A].OutVol = dlt645_info.out_vol;			//电压		//输出电压  0.1
//		YKCSendCmd23[GUN_A].OutCur = stChTcb.stRn8209.usCurrent/10;		//输出电流	0.1
//	}
//	YKCSendCmd23[GUN_A].ChargeTime = (time(RT_NULL) - stChTcb.stCh.timestart) /60;	//充电时间  min

//    return YKCFreamSend(YKC_SEND_CMD_23,(uint8_t*)&YKCSendCmd23[GUN_A],sizeof(_YKC_SEND_CMD23));
//}


/*******************************非周期性发送数据*****************************/
/*****************************************************************************
* Function     : YKC_SendPriModel
* Description  : 发送计费模型
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t   YKC_SendPriModel(void)
{
    static uint8_t buf[9];
    memset(buf,0,sizeof(buf));
    memcpy(buf,YCKDevNum,sizeof(YCKDevNum));
    //计费模型
    buf[7] = 0x00;
    buf[8] = 0;
    return YKCFreamSend(YKC_SEND_CMD_05,buf,sizeof(buf));
}

/*****************************************************************************
* Function     : YKC_SendPriReq
* Description  : 发送计费请求
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t   YKC_SendPriReq(void)
{
    static uint8_t buf[7];
    memset(buf,0,sizeof(buf));
    memcpy(buf,YCKDevNum,sizeof(YCKDevNum));
    return YKCFreamSend(YKC_SEND_CMD_09,buf,sizeof(buf));
}


/*****************************************************************************
* Function     : YKC_SendRateAck
* Description  : 费率设置应答
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t   YKC_SendRateAck(uint8_t cmd)
{
    static uint8_t buf[8] = {0};
    memcpy(buf,YCKDevNum,sizeof(YCKDevNum));
    buf[7] = 0x01; //成功
    return YKCFreamSend(YKC_SEND_CMD_57,buf,8);
}




/*****************************************************************************
* Function     : YKC_SendStartAck
* Description  : 启动应答
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t   YKC_SendStartAck(_GUN_NUM gun)
{
    //发送启动应答说明已经启动成功了，若启动失败，则不发送启动应答，直接发送订单

    uint8_t * pdevnum = DisSysConfigInfo.DevNum;
    //_DISP_CONTROL* pdisp_conrtol = APP_GetDispControl();
    if(gun >= GUN_MAX)
    {
        return FALSE;
    }

    memset(&YKCSendCmd33[gun],0,sizeof(_YKC_SEND_CMD33));
    if(stChTcb.stGunStat.ucCpStat != CP_12V)
    {

        //表示启动成功
        YKCSendCmd33[gun].FailReason = 0;
        YKCSendCmd33[gun].StartState = 1; //启动成功


    }
    else
    {
        YKCSendCmd33[gun].FailReason = 5;  //枪未连接
        YKCSendCmd33[gun].StartState = 0; //启动失败
    }

    memcpy(YKCSendCmd33[gun].SerialNum,YKCRecvCmd34[gun].SerialNum,16);	//订单号
    memcpy(YKCSendCmd33[gun].DevNun,YCKDevNum,sizeof(YCKDevNum));       //装编号
    YKCSendCmd33[gun].GunNum = gun+1;
    return YKCFreamSend(YKC_SEND_CMD_33,(uint8_t*)&YKCSendCmd33[gun],sizeof(_YKC_SEND_CMD33));
}


/*****************************************************************************
* Function     : ZH_SendStopAck
* Description  : 停止应答
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t   YKC_SendStopAck(_GUN_NUM gun)
{
    if(gun >= GUN_MAX)
    {
        return FALSE;
    }
    memset(&YKCSendCmd35[gun],0,sizeof(_YKC_SEND_CMD35));
    memcpy(YKCSendCmd35[gun].DevNun,YCKDevNum,sizeof(YCKDevNum));       //装编号
    YKCSendCmd35[gun].StopResul = 1;
    YKCSendCmd35[gun].GunNum = gun+1;
    return YKCFreamSend(YKC_SEND_CMD_35,(uint8_t*)&YKCSendCmd35[gun],sizeof(_YKC_SEND_CMD35));
}

/*****************************************************************************
* Function     : PreYKCBill
* Description  : 保存汇誉订单
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2021年1月12日
*****************************************************************************/
uint8_t   PreYKCBill(_GUN_NUM gun,uint8_t *pdata)
{
    uint8_t i;
    uint8_t errcode;
    if(gun >= GUN_MAX || pdata == NULL)
    {
        return FALSE;
    }

    _YKC_SEND_CMD3B * pbill = (_YKC_SEND_CMD3B *)pdata;
    memset(pbill,0,sizeof(_YKC_SEND_CMD3B));
    memcpy(pbill->DevNun,YCKDevNum,sizeof(YCKDevNum));       //装编号
    memcpy(pbill->SerialNum,YKCRecvCmd34[gun].SerialNum,16);	//订单号
    pbill->GunNum = gun+1;
    pbill->StartTime.Second = stChTcb.stCh.uiChStartTime.ucSec * 1000;
    pbill->StartTime.Day =stChTcb.stCh.uiChStartTime.ucDay;
    pbill->StartTime.Hour = stChTcb.stCh.uiChStartTime.ucHour;
    pbill->StartTime.Minute = stChTcb.stCh.uiChStartTime.ucMin;
    pbill->StartTime.Month = stChTcb.stCh.uiChStartTime.ucMonth;
    pbill->StartTime.Year = stChTcb.stCh.uiChStartTime.ucYear - 100;



    pbill->StopTime.Second = gs_SysTime.ucSec * 1000;;
    pbill->StopTime.Day = gs_SysTime.ucDay;
    pbill->StopTime.Hour =gs_SysTime.ucHour;
    pbill->StopTime.Minute = gs_SysTime.ucMin;
    pbill->StopTime.Month =  gs_SysTime.ucMonth;
    pbill->StopTime.Year = gs_SysTime.ucYear - 100;

    for(i = 0; i <4; i++)
    {
        pbill->BillFsInfo[i].Pric = YKCGunBill[gun].PrcInfo[i].Fric +  YKCGunBill[gun].PrcInfo[i].ServiceFric;
        pbill->BillFsInfo[i].PowerKwh =stChTcb.stCh.jfpg_kwh[i]*10;
        pbill->BillFsInfo[i].Money = stChTcb.stCh.uiaChargeEnergy[i]/100;
    }
    if(DisSysConfigInfo.energymeter == 1)
    {
        pbill->ChargeStartKwh = (stChTcb.stHLW8112.uiE - stChTcb.stCh.uiChargeEnergy) * 10; //充电前电表读数
        pbill->ChargeStopKwh =stChTcb.stHLW8112.uiE * 10;
    }
    else
    {
        pbill->ChargeStartKwh = ((dlt645_info.cur_hwh * 1000) - stChTcb.stCh.uiChargeEnergy )* 10;
        pbill->ChargeStartKwh = dlt645_info.cur_hwh* 10000;
    }



    pbill->ChargeAllKwh = stChTcb.stCh.uiChargeEnergy*10;   //小数点4位

    pbill->ChargeMoney = stChTcb.stCh.uiAllEnergy;

    //目前都位APP启动
    pbill->ChargeType = 1;				// 1 app启动  2卡启动   3离线卡启动			5VIN码启动


    pbill->JYTime.Second = gs_SysTime.ucSec * 1000;;
    pbill->JYTime.Day = gs_SysTime.ucDay;
    pbill->JYTime.Hour =gs_SysTime.ucHour;
    pbill->JYTime.Minute = gs_SysTime.ucMin;
    pbill->JYTime.Month =  gs_SysTime.ucMonth;
    pbill->JYTime.Year = gs_SysTime.ucYear - 100;

    if(IfAPPStop[gun])
    {
        pbill->StopReason = 0x40;			//APP主动停止
    }
    else
    {

        //其他原因
        if((stChTcb.stCh.reason == NORMAL) || (stChTcb.stCh.reason == END_CONDITION) || \
                (stChTcb.stCh.reason == NO_CURRENT) || (stChTcb.stCh.reason == NO_CURRENT_9V)||( stChTcb.stCh.reason == STOP_CHARGEFULL) )
        {
            pbill->StopReason = 0x41;  //充满停止
        }
        else if((stChTcb.stCh.reason == E_END_BY_APP) || (stChTcb.stCh.reason == E_END_BY_CARD))
        {
            pbill->StopReason = 0x40; //主动停止
        }
        else if(stChTcb.stCh.reason == EM_STOP)
        {
            pbill->StopReason = 0x72;  //急停
        }
        else if(stChTcb.stCh.reason == UNPLUG)
        {
            pbill->StopReason = 0x6b; //非法拔枪
        }
        else
        {
            pbill->StopReason = 0X90;
        }
    }



    //  pbill->StopReason = 0x83;  //为了测试通过
    return TRUE;
}
/*****************************************************************************
* Function     : _HY_RestUpdataData
* Description  : 复位更新数据
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2021年1月12日
*****************************************************************************/
uint8_t   _YKC_RestUpdataData(void)
{
    _YKC_RECV_CMD34 *pBill	= (_YKC_RECV_CMD34 *)APP_GetBillInfo(GUN_A);
    memcpy(YKCRecvCmd34[GUN_A].SerialNum,pBill->SerialNum,16);	//订单号
    return TRUE;
}

/*****************************************************************************
* Function     : YKC_SendBill
* Description  : 发送订单
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t YKC_SendBill(_GUN_NUM gun)
{
    uint8_t i;
    uint8_t errcode;

    if(gun >= GUN_MAX)
    {
        return FALSE;
    }
    memset(&YKCSendCmd3B[gun],0,sizeof(_YKC_SEND_CMD3B));
    memcpy(YKCSendCmd3B[gun].DevNun,YCKDevNum,sizeof(YCKDevNum));       //装编号
    memcpy(YKCSendCmd3B[gun].SerialNum,YKCRecvCmd34[gun].SerialNum,16);	//订单号
    YKCSendCmd3B[gun].GunNum = gun+1;

    YKCSendCmd3B[gun].StartTime.Second = stChTcb.stCh.uiChStartTime.ucSec * 1000;
    YKCSendCmd3B[gun].StartTime.Day =stChTcb.stCh.uiChStartTime.ucDay;
    YKCSendCmd3B[gun].StartTime.Hour = stChTcb.stCh.uiChStartTime.ucHour;
    YKCSendCmd3B[gun].StartTime.Minute = stChTcb.stCh.uiChStartTime.ucMin;
    YKCSendCmd3B[gun].StartTime.Month = stChTcb.stCh.uiChStartTime.ucMonth;
    YKCSendCmd3B[gun].StartTime.Year = stChTcb.stCh.uiChStartTime.ucYear - 100;

    YKCSendCmd3B[gun].StopTime.Second = gs_SysTime.ucSec * 1000;
    YKCSendCmd3B[gun].StopTime.Day = gs_SysTime.ucDay;
    YKCSendCmd3B[gun].StopTime.Hour =gs_SysTime.ucHour;
    YKCSendCmd3B[gun].StopTime.Minute = gs_SysTime.ucMin;
    YKCSendCmd3B[gun].StopTime.Month =  gs_SysTime.ucMonth;
    YKCSendCmd3B[gun].StopTime.Year = gs_SysTime.ucYear - 100;
    for(i = 0; i <4; i++)
    {
        YKCSendCmd3B[gun].BillFsInfo[i].Pric = YKCGunBill[gun].PrcInfo[i].Fric +  YKCGunBill[gun].PrcInfo[i].ServiceFric;
        YKCSendCmd3B[gun].BillFsInfo[i].PowerKwh =stChTcb.stCh.jfpg_kwh[i]*10;
        YKCSendCmd3B[gun].BillFsInfo[i].Money = stChTcb.stCh.uiaChargeEnergy[i]/100;
    }
    if(DisSysConfigInfo.energymeter == 1)
    {
        YKCSendCmd3B[gun].ChargeStartKwh = (stChTcb.stHLW8112.uiE - stChTcb.stCh.uiChargeEnergy) * 10; //充电前电表读数
        YKCSendCmd3B[gun].ChargeStopKwh =stChTcb.stHLW8112.uiE * 10;
    }
    else
    {
        YKCSendCmd3B[gun].ChargeStartKwh = ((dlt645_info.cur_hwh * 1000) - stChTcb.stCh.uiChargeEnergy )* 10;
        YKCSendCmd3B[gun].ChargeStartKwh = dlt645_info.cur_hwh* 10000;
    }



    YKCSendCmd3B[gun].ChargeAllKwh = stChTcb.stCh.uiChargeEnergy*10;   //小数点4位

    YKCSendCmd3B[gun].ChargeMoney = stChTcb.stCh.uiAllEnergy;

    //目前都位APP启动
    YKCSendCmd3B[gun].ChargeType = 1;				// 1 app启动  2卡启动   3离线卡启动			5VIN码启动


    YKCSendCmd3B[gun].JYTime.Second = gs_SysTime.ucSec * 1000;;
    YKCSendCmd3B[gun].JYTime.Day = gs_SysTime.ucDay;
    YKCSendCmd3B[gun].JYTime.Hour =gs_SysTime.ucHour;
    YKCSendCmd3B[gun].JYTime.Minute = gs_SysTime.ucMin;
    YKCSendCmd3B[gun].JYTime.Month =  gs_SysTime.ucMonth;
    YKCSendCmd3B[gun].JYTime.Year = gs_SysTime.ucYear - 100;

    if(IfAPPStop[gun])
    {
        YKCSendCmd3B[gun].StopReason = 0x40;			//APP主动停止
    }
    else
    {

        //其他原因
        if((stChTcb.stCh.reason == NORMAL) || (stChTcb.stCh.reason == END_CONDITION) || \
                (stChTcb.stCh.reason == NO_CURRENT)|| (stChTcb.stCh.reason == NO_CURRENT_9V) ||( stChTcb.stCh.reason == STOP_CHARGEFULL) )
        {
            YKCSendCmd3B[gun].StopReason = 0x41;  //充满停止
        }
        else if((stChTcb.stCh.reason == E_END_BY_APP) || (stChTcb.stCh.reason == E_END_BY_CARD))
        {
            YKCSendCmd3B[gun].StopReason = 0x40; //主动停止
        }
        else if(stChTcb.stCh.reason == EM_STOP)
        {
            YKCSendCmd3B[gun].StopReason = 0x72;  //急停
        }
        else if(stChTcb.stCh.reason == UNPLUG)
        {
            YKCSendCmd3B[gun].StopReason = 0x6b; //非法拔枪
        }
        else if(stChTcb.stCh.reason == E_END_APP_BALANC)
        {
            YKCSendCmd3B[gun].StopReason = 0x6E; //app余额不足
        }
        else
        {
            YKCSendCmd3B[gun].StopReason = 0X90;
        }
    }

    WriterFmBill(gun,1);
    ResendBillControl[gun].CurTime = OSTimeGet(&timeerr);;
    ResendBillControl[gun].CurTime = ResendBillControl[gun].LastTime;

    if(gun == GUN_A)
    {
        memcpy(&stChRateA,&stChRate,sizeof(RATE_T));   //更新费率
    }


    return YKCFreamSend(YKC_SEND_CMD_3B,(uint8_t*)&YKCSendCmd3B[gun],sizeof(_YKC_SEND_CMD3B));

}
uint8_t YKC_Balance_judgment(_GUN_NUM gun)  //判断金额
{
    if(stChTcb.stCh.uiAllEnergy >= Balance[gun]*100) //充电金额 > 余额时，停止
    {
        return 1;   //App停止
    }
    return 0;
}

/*****************************************************************************
* Function     : YKC_SendBalanceAck
* Description  : 发送更新余额应答
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t   YKC_SendBalanceAck(_GUN_NUM gun)
{
    static uint8_t buf[16];
    if(gun >= GUN_MAX)
    {
        return FALSE;
    }
    memset(buf,0,16);
    memcpy(buf,YCKDevNum,sizeof(YCKDevNum));       //装编号
    buf[15] = 0; //修改成功
    return YKCFreamSend(YKC_SEND_CMD_41,buf,16);
}


/*****************************************************************************
* Function     : YKC_SendBillData
* Description  : 云快充重发订单数据
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t YKC_SendBillData(uint8_t * pdata,uint8_t len)
{
    _YKC_SEND_CMD3B *pbill;
    uint8_t gun;

    if((pdata == NULL) || (len < sizeof(_YKC_SEND_CMD3B)))
    {
        return FALSE;
    }
    pbill = (_YKC_SEND_CMD3B *)pdata;
    gun = pbill->GunNum - 1;
    if(gun >= GUN_MAX)
    {
        return FALSE;
    }
//	pbill->StopReason = 0x83;   //为了测试通过
    memcpy(YKCRecvCmd34[gun].SerialNum,pbill->SerialNum,sizeof(YKCRecvCmd34[gun].SerialNum));   //交易记录返回需要通过交易流水号判断
    return YKCFreamSend(YKC_SEND_CMD_3B,pdata,sizeof(_YKC_SEND_CMD3B));
}

/*****************************************************************************
* Function     : YKC_SendBillData
* Description  : 云快充重发订单数据
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
uint8_t YKC_SendSetTimeAck(void)
{
    YKCSendCmd55.CurTime.Second = gs_SysTime.ucSec* 1000;
    YKCSendCmd55.CurTime.Day = gs_SysTime.ucDay;
    YKCSendCmd55.CurTime.Hour =gs_SysTime.ucHour;
    YKCSendCmd55.CurTime.Minute = gs_SysTime.ucMin;
    YKCSendCmd55.CurTime.Month =gs_SysTime.ucMonth;
    YKCSendCmd55.CurTime.Year = gs_SysTime.ucYear - 100;
    memcpy(YKCSendCmd55.DevNun,YCKDevNum,sizeof(YCKDevNum));       //装编号
    return YKCFreamSend(YKC_SEND_CMD_55,(uint8_t*)&YKCSendCmd55,sizeof(_YKC_SEND_CMD55));
}


/***************************************************************
**Function   :YKC_SendCardInfo
**Description: YKC网络卡发送卡鉴权
**Input      :gun: [输入/出]
**Output     :
**Return     :
**note(s)    :
**Author     :CSH
**Create_Time:2023-2-17
***************************************************************/
uint8_t  YKC_SendCardInfo(_GUN_NUM gun)
{
    uint32_t  CardNum;   //CSH0512声明指针必须初始化一下（不初始化，编译不会报错，但是下面传输数据会错误）
    if(gun >= GUN_MAX)
    {
        return FALSE;
    }
    if(APP_GetAppRegisterState(LINK_NUM) != STATE_OK)	//注册不成功，返回失败
    {
        return  FALSE;
    }

    memset(&YKCSendCmd31[gun],0,sizeof(_YKC_SEND_CMD31));
    uint8_t * pdevnum = DisSysConfigInfo.DevNum; //设备号 (原始为ASCII)
    memset(&YKCSendCmd1,0,sizeof(_YKC_SEND_CMD01));
    YCKDevNum[0] = (pdevnum[0] - '0') *0x10 +  (pdevnum[1] - '0');
    YCKDevNum[1] = (pdevnum[2] - '0') *0x10 +  (pdevnum[3] - '0');
    YCKDevNum[2] = (pdevnum[4] - '0') *0x10 +  (pdevnum[5] - '0');
    YCKDevNum[3] = (pdevnum[6] - '0') *0x10 +  (pdevnum[7] - '0');
    YCKDevNum[4] = (pdevnum[8] - '0') *0x10 +  (pdevnum[9] - '0');
    YCKDevNum[5] = (pdevnum[10] - '0') *0x10 +  (pdevnum[11] - '0');
    YCKDevNum[6] = (pdevnum[12] - '0') *0x10 +  (pdevnum[13] - '0');
    memcpy(YKCSendCmd31[gun].DevNun,YCKDevNum,sizeof(YCKDevNum));  //桩编号BCD码
    YKCSendCmd31[gun].GunNum = gun+1;   //枪号
    YKCSendCmd31[gun].Startmode = 0x01; //刷卡启动充电

    //赋值 物理卡号
    CardNum = ((m1_card_info.uidByte[3]) | (m1_card_info.uidByte[2] << 8) |\
               (m1_card_info.uidByte[1] << 16) | (m1_card_info.uidByte[0] << 24));
    memcpy(&YKCSendCmd31[gun].card_NUM[4],&CardNum,4);   //复制物理卡号

//	YKCSendCmd31[gun].card_NUM[4]=m1_card_info.uidByte[3];
//	YKCSendCmd31[gun].card_NUM[5]=m1_card_info.uidByte[2];
//	YKCSendCmd31[gun].card_NUM[6]=m1_card_info.uidByte[1];
//	YKCSendCmd31[gun].card_NUM[7]=m1_card_info.uidByte[0];

    YKCSendCmd31[gun].password = 0;  //不需要密码
    memset(&YKCSendCmd31[gun].VIN,0x30,sizeof(YKCSendCmd31[gun].VIN));  //其他方式置零( ASCII码)

    return YKCFreamSend(YKC_SEND_CMD_31,(uint8_t*)&YKCSendCmd31[gun],sizeof(_YKC_SEND_CMD31));
}




/******************************************接收函数*******************************************/
/*****************************************************************************
* Function     : YKC_RecvFrameDispose
* Description  : 合肥乾古接收帧处理
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年7月27日
*****************************************************************************/
uint8_t   YKC_RecvFrameDispose(uint8_t * pdata,uint16_t len)
{
    uint8_t i = 0, Recvstate = 0;
    uint8_t cmd;
    uint16_t datalen;
    static uint8_t buf[100];
    uint8_t *pframedata;

    pframedata = pdata;

    /**加一个whil 防止包与包之间相粘，不加这个一定会应答不了**/
    if((pdata == NULL) || (len < 8) )
    {
        return FALSE;
    }

    while(len > 8)
    {
        if(pdata == NULL)
        {
            return FALSE;
        }
        //帧头帧尾判断
        if(pframedata[0] != 0x68)
        {
            return FALSE;
        }
        //提取数据长度
        datalen= pframedata[1];
        if(((datalen+4) > len) ||(datalen >100) )
        {
            return FALSE;
        }
        cmd = pframedata[5];  //提取命令字
        //提取数据
        memcpy(buf,&pframedata[6],datalen - 4);

        for(i = 0; i < YKC_RECV_FRAME_LEN; i++)
        {
            if(YKCRecvTable[i].cmd == cmd)
            {
                Recvstate = 1;  //真正接收到数据
                if(YKCRecvTable[i].recvfunction != NULL)
                {
                    YKCRecvTable[i].recvfunction(buf,datalen - 4);
                }
                break;
            }
        }
        len =len -  datalen - 4;
        pframedata = &pdata[datalen+4];
    }

    if(Recvstate)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

/*****************************************************************************
* Function     : YKC_RecvRegisterAck
* Description  : 注册应答
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2021年1月12日
*****************************************************************************/
uint8_t   YKC_RecvRegisterAck(uint8_t *pdata,uint16_t len)
{
    //参照协议
    if((pdata == NULL) || (len != 8) )
    {
        return FALSE;
    }
    if(pdata[7] == 0x00) //0：注册不通过   0x01登录失败
    {
        APP_SetAppRegisterState(LINK_NUM,STATE_OK);
        if(stChTcb.ucState != CHARGING)   //没在充电中读取
        {
            ReadFmBill();   //是否需要发送订单
        }
        //发送计费模型验证
        mq_service_send_to_4gsend(APP_RATE_MODE,GUN_A ,0 ,NULL);
        //发送实时数据
        mq_service_send_to_4gsend(APP_SJDATA_QUERY,GUN_A ,0 ,NULL);

    }

    return TRUE;
}


/*****************************************************************************
* Function     : YKC_RecvHeartAck
* Description  : 心跳应答
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2021年1月12日
*****************************************************************************/
uint8_t   YKC_RecvHeartAck(uint8_t *pdata,uint16_t len)
{
    if(pdata == NULL )
    {
        return FALSE;
    }
    return TRUE;
}


/*****************************************************************************
* Function     : YKC_RecvPrimodelAck
* Description  : 返回计费模型应答
* Input        : void *pdata
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2021年1月12日
*****************************************************************************/
uint8_t   YKC_RecvPrimodelAck(uint8_t *pdata,uint16_t len)
{
    //参照协议
    if((pdata == NULL) || (len != 10) )
    {
        return FALSE;
    }
    if(pdata[9] == 0)  //0 桩计费模型与平台一直  1 桩计费模型与平台不一致
    {
        return TRUE;
    }
    else
    {
        //计费模型不一致，发送 09费率请求
        mq_service_send_to_4gsend(APP_QUERY_RATE,GUN_A ,0 ,NULL);
    }
    return FALSE;
}


/*****************************************************************************
* Function     : YKC_RecvPricAck
* Description  : 费率下发
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   YKC_RecvPricAck(uint8_t *pdata,uint16_t len)
{
    uint8_t i = 0,count,lasti;
    RATE_T rate = {0};


    if((len != (sizeof(YKCRecvCmd0A))) || (pdata == NULL) )
    {
        return FALSE;
    }
    memcpy((uint8_t*)&YKCRecvCmd0A,pdata,len);


    for(i = 0; i < 4; i++)
    {
        rate.Prices[i] = YKCRecvCmd0A.PrcInfo[i].Fric;
        rate.fwPrices[i] = YKCRecvCmd0A.PrcInfo[i].ServiceFric;
    }
    memcpy(rate.ucSegNum,YKCRecvCmd0A.FricNum,sizeof(YKCRecvCmd0A.FricNum));
    count = 1;
    lasti = YKCRecvCmd0A.FricNum[0];
    for(i = 0; i< 48; i++)
    {
        if(lasti != YKCRecvCmd0A.FricNum[i])
        {
            lasti = YKCRecvCmd0A.FricNum[i];
            count++;
        }
    }
    rate.TimeQuantumNum = count;
    memcpy(&stChRate,&rate,sizeof(rate));
    fal_partition_write(CHARGE_RATE,0,(uint8_t*)&stChRate,sizeof(RATE_T));
    mq_service_send_to_4gsend(APP_RATE_ACK,GUN_A ,0 ,NULL);

    if(stChTcb.ucState != CHARGING)
    {
        memcpy(&stChRateA,&stChRate,sizeof(RATE_T));
        memcpy(&YKCGunBill[GUN_A],&YKCRecvCmd0A,sizeof(YKCRecvCmd0A));
    }
//    if(pdisp_conrtol->NetGunState[GUN_B] != GUN_CHARGEING)
//    {
//    	memcpy(&stChRateB,&stChRate,sizeof(RATE_T));
//    	memcpy(&YKCGunBill[GUN_B],&YKCRecvCmd0A,sizeof(YKCRecvCmd0A));
//    }
    //	IfRecvBill = 1;
    checkYKConline = 3;
    return TRUE;
}

/*****************************************************************************
* Function     : YKC_RecvPricAck
* Description  : 费率下发
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   YKC_RecvPricSet(uint8_t *pdata,uint16_t len)
{
    uint8_t i = 0,count,lasti;
    RATE_T rate = {0};
    if((len != (sizeof(YKCRecvCmd58))) || (pdata == NULL) )
    {
        return FALSE;
    }
    memcpy((uint8_t*)&YKCRecvCmd58,pdata,len);
    memcpy((uint8_t*)&YKCRecvCmd0A,pdata,len);

    for(i = 0; i < 4; i++)
    {
        rate.Prices[i] = YKCRecvCmd58.PrcInfo[i].Fric;
        rate.fwPrices[i] = YKCRecvCmd58.PrcInfo[i].ServiceFric;
    }
    memcpy(rate.ucSegNum,YKCRecvCmd58.FricNum,sizeof(YKCRecvCmd58.FricNum));
    count = 1;
    lasti = YKCRecvCmd58.FricNum[0];
    for(i = 0; i< 48; i++)
    {
        if(lasti != YKCRecvCmd58.FricNum[i])
        {
            lasti = YKCRecvCmd58.FricNum[i];
            count++;
        }
    }
    rate.TimeQuantumNum = count;
    memcpy(&stChRate,&rate,sizeof(rate));
    fal_partition_write(CHARGE_RATE,0,(uint8_t*)&stChRate,sizeof(RATE_T));
    mq_service_send_to_4gsend(APP_RATE_ACK,GUN_A ,0 ,NULL);



    //不在充电中费率才生效，再充电中费率不生效
    if(stChTcb.ucState != CHARGING)
    {
        memcpy(&stChRateA,&stChRate,sizeof(RATE_T));
        memcpy(&YKCGunBill[GUN_A],&YKCRecvCmd0A,sizeof(YKCRecvCmd0A));
    }
//    if(pdisp_conrtol->NetGunState[GUN_B] != GUN_CHARGEING)
//    {
//    	memcpy(&stChRateB,&stChRate,sizeof(RATE_T));
//    	memcpy(&YKCGunBill[GUN_B],&YKCRecvCmd0A,sizeof(YKCRecvCmd0A));
//    }
    mq_service_send_to_4gsend(APP_RATE_ACK,GUN_A ,0 ,NULL);

    //	IfRecvBill = 1;
    return TRUE;
}



/***************************************************************
**Function   :YKC_RecvCard
**Description: YKC接收卡鉴权 cmd=0x32
**Input      :pdata: [输入/出]
**			 len: [输入/出]
**Output     :
**Return     :
**note(s)    :
**Author     :CSH
**Create_Time:2022-12-13
***************************************************************/
static uint8_t  YKC_RecvCard(uint8_t *pdata,uint16_t len)
{
    uint8_t gun = 0;

    if((len != (sizeof(_YKC_RECV_CMD32))) || (pdata == NULL) )
    {
        return FALSE;
    }
    gun = pdata[23] - 1; //提取枪号
    if(gun >= GUN_MAX)
    {
        return FALSE;
    }
    memcpy(&YKCRecvCmd32[gun],pdata,len);

    memcpy(&YKCRecvCmd34[gun].SerialNum,&pdata[0],16);  //YKC和塑云接收卡鉴权时，会下发流水号，赋值cmd34

    Balance[gun] = YKCRecvCmd32[gun].Money;  //余额
    fal_partition_write(YKC_PRIC,0,(uint8_t*)Balance,sizeof(Balance));
    if(YKCRecvCmd32[gun].Success_flag == 1)
    {
        send_ch_ctl_msg(1,0,4,0); 					//发送启动充电
        _4G_SetStartType(gun,_4G_APP_CARD);			//设置为卡启动
    }
    return TRUE;
}













/*****************************************************************************
* Function     : YKC_RecvStart
* Description  : 接收倒启动命令
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   YKC_RecvStart(uint8_t *pdata,uint16_t len)
{
    uint8_t gun = 0;

    if((len != (sizeof(_YKC_RECV_CMD34))) || (pdata == NULL) )
    {
        return FALSE;
    }
    gun = pdata[23] - 1; //提取枪号
    if(gun >= GUN_MAX)
    {
        return FALSE;
    }
    IfAPPStop[gun] = 0;
    memcpy(&YKCRecvCmd34[gun],pdata,len);

    ResendBillControl[gun].ResendBillState = FALSE;	  //之前的订单无需发送了
    ResendBillControl[gun].SendCount = 0;
    //发送启动充电
    send_ch_ctl_msg(1,0,4,0);


    mq_service_send_to_4gsend(APP_START_ACK,gun ,0 ,NULL);
    Balance[gun] = YKCRecvCmd34[gun].Money;
    fal_partition_write(YKC_PRIC,0,(uint8_t*)Balance,sizeof(Balance));
    YKCRecvCmd42[gun].Balance = 0;  //可能不会发送余额
    return TRUE;
}


/*****************************************************************************
* Function     : YKC_RecvStop
* Description  : 接收到停止充电
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年9月15日
*****************************************************************************/
static uint8_t   YKC_RecvStop(uint8_t *pdata,uint16_t len)
{
    uint8_t gun = 0;

    if(len != 8 || (pdata == NULL) )
    {
        return FALSE;
    }
    gun = pdata[7] - 1; //提取枪号
    if(gun>=GUN_MAX)
    {
        return FALSE;
    }
    IfAPPStop[gun] = 1;
    //发送停止充电
    send_ch_ctl_msg(2,0,4,0);


    //发送停止充电应答
    mq_service_send_to_4gsend(APP_STOP_ACK,gun ,0 ,NULL);
    return TRUE;
}

/*****************************************************************************
* Function     : YKC_RecvReadData
* Description  : 接收读取实时数据
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   YKC_RecvReadData(uint8_t *pdata,uint16_t len)
{
    uint8_t gun;

    if((pdata == NULL) || (len != 8))
    {
        return FALSE;
    }
    gun = pdata[7] - 1;    //提取枪号
    if(gun >= GUN_MAX)
    {
        return FALSE;
    }

    mq_service_send_to_4gsend(APP_SJDATA_QUERY,gun ,0 ,NULL);
    return TRUE;
}

/*****************************************************************************
* Function     : YKC_RecvBalance
* Description  : 更新余额
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   YKC_RecvBalance(uint8_t *pdata,uint16_t len)
{
    uint8_t gun;

    if((pdata == NULL) || (len != sizeof(_YKC_RECV_CMD42)))
    {
        return FALSE;
    }
    gun = pdata[7] - 1;    //提取枪号
    if(gun >= GUN_MAX)
    {
        return FALSE;
    }
    memcpy((uint8_t*)&YKCRecvCmd42[gun],pdata,len);
    Balance[gun] = YKCRecvCmd42[gun].Balance;
    fal_partition_write(YKC_PRIC,0,(uint8_t*)Balance,sizeof(Balance));
    mq_service_send_to_4gsend(APP_UPDADA_BALANCE,gun ,0 ,NULL);

    return TRUE;
}



/*****************************************************************************
* Function     : HY_RecvRecordAck
* Description  : 订单应答
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   YKC_RecvRecordAck(uint8_t *pdata,uint16_t len)
{
    uint8_t i;
    if(len != 17)
    {
        return FALSE;
    }
//    //比较交易流水号
//    for(i = 0;i < 16;i++)
//    {
//    	if(YKCRecvCmd34[GUN_A].SerialNum[i] != pdata[i])
//    	{
//    		break;
//    	}
//    }
//    if(i == 16)
//    {
//    	//与A枪交易流水号一致
//    	if(pdata[16] == 0)
//    	{
//    		ResendBillControl[GUN_A].ResendBillState = RT_FALSE;			//订单确认，不需要重发订单
//    		ResendBillControl[GUN_A].SendCount = 0;
//    		WriterFmBill(GUN_A,0);
//    	}
//    	return RT_TRUE;
//    }


//    for(i = 0;i < 16;i++)
//    {
//    	if(YKCRecvCmd34[GUN_B].SerialNum[i] != pdata[i])
//    	{
//    		break;
//    	}
//    }
//    if(i == 16)
//    {
//    	//与A枪交易流水号一致
//    	if(pdata[16] == 0)
//    	{
//    		ResendBillControl[GUN_B].ResendBillState = RT_FALSE;			//订单确认，不需要重发订单
//    		ResendBillControl[GUN_B].SendCount = 0;
//    		WriterFmBill(GUN_B,0);
//    	}
//    	return RT_TRUE;
//    }
//

    ResendBillControl[GUN_A].ResendBillState = FALSE;			//订单确认，不需要重发订单
    ResendBillControl[GUN_A].SendCount = 0;
    WriterFmBill(GUN_A,0);
    return TRUE;
}


/*****************************************************************************
* Function     : AP_RecvTime
* Description  : 校准时间
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2021年3月19日
*****************************************************************************/
static uint8_t   YKC_RecvTime(uint8_t *pdata,uint16_t len)
{
    uint8_t times = 3;							//如果设置失败反复设置三次

    uint8_t Second;
    if(len != (sizeof(YKCRecvCmd56)) )
    {
        return FALSE;
    }
    memcpy((uint8_t*)&YKCRecvCmd56,pdata,len);

    Second = YKCRecvCmd56.CurTime.Second / 1000;

    set_date(YKCRecvCmd56.CurTime.Year + 2000,YKCRecvCmd56.CurTime.Month,YKCRecvCmd56.CurTime.Day);
    set_time(YKCRecvCmd56.CurTime.Hour,YKCRecvCmd56.CurTime.Minute,Second);



    return TRUE;
}




/***************************************************************
**Function   :APP_GetYKCStartType
**Description:获取YKC启动方式
**Input      :gun: [输入/出]
**			 type: [输入/出]
**Output     :
**Return     :
**note(s)    :
**Author     :CSH
**Create_Time:2023-02-20
***************************************************************/
uint8_t   APP_GetYKCStartType(uint8_t gun)
{
    if(gun >= GUN_MAX)
    {
        return _4G_APP_START;
    }
    return (uint8_t)YKCStartType[gun];
}


/***************************************************************
**Function   :APP_SetYKCStartType
**Description:设置YKC启动方式
**Input      :gun: [输入/出]
**			 type: [输入/出]
**Output     :
**Return     :
**note(s)    :
**Author     :CSH
**Create_Time:2023-02-20
***************************************************************/
uint8_t   APP_SetYKCStartType(uint8_t gun ,_4G_START_TYPE  type)
{
    if((type >=  _4G_APP_MAX) || (gun >= GUN_MAX))
    {
        return FALSE;
    }

    YKCStartType[gun] = type;
    return TRUE;
}




uint8_t  APP_YKCSENDFEILV(void)
{
    //连续空闲3个小时或者5个小时
    static uint8_t QUENUM = 0; //请求次数
    static uint32_t  current_time = 0,last_time = 0;

    if(stChTcb.ucState == STANDBY)
    {
        current_time = OSTimeGet(&timeerr);  	//判断是否是连续空闲
    }
    else
    {
        current_time = OSTimeGet(&timeerr);   //不连续空闲就清空时间
        last_time = OSTimeGet(&timeerr);
    }


    if(checkYKConline == 0)
    {
        checkYKConline = 1;
        current_time = OSTimeGet(&timeerr);   //不连续空闲就清空时间
        last_time = OSTimeGet(&timeerr);
    }


    if(APP_GetAppRegisterState(LINK_NUM) == STATE_OK)  //注册成功后 联网的状态下
    {
        if((((current_time > last_time) ? (current_time - last_time):((0xFFFFFFFF - last_time) + current_time)) > CH_TIME_60_SEC) &&(checkYKConline == 1))
        {
            last_time = current_time;
            YKC_SendPriReq();    //请求一下费率，有可能不回复,下面就是不断的发送
            checkYKConline = 2;
            QUENUM = 0;
        }
        else if((((current_time > last_time) ? (current_time - last_time):((0xFFFFFFFF - last_time) + current_time)) > CH_TIME_5_SEC)&&(checkYKConline ==2))
        {
            last_time = current_time;
            YKC_SendPriReq();  //请求一下费率
            QUENUM++;
            if(QUENUM >10)
            {
                APP_SetNetNotConect(0);		//关闭网络操作
                checkYKConline = 1;
            }
        }
        else if(checkYKConline ==3)   //正常返回
        {
            checkYKConline = 1;
        }
    }

    return TRUE;
}



#endif
/************************(C)COPYRIGHT 2020 杭州汇誉*****END OF FILE****************************/

