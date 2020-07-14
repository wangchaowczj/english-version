#include "includes.h"


u8 WaitVerifyCode[6];

/**
  *************************************************************************************
  * @brief  根据批号转出芯片类型
  * @param[in]  Type 雷管批号
  * @retval 类型
  * @author ZH
  * @date   2016年7月6日
  * @note   
  *************************************************************************************  
  */
u8 XA_GetDetTypeFromBat(u8 Bat)
{
	return (Bat>>5);//高3bit表示雷管类型
}
/**
  *************************************************************************************
  * @brief  根据芯片类型判断是否为单电容类型
  * @param[in]  Type 雷管类型
  * @retval 0不是单电容，其他值表示单电容
  * @author ZH
  * @date   2016年7月6日
  * @note   
  *************************************************************************************  
  */
u8 XA_DetIsSingleCap(u8 Type)
{
	if(Type == DET_TYPE_XA_BD1)
	{
		return 1;
	}
	return 0;
}
/**
  *************************************************************************************
  * @brief  根据芯片类型判断是否为固定延期类型
  * @param[in]  Type 雷管类型
  * @retval 0不是固定延期，其他值表示固定延期
  * @author ZH
  * @date   2016年7月6日
  * @note   
  *************************************************************************************  
  */
u8 XA_DetIsFixDelay(u8 Type)
{
	if((Type == DET_TYPE_XA_BD1) || (Type == DET_TYPE_XA_BD2))
	{
		return 1;
	}
	return 0;	
}



u16 XA_GetDetPwd(u8 Serl_1, u8 Serl_2, u8 Serl_3, u8 Factor, u8 Bat, u8 Year)
{
	const u8 Bytes = 6;
	u8 Buffer[6];
	u8 DetPwd[2];
	u8 Sum = 0;
	u8 i;
	
	if((Serl_1 == 0) && (Serl_2 == 0) && (Serl_3 == 0) && (Factor == 0) && (Bat == 0) && (Year == 0))
	{//还没有写入内码
		return 0x0000;
	}	
	Buffer[0] = Serl_1;
	Buffer[1] = Serl_2;
	Buffer[2] = Serl_3;
	Buffer[3] = Factor;
	Buffer[4] = Bat;
	Buffer[5] = Year;
	GetCrc16Value(0xFFFF, &DetPwd[0], &Buffer[0], 6);

	for(i=0; i<Bytes; i++)
	{
		Sum += Buffer[i];
	}
	DetPwd[0] ^= Sum;
	DetPwd[1] ^= Sum;	
	return ((((u16)DetPwd[0])<<8) + DetPwd[1]);
}
/**
  *************************************************************************************
  * @brief  密码测试
  * @param  TimeOut 超时时间
  * @retval SUCCESS 成功， ERROR失败
  * @author ZH
  * @date   2018年3月26日
  * @note   无
  *************************************************************************************
  */
ErrorStatus XA_DetPwdTest(DET_INFO *det)
{
	u8 Pwd1, Pwd2;
    ErrorStatus result;
    
    Pwd1 = det->Pwd1;
    Pwd2 = det->Pwd2;    
	result = XA_DetUnLockPwd(det->ser1,det->ser2,det->ser3, Pwd1, Pwd2);
	if(result == ERROR )
	{
        Pwd1 = DET_PWD_FIX_VALUE&0xFF;
        Pwd2 = (DET_PWD_FIX_VALUE>>8)&0xFF;
        if((det->Pwd1 != Pwd1) || (det->Pwd2 != Pwd2))
        {
            result = XA_DetUnLockPwd(det->ser1,det->ser2,det->ser3, Pwd1, Pwd2);
        }		
	}
    if(result == ERROR)
    {
        return ERROR;//密码错误
    }

    det->Pwd1 = Pwd1;
    det->Pwd2 = Pwd2; 
    
    return SUCCESS;
}
void XA_SaveDetInfo(u8 Index)
{
	u16 DetPwd;
	
	OSSchedLock();
	
	Channel.Info[Index].ser1 = DetProtocol.RecData.Data[1];   //保存雷管身份信息
    Channel.Info[Index].ser2 = DetProtocol.RecData.Data[2];
    Channel.Info[Index].ser3  = DetProtocol.RecData.Data[3];
    Channel.Info[Index].fac = DetProtocol.RecData.Data[4];
    Channel.Info[Index].bat  = DetProtocol.RecData.Data[5];
    Channel.Info[Index].year = DetProtocol.RecData.Data[6];
    Channel.Info[Index].freq = GetDetFreqFromHead();
	Channel.Info[Index].type = Channel.Info[Index].bat>>5;
	
	DetPwd = XA_GetDetPwd(Channel.Info[Index].ser1, Channel.Info[Index].ser2, Channel.Info[Index].ser3, Channel.Info[Index].fac, Channel.Info[Index].bat, Channel.Info[Index].year);
	
	Channel.Info[Index].Pwd1 = DetPwd&0xFF;
	Channel.Info[Index].Pwd2 = DetPwd>>8;
	OSSchedUnlock();
}




DET_ERR_CODE XA_DetTypeTest(u8 DetType)
{        
	if((DetType&0x07) == (UserParam.DetType&0x07))
	{
		return DET_PASS;
	}
	else 
	{
		return DET_TYPE_ERR; 
	} 					
}

