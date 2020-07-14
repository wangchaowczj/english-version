#include "includes.h"

u8 FactoryCodeSupport[FACTORY_CODE_SUPPORT_MAX] = {44,34,56,10,36,53,52 };//支持的工厂代码
//卫东机械厂对应44  江西钢丝厂对应32  四川宜宾威力对应56  内蒙红旗对应10 山东圣世达对应 36  广西金建华对应53 广东韶华52
BARCODE BarCode;
/**
  *************************************************************************************
  * @brief  外部工厂代码转换成芯片的工厂代码
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年7月2日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus FactoryCodeOutsideToInside(u8 Outside, u8* Inside)
{
	u8 i;
	
	for(i=0;i< FACTORY_CODE_SUPPORT_MAX;i++)
	{
		if(Outside == FactoryCodeSupport[i])
		{
			*Inside = i+1;
			return SUCCESS;
		}
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  成芯片的工厂代码转换成外部工厂代码
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2016年7月2日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus FactoryCodeInsideToOutside(u8 Inside, u8 *Outside)
{	
	if ((Inside == 0) || (Inside >= FACTORY_CODE_SUPPORT_MAX))
	{
		return ERROR;
	}
	if ((FactoryCodeSupport[Inside-1] >= 1) && (FactoryCodeSupport[Inside-1] <= 74))
	{
		*Outside = FactoryCodeSupport[Inside-1];
		return SUCCESS;
	}
	return ERROR;
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
//void BarCodeDataAnalyst(void)
//{
//	u8 ScanBuffer[35];
//	u8 CRC16[2];
//	u8 Buffer[10];
//	u8 Year, Month, Day, Code, BoxInsideIndex;
//	u16 BoxNumber;
//	
//	BarCode.Status = BARCODE_ERROR;
//	if ((ScanKeyBoard.Bytes != 34) || (ScanKeyBoard.Status != SCAN_KEYBOARD_END))
//	{
//		return;
//	}
//	memset((char*)ScanBuffer, 0, sizeof(ScanBuffer));
//	memcpy((char*)ScanBuffer,  &ScanKeyBoard.Data[0], 34);
//	
//	//校验，判断数据是否正确
//	GetCrc16Value(0xFFFF, CRC16, ScanBuffer, 30);
//	HexStrToCharStr(&ScanBuffer[30], 4, &Buffer[0]);
//	if((Buffer[0] != CRC16[0]) || (Buffer[1] != CRC16[1]))
//	{
//		return;
//	}
//	
//	//第2-14位表示外码
//	memset((char*)BarCode.CodeExternal, 0, sizeof(BarCode.CodeExternal));
//	memcpy((char*)BarCode.CodeExternal, &ScanBuffer[1], 13);
//	
//	//第2、3位：生产企业代码，用“01～99”2位阿拉伯数字表达
//	Code = (ScanBuffer[1] - '0')*10 + (ScanBuffer[2] - '0');
//	if(FactoryCodeOutsideToInside(Code, &BarCode.FactoryCode) == ERROR)
//	{
//		return;
//	}
//	
//	//第4位：生产年份代码：用：“0～9”1位阿拉伯数字表达
//	Year = ScanBuffer[3] - '0';
//	if(Year > 9)
//	{
//		return;
//	}
//	//第5、6位：生产月份代码：用“01～12”2位阿拉伯数字表达
//	Month = (ScanBuffer[4] - '0')*10 + (ScanBuffer[5] - '0');
//	if ((Month == 0) || (Month > 12))
//	{
//		return;
//	}
//	//第7、8位：生产日期代码：用“01～31”2位阿拉伯数字表达
//	Day = (ScanBuffer[6] - '0')*10 + (ScanBuffer[7] - '0');
//	if ((Day == 0) || (Day > 31))
//	{
//		return;
//	}	
//	BarCode.ProductTime = Year*10000UL+Month*100UL+Day;
//	
//	//第9位：特征号
//	BarCode.Feature = ScanBuffer[8];
//	
//	//第10～12位：盒号
//	BoxNumber = (ScanBuffer[9] - '0')*100 + (ScanBuffer[10] - '0')*10 + (ScanBuffer[11] - '0');
//	if(BoxNumber > 999)
//	{
//		return;
//	}
//	//第13～14位：盒内流水号
//	BoxInsideIndex = (ScanBuffer[12] - '0')*10 + (ScanBuffer[13] - '0');
//	if(BoxInsideIndex > 99)
//	{
//		return;
//	}	
//	//第15～20位：芯片内码，用“000000～FFFFFF”十六进制表示芯片的内码
//	HexStrToCharStr(&ScanBuffer[14], 6, &BarCode.DetCode[0]);

//	//第21～24位：用“0000～1500”表示雷管的延期时间从0ms至1500ms，在线可编程电子雷管显示字母PPPP
//	memset(Buffer, 0, sizeof(Buffer));
//	memcpy(Buffer, &ScanBuffer[20], 4);
//	if(ERROR == CheckNumberChars((char*)Buffer, 0, 4))
//	{//在线可编程
//		BarCode.Delay = 0x00;
//	}
//	else
//	{
//		BarCode.Delay = (u16)AsciiToU32(Buffer);
//	}
//	
//	//第25、26位，用两位数字“02～99”表示脚线的长度2～99米，输入
//	memset(Buffer, 0, sizeof(Buffer));
//	memcpy(Buffer, &ScanBuffer[24], 2);	
//	BarCode.DetLineLength = (u16)AsciiToU32(Buffer);
//	
//	//第27位：表示雷管类型：
//	switch(ScanBuffer[26]-'0')
//	{
//		case DET_TYPE_XA_02:
//		case DET_TYPE_XA_06:
//		case DET_TYPE_XA_21:
//			BarCode.DetType = ScanBuffer[26]-'0';
//			break;			
//		default:
//			return;
//	}
//	//确认延期不超限
//	if(BarCode.Delay > GetMaxDelay(BarCode.DetType))
//	{
//		return;
//	}
//	/*
//	第28位：表示脚线材质类型：
//	脚线类型定义：
//	0001隧道用脚线
//	0002普通圆脚线
//	0003耐高温脚线
//	*/
//	switch(ScanBuffer[27]-'0')
//	{
//		case 1:
//			BarCode.DetLineType = TUNNEL;
//			break;
//		case 2:
//			BarCode.DetLineType = ROUND_GENERAL;
//			break;
//		case 3:
//			BarCode.DetLineType = DURABLE;
//			break;
//		default:
//			return;
//	}	
//	//第29、30位为保留位，默认为0
//	if(ScanBuffer[29] != '0')
//	{
//		return;
//	}
//	if(ScanBuffer[28] == '0')
//	{//保留位“00”
//		BarCode.DetCode[3] = (BarCode.FactoryCode<<4)+0x00;//高4bit表示工厂代码,低4bit表示晶圆生产批流水号(在写内码前先读出再写入)
//		BarCode.DetCode[4] = (BarCode.DetType<<5)+(Day&0x1F);//高3bit表示芯片类型,低5bit表示生产日期（1～31）
//		BarCode.DetCode[5] = (Month<<4)+(Year&0x0F);//高4bit表示生产月份（1～12）,低4bit表示生产年份（0～9）		
//	}
//	else if(ScanBuffer[28] == '1')
//	{//保留位“10”
//		if ((BarCode.Feature >= '0')&&(BarCode.Feature <= 'F'))//有效特征号为"0"-"F"
//		{
//			if((BarCode.Feature >= '0') && (BarCode.Feature <= '9'))
//			{
//				BarCode.DetCode[3] = (BarCode.FactoryCode<<4)+(BarCode.Feature-'0');
//			}
//			else
//			{
//				BarCode.DetCode[3] = (BarCode.FactoryCode<<4)+(BarCode.Feature-'A'+10);
//			}
//			BarCode.DetCode[4] = (BarCode.DetType<<5)+(Day&0x1F);//高3bit表示芯片类型,低5bit表示生产日期（1～31）
//			BarCode.DetCode[5] = (Year*12+(Month-1))&0x7F;//低7bit表示生产年和月，最大值是X9年12月即9*12+11
//			if((BoxNumber*100L+BoxInsideIndex) > 0xFFFF)
//			{
//				BarCode.DetCode[5] |= 0x80;//bit7表示盒流水号已经超过0xFFFF
//			}
//		}
//		else
//		{
//			return;
//		}
//	}	
//	else
//	{
//		return;
//	}
//	
//	//第31、32、33、34位为CRC16校验位。
//	
//	//验证得到的码信息与用户设置的信息是否相符
//	if(UserParam.DetType != BarCode.DetType)
//	{
//		BarCode.Result = BARCODE_DET_TYPE_ERR;
//	}
//	else if(UserParam.DetLineType != BarCode.DetLineType)
//	{
//		BarCode.Result = BARCODE_DET_LINE_TYPE_ERR;
//	}
//	else if(UserParam.DetLineLength != BarCode.DetLineLength)
//	{
//		BarCode.Result = BARCODE_DET_LINE_LENGTH_ERR;
//	}
//	else if(UserParam.FactoryCode != BarCode.FactoryCode)
//	{
//		BarCode.Result = BARCODE_FACTORY_ERR;
//	}
//	else if(UserParam.ProductTime != BarCode.ProductTime)
//	{
//		BarCode.Result = BARCODE_PRODUCT_TIME_ERR;
//	}
//	else if(UserParam.Feature != BarCode.Feature)
//	{
//		BarCode.Result = BARCODE_FEATURE_ERR;
//	}	
//	else if(UserParam.DetDelay != BarCode.Delay)
//	{
//		BarCode.Result = BARCODE_DELAY_ERR;
//	}
//	else
//	{
//		BarCode.Result = BARCODE_PASS;
//	}
//		
//	BarCode.Status = BARCODE_NEW;		
//}
void BarCodeDataAnalyst(void)
{
    u16 i;
    u8 CRC8;
    U32_UNION Serl, Pwd;
	u8 ScanBuffer[18];
	u8 Year, Month, Day, Code;
	
	memset((char*)ScanBuffer, 0, sizeof(ScanBuffer));
	memcpy((char*)ScanBuffer,  &ScanKeyBoard.Data[0], 18);
    
    if(ScanBuffer[0] != '0')//条码类型
    {
        return;
    }
    for(i=1; i<8; i++)
    {
        if((ScanBuffer[i] < '0') || (ScanBuffer[i] >'9'))
        {
            return;
        }
    }

    for(i=9; i< 14; i++)
    {
        if((ScanBuffer[i] < '0')||(ScanBuffer[i] >'9'))
        {
            return;
        }
    }
    for(i=14; i< 17; i++)
    {
        if((ScanBuffer[i] < '0')&&(ScanBuffer[i] > 'F'))//检查是不是HEX
        {
            return;
        }
    }
    CRC8 = (HexCharToValue(ScanBuffer[15])<<4)+HexCharToValue(ScanBuffer[16]);
    if(CRC8 != GetCrc8Value(&ScanBuffer[0], 15))
    {
        return;
    }
    
    BarCode.DetType = HexCharToValue(ScanBuffer[14]);
    BarCode.Delay = 0x00;
    if(ERROR == XA2_DetOutside2Inside(BarCode.DetType, &ScanBuffer[1], &Serl.U32Value, &Pwd.U32Value))
    {
        return;
    }
    if(!DetCodeRulePass(Serl.U32Value, Pwd.U32Value))
    {
        return;
    }
	//	//第2、3位：生产企业代码，用“01～99”2位阿拉伯数字表达
	Code = (ScanBuffer[1] - '0')*10 + (ScanBuffer[2] - '0');
	if(FactoryCodeOutsideToInside(Code, &BarCode.FactoryCode) == ERROR)
	{
		return;
	}
	
	//第4位：生产年份代码：用：“0～9”1位阿拉伯数字表达
	Year = ScanBuffer[3] - '0';
	if(Year > 9)
	{
		return;
	}
	//第5、6位：生产月份代码：用“01～12”2位阿拉伯数字表达
	Month = (ScanBuffer[4] - '0')*10 + (ScanBuffer[5] - '0');
	if ((Month == 0) || (Month > 12))
	{
		return;
	}
	//第7、8位：生产日期代码：用“01～31”2位阿拉伯数字表达
	Day = (ScanBuffer[6] - '0')*10 + (ScanBuffer[7] - '0');
	if ((Day == 0) || (Day > 31))
	{
		return;
	}	
	BarCode.ProductTime = Year*10000UL+Month*100UL+Day;
	
	//第9位：特征号
	BarCode.Feature = ScanBuffer[8];
	
	//	//验证得到的码信息与用户设置的信息是否相符
	if(UserParam.DetType != BarCode.DetType)
	{
		BarCode.Result = BARCODE_DET_TYPE_ERR;
	}
	else if(UserParam.FactoryCode != BarCode.FactoryCode)
	{
		BarCode.Result = BARCODE_FACTORY_ERR;
	}
	else if(UserParam.ProductTime != BarCode.ProductTime)
	{
		BarCode.Result = BARCODE_PRODUCT_TIME_ERR;
	}
	else if(UserParam.Feature != BarCode.Feature)
	{
		BarCode.Result = BARCODE_FEATURE_ERR;
	}	
	else if(UserParam.DetDelay != BarCode.Delay)
	{
		BarCode.Result = BARCODE_DELAY_ERR;
	}
	else
	{
		BarCode.Result = BARCODE_PASS;
	}
    memcpy(&BarCode.DetCode[0], &Serl.U8Buffer[0], 4);
    memcpy(&BarCode.DetCode[4], &Pwd.U8Buffer[0], 4);
    memcpy(&BarCode.CodeExternal, &ScanBuffer[1], 13);
	
    BarCode.Status = BARCODE_NEW;
	
}


