#include "includes.h"

ErrorStatus XA2_ReadPwdRetry(u32 Serl, u32* Pwd)
{
    u8 Retry = 2;
    
    if(ERROR != XA2_ReadPwd(Serl, Pwd))
    {
        return SUCCESS;
    }
    while(Retry--)
    {
        XA2_Reset(Serl);
        OSTimeDlyHMSM(0,0,0,80);//延时80ms
        XA2_DetCheckIn();
        OSTimeDlyHMSM(0,0,0,80);//延时等芯片读EEPROM     
        if(ERROR != XA2_ReadPwd(Serl, Pwd))
        {
            return SUCCESS;
        }
    }
    return ERROR;
}
u8 DetCodeRulePass(u32 Serl, u32 Pwd)
{
    u8 Buffer[8];
    u8 Crc8;
    
    Buffer[0] = Serl&0xFF;
    Buffer[1] = (Serl>>8)&0xFF;
    Buffer[2] = (Serl>>16)&0xFF;
    Buffer[3] = (Serl>>24)&0xFF;
    Buffer[4] = Pwd&0xFF;
    Buffer[5] = (Pwd>>8)&0xFF;
    Buffer[6] = (Pwd>>16)&0xFF;
    Buffer[7] = (Pwd>>24)&0xFF;
    
    Crc8 = (~GetCrc8Value(Buffer, 7))&0xFF;
    
    return (Crc8==Buffer[7])?1:0; 
}
void XA2_AllChannelDetResetAll(void)
{
	u8 i;
	
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		XA2_ResetAll();
	}
	OSTimeDlyHMSM(0,0,0,50);
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		XA2_ResetAll();
	}
    OSTimeDlyHMSM(0,0,0,10);	
}
void XA2_AllChannelDetClrAll(void)
{
	u8 i;
	
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		XA2_ClearAll();
	}
	OSTimeDlyHMSM(0,0,0,50);
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		XA2_ClearAll();
	}
    OSTimeDlyHMSM(0,0,0,10);	
}
void XA2_AllChannelDetSerlRdStatusTest(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.NewStatus[i])
		{
			ConnectChannel(i);
			if (XA2_ReadSerl(&Channel.XA2_Info[i].Serl.U32Value) == ERROR)//发送序列码回读指令	                     
			{
				Channel.NewStatus[i] = DET_INFO_READ_ERR;//序列码通信错误		
			}					
		}		
	}	
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
void XA2_AllChannelDetSerlRd(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			if (XA2_ReadSerl(&Channel.XA2_Info[i].Serl.U32Value) == ERROR)//发送序列码回读指令	                     
			{
				Channel.XA2_Info[i].Status = DET_INFO_READ_ERR;//序列码通信错误		
			}
			else
			{
				Channel.XA2_Info[i].freq = GetDetFreqFromHead();
			}					
		}		
	}	
}
/**
  *************************************************************************************
  * @brief  读密码
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2019年4月23日
  * @note   无
  *************************************************************************************
  */
void XA2_AllChannelDetPwdRd(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			if (XA2_ReadPwdRetry(Channel.XA2_Info[i].Serl.U32Value, &Channel.XA2_Info[i].Pwd.U32Value) == ERROR)                     
			{
				Channel.XA2_Info[i].Status = DET_PASSWORD_READ_ERR;		
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
void XA2_AllChannelDetUnLockPwd(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			switch(XA2_PwdUnlock(Channel.XA2_Info[i].Serl.U32Value, Channel.XA2_Info[i].Pwd.U32Value))
			{
                case 0:
                    break;
                case 1:
                    Channel.XA2_Info[i].Status = DET_PASSWORD_ERR;//密码错误
                    break;
                default:
                    Channel.XA2_Info[i].Status = DET_PASSWORD_NOT_ACK;//通讯异常
                    break;                    
				
			}					
		}		
	}	
}

u8 XA2_GetDetType(u32 Serl, u32 Pwd)
{
    uint64_t U64Value;
    
    U64Value = Serl +  ((uint64_t)Pwd<<32);
	return (U64Value>>46)&0x1F;//  bit46-bit50共5位
}
/**
  *************************************************************************************
  * @brief  根据芯片类型判断是否为单电容类型
  * @param[in]  无
  * @retval 0不是单电容，其他值表示单电容
  * @author ZH
  * @date   2016年7月6日
  * @note   
  *************************************************************************************  
  */