DET_ERR_CODE XA_DetBitTest(DET_INFO* det)
{
    unsigned char /*retry,*/retry1;
	ErrorStatus result;
	DET_ERR_CODE  err_code = DET_PASS;
	
	retry1=3;
	do
	{  
		XA_DetClrAll();	     //发送全局复位指令
		if(XA_DetIsSingleCap(XA_GetDetTypeFromBat(det->bat)) == 0)
		{//双电容
			XA_DetBit(det->ser1);     //发送BIT1，不判断返回
			XA_DetBit(det->ser1);     //发送BIT1，不判断返回
			OSTimeDlyHMSM(0,0,1,500);//
		}		
 		       
		result = XA_DetStatusRd(det->ser1,det->ser2,det->ser3);	 //发送延期时间回读指令，延时回读错误情况下，返回1
		if(result == ERROR )
		{//状态回读出现错误
			err_code = DET_BIT_STATUS_READ_ERR;	
		}
		else
		{
			if(XA_DetIsSingleCap(XA_GetDetTypeFromBat(det->bat)))
			{//单电容
				if(XA_GetDetStatus(BIT_OUT_STATUS)== ERROR)
				{
					err_code = DET_BIT_ERR;	//Bit错误
				}
				else
				{
					err_code = DET_PASS;//BIT测试通过
				}						
			}
			else
			{//双电容
				if(XA_GetDetStatus(BIT_STATUS)== ERROR)
				{
					err_code = DET_BIT_ERR;	//Bit错误
				}
				else
				{
					err_code = DET_PASS;//BIT测试通过
				}			
			}
		}
	}while((result == ERROR)&&(--retry1));

	return err_code;
}

DET_ERR_CODE XA_DetChargeTest(DET_INFO* det)
{
	//u8 retry=3;
	ErrorStatus result;
	
	result = XA_DetPwdTest(det);
	if(result == ERROR )
	{
		return DET_PASSWORD_ERR;//密码错误
	}
	XA_DetCharge(det->ser1);	   //发送CHARGE
	XA_DetCharge(det->ser1);	   //发送CHARGE
	OSTimeDly(200);
	WaitRxPinReady(4000);
//	OSTimeDlyHMSM(0,0,0,90);
	
	result = XA_DetStatusRd(det->ser1,det->ser2,det->ser3);
	if(result == ERROR )
	{//出现错误
		return DET_CHARGE_STATUS_READ_ERR;		//BB
	}
	else
	{
		if(XA_GetDetStatus(CHARGE_STATUS) == ERROR)
		{	
			return DET_CHARGE_ERR;	//充电错误		
		}
		else
		{
			return DET_PASS;//充电测试通过
		}
	}
}

DET_ERR_CODE XA_DetExpenTest(DET_INFO* det)
{
	ErrorStatus result;
	
	XA_DetExpen();                  //准备指令
	XA_DetExpen();                  //准备指令

	OSTimeDlyHMSM(0,0,0,20);	
	result = XA_DetStatusRd(det->ser1,det->ser2,det->ser3);
	if(result == ERROR )
	{//延期时间回读出现错误
		return DET_EXPEN_TIMERD_ERR;		//BB
	}
	else
	{
		if(XA_GetDetStatus(EXPEN_STATUS) == ERROR)
		{
			return DET_EXPEN_ERR;	//准备起爆错误			
		}
		else
		{
			return DET_PASS;//准备起爆测试通过
		}
	}
}

