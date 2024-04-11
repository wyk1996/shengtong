/*****************************************Copyright(C)******************************************
*******************************************杭州汇誉*********************************************
*------------------------------------------文件信息---------------------------------------------
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
#include "APFrame.h"
#include "DwinProtocol.h"
#include "dwin_com_pro.h"
#include "main.h"
/* Private define-----------------------------------------------------------------------------*/
#define   GPRSSEND_Q_LEN  								20
/* Private typedef----------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables--------------------------------------------------------------------------*/


uint8_t OFFlineBuf[300];		//离线交易记录写入读取换成
uint8_t OFFFSlineBuf[300];	//离线分时交易记录写入读取换成

OS_Q Send4GMq;
/*****************************************************************************
* Function     : Task4GSend
* Description  : 4G发送任务
* Input        : void
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年6月14日
*****************************************************************************/
void AppTask4GSend(void *p_arg)
{
    OS_ERR err;
    MQ_MSG_T stMsg = {0};
    static uint8_t sendbuf[10];
    uint8_t* pBillGunA = APP_GetBillInfo(GUN_A);
    uint8_t i = 0;
    static uint32_t curtime = 0,lasttime = 0;

    OSTimeDly(2000, OS_OPT_TIME_PERIODIC, &timeerr);
    if(DisSysConfigInfo.standaloneornet != DISP_NET)
    {
        return;
    }

    OSQCreate (&Send4GMq,
               "4G send mq",
               20,
               &err);
    if(err != OS_ERR_NONE)
    {
        printf("OSQCreate %s Fail", "4G send mq");
        return;
    }
    //mq_service_bind(CM_4GSEND_MODULE_ID,"4G send mq");
    while(1)
    {
        if(mq_service_recv_msg(&Send4GMq,&stMsg,sendbuf,sizeof(sendbuf),CM_TIME_1_SEC) == 0 )
        {
#if(UPDATA_STATE)
            if(APP_GetSIM7600Mode() == MODE_HTTP)   //远程升级其他无关数据帧都不不要发送和处理
            {
                OSTimeDly(1000, OS_OPT_TIME_PERIODIC, &timeerr);
                continue;
            }
#endif
            switch(stMsg.uiMsgCode)   //消息码
            {
            case APP_START_ACK:		//开始充电应答
                if((APP_GetSIM7600Status() == STATE_OK) && (APP_GetModuleConnectState(0) == STATE_OK)) //连接上服务器
                {
                    if(stMsg.uiMsgVar < GUN_MAX) //枪号保护
                    {
                        _4G_SendStartAck((_GUN_NUM)stMsg.uiMsgVar);
                        if(NetConfigInfo[NET_YX_SELCT].NetNum > 1)
                        {
#if(NET_YX_SELCT != XY_AP)
                            ZF_SendStartCharge();				//发送启动帧
#else
                            __NOP();
#endif
                        }
                    }
                }
                break;

            case APP_STOP_ACK:		//停止充电应答
                if((APP_GetSIM7600Status() == STATE_OK) && (APP_GetModuleConnectState(0) == STATE_OK)) //连接上服务器
                {
                    if(stMsg.uiMsgVar < GUN_MAX) //枪号保护
                    {
                        _4G_SendStopAck((_GUN_NUM)stMsg.uiMsgVar);
                        //HFQG_SendBill((_GUN_NUM)pMsg->DataLen);    //发送结算
                    }
                }
                break;
            case APP_STOP_BILL:		//停止结算
                //if((APP_GetSIM7600Status() == STATE_OK) && (APP_GetModuleConnectState(0) == STATE_OK)) //连接上服务器
                //{

                if(stMsg.uiMsgVar < GUN_MAX) //枪号保护
                {
#if(NET_YX_SELCT == XY_AP)
                    {
                        if(APP_GetStartNetState(stMsg.uiMsgVar) == NET_STATE_ONLINE)
                        {
                            //在线记录
                            WriterFmBill(stMsg.uiMsgVar,1);
                            APP_SetResendBillState(stMsg.uiMsgVar,TRUE);
                            _4G_SendBill((_GUN_NUM)stMsg.uiMsgVar);
                        }
                        else
                        {
                            //不管A,B枪的离线交易记录都用一个，统一用A
                            ResendBillControl[GUN_A].OffLineNum = APP_GetNetOFFLineRecodeNum();
                            if(ResendBillControl[GUN_A].OffLineNum < 100)
                            {
                                ResendBillControl[GUN_A].OffLineNum++;			//离线交易记录+1
                                APP_SetNetOFFLineRecodeNum(ResendBillControl[GUN_A].OffLineNum);
                                PreAPOffLineBill(stMsg.uiMsgVar,OFFlineBuf); 		//读取离线交易记录
                                PreAPFSOffLineBill(stMsg.uiMsgVar,OFFFSlineBuf);		//离线分时交易记录
                                APP_RWNetOFFLineRecode(ResendBillControl[GUN_A].OffLineNum,FLASH_ORDER_WRITE,OFFlineBuf);
                                APP_RWNetFSOFFLineRecode(ResendBillControl[GUN_A].OffLineNum,FLASH_ORDER_WRITE,OFFFSlineBuf);
                            }
                        }
                    }
#else
                    {
                        //OSTimeDly(SYS_DELAY_3s);
                        WriterFmBill(stMsg.uiMsgVar,1);
                        APP_SetResendBillState(stMsg.uiMsgVar,TRUE);
                        _4G_SendBill((_GUN_NUM)stMsg.uiMsgVar);
                        _4G_SetStartType((_GUN_NUM)stMsg.uiMsgVar,_4G_APP_START);			//设置为APP
                    }
#endif
                }
                //}
                if(NetConfigInfo[NET_YX_SELCT].NetNum > 1)
                {
#if(NET_YX_SELCT != XY_AP)
                    ZF_SendBill();
#else
                    __NOP();
#endif

                }

                break;
            case	APP_STE_DEVS:     //查询设备状态
                _4G_SendDevState(stMsg.uiMsgVar);			//发送设备状态
                break;
            case	APP_UPDADA_BALANCE:     //更新余额

#if(NET_YX_SELCT == XY_YKC)
            {
                YKC_SendBalanceAck(stMsg.uiMsgVar);			//更新余额
            }
#endif
            break;
#if(NET_YX_SELCT == XY_YKC)
            case	APP_SJDATA_QUERY:     //读取实时数据
                if((APP_GetSIM7600Status() == STATE_OK) && (APP_GetModuleConnectState(0) == STATE_OK)) //连接上服务器
                {
                    if(stMsg.uiMsgVar == GUN_A)
                    {
                        YKC_SendSJDataGunACmd13();
                    }

                }
                break;
#endif
            case	APP_STE_BILL:     //查询订单
                if(stMsg.uiMsgVar == GUN_A)
                {
                    ReSendBill(GUN_A,pBillGunA,1);
                }

                break;
            case APP_RATE_ACK:		//停止结算
                _4G_SendRateAck(stMsg.uiMsgVar);	 //费率设置应答
                break;
            case APP_QUERY_RATE:	//费率请求
                _4G_SendQueryRate();
                break;
            case APP_RATE_MODE:		//计费模型验证
                _4G_SendRateMode();
                break;
#if NET_YX_SELCT == XY_HY
            case APP_STE_RATE:			//费率应答
                HY_SendQueryRateAck();
                break;
#endif
            case APP_STE_TIME:			//校准时间
                _4G_SendSetTimeAck();
                break;

            case APP_CARDVIN_CHARGE:
                _4G_SendCardVinCharge(stMsg.uiMsgVar);   //刷卡启动上报
                break;
            case APP_CARD_INFO:						//卡鉴权
                _4G_SendCardInfo(stMsg.uiMsgVar);
                break;

            case APP_VIN_INFO:						//Vin鉴权
                _4G_SendVinInfo(stMsg.uiMsgVar);
                break;
            case APP_CARD_WL:			//卡白名单
#if(NET_YX_SELCT == XY_AP)
            {
                AP_SendCardWLAck();
            }
#endif
            break;
            case APP_VINCARD_RES:
#if(NET_YX_SELCT == XY_AP)
            {
                AP_SendVinCardResAck();
            }
#endif
            break;
            case APP_OFFLINE_ACK:
#if(NET_YX_SELCT == XY_AP)
            {
                if(ResendBillControl[GUN_A].OffLineNum)
                {
                    //离线记录应答了 -1
                    ResendBillControl[GUN_A].OffLineNum--;
                    APP_SetNetOFFLineRecodeNum(ResendBillControl[GUN_A].OffLineNum);
                }
            }
#endif
            break;
            case BSP_4G_SENDNET1:
                if(NetConfigInfo[NET_YX_SELCT].NetNum > 1)
                {
                    Send_AT_CIPRXGET(0);   //主动读取数据
                }
                break;
            case BSP_4G_SENDNET2:
                if(NetConfigInfo[NET_YX_SELCT].NetNum > 1)
                {
                    Send_AT_CIPRXGET(1);	//主动读取数据
                }
                break;
            default:
                break;

            }
        }
#if(UPDATA_STATE)
        if(APP_GetSIM7600Mode() == MODE_HTTP)   //远程升级其他无关数据帧都不不要发送和处理
        {
            OSTimeDly(1000, OS_OPT_TIME_PERIODIC, &timeerr);
            continue;
        }
#endif

        for(i = 0; i < NetConfigInfo[NET_YX_SELCT].NetNum; i++)
        {
            if((APP_GetSIM7600Status() == STATE_OK) && (APP_GetModuleConnectState(i) == STATE_OK)) //连接上服务器
            {
                //发送数据给服务器
                //ModuleSIM7600_SendData(i, (uint8_t*)"hello word qiangge\r\n", strlen("hello word qiangge\r\n"));
                if(i == 0)
                {
                    _4G_SendFrameDispose();  //合肥乾古周期性发送帧
                }
                else
                {
#if(NET_YX_SELCT != XY_AP)
                    ZF_SendFrameDispose();
#else
                    __NOP();
#endif
                }
            }
        }
        if (NetConfigInfo[NET_YX_SELCT].NetNum > 1)
        {
            //每10s读取一次信号强度
            if((curtime >= lasttime) ? ((curtime - lasttime) >= CM_TIME_10_SEC) : \
                    ((curtime + (0xffffffffU - lasttime)) >= CM_TIME_10_SEC))
            {
                lasttime = curtime;
                Send_AT_CSQ();				//读取信号强度
            }
        }
        //处理发送订单
        for(i = 0; i < GUN_MAX; i++)
        {
            if(APP_GetResendBillState(i) == TRUE)
            {
                if((APP_GetSIM7600Status() == STATE_OK) && (APP_GetModuleConnectState(0) == STATE_OK) && (APP_GetAppRegisterState(0) == STATE_OK)	) //连接上服务器
                {
                    if(i == GUN_A)
                    {
                        ReSendBill(i,pBillGunA,0);
                    }
                }
            }
            else
            {
                ResendBillControl[i].CurTime = OSTimeGet(&timeerr);		//获取当前时间
                ResendBillControl[i].LastTime = ResendBillControl[i].CurTime;
            }
            if((APP_GetSIM7600Status() == STATE_OK) && (APP_GetModuleConnectState(i) == STATE_OK) && (APP_GetAppRegisterState(i) == STATE_OK)	) //连接上服务器
            {
#if(NET_YX_SELCT == XY_AP)
                {
                    ReSendOffLineBill();      //离线交易记录获取
                }
#endif
            }
        }
		
		//YKC协议不停的请求费率，确保连接服务器端正常连接
		
		
		
		
    }
}



/************************(C)COPYRIGHT 2022 杭州汇誉*****END OF FILE****************************/