DET_ERR_CODE XA2_DetTypeTest(u8 DetType)
{          	      
	if((DetType&0x1F) == (UserParam.DetType&0x1F))
	{
		return DET_PASS;
	}
	else 
	{
		return DET_TYPE_ERR; 
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
void XA2_AllChannelDetTypeTest(void)
{
	u8 i=0;
	DET_ERR_CODE Result;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{		
			Result = XA2_DetTypeTest(XA2_GetDetType(Channel.XA2_Info[i].Serl.U32Value, Channel.XA2_Info[i].Pwd.U32Value)); //判断芯片类型
			if(DET_PASS != Result)
			{
				Channel.XA2_Info[i].Status = Result;
			}
		}		
	}	
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
u8 XA2_DetIsFixDelay(u8 Type)
{
	if((Type == DET_TYPE_XA_02) || (Type == DET_TYPE_XA_06) || (Type == DET_TYPE_XA_21))
	{
		return 0;
	}
	else
	{
		return 1;
	}		
}
DET_ERR_CODE XA2_DetBitTest(XA2_DET_INFO* det)
{
    unsigned char /*retry,*/retry1;
	ErrorStatus result;
	DET_ERR_CODE  err_code = DET_PASS;
    u16 Status;
	
	retry1=3;
	do
	{  
		XA2_ClearAll();
        OSTimeDly(50);
		XA2_Bit(det->Serl.U8Buffer[0]);
        XA2_Bit(det->Serl.U8Buffer[0]);
 		OSTimeDlyHMSM(0,0,1,500);       
		result = XA2_ReadStatus(det->Serl.U32Value, &Status);	 //发送延期时间回读指令，延时回读错误情况下，返回1
		if(result == ERROR )
		{//状态回读出现错误
			err_code = DET_BIT_STATUS_READ_ERR;	
		}
		else
		{
			if(XA2_GetDetStatus(Status, XA2_BIT_STATUS)== ERROR)
            {
                err_code = DET_BIT_ERR;	//Bit错误
            }
            else
            {
                err_code = DET_PASS;//BIT测试通过
            }
		}
	}while((result == ERROR)&&(--retry1));

	return err_code;
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
void XA2_AllChannelDetBitTest(void)  	
{
	u8 i;
	ErrorStatus Result;
	u16 Status;
    
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			XA2_ClearAll();	     //发送全局复位指令
            OSTimeDly(50);
			XA2_Bit(Channel.XA2_Info[i].Serl.U8Buffer[0]);     //发送BIT1，不判断返回
            XA2_Bit(Channel.XA2_Info[i].Serl.U8Buffer[0]);
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			OSTimeDlyHMSM(0,0,1,500);//延时等待BIT测试完成,从波形看，需要600ms左右/6.7V, 5.5V,起爆电容68uf要1.3秒
			break;		
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			Result = XA2_ReadStatus(Channel.XA2_Info[i].Serl.U32Value, &Status);	 //发送延期时间回读指令，延时回读错误情况下，返回1
			if(Result == ERROR )
			{//状态回读出现错误
				Channel.XA2_Info[i].Status = DET_BIT_STATUS_READ_ERR;	
			}
			else
			{
				if(XA2_GetDetStatus(Status, XA2_BIT_STATUS)== ERROR)
                {
                    Channel.XA2_Info[i].Status = DET_BIT_ERR;	//Bit错误
                }
			}		
		}
	}
}
DET_ERR_CODE XA2_DetChargeTest(XA2_DET_INFO* det)
{
	//u8 retry=3;
	ErrorStatus result;
    u16 Status;
    
    switch(XA2_PwdUnlock(det->Serl.U32Value, det->Pwd.U32Value))
    {
        case 0:
            break;
        case 1:
            return DET_PASSWORD_ERR;//密码错误
        default:
            return DET_PASSWORD_NOT_ACK;//通讯异常                   
        
    }

	XA2_Charge(det->Serl.U8Buffer[0]);	   //发送CHARGE
	XA2_Charge(det->Serl.U8Buffer[0]);	   //发送CHARGE
	OSTimeDly(200);
	WaitRxPinReady(4000);
//	OSTimeDlyHMSM(0,0,0,90);
	
	result = XA2_ReadStatus(det->Serl.U32Value, &Status);
	if(result == ERROR )
	{//出现错误
		return DET_CHARGE_STATUS_READ_ERR;		//BB
	}
	else
	{
		if(XA2_GetDetStatus(Status, XA2_CHARGE_STATUS) == ERROR)
		{	
			return DET_CHARGE_ERR;	//充电错误		
		}
		else
		{
			return DET_PASS;//充电测试通过
		}
	}
}
void XA2_AllChannelDetChargeTest(void)
{
	u8 i;
	u16 Status;
    
	for(i = 0; i < Channel.Count;i++)
	{
		ConnectChannel(i);
		
		//写延期时间检测
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			if(XA2_WriteTime(Channel.XA2_Info[i].Serl.U32Value, 0x55) == ERROR)
			{
				Channel.XA2_Info[i].Status = DET_WRITE_DELAY_ERR;      //写延期错误
			}		
		}
		//解密
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			switch(XA2_PwdUnlock(Channel.XA2_Info[i].Serl.U32Value, Channel.XA2_Info[i].Pwd.U32Value))
            {
                case 0:
                    break;
                case 1:
                    Channel.XA2_Info[i].Status = DET_PASSWORD_ERR;//密码错误
                    break;
                default:
                    Channel.XA2_Info[i].Status = DET_PASSWORD_NOT_ACK;//通讯异常
                    break;
                
            }		
		}
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			XA2_Charge(Channel.XA2_Info[i].Serl.U8Buffer[0]);	   //发送CHARGE
			XA2_Charge(Channel.XA2_Info[i].Serl.U8Buffer[0]);	   //发送CHARGE			
		}
	}
	//等待充电
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			OSTimeDly(200);
			break;
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			WaitRxPinReady(4000);
			
			if(XA2_ReadStatus(Channel.XA2_Info[i].Serl.U32Value, &Status) == ERROR )
			{//出现错误
				Channel.XA2_Info[i].Status = DET_CHARGE_STATUS_READ_ERR;		//BB
			}
			else
			{
				if(XA2_GetDetStatus(Status, XA2_CHARGE_STATUS) == ERROR)
				{	
					Channel.XA2_Info[i].Status = DET_CHARGE_ERR;	//充电错误		
				}
				else
				{
					;//充电测试通过
				}
			}		
		}
	}
}
u8 XA2_WriteFixDelayTest(u32 Serl, u16 FixDelay)
{
	u8 Retry = 3;
	
	while(Retry--)
	{
		if(XA2_WriteFixDelayUnlock(Serl) == ERROR)
		{
			return 1;//解EEPROM错误
		}				
		else if(XA2_WriteFixDelay(Serl, FixDelay) != ERROR)
		{
			return 0;
		}	
	}
	return 2;//写固定延期错误
}
void XA2_AllChannelWriteFixedDelay(void)
{
	u8 i=0;
	u16 FixDelay;
    
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			if(XA2_ReadTimeClock(Channel.XA2_Info[i].Serl.U32Value, NULL, &FixDelay, NULL)  == ERROR)
			{
				Channel.XA2_Info[i].Status = DET_FIX_DELAY_READ_ERR;
			}
			else
			{
				if(FixDelay != (UserParam.DetDelay+XA2_BLAST_DELAY_WHOLE_ADD_VALUE))
				{
					switch(XA2_WriteFixDelayTest(Channel.XA2_Info[i].Serl.U32Value, UserParam.DetDelay+XA2_BLAST_DELAY_WHOLE_ADD_VALUE))
					{
						case 0:
							break;
						case 1:
							Channel.XA2_Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//解EEPROM错误
							break;
						default:
							Channel.XA2_Info[i].Status = DET_WR_CODE_WR_FIXED_ERR;//写固定延期错误
							break;
					}
				}			
			}			
		}
	}	
}
u8 XA2_UnlockAndWriteEEPROM(u32 Serl, u8 Addr, u8* Buffer)
{
    u8 Retry = 2;
    
    while(Retry--)
    {
        if(ERROR == XA2_WriteDataUnlock(Serl))
        {
            return 1;//解EEPROM错误
        }
        if(ERROR == XA2_WriteEEPROM(Addr, Buffer))
        {
            if(Addr == 2)
            {
                XA2_ReadSerl(&Serl);
            }
            continue;
        }
        return 0;    
    }
    return 2;//写EEPROM错误
}
void XA2_AllChannelDetCodeWrite(void)
{
	u8 i=0;
	u8 U8Value;
    
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			if(Channel.BarCode[i].Status != BARCODE_NEW)
			{
				Channel.XA2_Info[i].Status = DET_WR_CODE_NULL_ERR;
			}				
			else if((Channel.BarCode[i].Result == BARCODE_REPEAT_ERR) || (Channel.BarCode[i].Result == BARCODE_PASS))
			{
                Channel.BarCode[i].Status = BARCODE_WR_END;//不管写没写成功，都认为该内码已经使用
                
                U8Value = XA2_UnlockAndWriteEEPROM(Channel.XA2_Info[i].Serl.U32Value, 4, &Channel.BarCode[i].DetCode[0]);
                if(U8Value == 1)
                {//解EEPROM错误
                    Channel.XA2_Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//解EEPROM错误
                    continue;
                }
                else if(U8Value != 0)
                {//写失败
                    Channel.XA2_Info[i].Status = DET_WR_CODE_ERR;
                    continue;
                }
                
                U8Value = XA2_UnlockAndWriteEEPROM(Channel.XA2_Info[i].Serl.U32Value, 5, &Channel.BarCode[i].DetCode[4]);
                if(U8Value == 1)
                {//解EEPROM错误
                    Channel.XA2_Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//解EEPROM错误
                    continue;
                }
                else if(U8Value != 0)
                {//写失败
                    Channel.XA2_Info[i].Status = DET_WR_CODE_ERR;
                    continue;
                }
                U8Value = XA2_UnlockAndWriteEEPROM(Channel.XA2_Info[i].Serl.U32Value, 3, &Channel.BarCode[i].DetCode[4]);
                if(U8Value == 1)
                {//解EEPROM错误
                    Channel.XA2_Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//解EEPROM错误
                    continue;
                }
                else if(U8Value != 0)
                {//写失败
                    Channel.XA2_Info[i].Status = DET_WR_CODE_ERR;
                    continue;
                }
                
                U8Value = XA2_UnlockAndWriteEEPROM(Channel.XA2_Info[i].Serl.U32Value, 2, &Channel.BarCode[i].DetCode[0]);
                if(U8Value == 1)
                {//解EEPROM错误
                    Channel.XA2_Info[i].Status = DET_WR_CODE_UNLOCK_EEPROM_ERR;//解EEPROM错误
                    continue;
                }
                else if(U8Value != 0)
                {//写失败
                    Channel.XA2_Info[i].Status = DET_WR_CODE_ERR;
                    continue;
                }
                
                //确认芯片是否已经正确写入内码
                XA2_ResetAll();
                XA2_ResetAll();
                OSTimeDlyHMSM(0,0,0,50);
                XA2_ClearAll();
                XA2_ClearAll();
                OSTimeDly(50);
                
                if(ERROR == XA2_ReadSerl(&Channel.XA2_Info[i].Serl.U32Value))
                {
                    Channel.XA2_Info[i].Status = DET_WR_CODE_READ_ERR;
                    continue;
                }
                if (0 != memcmp(&Channel.BarCode[i].DetCode[0], Channel.XA2_Info[i].Serl.U8Buffer, 4))
                {
                    Channel.XA2_Info[i].Status = DET_WR_CODE_VERIFY_ERR;//写内码失败
                    continue;
                }    
                if(ERROR == XA2_ReadPwdRetry(Channel.XA2_Info[i].Serl.U32Value, &Channel.XA2_Info[i].Pwd.U32Value))
                {
                    Channel.XA2_Info[i].Status = DET_READ_EEPROM_ERR;
                    continue;
                }
                if (0 != memcmp(&Channel.BarCode[i].DetCode[4], &Channel.XA2_Info[i].Pwd.U8Buffer[0], 4))
                {
                    Channel.XA2_Info[i].Status = DET_WR_CODE_VERIFY_ERR;//写内码失败
                    continue;
                }     			
			}
			else
			{
				Channel.XA2_Info[i].Status = DET_WR_CODE_NULL_ERR;
			}			
		}
	}
}