DET_ERR_CODE XA_DetWriteCode(u8 Index)
{
	u16 DetPwd;
	u8 i = 0;
	u8 Buffer[8];
	
	memcpy((char*)&WaitVerifyCode[0], (char*)&Channel.BarCode[Index].DetCode[0], 6);
	//WaitVerifyCode[3] = (Channel.BarCode[Index].DetCode[3]&0xF0) + (Channel.Info[Index].fac&0x0F);//工厂号 ,低4位表示晶圆批号，不修改
	
//	DetPwd = GetDetPwd(WaitVerifyCode[0], WaitVerifyCode[1], WaitVerifyCode[2], WaitVerifyCode[3], WaitVerifyCode[4], WaitVerifyCode[5]);
	DetPwd = DET_PWD_FIX_VALUE;
    
	Buffer[i++] = WaitVerifyCode[0];//序列码1
	Buffer[i++] = WaitVerifyCode[1];//序列码2
	Buffer[i++] = WaitVerifyCode[2];//序列码3
	Buffer[i++] = DetPwd&0xFF;//密码1
	Buffer[i++] = (DetPwd>>8)&0xFF;//密码2
	Buffer[i++] = WaitVerifyCode[3];
	Buffer[i++] = WaitVerifyCode[4];//批号
	Buffer[i++] = WaitVerifyCode[5];//年号
	
	if(XA_DetWrEEPROM(Buffer) == ERROR)
	{
		return DET_WR_CODE_ERR;//写EEPROM错误
	}
	return DET_PASS;
}
DET_ERR_CODE XA_ChangeDetType(u8 Index)
{
	u16 DetPwd;
	u8 i = 0;
	u8 Buffer[8];
		
	WaitVerifyCode[i++] = Channel.Info[Index].ser1;//序列码1
	WaitVerifyCode[i++] = Channel.Info[Index].ser2;//序列码2
	WaitVerifyCode[i++] = Channel.Info[Index].ser3;//序列码3
	WaitVerifyCode[i++] = Channel.Info[Index].fac;
	WaitVerifyCode[i++] = (Channel.Info[Index].bat&0x1F)+ (UserParam.DetType<<5);//批号,高3位表示雷管类型
	WaitVerifyCode[i++] = Channel.Info[Index].year;//年号
	
//	DetPwd = GetDetPwd(WaitVerifyCode[0], WaitVerifyCode[1], WaitVerifyCode[2], WaitVerifyCode[3], WaitVerifyCode[4], WaitVerifyCode[5]);
	DetPwd = DET_PWD_FIX_VALUE;
    
	i = 0;
	Buffer[i++] = WaitVerifyCode[0];//序列码1
	Buffer[i++] = WaitVerifyCode[1];//序列码2
	Buffer[i++] = WaitVerifyCode[2];//序列码3
	Buffer[i++] = DetPwd&0xFF;//密码1
	Buffer[i++] = (DetPwd>>8)&0xFF;//密码2
	Buffer[i++] = WaitVerifyCode[3];
	Buffer[i++] = WaitVerifyCode[4];//批号,高3位表示雷管类型
	Buffer[i++] = WaitVerifyCode[5];//年号
	
	if(XA_DetWrEEPROM(Buffer) == ERROR)
	{
		return DET_WR_TYPE_ERR;//写EEPROM错误
	}
	return DET_PASS;
}
DET_ERR_CODE XA_ChangeDetPwd(u8 Index)
{
	u16 DetPwd;
	u8 i = 0;
	u8 Buffer[8];
		
	WaitVerifyCode[i++] = Channel.Info[Index].ser1;//序列码1
	WaitVerifyCode[i++] = Channel.Info[Index].ser2;//序列码2
	WaitVerifyCode[i++] = Channel.Info[Index].ser3;//序列码3
	WaitVerifyCode[i++] = Channel.Info[Index].fac;
	WaitVerifyCode[i++] = Channel.Info[Index].bat;
	WaitVerifyCode[i++] = Channel.Info[Index].year;//年号
	
//	DetPwd = GetDetPwd(WaitVerifyCode[0], WaitVerifyCode[1], WaitVerifyCode[2], WaitVerifyCode[3], WaitVerifyCode[4], WaitVerifyCode[5]);
	DetPwd = DET_PWD_FIX_VALUE;
    
	i = 0;
	Buffer[i++] = WaitVerifyCode[0];//序列码1
	Buffer[i++] = WaitVerifyCode[1];//序列码2
	Buffer[i++] = WaitVerifyCode[2];//序列码3
	Buffer[i++] = DetPwd&0xFF;//密码1
	Buffer[i++] = (DetPwd>>8)&0xFF;//密码2
	Buffer[i++] = WaitVerifyCode[3];
	Buffer[i++] = WaitVerifyCode[4];//批号,高3位表示雷管类型
	Buffer[i++] = WaitVerifyCode[5];//年号
	
	if(XA_DetWrEEPROM(Buffer) == ERROR)
	{
		return DET_WR_TYPE_ERR;//写EEPROM错误
	}
	return DET_PASS;
}
ErrorStatus XA_VerifyCodeWriteResult(u8 Index)
{
	if ((Channel.Info[Index].ser1 == WaitVerifyCode[0]) && (Channel.Info[Index].ser2 == WaitVerifyCode[1]) && 
		(Channel.Info[Index].ser3== WaitVerifyCode[2]) && (Channel.Info[Index].fac == WaitVerifyCode[3]) && 
		(Channel.Info[Index].bat == WaitVerifyCode[4]) && (Channel.Info[Index].year == WaitVerifyCode[5]))
	{
		return SUCCESS;
	}
	return ERROR;
}


/**
  *************************************************************************************
  * @brief  读序列码
  * @param  ReverseFlag  0时测正向，1测反向
  * @retval 无
  * @author ZH
  * @date   2016年11月21日
  * @note   无
  *************************************************************************************
  */
void XA_AllChannelDetSerlRd(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			if (XA_DetSerlRd() == ERROR)//发送序列码回读指令	                     
			{
				Channel.Info[i].Status = DET_INFO_READ_ERR;//序列码通信错误		
			}
			else
			{
				XA_SaveDetInfo(i);
			}					
		}		
	}	
}
void XA_AllChannelDetSerlRdStatusTest(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.NewStatus[i])
		{
			ConnectChannel(i);
			if (XA_DetSerlRd() == ERROR)//发送序列码回读指令	                     
			{
				Channel.NewStatus[i] = DET_INFO_READ_ERR;//序列码通信错误		
			}					
		}		
	}	
}
/**
  *************************************************************************************
  * @brief  对雷管解密
  * @param  ErrCode  错误时的代码
  * @retval 无
  * @author ZH
  * @date   2016年11月21日
  * @note   无
  *************************************************************************************
  */
void XA_AllChannelDetUnLockPwd(DET_ERR_CODE ErrCode)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			if(XA_DetPwdTest(&Channel.Info[i]) == ERROR)
			{
				Channel.Info[i].Status = ErrCode;//密码错误
			}					
		}		
	}	
}


/**
  *************************************************************************************
  * @brief  雷管类型测试
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年11月21日
  * @note   无
  *************************************************************************************
  */
