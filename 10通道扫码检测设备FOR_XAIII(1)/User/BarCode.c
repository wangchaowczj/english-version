#include "includes.h"

u8 FactoryCodeSupport[FACTORY_CODE_SUPPORT_MAX] = {44,34,56,10,36,53,52 };//֧�ֵĹ�������
//������е����Ӧ44  ������˿����Ӧ32  �Ĵ��˱�������Ӧ56  ���ɺ����Ӧ10 ɽ��ʥ�����Ӧ 36  �����𽨻���Ӧ53 �㶫�ػ�52
BARCODE BarCode;
/**
  *************************************************************************************
  * @brief  �ⲿ��������ת����оƬ�Ĺ�������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��7��2��
  * @note   ��
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
  * @brief  ��оƬ�Ĺ�������ת�����ⲿ��������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��7��2��
  * @note   ��
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
  * @brief  ����ɨ��ǹɨ��������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2016��7��2��
  * @note   ��
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
//	//У�飬�ж������Ƿ���ȷ
//	GetCrc16Value(0xFFFF, CRC16, ScanBuffer, 30);
//	HexStrToCharStr(&ScanBuffer[30], 4, &Buffer[0]);
//	if((Buffer[0] != CRC16[0]) || (Buffer[1] != CRC16[1]))
//	{
//		return;
//	}
//	
//	//��2-14λ��ʾ����
//	memset((char*)BarCode.CodeExternal, 0, sizeof(BarCode.CodeExternal));
//	memcpy((char*)BarCode.CodeExternal, &ScanBuffer[1], 13);
//	
//	//��2��3λ��������ҵ���룬�á�01��99��2λ���������ֱ��
//	Code = (ScanBuffer[1] - '0')*10 + (ScanBuffer[2] - '0');
//	if(FactoryCodeOutsideToInside(Code, &BarCode.FactoryCode) == ERROR)
//	{
//		return;
//	}
//	
//	//��4λ��������ݴ��룺�ã���0��9��1λ���������ֱ��
//	Year = ScanBuffer[3] - '0';
//	if(Year > 9)
//	{
//		return;
//	}
//	//��5��6λ�������·ݴ��룺�á�01��12��2λ���������ֱ��
//	Month = (ScanBuffer[4] - '0')*10 + (ScanBuffer[5] - '0');
//	if ((Month == 0) || (Month > 12))
//	{
//		return;
//	}
//	//��7��8λ���������ڴ��룺�á�01��31��2λ���������ֱ��
//	Day = (ScanBuffer[6] - '0')*10 + (ScanBuffer[7] - '0');
//	if ((Day == 0) || (Day > 31))
//	{
//		return;
//	}	
//	BarCode.ProductTime = Year*10000UL+Month*100UL+Day;
//	
//	//��9λ��������
//	BarCode.Feature = ScanBuffer[8];
//	
//	//��10��12λ���к�
//	BoxNumber = (ScanBuffer[9] - '0')*100 + (ScanBuffer[10] - '0')*10 + (ScanBuffer[11] - '0');
//	if(BoxNumber > 999)
//	{
//		return;
//	}
//	//��13��14λ��������ˮ��
//	BoxInsideIndex = (ScanBuffer[12] - '0')*10 + (ScanBuffer[13] - '0');
//	if(BoxInsideIndex > 99)
//	{
//		return;
//	}	
//	//��15��20λ��оƬ���룬�á�000000��FFFFFF��ʮ�����Ʊ�ʾоƬ������
//	HexStrToCharStr(&ScanBuffer[14], 6, &BarCode.DetCode[0]);

//	//��21��24λ���á�0000��1500����ʾ�׹ܵ�����ʱ���0ms��1500ms�����߿ɱ�̵����׹���ʾ��ĸPPPP
//	memset(Buffer, 0, sizeof(Buffer));
//	memcpy(Buffer, &ScanBuffer[20], 4);
//	if(ERROR == CheckNumberChars((char*)Buffer, 0, 4))
//	{//���߿ɱ��
//		BarCode.Delay = 0x00;
//	}
//	else
//	{
//		BarCode.Delay = (u16)AsciiToU32(Buffer);
//	}
//	
//	//��25��26λ������λ���֡�02��99����ʾ���ߵĳ���2��99�ף�����
//	memset(Buffer, 0, sizeof(Buffer));
//	memcpy(Buffer, &ScanBuffer[24], 2);	
//	BarCode.DetLineLength = (u16)AsciiToU32(Buffer);
//	
//	//��27λ����ʾ�׹����ͣ�
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
//	//ȷ�����ڲ�����
//	if(BarCode.Delay > GetMaxDelay(BarCode.DetType))
//	{
//		return;
//	}
//	/*
//	��28λ����ʾ���߲������ͣ�
//	�������Ͷ��壺
//	0001����ý���
//	0002��ͨԲ����
//	0003�͸��½���
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
//	//��29��30λΪ����λ��Ĭ��Ϊ0
//	if(ScanBuffer[29] != '0')
//	{
//		return;
//	}
//	if(ScanBuffer[28] == '0')
//	{//����λ��00��
//		BarCode.DetCode[3] = (BarCode.FactoryCode<<4)+0x00;//��4bit��ʾ��������,��4bit��ʾ��Բ��������ˮ��(��д����ǰ�ȶ�����д��)
//		BarCode.DetCode[4] = (BarCode.DetType<<5)+(Day&0x1F);//��3bit��ʾоƬ����,��5bit��ʾ�������ڣ�1��31��
//		BarCode.DetCode[5] = (Month<<4)+(Year&0x0F);//��4bit��ʾ�����·ݣ�1��12��,��4bit��ʾ������ݣ�0��9��		
//	}
//	else if(ScanBuffer[28] == '1')
//	{//����λ��10��
//		if ((BarCode.Feature >= '0')&&(BarCode.Feature <= 'F'))//��Ч������Ϊ"0"-"F"
//		{
//			if((BarCode.Feature >= '0') && (BarCode.Feature <= '9'))
//			{
//				BarCode.DetCode[3] = (BarCode.FactoryCode<<4)+(BarCode.Feature-'0');
//			}
//			else
//			{
//				BarCode.DetCode[3] = (BarCode.FactoryCode<<4)+(BarCode.Feature-'A'+10);
//			}
//			BarCode.DetCode[4] = (BarCode.DetType<<5)+(Day&0x1F);//��3bit��ʾоƬ����,��5bit��ʾ�������ڣ�1��31��
//			BarCode.DetCode[5] = (Year*12+(Month-1))&0x7F;//��7bit��ʾ��������£����ֵ��X9��12�¼�9*12+11
//			if((BoxNumber*100L+BoxInsideIndex) > 0xFFFF)
//			{
//				BarCode.DetCode[5] |= 0x80;//bit7��ʾ����ˮ���Ѿ�����0xFFFF
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
//	//��31��32��33��34λΪCRC16У��λ��
//	
//	//��֤�õ�������Ϣ���û����õ���Ϣ�Ƿ����
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
    
    if(ScanBuffer[0] != '0')//��������
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
        if((ScanBuffer[i] < '0')&&(ScanBuffer[i] > 'F'))//����ǲ���HEX
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
	//	//��2��3λ��������ҵ���룬�á�01��99��2λ���������ֱ��
	Code = (ScanBuffer[1] - '0')*10 + (ScanBuffer[2] - '0');
	if(FactoryCodeOutsideToInside(Code, &BarCode.FactoryCode) == ERROR)
	{
		return;
	}
	
	//��4λ��������ݴ��룺�ã���0��9��1λ���������ֱ��
	Year = ScanBuffer[3] - '0';
	if(Year > 9)
	{
		return;
	}
	//��5��6λ�������·ݴ��룺�á�01��12��2λ���������ֱ��
	Month = (ScanBuffer[4] - '0')*10 + (ScanBuffer[5] - '0');
	if ((Month == 0) || (Month > 12))
	{
		return;
	}
	//��7��8λ���������ڴ��룺�á�01��31��2λ���������ֱ��
	Day = (ScanBuffer[6] - '0')*10 + (ScanBuffer[7] - '0');
	if ((Day == 0) || (Day > 31))
	{
		return;
	}	
	BarCode.ProductTime = Year*10000UL+Month*100UL+Day;
	
	//��9λ��������
	BarCode.Feature = ScanBuffer[8];
	
	//	//��֤�õ�������Ϣ���û����õ���Ϣ�Ƿ����
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
  * @brief  ����ת����
  * @param  Serl ������
  * @param  ���� Pwd
  * @param  Ptr ����
  * @retval ���
  * @author ZH
  * @date   2019��4��20��
  * @note   ��
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
    
    U16Value = (U64Value>>39)&0x7F;//��������bit39-bit45��7λ
    if((U16Value == 0) || (U16Value > 74))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)Ptr, "%02d", U16Value);    
    }
    
    U16Value = (U64Value>>35)&0x0F;//��bit35-bit38��4λ
    if(U16Value > 9)
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+2), "%d", U16Value);    
    }

    U16Value = (U64Value>>22)&0x0F;//��bit22-bit25��4λ
    if((U16Value == 0) || (U16Value > 12))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+3), "%02d", U16Value);    
    }

    U16Value = (U64Value>>17)&0x1F;//��bit17-bit21��5λ
    if((U16Value == 0) || (U16Value > 31))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+5), "%02d", U16Value);    
    }
    
    U16Value = (U64Value>>27)&0xFF;//������bit27-bit34��8λ,���ֻ�Ӣ��ASCII��
    if((U16Value < 0x30) || (U16Value > 0x80))
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        *(Ptr+7) = U16Value;    
    }
     
    U16Value = (U64Value>>7)&0x3FF;//�к�bit7-bit16��10λ
    if(U16Value > 999)
    {
        return ERROR;
    }
    if(Ptr != NULL)
    {
        sprintf((char*)(Ptr+8), "%03d", U16Value);    
    }
    
    U16Value = U64Value&0x7F;//������ˮ��bit0-bi6��7λ
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
  * @brief  ����ת����
  * @param  Serl ������
  * @param  ���� Pwd
  * @param  Ptr ����
  * @retval ���
  * @author ZH
  * @date   2019��4��20��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus XA2_DetOutside2Inside(u8 DetType, u8* Ptr, u32* Serl, u32* Pwd)
{
    uint64_t U64Value = 0;
    u32 Value;
    u8 Buffer[8];
    
    //������ˮ��bit0-bi6��7λ
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+11, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if(Value > 99)
    {
        return ERROR;
    }
    
    U64Value = Value;

    //����bit7-bit16��10λ
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+8, 3);
    sscanf((char*)Buffer, "%d", &Value);
    if(Value > 999)
    {
        return ERROR;
    }
    U64Value |= Value<<7;

    //�� bit17-bit21��5λ
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+5, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0) || (Value > 31))
    {
        return ERROR;
    }
    U64Value |= Value<<17;

    //�� bit22-bit25��4λ
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+3, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0) || (Value > 12))
    {
        return ERROR;
    }
    U64Value |= Value<<22; 

    //�� bit22-bit25��4λ
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+3, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0) || (Value > 12))
    {
        return ERROR;
    }
    U64Value |= Value<<22; 
    
    // ��ĵ�λ bit26��1λ
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr+2, 1);
    sscanf((char*)Buffer, "%d", &Value);
    if(Value > 9)
    {
        return ERROR;
    }
    U64Value |= (Value&0x01)<<26; 
    
    //�� bit35-bit38��4λ
    U64Value |= ((uint64_t)Value)<<35; 
    
    //������bit27-bit34��8λ ���ֻ�Ӣ��ASCII��
    Value = *(Ptr+7);
    if((Value < 0x30) || (Value > 0x80))
    {
        return ERROR;
    }
    U64Value |= ((uint64_t)Value)<<27; 
    
    //�������� bit39-bit45��7λ
    memset(Buffer, 0, sizeof(Buffer));
    memcpy(Buffer, Ptr, 2);
    sscanf((char*)Buffer, "%d", &Value);
    if((Value == 0x00) || (Value > 74))
    {
        return ERROR;
    }
    U64Value |= ((uint64_t)Value)<<39; 

    //�׹�����  bit46-bit50��5λ
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














