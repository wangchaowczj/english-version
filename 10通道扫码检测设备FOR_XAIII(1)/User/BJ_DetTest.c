#include "includes.h"



/**
  *************************************************************************************
  * @brief  判断BIT状态位
  * @param  无
  * @retval 结果
  *      @arg SUCCESS BIT测试成功功
  *      @arg ERROR BIT测试失败
  * @author ZH
  * @date   2018年3月10日
  * @note   芯片内码保存在全局变量DetProtocol.RecData.Data,第1个字节是同步头
  *************************************************************************************  
  */
ErrorStatus BJ_GetBitStatus(void)
{   
    if((DetProtocol.RecData.Data[1] & BJ_DET_MASK_TEST_ERR) != 0)  // 判断BIT测试的状态
    {
        return ERROR;
    }
    return SUCCESS;
}

/**
  *************************************************************************************
  * @brief  判断充电状态位
  * @param  无
  * @retval 结果
  *      @arg SUCCESS 已经充电
  *      @arg ERROR 没有充电
  * @author ZH
  * @date   2018年3月10日
  * @note   芯片内码保存在全局变量DetProtocol.RecData.Data,第1个字节是同步头
  *************************************************************************************  
  */
ErrorStatus BJ_GetChargeBitStatus(void)
{   
    if((DetProtocol.RecData.Data[1] & BJ_DET_MASK_CHARGE))  // 判断BIT测试的状态
    {
        return SUCCESS;
    }
    return ERROR;
}

/**
  *************************************************************************************
  * @brief  判断放电状态位
  * @param  无
  * @retval 结果
  *      @arg SUCCESS 已经放电
  *      @arg ERROR 没有放电
  * @author ZH
  * @date   2018年3月10日
  * @note   芯片内码保存在全局变量DetProtocol.RecData.Data,第1个字节是同步头
  *************************************************************************************  
  */
ErrorStatus BJ_GetDisChargeBitStatus(void)
{   
    if((DetProtocol.RecData.Data[1] & BJ_DET_MASK_DISCHARGE))  // 判断BIT测试的状态
    {
        return SUCCESS;
    }
    return ERROR;
}
/**
  *************************************************************************************
  * @brief  判断放电状态位
  * @param  无
  * @retval 结果
  *      @arg SUCCESS 已经放电
  *      @arg ERROR 没有放电
  * @author ZH
  * @date   2018年3月10日
  * @note   芯片内码保存在全局变量DetProtocol.RecData.Data,第1个字节是同步头
  *************************************************************************************  
  */
ErrorStatus BJ_GetExpenBitStatus(void)
{   
    if((DetProtocol.RecData.Data[1] & BJ_DET_MASK_WAIT_EXPEN))  // 判断BIT测试的状态
    {
        return SUCCESS;
    }
    return ERROR;
}
/**
  *************************************************************************************
  * @brief  判断写延期状态位
  * @param  无
  * @retval 结果
  *      @arg SUCCESS 已经写延期
  *      @arg ERROR 没有写延期
  * @author ZH
  * @date   2018年3月10日
  * @note   芯片内码保存在全局变量DetProtocol.RecData.Data,第1个字节是同步头
  *************************************************************************************  
  */
ErrorStatus BJ_GetTimeWriteBitStatus(void)
{   
    if((DetProtocol.RecData.Data[1] & BJ_DET_MASK_SETTIME_OK))  // 判断BIT测试的状态
    {
        return SUCCESS;
    }
    return ERROR;
}

void BJ_SaveDetInfo(u8 Index)
{
	OSSchedLock();
	
	Channel.Info[Index].ser1 = DetProtocol.RecData.Data[1];   //保存雷管身份信息
    Channel.Info[Index].ser2 = DetProtocol.RecData.Data[2];
    Channel.Info[Index].ser3  = DetProtocol.RecData.Data[3];
    Channel.Info[Index].fac = DetProtocol.RecData.Data[4];
    Channel.Info[Index].bat  = DetProtocol.RecData.Data[5];
    Channel.Info[Index].year = DetProtocol.RecData.Data[6];
    Channel.Info[Index].freq = GetDetFreqFromHead();
	Channel.Info[Index].type = Channel.Info[Index].bat&0x0F;
	
	Channel.Info[Index].Pwd1 = Channel.Info[Index].ser3;//默认密码
	Channel.Info[Index].Pwd2 = 0XFF-Channel.Info[Index].ser1;//为了兼容老芯片，密码与序列码1相关
	OSSchedUnlock();
}
DET_ERR_CODE BJ_DetTypeTest(u8 DetType)
{
	if(UserParam.DetType == DET_TYPE_BJ_T)
	{
		if(DetType != UserParam.DetType)
		{
			return DET_TYPE_ERR; 
		}	
	}
	return DET_PASS;					
}
DET_ERR_CODE BJ_DetPwdTest(DET_INFO* det)
{
	ErrorStatus result;
	u8 pwd;
	
	pwd = det->Pwd1;
	
	result = BJ_DetTimeWr(det->ser1, det->ser2, pwd, 1000);
	if((result == ERROR) && (det->Pwd1 != det->Pwd2))
	{
		pwd = det->Pwd2;
		result = BJ_DetTimeWr(det->ser1, det->ser2, pwd, 1000);
	}
	if(result == ERROR)
	{
		return DET_PASSWORD_ERR;
	}
	det->Pwd1 = pwd;
	
	return DET_PASS;
}