void XA_AllChannelDetTypeTest(void)
{
	u8 i=0;
	DET_ERR_CODE Result;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{		
			Result = XA_DetTypeTest(Channel.Info[i].type); //判断芯片类型
			if(DET_PASS != Result)
			{//PD1与BD2可以相互转换
				if(((Channel.Info[i].type == DET_TYPE_XA_PD1) && (UserParam.DetType == DET_TYPE_XA_BD2)) ||
					((Channel.Info[i].type == DET_TYPE_XA_BD2) && (UserParam.DetType == DET_TYPE_XA_PD1)))
				{
					Result = DET_PASS;//后边会修改雷管类型
				}
			}
			if(Result != DET_PASS)
			{
				Channel.Info[i].Status = Result;
			}
		}		
	}	

}


/**
  *************************************************************************************
  * @brief  bit测试
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年11月21日
  * @note   无
  *************************************************************************************
  */
void XA_AllChannelDetBitTest(void)  	
{
	u8 i;
	ErrorStatus Result;
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			XA_DetClrAll();	     //发送全局复位指令
            OSTimeDly(50);
			if(XA_DetIsSingleCap(XA_GetDetTypeFromBat(Channel.Info[i].bat)) == 0)
			{//双电容
				XA_DetBit(Channel.Info[i].ser1);     //发送BIT1，不判断返回
				XA_DetBit(Channel.Info[i].ser1);     //发送BIT1，不判断返回
			}		
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			if(XA_DetIsSingleCap(XA_GetDetTypeFromBat(Channel.Info[i].bat)) == 0)
			{//双电容
				OSTimeDlyHMSM(0,0,1,500);//延时等待BIT测试完成,从波形看，需要600ms左右/6.7V, 5.5V,起爆电容68uf要1.3秒
				break;
			}		
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			Result = XA_DetStatusRd(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3);	 //发送延期时间回读指令，延时回读错误情况下，返回1
			if(Result == ERROR )
			{//状态回读出现错误
				Channel.Info[i].Status = DET_BIT_STATUS_READ_ERR;	
			}
			else
			{
				if(XA_DetIsSingleCap(XA_GetDetTypeFromBat(Channel.Info[i].bat)))
				{//单电容
					if(XA_GetDetStatus(BIT_OUT_STATUS)== ERROR)
					{
						Channel.Info[i].Status = DET_BIT_ERR;	//Bit错误
					}					
				}
				else
				{//双电容
					if(XA_GetDetStatus(BIT_STATUS)== ERROR)
					{
						Channel.Info[i].Status = DET_BIT_ERR;	//Bit错误
					}		
				}
			}		
		}
	}
}

/**
  *************************************************************************************
  * @brief  雷管内码验证
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年11月21日
  * @note   无
  *************************************************************************************
  */
void XA_AllChannelDetCodeVerify(void)
{
	u8 i=0;
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)	
		{			
			if(Channel.BarCode[i].Status != BARCODE_NEW)
			{
				Channel.Info[i].Status = DET_WR_CODE_NULL_ERR;
			}				
			else if((Channel.BarCode[i].Result == BARCODE_REPEAT_ERR) || (Channel.BarCode[i].Result == BARCODE_PASS))
			{
				if(memcmp(&Channel.Info[i].ser1, &Channel.BarCode[i].DetCode[0], 6) != 0)
				{
					Channel.Info[i].Status = DET_CODE_NOT_SAME;//内码写条码不一致
				}				
			}
			else
			{
				Channel.Info[i].Status = DET_WR_CODE_NULL_ERR;
			}			
		}
	}
}

void XA_AllChannelDetChargeTest(void)
{
	u8 i;
	
	for(i = 0; i < Channel.Count;i++)
	{
		ConnectChannel(i);
		
		//写延期时间检测
		if(DET_TESTING == Channel.Info[i].Status)
		{
			if(XA_DetTimeWr(Channel.Info[i].ser1,Channel.Info[i].ser2,Channel.Info[i].ser3, 0x55) == ERROR)
			{
				Channel.Info[i].Status = DET_WRITE_DELAY_ERR;      //写延期错误
			}		
		}
		//解密
		if(DET_TESTING == Channel.Info[i].Status)
		{
			if(XA_DetPwdTest(&Channel.Info[i]) == ERROR)
			{
				Channel.Info[i].Status = DET_PASSWORD_ERR;//密码错误
			}		
		}
		if(DET_TESTING == Channel.Info[i].Status)
		{
			XA_DetCharge(Channel.Info[i].ser1);	   //发送CHARGE
			XA_DetCharge(Channel.Info[i].ser1);	   //发送CHARGE			
		}
	
	}
	//等待充电
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			OSTimeDly(200);
			break;
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			WaitRxPinReady(4000);
			
			if(XA_DetStatusRd(Channel.Info[i].ser1,Channel.Info[i].ser2, Channel.Info[i].ser3) == ERROR )
			{//出现错误
				Channel.Info[i].Status = DET_CHARGE_STATUS_READ_ERR;		//BB
			}
			else
			{
				if(XA_GetDetStatus(CHARGE_STATUS) == ERROR)
				{	
					Channel.Info[i].Status = DET_CHARGE_ERR;	//充电错误		
				}
				else
				{
					;//充电测试通过
				}
			}		
		}
	}
}


