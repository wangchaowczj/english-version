#include "includes.h"

BARCODE BarCode;


u16 GetScanDataBytes(void)
{
    u16 Bytes;
    
    Bytes = ScanData.Bytes;
    while (Bytes > 0)
    {
        if((ScanData.Buffer[Bytes-1] == '\r') || (ScanData.Buffer[Bytes-1] == '\n') || (ScanData.Buffer[Bytes-1] == '\t'))
        {
            Bytes--;           
        }
        else
        {
            break;
        }
    }
    return Bytes;
}

/**
  *************************************************************************************
  * @brief 条码分析
  * @param[in] 无
  * @param[out]无
  * @retval 无 
  * @author ZH
  * @date   2019年4月25日
  * @note   13位管码
  *************************************************************************************  
  */
void BarCodeAnalyst13(void)
{
    u16 StartIndex = 0;
    
    if(GetScanDataBytes() > 13)
    {
        StartIndex = 1;
    }
    BarCode.DetType = Det.Type;
    BarCode.Delay = 0x00;
    if(ERROR == XA3_DetOutside2Inside(BarCode.DetType, &ScanData.Buffer[StartIndex+0], &BarCode.DetSerl.U32Value, &BarCode.DetPwd.U32Value))
    {
        return;
    }
    
    BarCode.Status = BARCODE_NEW;	    
}
/**
  *************************************************************************************
  * @brief 条码分析
  * @param[in] 无
  * @param[out]无
  * @retval 无 
  * @author ZH
  * @date   2019年4月25日
  * @note   14的条码有两种，一种是山东圣世达的1位前导码+13位管码
  *另一种是江西、内蒙自动化生产线的 13位管码+1位芯片类型
  *************************************************************************************  
  */
void BarCodeAnalyst14(void)
{ 
    if(BlasterIs36())//山东圣世达扫出来的码有可能是14位，第1位为前导码0
    {//1位前导码+13位管码
        if((ScanData.Buffer[0] == '0') && (ScanData.Buffer[1] == '3')&& (ScanData.Buffer[2] == '6'))
        {
            BarCodeAnalyst13();
            return;        
        }
    }
    else
    {//13位管码+1位芯片类型        
        if(!IsHexChar(ScanData.Buffer[13]))
        {
            return;
        }
        BarCode.DetType = HexCharToValue(ScanData.Buffer[13]);
        BarCode.Delay = 0x00;
        if(ERROR == XA3_DetOutside2Inside(BarCode.DetType, &ScanData.Buffer[0], &BarCode.DetSerl.U32Value, &BarCode.DetPwd.U32Value))
        {
            return;
        }

        BarCode.Status = BARCODE_NEW;	       
    }
}
/**
  *************************************************************************************
  * @brief 条码分析
  * @param[in] 无
  * @param[out]无
  * @retval 无 
  * @author ZH
  * @date   2018年6月11日
  * @note   17位条码：1位条码类型+13位管码+1位雷管类型+2位CRC8校验，
  *************************************************************************************  
  */
void BarCodeAnalyst17(void)
{
    u16 i;
    u8 CRC8;
    
    if(ScanData.Buffer[0] != '0')//条码类型
    {
        return;
    }
    for(i=1; i<8; i++)
    {
        if((ScanData.Buffer[i] < '0') || (ScanData.Buffer[i] >'9'))
        {
            return;
        }
    }
    for(i=9; i< 14; i++)
    {
        if((ScanData.Buffer[i] < '0')||(ScanData.Buffer[i] >'9'))
        {
            return;
        }
    }
    for(i=14; i< 17; i++)
    {
        if(!IsHexChar(ScanData.Buffer[i]))
        {
            return;
        }
    }
    CRC8 = (HexCharToValue(ScanData.Buffer[15])<<4)+HexCharToValue(ScanData.Buffer[16]);
    if(CRC8 != GetCrc8Value(&ScanData.Buffer[0], 15))
    {
        return;
    }
    
    BarCode.DetType = HexCharToValue(ScanData.Buffer[14]);
    BarCode.Delay = 0x00;
    if(ERROR == XA3_DetOutside2Inside(BarCode.DetType, &ScanData.Buffer[1], &BarCode.DetSerl.U32Value, &BarCode.DetPwd.U32Value))
    {
        return;
    }
   
    BarCode.Status = BARCODE_NEW;
}
/**
  *************************************************************************************
  * @brief 箱条码分析
  * @param[in] 无
  * @param[out]无
  * @retval 无 
  * @author ZH
  * @date   2018年6月11日
  * @note   J44zc01A161201022,目前发现，条码在最后会多出一个字母，用途未知,估计是用于校验
  *************************************************************************************  
  */