void XA2_AllChannelFixedDelayCompare(void)
{
	u8 i=0;
	u16 FixedDelay;
    
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			if(XA2_ReadTimeClock(Channel.XA2_Info[i].Serl.U32Value, NULL, &FixedDelay, NULL)  == ERROR)
			{
				Channel.XA2_Info[i].Status = DET_FIX_DELAY_READ_ERR;
			}
			else
			{
				if(FixedDelay != (UserParam.DetDelay+XA2_BLAST_DELAY_WHOLE_ADD_VALUE))
				{
					Channel.XA2_Info[i].Status = DET_FIX_DELAY_NOT_EQUAL;
				}			
			}			
		}
	}
}
/**
  *************************************************************************************
  * @brief  雷管写入检测记录
  * @param[in]  无
  * @retval ERROR 还没有写入 ；SUCCESS已经写入
  * @author ZH
  * @date   2014年9月15日
  * @note   DATA1-DATA4作为序列码的备份，DATA5-DATA8作为密码的备份。
  * 也就是说在写码时，同时写2遍，在使用中如果出现序列码或密码变化时，可以从备份数据中恢复。
  * DATA9中BIT0-BIT4，用于表示已经过I型、II型、入厂检、半成品检、成品检
  *************************************************************************************  
  */
DET_ERR_CODE DetWriteTestRecorder(XA2_DET_INFO* det)
{
    u8 Buffer[4];
    u8 Crc8;
    u8 NeedWrite = 0;
    
    if(ERROR == XA2_ReadEEPROM(0x06, Buffer))
	{
        return DET_READ_EEPROM_ERR;
    }
    Crc8 = (~GetCrc8Value(Buffer, 3))&0xFF;
    if(Crc8 == Buffer[3])
    {
        if((Buffer[1] != 0x00) || (Buffer[2] != 0x00))
        {
            return DET_WR_CODE_VERIFY_ERR;
        }
		if((Buffer[0]&0x01) == 0)
		{//没有经过I型检测
			return DET_TEST_RECORDER_ERR1;
		}
		if((Buffer[0]&0x02) == 0)
		{//没有经过II型检测
			return DET_TEST_RECORDER_ERR2;
		}
        switch(UserParam.Stage)
        {    
            case ENTER_FACTORY:
                if((Buffer[0]&0x04) == 0)
                {
                    Buffer[0] |= 0x04;
                    NeedWrite = 1;
                }
                break;
            case SEMIFINISHED:
                if((Buffer[0]&0x08) == 0)
                {
                    Buffer[0] |= 0x08;
                    NeedWrite = 1;
                }
//				else if((UserParam.NetMode == 0) && (UserParam.DetCodeWrite == 1))
//				{//半成品重复检验 非联网模式的写码模式才有意义
//					return DET_TEST_RECORDER_REPEAT_ERR1;
//				}
                break;
            case FINISHED:
                if(((Buffer[0]&0x08) == 0) && (UserParam.NetMode == 0))
                {//没有经过半成品检测（因为自动化生产线有的不做半成品，故非联网模式不判断）
                    return DET_TEST_RECORDER_ERR3;
                }
                if((Buffer[0]&0x10) == 0)
                {
                    Buffer[0] |= 0x10;
                    NeedWrite = 1;
                }
//				else  if((UserParam.NetMode == 0) && (UserParam.DetCodeWrite == 1))
//				{//成品重复检验 非联网模式的写码模式才有意义
//					return DET_TEST_RECORDER_REPEAT_ERR2;
//				}
                break;
            default:
                break;
        }
    }
    else
    {
		u8 Buffer1[4];
		
		if(ERROR == XA2_ReadEEPROM(0x06, Buffer1))
		{
			return DET_READ_EEPROM_ERR;
		}
		if(0 != memcmp(Buffer1, Buffer,4))
		{
			return DET_TEST_RECORDER_CRC_DIFFERENT_ERR;//两次数据不同
		}
		return DET_TEST_RECORDER_CRC_ERR;//CRC校验不过
			
    }

    if(NeedWrite)
    {       
        Buffer[3] = (~GetCrc8Value(Buffer, 3))&0xFF;
        
        switch(XA2_UnlockAndWriteEEPROM(det->Serl.U32Value, 0x06, &Buffer[0]))
        {
            case 0:
                break;
            case 1:
                return DET_WR_CODE_UNLOCK_EEPROM_ERR;
            default:
                return DET_WR_CODE_ERR;
        }
    }
    
    return DET_PASS;
}
/**
  *************************************************************************************
  * @brief  雷管成品与半成品重复检测记录
  * @param[in]  ChannelCount通道号
  * @retval ERROR 重复写入 ；SUCCESS不重复
  * @author ZH
  * @date   2014年9月15日
  * @note   DATA1-DATA4作为序列码的备份，DATA5-DATA8作为密码的备份。
  * 也就是说在写码时，同时写2遍，在使用中如果出现序列码或密码变化时，可以从备份数据中恢复。
  * DATA9中BIT0-BIT4，用于表示已经过I型、II型、入厂检、半成品检、成品检
  *************************************************************************************  
  */
