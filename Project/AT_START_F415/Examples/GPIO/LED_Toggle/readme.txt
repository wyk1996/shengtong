/********************************************************************************
=================更新记录：【1】22/09/25-19/15=================
（1）合并YXY，修改了喂狗时间。
（2）修改了修改协议IP，可以保存和读取
（3）YKC_RecvFrameDispose();  YKC接收帧处理和AT大板子相同


=================更新记录：【1】22/09/27-09/05=================
	  /**加一个whil 防止包与包之间相粘，不加这个一定会应答不了**/
	  if((pdata == NULL) || (len < 8) )
    {
        return FALSE;
    }


=================更新记录：【1】22/10/17=================
uint8_t ch_is_under_volt(CH_TASK_T *pstChTcb)
 if(OSTimeGet(&timeerr) - pstChTcb->stUnderVolt.uiTick < CH_TIME_10_SEC)  //大于10s后，返回
 
 
 