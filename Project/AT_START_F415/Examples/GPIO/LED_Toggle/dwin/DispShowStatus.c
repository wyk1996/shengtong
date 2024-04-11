/*******************************************************************************
 *          Copyright (c) 2020-2050,  Co., Ltd.
 *                              All Right Reserved.
 * @file DispStatus.c
 * @note
 * @brief �ṩϵͳ״̬��Ϣ,�ṩ��ʾ����ʾ����ʹ��,���ı���ʽΪANSI
 *
 * @author
 * @date     2021-05-02
 * @version  V1.0.0
 *
 * @Description
 *
 * @note History:
 * @note     <author>   <time>    <version >   <desc>
 * @note
 * @warning
 *******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "DispShowStatus.h"
#include "DispKeyFunction.h"
#include "MenuDisp .h"
#include "DwinProtocol.h"
#include "ch_port.h"


typedef struct
{
    _SHOW_NUM ShowNum;   //��ʾ״̬
    const char *pStr;    //��ַ
    uint16_t ShowLen;    //����
} _SHOW_STATUS;

static const _SHOW_STATUS ShowStatus[SHOW_MAX] =
{
    {SHOW_XY_HY,			"����"		,4 },
    {SHOW_XY_YKC,			"�ƿ��"	,6 },
    {SHOW_XY_AP,			"����"		,4 },
    {SHOW_XY_HFQG,		"�Ϸ�Ǭ��"		,8 },
    {SHOW_XY_XH,			"���"		,4 },     //С�����
    {SHOW_XY_SY,			"����"		,4 },
    {SHOW_XY_TD,			"����"		,4 },
    {SHOW_XY_JG,			"���۳�"		,6},  
    {SHOW_XY_YC,			"���̶�IP"  ,10 },
    {SHOW_START_APP,  "APP����",	    7},
    {SHOW_START_CARD,  "ˢ������",      8},
    {SHOW_BILL,  			"�ѽ���",	6},
    {SHOW_NOTBIL,  		"δ����",		6},
    {SHOW_STOP_ERR_NONE,	"����ֹͣ",	8},
    {SHOW_STOP_FAIL,	  	"����ֹͣ",	8},
    {SHOW_STOP_LESS,	  	"����",	8},
    {SHOW_GUNA,  			"Aǹ",		3},
    {SHOW_GUNB,  		  "Bǹ",		3},
};

/**
 * @brief ��ʾʱ��
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
//uint8_t Dis_ShowTime(uint16_t add_show ,_BSPRTC_TIME gRTC)
//{
//	char * buf[30];
//	ST_Menu* CurMenu = GetCurMenu();
//	if(CurMenu == NULL)
//	{
//		return FALSE;
//	}
//	memset(buf,0,sizeof(buf));
//
//	snprintf((char *)buf, sizeof(buf), "20%02x-%02x-%02x %02x:%02x:%02x",(gRTC.Year),(gRTC.Month),(gRTC.Day),\
//		(gRTC.Hour),(gRTC.Minute),(gRTC.Second));


//	PrintStr(add_show,(uint8_t *)buf,30);   //��ʾ״̬
//	SetVariColor((add_show & 0xFFC0 ) + 0x30,WHITE);
//	return TRUE;
//}

/**
 * @brief ��ʾ״̬
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
uint8_t Dis_ShowStatus(uint16_t add_show,_SHOW_NUM show_num )
{
    uint8_t i = 0;
    ST_Menu* CurMenu = GetCurMenu();
    if((show_num >= SHOW_MAX) || (CurMenu == NULL) )
    {
        return 0;
    }

    for(i = 0; i < SHOW_MAX; i++)
    {
        if(ShowStatus[i].ShowNum == show_num)
        {
            PrintStr(add_show,(uint8_t *)ShowStatus[i].pStr,ShowStatus[i].ShowLen);   //��ʾ״̬
            break;
        }
    }
    return 1;
}

/**
 * @brief ����
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
uint8_t Dis_ShowCopy(uint8_t* pdata,_SHOW_NUM show_num)
{

    uint8_t i = 0;
    ST_Menu* CurMenu = GetCurMenu();
    if((show_num >= SHOW_MAX) || (CurMenu == NULL))
    {
        return 0;
    }

    for(i = 0; i < SHOW_MAX; i++)
    {
        if(ShowStatus[i].ShowNum == show_num)
        {
            memcpy(pdata,(uint8_t *)ShowStatus[i].pStr,ShowStatus[i].ShowLen);
            break;
        }
    }
    return 1;
}




typedef struct
{
    STOP_REASON Stop_reasonnum;  //��ʾ״̬
    const char *pStr;    //��ַ
    uint16_t ShowLen;    //����
} _SHOW_Reason;

const _SHOW_Reason Showstopreason[STOP_MAX] =
{
    {NORMAL,			 "����ֹͣ"	 ,8 },
    {R_POWER_DOWN,	"�޵���"	,6 },
    {LEAKAGE_FAULT,	 "©�����"	,8},
    {SPD_FAULT,		  "�޽��"	,6},
    {OTHER_FAULT,		"��������"	,8 },
    {DOOR_OPEN,			"���Ѵ�"	,8},
    {EM_STOP,			"��ͣ����"		,8 },
    {OVER_VOLT,			"��ѹ"		,4 },
    {OVER_CURRENT,  "����",	4},
    {READY_TIMEOUT,  "����ʧ��",8},
    {CP_LINK_ABN,  		"CP�쳣",	6},
    {UNPLUG,  		    "�Ƿ���ǹ",		8},
    {END_CONDITION,	"ѭ���洢",	8},
    {NO_CURRENT,	  "�޵���6V",	8},
    {NO_CURRENT_9V,	  "�޵���9V",	8},
    {E_END_BY_APP,	 "App����",	7},
    {E_END_BY_CARD,  	"ˢ������",		8},
    {UNDER_VOLT,  		 "Ƿѹ",		4},
    {STOP_CHARGEFULL,	 "����",	4},
    {STOP_OTHEN,  	"����",		4},
    {E_END_APP_BALANC,  "����",8}
};



//=======��ʾֹͣȫ��ԭ��================
uint8_t Dis_Showstop_reason(uint8_t* pdata,STOP_REASON Stop_reasonnum)
{

    uint8_t i = 0;
    ST_Menu* CurMenu = GetCurMenu();
    if((Stop_reasonnum >= STOP_MAX) || (CurMenu == NULL))
    {
        return 0;
    }

    for(i = 0; i < STOP_MAX; i++)
    {
        if(Showstopreason[i].Stop_reasonnum == Stop_reasonnum)
        {
            memcpy(pdata,(uint8_t *)Showstopreason[i].pStr,Showstopreason[i].ShowLen);
            break;
        }
    }
    return 1;
}





