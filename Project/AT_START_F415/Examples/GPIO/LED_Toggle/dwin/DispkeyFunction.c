/*******************************************************************************
 *          Copyright (c) 2020-2050,  Co., Ltd.
 *                              All Right Reserved.
 * @file
 * @note
 * @brief
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
#include "DispKey.h"
#include "dwin_com_pro.h"
#include "DwinProtocol.h"
#include "stdio.h"

ST_Menu *gps_CurMenu = NULL;

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
ST_Menu* GetCurMenu(void)
{
    return gps_CurMenu;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void SetCurMenu(ST_Menu* pmenu)
{
    if(pmenu == NULL)
    {
        return;
    }

    gps_CurMenu = pmenu;

}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu25(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu1(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu2(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu3(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu4(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu5(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu6(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu7(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu8(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu9(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu10(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu11(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu12(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu13(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu14(void)
{
    ;
}


/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu15(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu16(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu17(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu18(void)
{
    ;
}


/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu19(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu20(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu21(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu22(void)
{
    ;
}


/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu23(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu24(void)
{
    ;
}


/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void HYDisplayMenu26(void)
{
    ;
}

void HYDisplayMenu27(void)
{
    ;
}
void HYDisplayMenu28(void)
{
    ;
}

void HYDisplayMenu30(void)
{
    ;
}

void HYDisplayMenu31(void)
{
    ;
}

void HYDisplayMenu32(void)
{
    ;
}

void HYDisplayMenu33(void)
{
    ;
}


void HYDisplayMenu34(void)
{
    ;
}

void HYDisplayMenu35(void)
{
    ;
}

void HYDisplayMenu36(void)
{
    ;
}

void HYDisplayMenu37(void)
{
    ;
}

void HYDisplayMenu38(void)
{
    ;
}

/**
 * @brief
 * @param[in]
 * @param[out]
 * @return
 * @note
 */
void DisplayCommonMenu(struct st_menu* pMenu,struct st_menu * pPrepage)
{
    if(pMenu == NULL)
    {
        return;
    }
    if(pPrepage != NULL)
    {
        pMenu->Menu_PrePage = pPrepage;
    }
    gps_CurMenu = pMenu;
    DispControl.CountDown = pMenu->CountDown;

//      PrintNum16uVariable(ADDR_COUNTDOWN_TIME,pMenu->CountDown);		//显示倒计时地址
    PrintBackImage(gps_CurMenu->FrameID);

    if(gps_CurMenu->function3 && (gps_CurMenu->function3 != (void (*))0xFFFFFFFF))
    {
        gps_CurMenu->function3();
    }
}