void XA_AllChannelDetClrAll(void)
{
	u8 i;
	
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		XA_DetClrAll();
	}
	OSTimeDlyHMSM(0,0,0,50);
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		XA_DetClrAll();
	}
    OSTimeDlyHMSM(0,0,0,10);	
}
void XA_AllChannelWriteFixedDelay(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			if(XA_DetTimeRd(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3)  == ERROR)
			{
				Channel.Info[i].Status = DET_FIX_DELAY_READ_ERR;
			}
			else
			{
				if(XA_GetDetFixedDelay() != (UserParam.DetDelay+BLAST_DELAY_WHOLE_ADD_VALUE))
				{
					if(XA_DetUnLockEEPROM(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3) == ERROR)
					{
						Channel.Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//解EEPROM错误
					}				
					else
					{
						if(XA_DetFixed(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3, UserParam.DetDelay+BLAST_DELAY_WHOLE_ADD_VALUE) == ERROR)
						{
							Channel.Info[i].Status = DET_WR_CODE_WR_FIXED_ERR;//写固定延期错误	
						}
					}
					//芯片设计有问题，写完固定延期后最好复位雷管
					XA_DetClrAll();	     //发送全局复位指令
				}			
			}			
		}
	}	
}
void XA_AllChannelDetCodeWrite(void)
{
	DET_ERR_CODE Status;
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			if(Channel.BarCode[i].Status != BARCODE_NEW)
			{
				Channel.Info[i].Status = DET_WR_CODE_NULL_ERR;
			}				
			else if((Channel.BarCode[i].Result == BARCODE_REPEAT_ERR) || (Channel.BarCode[i].Result == BARCODE_PASS))
			{
				if(XA_DetPwdTest(&Channel.Info[i]) == ERROR)
				{
					Channel.Info[i].Status = DET_WR_CODE_UNLOCK_PWD_ERR;//密码错误
				}			

				if(DET_TESTING == Channel.Info[i].Status)
				{
					if(XA_DetUnLockEEPROM(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3) == ERROR)
					{
						Channel.Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//解EEPROM错误
					}
					
				}				
				if(DET_TESTING == Channel.Info[i].Status)
				{
					Status = XA_DetWriteCode(i);
					if(DET_PASS == Status)
					{					
						if (XA_DetSerlRd() == ERROR)//发送序列码回读指令	                     
						{
							Channel.Info[i].Status = DET_WR_CODE_READ_ERR;//序列码通信错误		
						}
						else
						{
							XA_SaveDetInfo(i);
							if(XA_VerifyCodeWriteResult(i) == ERROR)
							{
								Channel.Info[i].Status = DET_WR_CODE_VERIFY_ERR;//写入数据错误
							}
						}				
					}
					else
					{
						Channel.Info[i].Status = Status;
					}
					Channel.BarCode[i].Status = BARCODE_WR_END;//不管写没写成功，都认为该内码已经使用
				}			
			}
			else
			{
				Channel.Info[i].Status = DET_WR_CODE_NULL_ERR;
			}			
		}
	}

}
void XA_AllChannelChangeDetType(void)
{
	u8 i=0;
	DET_ERR_CODE Status;
	
	for(i=0; i<Channel.Count; i++)
	{
		if ((DET_TESTING == Channel.Info[i].Status) && (DET_PASS != XA_DetTypeTest(Channel.Info[i].type)))
		{
			ConnectChannel(i);
			
			if(XA_DetPwdTest(&Channel.Info[i]) == ERROR)
			{
				Channel.Info[i].Status = DET_WR_CODE_UNLOCK_PWD_ERR;//密码错误
			}				

			if(DET_TESTING == Channel.Info[i].Status)
			{
				if(XA_DetUnLockEEPROM(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3) == ERROR)
				{
					Channel.Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//解EEPROM错误
				}				
			}				
			if(DET_TESTING == Channel.Info[i].Status)
			{
				Status = XA_ChangeDetType(i);				
				if(DET_PASS ==Status)
				{					
					if (XA_DetSerlRd() == ERROR)//发送序列码回读指令	                     
					{
						Channel.Info[i].Status = DET_WR_TYPE_READ_ERR;//序列码通信错误		
					}
					else
					{
						XA_SaveDetInfo(i);
						if(XA_VerifyCodeWriteResult(i) == ERROR)
						{
							Channel.Info[i].Status = DET_WR_CODE_VERIFY_ERR;//写入数据错误
						}								
					}				
				}
				else
				{
					Channel.Info[i].Status = Status;
				}
			}		
		}
	}
}
void XA_AllChannelChangeDetPwd(void)
{
	u8 i=0;
	DET_ERR_CODE Status;
	
	for(i=0; i<Channel.Count; i++)
	{
		if (DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			if(XA_DetPwdTest(&Channel.Info[i]) == ERROR)
			{
				Channel.Info[i].Status = DET_WR_CODE_UNLOCK_PWD_ERR;//密码错误
			}				
            if(DET_TESTING == Channel.Info[i].Status)
            {
                if(((Channel.Info[i].Pwd2<<8)+Channel.Info[i].Pwd1) != DET_PWD_FIX_VALUE)
                {
                    if(XA_DetUnLockEEPROM(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3) == ERROR)
                    {
                        Channel.Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//解EEPROM错误
                    }				
                    if(DET_TESTING == Channel.Info[i].Status)
                    {
                        Status = XA_ChangeDetPwd(i);				
                        if(DET_PASS ==Status)
                        {					
                            if (XA_DetSerlRd() == ERROR)//发送序列码回读指令	                     
                            {
                                Channel.Info[i].Status = DET_WR_TYPE_READ_ERR;//序列码通信错误		
                            }
                            else
                            {
                                XA_SaveDetInfo(i);
                                if(XA_VerifyCodeWriteResult(i) == ERROR)
                                {
                                    Channel.Info[i].Status = DET_WR_CODE_VERIFY_ERR;//写入数据错误
                                }								
                            }				
                        }
                        else
                        {
                            Channel.Info[i].Status = Status;
                        }
                    }                
                }
            }
		
		}
	}
}
void XA_AllChannelFixedDelayCompare(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			if(XA_DetTimeRd(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].ser3)  == ERROR)
			{
				Channel.Info[i].Status = DET_FIX_DELAY_READ_ERR;
			}
			else
			{
				if(XA_GetDetFixedDelay() != (UserParam.DetDelay+BLAST_DELAY_WHOLE_ADD_VALUE))
				{
					Channel.Info[i].Status = DET_FIX_DELAY_NOT_EQUAL;
				}			
			}			
		}
	}
}
//==================================================================================================
//| 函数名称 | DetTest 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 雷管测试
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 雷管测试代码
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2014-08-27 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void XA_DetTest(void)
{	
	CLOSE_HV_SW();//关闭高压输出开关
	SET_LIN_OPEN();
	ClearDetInfo();
	
	OPEN_PW_HV();
	
	/*******放电******/
	XA_AllChannelDetClrAll();
	
	AllChannelLinShortTest(0);//总线短路测试
	AllChannelOpenCircuitTest(0);//总线断路测试
			
	/********正向测试电流*******/
	if(TestParamIsSet(DET_LV_CURRENT_TEST_MASK_BIT))//正向电流测试功能选择
	{
		AllChannelCurrentTest(0);
	}	

	/*******反向后再次测试电流*******/
	if(TestParamIsSet(DET_LV_CURRENT_TEST_MASK_BIT))//反向电流测试功能选择
	{
		SET_LIN_REVERSE();
		OSTimeDlyHMSM(0,0,0,50);
		AllChannelCurrentTest(1);
		SET_LIN_REVERSE();
	}

	/*******监测端口电压是否正常******/
	AllChannelVoltageTest(0);//先测试电流后再测试电压，不然电压测试不准，因为AD采样点的电容充电较慢
	
	/***序列码回读检测****/
	XA_AllChannelDetSerlRd();
	
	/***检测密码是否正确****/	
	XA_AllChannelDetUnLockPwd(DET_WR_CODE_UNLOCK_PWD_ERR);//密码错误
	
	/*******判断芯片频率是否正常*******/
	AllChannelDetFreqTest();

	/*********判断芯片类型的功能************/
	XA_AllChannelDetTypeTest();
	
    if ((Channel.Count == 1) && (AllChannelTestEnd() == 0))
    {       
		if(!(((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED)) && UserParam.DetCodeWrite))
		{
            u8 Buffer[8];
            
            Buffer[0] = Channel.Info[0].ser1;
            Buffer[1] = Channel.Info[0].ser2;
            Buffer[2] = Channel.Info[0].ser3;
            Buffer[3] = Channel.Info[0].fac;
            Buffer[4] = Channel.Info[0].bat;
            Buffer[5] = Channel.Info[0].year;
            if(DetInfoQueueItemIsExist(&DetInfoQueue, &Buffer[0]))
            {
                Channel.Info[0].Status = DET_CODE_REPEAT_TEST;//重复检测	
            }        
		}
    }
	//BIT测试
	if(TestParamIsSet(DET_BIT_TEST_MASK_BIT))//进行bit检测功能的选择
	{ 	
		XA_AllChannelDetBitTest();
	}

	/*充电检测，测试电流*/
	if(XA_DetIsSingleCap(UserParam.DetType) == 0)
	{//单电容时不测	
		if(TestParamIsSet(DET_CHARGE_TEST_MASK_BIT)) 
		{    
			XA_AllChannelDetChargeTest();

			if(TestParamIsSet(DET_CHARGE_HV_CURRENT_TEST_MASK_BIT)) //充电后的高压正向电流测试
			{
				if(AllChannelTestEnd() == 0)
				{
					OPEN_HV_SW();//输出高压
					OSTimeDly(50);
					AllChannelDetHVCurrentTest(0);//充电后的高压正向电流测试	
				}
											
				/*******监测端口电压是否正常******/
				if(AllChannelTestEnd() == 0)
				{
					AllChannelVoltageTest(1);
				}
					

				if(AllChannelTestEnd() == 0)
				{
					OPEN_HV_SW();//输出高压
					SET_LIN_REVERSE();//充电后的高压反向电流测试
					OSTimeDly(10);	
					AllChannelDetHVCurrentTest(1);
					
					if(AllChannelTestEnd() == 0)
					{
						AllChannelVoltageTest(1);
					}
					
					SET_LIN_REVERSE();	
				}
				CLOSE_HV_SW();//关闭高压输出开关
				OSTimeDly(50);				
			}						
		} 
	}
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if (UserParam.DelayWrite)
			{
				XA_AllChannelWriteFixedDelay();//写延期
			}
		}
	}
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if (UserParam.DetCodeWrite)
			{
				XA_AllChannelDetCodeWrite();	//写内码
			}
		}
	}
	if(AllChannelTestEnd() == 0)
	{
		XA_AllChannelChangeDetType();//PD1和BD2需要转换	
	}
	if(AllChannelTestEnd() == 0)
	{
		XA_AllChannelChangeDetPwd();//如果芯片密码不是固定密码，则强制改成固定密码
	}    
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if(UserParam.CheckDelay)
			{	
				if(XA_DetIsFixDelay(UserParam.DetType))
				{//固定延期芯片,判断雷管延期值是否与设置值一致
					XA_AllChannelFixedDelayCompare();
				}				
			}
		}
	}
	if(AllChannelTestEnd() == 0)
	{
		/***内码全0检测****/
		XA_AllChannelDetCodeAllZeroVerify();
	}
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if (UserParam.CheckDetCode)
			{
				/***内码验证****/
				XA_AllChannelDetCodeVerify();
			}
		}	
	}

	XA_AllChannelDetClrAll();
	SET_LIN_OPEN();

	AllChannelStatusReplace(DET_TESTING, DET_PASS);
	SaveChannelOldStatus();
	SoundActionByStatus();
	UpdateDetDetectorPassCount();
    
    if ((Channel.Count == 1) && (GetChannelOldStatusErrorCount() == 0))
	{
        u8 Buffer[8];
        
		Buffer[0] = Channel.Info[0].ser1;
		Buffer[1] = Channel.Info[0].ser2;
		Buffer[2] = Channel.Info[0].ser3;
		Buffer[3] = Channel.Info[0].fac;
		Buffer[4] = Channel.Info[0].bat;
		Buffer[5] = Channel.Info[0].year;	
		SaveDetInfoQueueItem(&DetInfoQueue, &Buffer[0]);//保存信息，缓冲区中的雷管在没有清除前不允许再检测
		if(NeedScan())
		{//保存外码，方便工作按10发一盒进行装盒，每盒中的雷管要求外码前12位相同
			SaveDetOutsideCodeQueueItem(&Channel.BarCode[0].CodeExternal[0]);
		}		
	}
}