DET_ERR_CODE DetWriteTestRecorderCheck(u8 ChannelCount)
{
    u8 Buffer[4];
    u8 Crc8;
	
//    OSTimeDly(50);
	ConnectChannel(ChannelCount);
	
    if(ERROR == XA2_ReadEEPROM(0x06, Buffer))
	{
        return DET_READ_EEPROM_ERR;
    }
    Crc8 = (~GetCrc8Value(Buffer, 3))&0xFF;
    if(Crc8 == Buffer[3])
	{
		switch(UserParam.Stage)
        {    
            case SEMIFINISHED:
                if((Buffer[0]&0x08) == 0x08)
                {//已经进行过半成品检
					return DET_TEST_RECORDER_REPEAT_ERR;
                }
				else
				{
					return DET_PASS;
				}
				
            case FINISHED:
                if((Buffer[0]&0x10) == 0x10)
                {//已经进行过成品检
					return DET_TEST_RECORDER_REPEAT_ERR;
                }
				else
				{
					return DET_PASS;
				}
            default:
				return DET_PASS;
		}
	}
	else
	{
		return DET_TEST_RECORDER_CRC_ERR;//CRC校验不过
	}		
}
void XA2_AllChannelDetWriteTestRecorder(void)
{
	u8 i=0;
	DET_ERR_CODE Result;
    
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			Result = DetWriteTestRecorder(&Channel.XA2_Info[i]);
            if(Result != DET_PASS)
            {
                Channel.XA2_Info[i].Status = Result;
            }
		}
	}
}
/**
  *************************************************************************************
  * @brief  雷管内码写入状态
  * @param[in]  无
  * @retval ERROR 还没有写入 ；SUCCESS已经写入
  * @author ZH
  * @date   2014年9月15日
  * @note   DATA1-DATA4作为序列码的备份，DATA5-DATA8作为密码的备份。
  * 也就是说在写码时，同时写2遍，在使用中如果出现序列码或密码变化时，可以从备份数据中恢复。
  * DATA9中BIT0-BIT4，用于表示已经过I型、II型、入厂检、半成品检、成品检
  *************************************************************************************  
  */