/**
  *************************************************************************************
  * @brief  内码转管码
  * @param  Serl 序列码
  * @param  密码 Pwd
  * @param  Ptr 管码
  * @retval 结果
  * @author ZH
  * @date   2019年4月20日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus XA2_DetInside2Outside(u32 Serl, u32 Pwd, u8* Ptr)
{
    uint64_t U64Value;
    u16 U16Value;
    char Buffer[3];
    
    if(!DetCodeRulePass(Serl, Pwd))
    {
        return ERROR;
    }
    
    U64Value = Pwd;
    U64Value <<= 32;
    U64Value |= Serl;
    
    U16Value = (U64Value>>39)&0x7F;//工厂代码bit39-bit45共7位
    if((U16Value == 0) || (U16Value > 74))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)Ptr, "%02d", U16Value);    
    }
    
    U16Value = (U64Value>>35)&0x0F;//年bit35-bit38共4位
    if(U16Value > 9)
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+2), "%d", U16Value);    
    }

    U16Value = (U64Value>>22)&0x0F;//月bit22-bit25共4位
    if((U16Value == 0) || (U16Value > 12))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+3), "%02d", U16Value);    
    }

    U16Value = (U64Value>>17)&0x1F;//日bit17-bit21共5位
    if((U16Value == 0) || (U16Value > 31))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+5), "%02d", U16Value);    
    }
    
    U16Value = (U64Value>>27)&0xFF;//特征号bit27-bit34共8位,数字或英文ASCII码
    if((U16Value < 0x30) || (U16Value > 0x80))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        *(Ptr+7) = U16Value;    
    }
     
    U16Value = (U64Value>>7)&0x3FF;//盒号bit7-bit16共10位
    if(U16Value > 999)
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+8), "%03d", U16Value);    
    }
    
    U16Value = U64Value&0x7F;//盒内流水号bit0-bi6共7位
    if(U16Value > 99)
    {
        return ERROR;
    }
    sprintf(Buffer, "%02d", U16Value);
    if(Ptr != NULL)
    {
        memcpy((char*)(Ptr+11), Buffer, 2);
    } 
    
    return SUCCESS;
}
/**
  *************************************************************************************
  * @brief  外码转内码
  * @param  Serl 序列码
  * @param  密码 Pwd
  * @param  Ptr 管码
  * @retval 结果
  * @author ZH
  * @date   2019年4月20日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus XA2_DetOutside2Inside(u8 DetType, u8* Ptr, u32* Serl, u32* Pwd)
{
    uint64_t U64Value = 0;
    u32 Value;
    u8 Buffer[8];
    
    //盒内流水号bit0-bi6共7位
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+11, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if(Value > 99)
    {
        return ERROR;
    }
    
    U64Value = Value;

    //盒内bit7-bit16共10位
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+8, 3);
    sscanf((char*)Buffer, "%d", &Value);
    if(Value > 999)
    {
        return ERROR;
    }
    U64Value |= Value<<7;

    //日 bit17-bit21共5位
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+5, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0) || (Value > 31))
    {
        return ERROR;
    }
    U64Value |= Value<<17;

    //月 bit22-bit25共4位
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+3, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0) || (Value > 12))
    {
        return ERROR;
    }
    U64Value |= Value<<22; 

    //月 bit22-bit25共4位
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+3, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0) || (Value > 12))
    {
        return ERROR;
    }
    U64Value |= Value<<22; 
    
    // 年的低位 bit26共1位
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+2, 1);
    sscanf((char*)Buffer, "%d", &Value);
    if(Value > 9)
    {
        return ERROR;
    }
    U64Value |= (Value&0x01)<<26; 
    
    //年 bit35-bit38共4位
    U64Value |= ((uint64_t)Value)<<35; 
    
    //特征号bit27-bit34共8位 数字或英文ASCII码
    Value = *(Ptr+7);
    if((Value < 0x30) || (Value > 0x80))
    {
        return ERROR;
    }
    U64Value |= ((uint64_t)Value)<<27; 
    
    //工厂代码 bit39-bit45共7位
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0x00) || (Value > 74))
    {
        return ERROR;
    }
    U64Value |= ((uint64_t)Value)<<39; 

    //雷管类型  bit46-bit50共5位
    Value = DetType&0x1F;
    U64Value |= ((uint64_t)Value)<<46; 
    
    memset(Buffer, 0, sizeof(Buffer));
    Buffer[0] = U64Value&0xFF;
    Buffer[1] = (U64Value>>8)&0xFF;
    Buffer[2] = (U64Value>>16)&0xFF;
    Buffer[3] = (U64Value>>24)&0xFF;
    Buffer[4] = (U64Value>>32)&0xFF;
    Buffer[5] = (U64Value>>40)&0xFF;
    Buffer[6] = (U64Value>>48)&0xFF;
    Buffer[7] = (~GetCrc8Value(Buffer, 7))&0xFF;
    
    if(Serl != NULL)
    {
        *Serl = U64Value;
    }
    if(Pwd != NULL)
    {
        Value = Buffer[7];
        Value <<= 24;
        Value |= (U64Value>>32)&0xFFFFFF;
        *Pwd = Value;
    }
    return SUCCESS;
}