u16 XA_DetAnalyst(void)
{
	u16 AnalystResult = 0;
	u16 temp[3];
	u32 c_time;
	u8 Result;
	
	CLOSE_HV_SW();//关闭高压输出开关
	SET_LIN_OPEN();
	
	OPEN_PW_HV();
	ClearDetInfo();
	
	ConnectChannel(0);
	
	/*******放电******/
	OSTimeDlyHMSM(0,0,0,500);	
	XA_DetClrAll();
	OSTimeDlyHMSM(0,0,0,50);
	XA_DetClrAll();
	OSTimeDlyHMSM(0,0,0,50);
	
	if(GetLinABVoltage(0, NULL) < 1000)
	{
		AnalystResult |= 0x8000;//雷管短路
	}
	else if(GetCurrentValue(10, NULL) < 500)
	{
		AnalystResult |= 0x4000;//雷管断路
	}
	else
	{
		/*******正向电流******/
		c_time = OSTimeGet();
		while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))
		{
			OSTimeDlyHMSM(0,0,0,100);
			temp[0] = GetCurrentValue(10, NULL); //低压下的正向电流值
			OSTimeDlyHMSM(0,0,0,100);
			temp[1] = GetCurrentValue(10, NULL); //低压下的正向电流值
			OSTimeDlyHMSM(0,0,0,100);
			temp[2] = GetCurrentValue(10, NULL); //低压下的正向电流值
			
			Channel.Info[0].lv_current = (temp[0]+temp[1]+temp[2])/3;
			
			if((abs(temp[0]-temp[1])<100) && (abs(temp[0]-temp[2])<100) && (abs(temp[1]-temp[2])<100))
			{//差值小于2uA
				if(Channel.Info[0].lv_current <= test_param.det_lv_current_max)
				{
					break;			
				}
			}
		}
		
		/*******反向电流******/
		SET_LIN_REVERSE();
		c_time = OSTimeGet();
		while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))
		{
			OSTimeDlyHMSM(0,0,0,100);
			temp[0] = GetCurrentValue(10, NULL); //低压下的正向电流值
			OSTimeDlyHMSM(0,0,0,100);
			temp[1] = GetCurrentValue(10, NULL); //低压下的正向电流值
			OSTimeDlyHMSM(0,0,0,100);
			temp[2] = GetCurrentValue(10, NULL); //低压下的正向电流值
			
			Channel.Info[0].lv_reverse_current = (temp[0]+temp[1]+temp[2])/3;
			
			if((abs(temp[0]-temp[1])<100) && (abs(temp[0]-temp[2])<100) && (abs(temp[1]-temp[2])<100))
			{//差值小于2uA
				if(Channel.Info[0].lv_reverse_current <= test_param.det_lv_current_max)
				{
					break;			
				}
			}
		}	
		SET_LIN_REVERSE();
		
		OSTimeDlyHMSM(0,0,0,50);
		if(ERROR != XA_DetSerlRd())
		{
			XA_SaveDetInfo(0);
			AnalystResult |= 0x0001;//序列码读取成功
			
			OSTimeDlyHMSM(0,0,0,10);
			if(ERROR != XA_DetTimeRd(Channel.Info[0].ser1, Channel.Info[0].ser2, Channel.Info[0].ser3)) 
			{
				AnalystResult |= 0x0002;//延期时间回读正常
			}
			OSTimeDlyHMSM(0,0,0,10);
			if(DET_PASS == XA_DetBitTest(&Channel.Info[0]))
			{
				AnalystResult |= 0x0004;//BIT正常正常
			}
			
			//写延期时间检测
			OSTimeDlyHMSM(0,0,0,10);
			if(ERROR != XA_DetTimeWr(Channel.Info[0].ser1,Channel.Info[0].ser2,Channel.Info[0].ser3, 0x100))
			{
				AnalystResult |= 0x0008;//写延期正常
			}
		
			OSTimeDlyHMSM(0,0,0,10);
			Result = XA_DetChargeTest(&Channel.Info[0]);
			if(DET_PASSWORD_ERR != Result)
			{
				AnalystResult |= 0x0010;//密码正确
			}
			if(DET_PASS == Result) 
			{			
				AnalystResult |= 0x0020;//充电正常
			}
			
			XA_DetMask(Channel.Info[0].ser1,Channel.Info[0].ser2,Channel.Info[0].ser3);//屏蔽雷管，方便重新发现新雷管
			
			OPEN_HV_SW();//输出高压
			OSTimeDlyHMSM(0,0,3,0);
			
			c_time = OSTimeGet();
			
			while(OSTimeGet() < (c_time+5000/(1000/OS_TICKS_PER_SEC)))
			{
				OSTimeDlyHMSM(0,0,0,100);
				temp[0] = GetCurrentValue(10, NULL); //高压下的正向电流值
				OSTimeDlyHMSM(0,0,0,100);
				temp[1] = GetCurrentValue(10, NULL); //高压下的正向电流值
				OSTimeDlyHMSM(0,0,0,100);
				temp[2] = GetCurrentValue(10, NULL); //高压下的正向电流值
				
				Channel.Info[0].hv_current = (temp[0]+temp[1]+temp[2])/3;
				
				if((abs(temp[0]-temp[1])<100) && (abs(temp[0]-temp[2])<100) && (abs(temp[1]-temp[2])<100))
				{//差值小于2uA
					if(Channel.Info[0].hv_current <= test_param.det_hv_current_max)
					{
						break;				
					}
				}
			}
			CLOSE_HV_SW();//关闭高压输出开关
			OSTimeDlyHMSM(0,0,0,100);
			XA_DetClrAll();
			OSTimeDlyHMSM(0,0,0,100);
			XA_DetClrAll();
			OSTimeDlyHMSM(0,0,0,100);			
		}	
	}

	return AnalystResult;
}
void XA_DetCodeVerify(void)
{
	CLOSE_HV_SW();//关闭高压输出开关
	SET_LIN_OPEN();
	ClearDetInfo();
	
	OPEN_PW_HV();
	
	/*******放电******/
	XA_AllChannelDetClrAll();
	
	AllChannelLinShortTest(0);//总线短路测试
	AllChannelOpenCircuitTest(0);//总线断路测试
	
	/***序列码回读检测****/
	XA_AllChannelDetSerlRd();

    XA_AllChannelChangeDetPwd();//如果芯片密码不是固定密码，则强制改成固定密码
    
	/***内码验证****/
	XA_AllChannelDetCodeVerify();
	
	XA_AllChannelDetClrAll();
	SET_LIN_OPEN();
       
	AllChannelStatusReplace(DET_TESTING, DET_PASS);
	SaveChannelOldStatus();
	SoundActionByStatus();
	UpdateDetDetectorPassCount();	
}
/**
  *************************************************************************************
  * @brief  判断各个通道是否已经接上雷管
  * @param[in]  无
  * @retval 无
  * @author ZH
  * @date   2016年12月5日
  * @note   
  *************************************************************************************  
  */