DET_ERR_CODE XA2_DetCodeWriteStatusTest(XA2_DET_INFO* det)
{
    u8 Buffer[4];
    u8 Crc8;
    
    if(ERROR == DetCodeRulePass(det->Serl.U32Value, det->Pwd.U32Value))
    {
        return DET_CODE_RULE_ERR;
    }
    
    if(ERROR == XA2_ReadEEPROM(0x04, Buffer))
	{
        return DET_READ_EEPROM_ERR;
    }
    if (0 != memcmp(Buffer, det->Serl.U8Buffer, 4))
    {
        return DET_CODE_BACKUP_ERR1;
    }
    if(ERROR == XA2_ReadEEPROM(0x05, Buffer))
	{
        return DET_READ_EEPROM_ERR;
    }
    if (0 != memcmp(Buffer, det->Pwd.U8Buffer, 4))
    {
        return DET_CODE_BACKUP_ERR2;
    }
    
    if(ERROR == XA2_ReadEEPROM(0x06, Buffer))
	{
        return DET_READ_EEPROM_ERR;
    }
    Crc8 = (~GetCrc8Value(Buffer, 3))&0xFF;
    if(Crc8 != Buffer[3])
    {
        return DET_TEST_RECORDER_DATA_ERR2;
    }
    return DET_PASS;
}
void XA2_AllChannelDetCodeWriteStatusTest(void)
{
	u8 i=0;
	DET_ERR_CODE Result;
    
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)
		{
			ConnectChannel(i);
			
			Result = XA2_DetCodeWriteStatusTest(&Channel.XA2_Info[i]);
            if(Result != DET_PASS)
            {
                Channel.XA2_Info[i].Status = Result;
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
void XA2_AllChannelDetCodeVerify(void)
{
	u8 i=0;
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.XA2_Info[i].Status)	
		{			
			if(Channel.BarCode[i].Status != BARCODE_NEW)
			{
				Channel.XA2_Info[i].Status = DET_WR_CODE_NULL_ERR;
			}				
			else if((Channel.BarCode[i].Result == BARCODE_REPEAT_ERR) || (Channel.BarCode[i].Result == BARCODE_PASS))
			{
				if ((memcmp(&Channel.XA2_Info[i].Serl.U8Buffer, &Channel.BarCode[i].DetCode[0], 4) != 0) || 
                    (memcmp(&Channel.XA2_Info[i].Pwd.U8Buffer, &Channel.BarCode[i].DetCode[4], 4) != 0))
				{
					Channel.XA2_Info[i].Status = DET_CODE_NOT_SAME;//内码写条码不一致
				}				
			}
			else
			{
				Channel.XA2_Info[i].Status = DET_WR_CODE_NULL_ERR;
			}			
		}
	}
}
void XA2_DetTest(void)
{	
	CLOSE_HV_SW();//关闭高压输出开关
	SET_LIN_OPEN();
	ClearDetInfo();
	
	OPEN_PW_HV();
	
    XA2_AllChannelDetResetAll();
    
	/*******放电******/
	XA2_AllChannelDetClrAll();
	
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
	XA2_AllChannelDetSerlRd();
	
    /***PWD回读检测****/
    XA2_AllChannelDetPwdRd();
    
	/***检测密码是否正确****/	
	XA2_AllChannelDetUnLockPwd();
	
	/*******判断芯片频率是否正常*******/
	AllChannelDetFreqTest();

	/*********判断芯片类型的功能************/
	XA2_AllChannelDetTypeTest();
	
    if ((Channel.Count == 1) && (AllChannelTestEnd() == 0))
    {       
		if(!(((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED)) && UserParam.DetCodeWrite))
		{
            u8 Buffer[8];
            
            Buffer[0] = Channel.XA2_Info[0].Serl.U8Buffer[0];
            Buffer[1] = Channel.XA2_Info[0].Serl.U8Buffer[1];
            Buffer[2] = Channel.XA2_Info[0].Serl.U8Buffer[2];
            Buffer[3] = Channel.XA2_Info[0].Serl.U8Buffer[3];
            Buffer[4] = Channel.XA2_Info[0].Pwd.U8Buffer[0];
            Buffer[5] = Channel.XA2_Info[0].Pwd.U8Buffer[1];
            Buffer[6] = Channel.XA2_Info[0].Pwd.U8Buffer[2];
            Buffer[7] = Channel.XA2_Info[0].Pwd.U8Buffer[3];
            if(DetInfoQueueItemIsExist(&DetInfoQueue, &Buffer[0]))
            {
                Channel.XA2_Info[0].Status = DET_CODE_REPEAT_TEST;//重复检测	
            } 

		}
    }
	//BIT测试
	if(TestParamIsSet(DET_BIT_TEST_MASK_BIT))//进行bit检测功能的选择
	{ 	
		XA2_AllChannelDetBitTest();
	}

	/*充电检测，测试电流*/
    if(TestParamIsSet(DET_CHARGE_TEST_MASK_BIT)) 
    {    
        XA2_AllChannelDetChargeTest();

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
	
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if (UserParam.DelayWrite)
			{
				XA2_AllChannelWriteFixedDelay();//写延期
			}
		}
	}
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if (UserParam.DetCodeWrite)
			{
				XA2_AllChannelDetCodeWrite();	//写内码
			}
		}
	}
   
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if(UserParam.CheckDelay)
			{	
				XA2_AllChannelFixedDelayCompare();				
			}
		}
	}
    if(AllChannelTestEnd() == 0)
    {
        /***写入检测记录****/
        XA2_AllChannelDetWriteTestRecorder();    
    }
   
	if(AllChannelTestEnd() == 0)
	{
		/***检测EEPROM中数据是否符合规则****/
		XA2_AllChannelDetCodeWriteStatusTest();
	}
	if(AllChannelTestEnd() == 0)
	{
		if((UserParam.Stage == SEMIFINISHED) || (UserParam.Stage == FINISHED))
		{
			if (UserParam.CheckDetCode)
			{
				/***内码验证****/
				XA2_AllChannelDetCodeVerify();
			}
		}	
	}

	XA2_AllChannelDetClrAll();
	SET_LIN_OPEN();

	AllChannelStatusReplace(DET_TESTING, DET_PASS);
	SaveChannelOldStatus();
	SoundActionByStatus();
	UpdateDetDetectorPassCount();
    
    if ((Channel.Count == 1) && (GetChannelOldStatusErrorCount() == 0))
	{
        u8 Buffer[8];
        
		Buffer[0] = Channel.XA2_Info[0].Serl.U8Buffer[0];
		Buffer[1] = Channel.XA2_Info[0].Serl.U8Buffer[1];
		Buffer[2] = Channel.XA2_Info[0].Serl.U8Buffer[2];
		Buffer[3] = Channel.XA2_Info[0].Serl.U8Buffer[3];
		Buffer[4] = Channel.XA2_Info[0].Pwd.U8Buffer[0];
		Buffer[5] = Channel.XA2_Info[0].Pwd.U8Buffer[1];
        Buffer[6] = Channel.XA2_Info[0].Pwd.U8Buffer[2];
        Buffer[7] = Channel.XA2_Info[0].Pwd.U8Buffer[3];        
		SaveDetInfoQueueItem(&DetInfoQueue, &Buffer[0]);//保存信息，缓冲区中的雷管在没有清除前不允许再检测
		if(NeedScan())
		{//保存外码，方便工作按10发一盒进行装盒，每盒中的雷管要求外码前12位相同
			SaveDetOutsideCodeQueueItem(&Channel.BarCode[0].CodeExternal[0]);
		}		
	}
}