ErrorStatus BarCodeAnalyst17ForBoxBig(BOX_BIG_INFO* BoxBigInfo)
{
    u16 i;
    u16 U16Value;
    
    if(GetScanDataBytes() != 18)//规则应该是17位，实际扫出来是18位
    {
        return ERROR;
    }
    //产品类型码
    if ((ScanData.Buffer[0] == '1') || (ScanData.Buffer[0] == 'O') || (ScanData.Buffer[0] == 'o'))
    {
        return ERROR;
    }
    else if (!(((ScanData.Buffer[0] >= '0') && (ScanData.Buffer[0] <= '8')) || 
        ((ScanData.Buffer[0] >= 'A') && (ScanData.Buffer[0] <= 'Z')) ||
        ((ScanData.Buffer[0] >= 'a') && (ScanData.Buffer[0] <= 'x'))))
    {
        return ERROR;
    }
    //工厂代码
    for(i=1; i<3; i++)
    {
        if((ScanData.Buffer[i] < '0') || (ScanData.Buffer[i] >'9'))
        {
            return ERROR;
        }
    }
    U16Value = (ScanData.Buffer[1]-'0')*10+(ScanData.Buffer[2]-'0');
    if((U16Value == 0) || (U16Value > 74))
    {
        return ERROR;
    }
    if(BoxBigInfo != NULL)
    {
        BoxBigInfo->Factory = U16Value;
    }
    
    //装箱规格,表示每箱有多少盒
    for(i=5; i<7; i++)
    {
        if((ScanData.Buffer[i] < '0') || (ScanData.Buffer[i] >'9'))
        {
            return ERROR;
        }
    }
    U16Value = (ScanData.Buffer[5]-'0')*10+(ScanData.Buffer[6]-'0');
    if (U16Value == 0)
    {
        return ERROR;
    }
    if(BoxBigInfo != NULL)
    {
        BoxBigInfo->InBoxBig = U16Value;
    }
    
    //装盒规格,表示每盒有多少发
    if((ScanData.Buffer[7] >= '1') && (ScanData.Buffer[7] <= '9'))
    {
        U16Value = 5+(ScanData.Buffer[7] - '1')*5;    
    }
    else if((ScanData.Buffer[7] >= 'A') && (ScanData.Buffer[7] <= 'K'))
    {
        U16Value = 50+(ScanData.Buffer[7] - 'A')*5;    
    }
    else if((ScanData.Buffer[7] >= 'L') && (ScanData.Buffer[7] <= 'N'))
    {
        U16Value = 1+(ScanData.Buffer[7] - 'L');    
    }
    else if(ScanData.Buffer[7] == 'O')
    {//空
        return ERROR;   
    }
    else if(ScanData.Buffer[7] == 'P')
    {
        U16Value = 4;    
    }
    else if((ScanData.Buffer[7] >= 'Q') && (ScanData.Buffer[7] <= 'T'))
    {
        U16Value = 6+(ScanData.Buffer[7] - 'Q');    
    } 
    else if((ScanData.Buffer[7] >= 'U') && (ScanData.Buffer[7] <= 'X'))
    {
        U16Value = 11+(ScanData.Buffer[7] - 'U');    
    }
    else if((ScanData.Buffer[7] >= 'Y') && (ScanData.Buffer[7] <= 'Z'))
    {
        U16Value = 16+(ScanData.Buffer[7] - 'Y');    
    }
    else if((ScanData.Buffer[7] >= 'a') && (ScanData.Buffer[7] <= 'b'))
    {
        U16Value = 18+(ScanData.Buffer[7] - 'a');    
    }
    else
    {
        return ERROR;
    }
    if((U16Value==0) || (U16Value > 100))
    {
        return ERROR;
    }    
    if(BoxBigInfo != NULL)
    {
        BoxBigInfo->InBoxSmall = U16Value;
    }
    
    //生产日期，6位
    for(i=8; i<14; i++)
    {
        if((ScanData.Buffer[i] < '0') || (ScanData.Buffer[i] >'9'))
        {
            return ERROR;
        }
    }
    if(BoxBigInfo != NULL)
    {
        BoxBigInfo->Year = (ScanData.Buffer[8]-'0')*10+(ScanData.Buffer[9]-'0');//年
    }
    
    U16Value = (ScanData.Buffer[10]-'0')*10+(ScanData.Buffer[11]-'0');//月
    if ((U16Value == 0) || (U16Value > 12))
    {
        return ERROR;
    }
    if(BoxBigInfo != NULL)
    {
        BoxBigInfo->Month = U16Value;
    }
    
    U16Value = (ScanData.Buffer[12]-'0')*10+(ScanData.Buffer[13]-'0');//日
    if ((U16Value == 0) || (U16Value > 31))
    {
        return ERROR;
    }
    if(BoxBigInfo != NULL)
    {
        BoxBigInfo->Day = U16Value;
    }
    //箱号，3位
    for(i=14; i<17; i++)
    {
        if((ScanData.Buffer[i] < '0') || (ScanData.Buffer[i] >'9'))
        {
            return ERROR;
        }
    }
    if(BoxBigInfo != NULL)
    {
        BoxBigInfo->Box = (ScanData.Buffer[14]-'0')*100+(ScanData.Buffer[15]-'0')*10+(ScanData.Buffer[16]-'0');
    }    
    return SUCCESS;
}
/**
  *************************************************************************************
  * @brief 盒条码分析
  * @param[in] 无
  * @param[out]无
  * @retval 无 
  * @author ZH
  * @date   2018年6月11日
  * @note   N610a201021018C168,目前发现，条码在最后会多出一个字母，用途未知,估计是用于校验
  *************************************************************************************  
  */