void XA_QueryDetConnectStatus(void)
{
	CLOSE_HV_SW();//关闭高压输出开关
	SET_LIN_OPEN();
    
	ClearNewStatus();
	
	/*******放电******/
	XA_AllChannelDetClrAll();
	
	AllChannelLinShortTest(1);//总线短路测试
	AllChannelOpenCircuitTest(1);//总线断路测试
			
	/*******监测端口电压是否正常******/
//	AllChannelVoltageTest(0);//先测试电流后再测试电压，不然电压测试不准，因为AD采样点的电容充电较慢
	
	/***序列码回读检测****/
	XA_AllChannelDetSerlRdStatusTest();	
	
	AllChannelNewStatusReplace(DET_TESTING, DET_PASS);
}
void XA_WaitAllChannelDetOff(void)
{
	u8 i=0;
	
	while(1)
	{
		for(i=0; i<Channel.Count; i++)
		{
			ConnectChannel(i);
			OSTimeDly(5);
			
			if(GetCurrentValue(10, NULL) >= 500)
			{//有电流说明雷管没有下线
				break;
			}
			if (XA_DetSerlRd() != ERROR)//发送序列码回读指令	                     
			{//有信号返回，说明没有下线
				break;
			}		
		}
		if(i == Channel.Count)
		{//都已经下线
			break;
		}
	}	
}