u16 XA2_DetAnalyst(void)
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
    OSTimeDlyHMSM(0,0,0,200);
    XA2_ResetAll();
    XA2_ResetAll();
	OSTimeDlyHMSM(0,0,0,50);	
	XA2_ClearAll();
	XA2_ClearAll();
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
			
			Channel.XA2_Info[0].lv_current = (temp[0]+temp[1]+temp[2])/3;
			
			if((abs(temp[0]-temp[1])<100) && (abs(temp[0]-temp[2])<100) && (abs(temp[1]-temp[2])<100))
			{//差值小于2uA
				if(Channel.XA2_Info[0].lv_current <= test_param.det_lv_current_max)
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
			
			Channel.XA2_Info[0].lv_reverse_current = (temp[0]+temp[1]+temp[2])/3;
			
			if((abs(temp[0]-temp[1])<100) && (abs(temp[0]-temp[2])<100) && (abs(temp[1]-temp[2])<100))
			{//差值小于2uA
				if(Channel.XA2_Info[0].lv_reverse_current <= test_param.det_lv_current_max)
				{
					break;			
				}
			}
		}	
		SET_LIN_REVERSE();
		
		OSTimeDlyHMSM(0,0,0,50);
		if(ERROR != XA2_ReadSerl(&Channel.XA2_Info[0].Serl.U32Value))
		{
            if(ERROR != XA2_ReadPwd(Channel.XA2_Info[0].Serl.U32Value, &Channel.XA2_Info[0].Pwd.U32Value))
            {
                AnalystResult |= 0x0001;//序列码读取成功
                
                Channel.XA2_Info[0].freq = GetDetFreqFromHead();
                
                OSTimeDlyHMSM(0,0,0,10);
                if(ERROR != XA2_ReadTimeClock(Channel.XA2_Info[0].Serl.U32Value, NULL,NULL,NULL)) 
                {
                    AnalystResult |= 0x0002;//延期时间回读正常
                }
                OSTimeDlyHMSM(0,0,0,10);
                if(DET_PASS == XA2_DetBitTest(&Channel.XA2_Info[0]))
                {
                    AnalystResult |= 0x0004;//BIT正常正常
                }
                
                //写延期时间检测
                OSTimeDlyHMSM(0,0,0,10);
                if(ERROR != XA2_WriteTime(Channel.XA2_Info[0].Serl.U32Value, 0x100))
                {
                    AnalystResult |= 0x0008;//写延期正常
                }
            
                OSTimeDlyHMSM(0,0,0,10);
                Result = XA2_DetChargeTest(&Channel.XA2_Info[0]);
                if(DET_PASSWORD_ERR != Result)
                {
                    AnalystResult |= 0x0010;//密码正确
                }
                if(DET_PASS == Result) 
                {			
                    AnalystResult |= 0x0020;//充电正常
                }
                
                XA2_Mask(Channel.XA2_Info[0].Serl.U32Value);//屏蔽雷管，方便重新发现新雷管
                
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
                    
                    Channel.XA2_Info[0].hv_current = (temp[0]+temp[1]+temp[2])/3;
                    
                    if((abs(temp[0]-temp[1])<100) && (abs(temp[0]-temp[2])<100) && (abs(temp[1]-temp[2])<100))
                    {//差值小于2uA
                        if(Channel.XA2_Info[0].hv_current <= test_param.det_hv_current_max)
                        {
                            break;				
                        }
                    }
                }
                CLOSE_HV_SW();//关闭高压输出开关
                OSTimeDlyHMSM(0,0,0,100);
                XA2_ClearAll();
                OSTimeDlyHMSM(0,0,0,100);
                XA2_ClearAll();
                OSTimeDlyHMSM(0,0,0,100);			
            }            
        }	
	}

	return AnalystResult;
}
void XA2_DetCodeVerify(void)
{
	CLOSE_HV_SW();//关闭高压输出开关
	SET_LIN_OPEN();
	ClearDetInfo();
	
	OPEN_PW_HV();
	
	/*******放电******/
	XA2_AllChannelDetClrAll();
	
	AllChannelLinShortTest(0);//总线短路测试
	AllChannelOpenCircuitTest(0);//总线断路测试
	
	/***序列码回读检测****/
	XA2_AllChannelDetSerlRd();

    /***PWD回读检测****/
    XA2_AllChannelDetPwdRd();
    
    XA2_AllChannelDetCodeWriteStatusTest();
    
	/***内码验证****/
	XA2_AllChannelDetCodeVerify();
	
	XA2_AllChannelDetClrAll();
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
void XA2_QueryDetConnectStatus(void)
{
	CLOSE_HV_SW();//关闭高压输出开关
	SET_LIN_OPEN();
    
	ClearNewStatus();
	
	/*******放电******/
	XA2_AllChannelDetClrAll();
	
	AllChannelLinShortTest(1);//总线短路测试
	AllChannelOpenCircuitTest(1);//总线断路测试
			
	/*******监测端口电压是否正常******/
//	AllChannelVoltageTest(0);//先测试电流后再测试电压，不然电压测试不准，因为AD采样点的电容充电较慢
	
	/***序列码回读检测****/
	XA2_AllChannelDetSerlRdStatusTest();	
	
	AllChannelNewStatusReplace(DET_TESTING, DET_PASS);
}
void XA2_WaitAllChannelDetOff(void)
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
			if (XA2_ReadSerl(NULL) != ERROR)//发送序列码回读指令	                     
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