DET_ERR_CODE BJ_DetBitTest(DET_INFO* det)
{
    unsigned char /*retry,*/retry1;
	ErrorStatus result;
	DET_ERR_CODE  err_code = DET_PASS;
	
	retry1=3;
	do
	{  
		BJ_DetClrAll();	     //发送全局复位指令
		OSTimeDly(50);
		BJ_DetClrAll();	     //发送全局复位指令
		OSTimeDly(50);
		
		BJ_DetBit(det->ser1, det->ser2);     //发送BIT1
		result = WaitRxPinHigh(5);
		if(ERROR != result)
		{
			//延时等待BIT测试完成,从波形看，需要600ms左右/6.7V, 5.5V,起爆电容68uf要1.3秒
			OSTimeDlyHMSM(0,0,1,500);
			result = BJ_DetTimeRd(det->ser1,det->ser2);
			if(result == ERROR )
			{//状态回读出现错误
				err_code = DET_BIT_STATUS_READ_ERR;	
			}
			else
			{
				if(BJ_GetBitStatus() == ERROR)
				{
					err_code = DET_BIT_ERR;	//Bit错误
				}
				else
				{
					err_code = DET_PASS;//BIT测试通过
				}				
			}		
		}
		else
		{
			err_code = DET_BIT_NO_RESPONSE;
		}
	}while((result == ERROR)&&(--retry1));

	return err_code;
}
DET_ERR_CODE BJ_DetChargeTest(DET_INFO* det)
{
    unsigned char /*retry,*/retry1;
	ErrorStatus result;
	DET_ERR_CODE  err_code = DET_PASS;
   
	retry1=3;
	do
	{    
		BJ_DetCharge(det->ser1, det->ser2, det->Pwd1);	   //发送CHARGE
		result = WaitRxPinHigh(5);
		if(ERROR == result)
		{
			err_code = DET_CHARGE_NO_RESPONSE;			
		}
		else
		{
			WaitRxPinReady(5000);
			OSTimeDly(100);
			
			result = BJ_DetTimeRd(det->ser1,det->ser2);
			if(result == ERROR )
			{//状态回读出现错误
				err_code = DET_CHARGE_STATUS_READ_ERR;	
			}
			else
			{
				if(BJ_GetChargeBitStatus() == ERROR)
				{
					err_code = DET_CHARGE_ERR;	//错误
				}
				else
				{
					err_code = DET_PASS;//测试通过
				}				
			}				
		}
	}while((result == ERROR)&&(--retry1));
	return err_code;
}
/**
  *************************************************************************************
  * @brief  准备起爆测试
  * @param  det 芯片数据
  * @retval 结果
  * @author ZH
  * @date   2018年6月13日
  * @note   无
  *************************************************************************************
  */
