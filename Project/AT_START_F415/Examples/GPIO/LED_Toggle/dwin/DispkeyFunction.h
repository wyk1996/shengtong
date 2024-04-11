/*****************************************Copyright(C)******************************************
****************************************************************************************
*------------------------------------------文件信息---------------------------------------------
* FileName			: Dispkey.h
* Author			:
* Date First Issued	:
* Version			:
* Description		:
*----------------------------------------历史版本信息-------------------------------------------
* History			:
* Description		:
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/
#ifndef _DISP_KEY_FUNCTION_H
#define _DISP_KEY_FUNCTION_H
#include "DispKey.h"
/* Includes-----------------------------------------------------------------------------------*/

/* Private typedef----------------------------------------------------------------------------*/
/*****************************************************************************
* Function     : GetCurMenu
* Description  : 获取液晶当前菜单结构体
* Input        : void
* Output       : None
* Return       :
* Note(s)      :
* Contributor  :  2018年6月14日
*****************************************************************************/
ST_Menu* GetCurMenu(void);

/*****************************************************************************
* Function     : GetCurMenu
* Description  : 设置液晶当前菜单结构使
* Input        : void
* Output       : None
* Return       :
* Note(s)      :
* Contributor  :  2018年7月17日
*****************************************************************************/
void SetCurMenu(ST_Menu*  pmenu);


void HYDisplayMenu25(void);
/********************************************************************
* Function Name : DisplayMenu1
* Description   : 主界面页面显示
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :  2018年6月14日
********************************************************************/
void HYDisplayMenu1(void);

/********************************************************************
* Function Name : DisplayMenu8
* Description   : 付费方式选择显示
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu2(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu3(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu4(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu5(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu6(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu7(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu8(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu9(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu10(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu11(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu12(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu13(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu14(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu15(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu16(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu17(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu18(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu19(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu20(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu21(void);


/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu22(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu23(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu24(void);


/********************************************************************
* Function Name : DisplayMenu1
* Description   :
* Input         :
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void HYDisplayMenu26(void);
void HYDisplayMenu27(void);
void HYDisplayMenu28(void);
void HYDisplayMenu30(void);
void HYDisplayMenu31(void);
void HYDisplayMenu32(void);
void HYDisplayMenu33(void);
void HYDisplayMenu34(void);
void HYDisplayMenu35(void);
void HYDisplayMenu36(void);
void HYDisplayMenu37(void);
void HYDisplayMenu38(void);
/********************************************************************
* Function Name : DisplayCommonMenu()
* Description   :
* Input         :
*
* Output        : None.
* Return        : None.
* Contributor   :
********************************************************************/
void DisplayCommonMenu(struct st_menu* pMenu,struct st_menu * pPrepage);
#endif	//__KEY_H_
/************************(C)COPYRIGHT 2018 *****END OF FILE****************************/