ErrorStatus BarCodeAnalyst18ForBoxSmall(BOX_SMALL_INFO* BoxSmallInfo)
{
    u16 i;
    u16 U16Value;
    
    if(GetScanDataBytes() != 19)//规则应该是18位，实际扫出来是19位
    {
        return ERROR;
    }
    //产品类型码
    if ((ScanData.Buffer[0] == '1') || (ScanData.Buffer[0] == 'O') || (ScanData.Buffer[0] == 'o'))
    {
        return ERROR;
    }
    else if (!(((ScanData.Buffer[0] >= '0') && (ScanData.Buffer[0] <= '8')) || 
        ((ScanData.Buffer[0] >= 'A') && (ScanData.Buffer[0] <= 'Z')) ||
        ((ScanData.Buffer[0] >= 'a') && (ScanData.Buffer[0] <= 'x'))))
    {
        return ERROR;
    }
    //工厂代码
    for(i=1; i<3; i++)
    {
        if((ScanData.Buffer[i] < '0') || (ScanData.Buffer[i] >'9'))
        {
            return ERROR;
        }
    }
    U16Value = (ScanData.Buffer[1]-'0')*10+(ScanData.Buffer[2]-'0');
    if((U16Value == 0) || (U16Value > 74))
    {
        return ERROR;
    }
    if(BoxSmallInfo != NULL)
    {
        BoxSmallInfo->Factory = U16Value;
    }
    
    //装盒规格, 每盒装多少发
    if (((ScanData.Buffer[5] >= '0') && (ScanData.Buffer[5] <= '9')) && 
        ((ScanData.Buffer[6] >= '0') && (ScanData.Buffer[6] <= '9')))
    {//都是数字时，为5进制
        U16Value = (ScanData.Buffer[5]-'0')*10+(ScanData.Buffer[6]-'0');
        U16Value *= 5;
    }
    else
    {
        if((ScanData.Buffer[5] >= '0') && (ScanData.Buffer[5] <= '9'))
        {
            U16Value = ScanData.Buffer[5]-'0';
        }
        else if(ScanData.Buffer[5] == 'A')
        {
            U16Value = 0;
        }
        else if(ScanData.Buffer[5] == 'B')
        {
            U16Value = 1;
        }
        else
        {
            return ERROR;
        }
        U16Value *= 10;
        
        if((ScanData.Buffer[6] >= '0') && (ScanData.Buffer[6] <= '9'))
        {
            U16Value += ScanData.Buffer[6]-'0';
        }
        else if(ScanData.Buffer[6] == 'A')
        {
            U16Value += 0;
        }
        else if(ScanData.Buffer[6] == 'B')
        {
            U16Value += 1;
        }
        else
        {
            return ERROR;
        }
    }
    if((U16Value==0) || (U16Value > 100))
    {
        return ERROR;
    }
    if(BoxSmallInfo != NULL)
    {
        BoxSmallInfo->InBoxSmall = U16Value;
    }    
    //生产日期，6位
    for(i=8; i<14; i++)
    {
        if((ScanData.Buffer[i] < '0') || (ScanData.Buffer[i] >'9'))
        {
            return ERROR;
        }
    }
    if(BoxSmallInfo != NULL)
    {
        BoxSmallInfo->Year = (ScanData.Buffer[8]-'0')*10+(ScanData.Buffer[9]-'0');//年
    }
    
    U16Value = (ScanData.Buffer[10]-'0')*10+(ScanData.Buffer[11]-'0');//月
    if ((U16Value == 0) || (U16Value > 12))
    {
        return ERROR;
    }
    if(BoxSmallInfo != NULL)
    {
        BoxSmallInfo->Month = U16Value;
    }
    
    U16Value = (ScanData.Buffer[12]-'0')*10+(ScanData.Buffer[13]-'0');//日
    if ((U16Value == 0) || (U16Value > 31))
    {
        return ERROR;
    }
    if(BoxSmallInfo != NULL)
    {
        BoxSmallInfo->Day = U16Value;
    }
    
    //盒号
    for(i=15; i<18; i++)
    {
        if((ScanData.Buffer[i] < '0') || (ScanData.Buffer[i] >'9'))
        {
            return ERROR;
        }
    }
    if(BoxSmallInfo != NULL)
    {
        BoxSmallInfo->Box = (ScanData.Buffer[15]-'0')*100+(ScanData.Buffer[16]-'0')*10+(ScanData.Buffer[17]-'0');
    }     
    return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  分析扫码枪扫到的数据
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年7月2日
  * @note   无
  *************************************************************************************  
  */
void BarCodeDataAnalyst(void)
{
	u16 Bytes;
    
	BarCode.Status = BARCODE_ERROR;
    Bytes = GetScanDataBytes();
	if (Bytes == 13)
	{
		BarCodeAnalyst13();
	}
	else if (Bytes == 14)
	{
		BarCodeAnalyst14();
	}
    else if (Bytes == 17)
    {
        BarCodeAnalyst17();
    }
    
    if(BarCode.Status != BARCODE_NEW)
    {
        return;
    }
    if(!DetCodeRulePass(BarCode.DetSerl.U32Value, BarCode.DetPwd.U32Value))
    {
        BarCode.Status = BARCODE_ERROR;
        return;
    }
    
	//验证得到的码信息与用户设置的信息是否相符
	if(Det.Type != BarCode.DetType)
	{
		BarCode.Result = BARCODE_DET_TYPE_ERR;
	}
	else if(BarCode.Delay > GetDetMaxDelay(BarCode.DetType))
	{
		BarCode.Result = BARCODE_DELAY_ERR;
	}
	else if(!DetFactoryCodeIsRulePass(BarCode.DetSerl.U32Value, BarCode.DetPwd.U32Value))
	{
		BarCode.Result = BARCODE_FACTORY_ERR;
	}    
	else
	{
		BarCode.Result = BARCODE_PASS;
	}	
}