DET_ERR_CODE BJ_DetExpenTest(DET_INFO* det)
{
	ErrorStatus result;
	
	result = BJ_DetExpen(det->ser1, det->ser2, det->Pwd1); 
	if(result == ERROR )
	{
		return DET_NO_RESPONSE_EXPEN;
	}
	result = BJ_DetTimeRd(det->ser1,det->ser2);
	if(result == ERROR )
	{//延期时间回读出现错误
		return DET_EXPEN_TIMERD_ERR;		//BB
	}

	if(BJ_GetExpenBitStatus() == ERROR)
	{
		return DET_EXPEN_ERR;	//准备起爆错误			
	}
	return DET_PASS;//准备起爆测试通过	
}
void BJ_AllChannelDetClrAll(void)
{
	u8 i;
	
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		BJ_DetClrAll();
	}
	OSTimeDlyHMSM(0,0,0,50);
	for(i=0; i<Channel.Count; i++)
	{
		ConnectChannel(i);
		BJ_DetClrAll();
	}
    OSTimeDlyHMSM(0,0,0,10);	
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
void BJ_AllChannelDetSerlRd(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			if (BJ_DetSerlRd() == ERROR)//发送序列码回读指令	                     
			{
				Channel.Info[i].Status = DET_INFO_READ_ERR;//序列码通信错误		
			}
			else
			{
				BJ_SaveDetInfo(i);
			}					
		}		
	}	
}
void BJ_AllChannelDetSerlRdStatusTest(void)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.NewStatus[i])
		{
			ConnectChannel(i);
			if (BJ_DetSerlRd() == ERROR)//发送序列码回读指令	                     
			{
				Channel.NewStatus[i] = DET_INFO_READ_ERR;//序列码通信错误		
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
void BJ_AllChannelDetTypeTest(void)
{
	u8 i=0;
	DET_ERR_CODE Result;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{		
			Result = BJ_DetTypeTest(Channel.Info[i].type); //判断芯片类型
			if(Result != DET_PASS)
			{
				Channel.Info[i].Status = Result;
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
void BJ_AllChannelDetPwdTest(DET_ERR_CODE ErrCode)
{
	u8 i=0;
	
	for(i=0; i<Channel.Count; i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			if(BJ_DetPwdTest(&Channel.Info[i]) != DET_PASS)
			{
				Channel.Info[i].Status = ErrCode;//密码错误
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
void BJ_AllChannelDetBitTest(void)  	
{
	u8 i;
	ErrorStatus Result;
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			
			BJ_DetClrAll();	     //发送全局复位指令
            OSTimeDly(50);
            BJ_DetBit(Channel.Info[i].ser1, Channel.Info[i].ser2);     //发送BIT1
            if(WaitRxPinHigh(5) == ERROR)
            {
                OSTimeDly(50);
                BJ_DetClrAll();	     //发送全局复位指令
                OSTimeDly(100);
                BJ_DetBit(Channel.Info[i].ser1, Channel.Info[i].ser2);     //发送BIT1  
                if(WaitRxPinHigh(5) == ERROR)
                {
                    Channel.Info[i].Status = DET_BIT_NO_RESPONSE;	
                }
            }                       
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
            //延时等待BIT测试完成,从波形看，需要600ms左右/6.7V, 5.5V,起爆电容68uf要1.3秒
			OSTimeDlyHMSM(0,0,1,500);
			break;		
		}
	}
	
	for(i = 0; i < Channel.Count;i++)
	{
		if(DET_TESTING == Channel.Info[i].Status)
		{
			ConnectChannel(i);
			Result = BJ_DetTimeRd(Channel.Info[i].ser1,Channel.Info[i].ser2);          
			if(Result == ERROR)
			{//状态回读出现错误
				Channel.Info[i].Status = DET_BIT_STATUS_READ_ERR;	
			}
			else if(BJ_GetBitStatus() == ERROR)
			{				
                Channel.Info[i].Status = DET_BIT_ERR;	//Bit错误
			}		
		}
	}
}
void BJ_AllChannelDetChargeTest(void)
{
	u8 i;
	
	for(i = 0; i < Channel.Count;i++)
	{
        if(DET_TESTING == Channel.Info[i].Status)
        {
            ConnectChannel(i);
            BJ_DetCharge(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].Pwd1);	   //发送CHARGE
            if(WaitRxPinHigh(5) == ERROR)
            {
                OSTimeDly(50);
                BJ_DetClrAll();	     //发送全局复位指令
                OSTimeDly(100);
                BJ_DetCharge(Channel.Info[i].ser1, Channel.Info[i].ser2, Channel.Info[i].Pwd1);	   //发送CHARGE
                if(WaitRxPinHigh(5) == ERROR)
                {
                    Channel.Info[i].Status = DET_CHARGE_NO_RESPONSE;	
                }
            }         
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
			
			if(BJ_DetTimeRd(Channel.Info[i].ser1,Channel.Info[i].ser2) == ERROR)
			{//出现错误
				Channel.Info[i].Status = DET_CHARGE_STATUS_READ_ERR;		//BB
			}
			else if(BJ_GetChargeBitStatus() == ERROR)
			{
				Channel.Info[i].Status = DET_CHARGE_ERR;	//充电错误
			}		
		}
	}
}

void BJ_DetTest(void)
{	
	CLOSE_HV_SW();//关闭高压输出开关
	SET_LIN_OPEN();
	ClearDetInfo();
	
	OPEN_PW_HV();
	
	/*******放电******/
	BJ_AllChannelDetClrAll();
	
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
	BJ_AllChannelDetSerlRd();	
	
    BJ_AllChannelDetPwdTest(DET_WR_CODE_UNLOCK_PWD_ERR);
     
	/*******判断芯片频率是否正常*******/
	AllChannelDetFreqTest();

	/*********判断芯片类型的功能************/
	BJ_AllChannelDetTypeTest();

    if ((Channel.Count == 1) && (AllChannelTestEnd() == 0))
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
	//BIT测试
	if(TestParamIsSet(DET_BIT_TEST_MASK_BIT))//进行bit检测功能的选择
	{ 	
		BJ_AllChannelDetBitTest();
	}

	/*充电检测，测试电流*/
    if(TestParamIsSet(DET_CHARGE_TEST_MASK_BIT)) 
    {    
        BJ_AllChannelDetChargeTest();

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

	BJ_AllChannelDetClrAll();
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
		if(NeedScan() && (DetOutsideVerify(Channel.BarCode[0].CodeExternal) != ERROR))
		{//保存外码，方便工作按10发一盒进行装盒，每盒中的雷管要求外码前12位相同
			SaveDetOutsideCodeQueueItem(&Channel.BarCode[0].CodeExternal[0]);
		}		
	}
}

u16 BJ_DetAnalyst(void)
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
	BJ_DetClrAll();
	OSTimeDlyHMSM(0,0,0,50);
	BJ_DetClrAll();
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
		if(ERROR != BJ_DetSerlRd())
		{
			BJ_SaveDetInfo(0);
			AnalystResult |= 0x0001;//序列码读取成功
			
            if(DET_PASS == BJ_DetPwdTest(&Channel.Info[0]))
            {
                AnalystResult |= 0x0010;//密码正确
            }
            
			OSTimeDlyHMSM(0,0,0,10);
			if(ERROR != BJ_DetTimeRd(Channel.Info[0].ser1, Channel.Info[0].ser2)) 
			{
				AnalystResult |= 0x0002;//延期时间回读正常
			}
			OSTimeDlyHMSM(0,0,0,10);
			if(DET_PASS == BJ_DetBitTest(&Channel.Info[0]))
			{
				AnalystResult |= 0x0004;//BIT正常正常
			}
			
			//写延期时间检测
			OSTimeDlyHMSM(0,0,0,10);
			if(ERROR != BJ_DetTimeWr(Channel.Info[0].ser1,Channel.Info[0].ser2,Channel.Info[0].Pwd1, 0x100))
			{
				AnalystResult |= 0x0008;//写延期正常
			}
		
			OSTimeDlyHMSM(0,0,0,10);
			Result = BJ_DetChargeTest(&Channel.Info[0]);
			if(DET_PASS == Result) 
			{			
				AnalystResult |= 0x0020;//充电正常
			}
			
			BJ_DetMask(Channel.Info[0].ser1,Channel.Info[0].ser2);//屏蔽雷管，方便重新发现新雷管
			
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
			BJ_DetClrAll();
			OSTimeDlyHMSM(0,0,0,100);
			BJ_DetClrAll();
			OSTimeDlyHMSM(0,0,0,100);			
		}	
	}

	return AnalystResult;
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
void BJ_QueryDetConnectStatus(void)
{
	CLOSE_HV_SW();//关闭高压输出开关
	SET_LIN_OPEN();
    
	ClearNewStatus();
	
	/*******放电******/
	BJ_AllChannelDetClrAll();
	
	AllChannelLinShortTest(1);//总线短路测试
	AllChannelOpenCircuitTest(1);//总线断路测试
			
	/*******监测端口电压是否正常******/
//	AllChannelVoltageTest(0);//先测试电流后再测试电压，不然电压测试不准，因为AD采样点的电容充电较慢
	
	/***序列码回读检测****/
	BJ_AllChannelDetSerlRdStatusTest();	
	
	AllChannelNewStatusReplace(DET_TESTING, DET_PASS);
}
void BJ_WaitAllChannelDetOff(void)
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
			if (BJ_DetCheckIn() != ERROR)//发送序列码回读指令	                     
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
void BJ_DetDataBindingTest(void)
{
	CLOSE_HV_SW();//关闭高压输出开关
	SET_LIN_OPEN();
	ClearDetInfo();
	
	OPEN_PW_HV();
	
	/*******放电******/
	BJ_AllChannelDetClrAll();
	
	AllChannelLinShortTest(0);//总线短路测试
	AllChannelOpenCircuitTest(0);//总线断路测试
    
	/***序列码回读检测****/
	BJ_AllChannelDetSerlRd();	
     
	/*******判断芯片频率是否正常*******/
	AllChannelDetFreqTest();

	/*********判断芯片类型的功能************/
	BJ_AllChannelDetTypeTest();
    
    if ((Channel.Count == 1) && (AllChannelTestEnd() == 0))
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
    
	BJ_AllChannelDetClrAll();
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
		if(NeedScan() && (DetOutsideVerify(Channel.BarCode[0].CodeExternal) != ERROR))
		{//保存外码，方便工作按10发一盒进行装盒，每盒中的雷管要求外码前12位相同
			SaveDetOutsideCodeQueueItem(&Channel.BarCode[0].CodeExternal[0]);
		}		
	}
}


