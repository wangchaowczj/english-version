#include "includes.h"

STR_COMM  GusetSendBuffer;
STR_COMM  GusetTempBuffer;

/**
  *************************************************************************************
  * @brief  ��ȡ�������Ӧ����
  * @param  buffer�����ݻ�����
  * @param  Cmd������
  * @param  PackIndex���ְ���  
  * @retval �� 
  * @author ZH
  * @date   2015��1��06��
  * @note   ��
  *************************************************************************************  
  */
void GetGeneralRespondData(STR_COMM* buffer, u16 Cmd, u16 PackIndex)
{
	buffer->bytenum = 0;
	buffer->uch_buf[buffer->bytenum++] = device_info.name;
	buffer->uch_buf[buffer->bytenum++] = (u8)(Cmd>>8);
	buffer->uch_buf[buffer->bytenum++] = (u8)(Cmd&0xFF);
	buffer->uch_buf[buffer->bytenum++] = (PackIndex>>8)&0xFF;
	buffer->uch_buf[buffer->bytenum++] = PackIndex&0xFF;
}
/**
  *************************************************************************************
  * @brief  ��ȡ�������е��ַ�������
  * @param  buffer�����ݻ�����
  * @param  MaxLenght�����ݻ��������� 
  * @retval �ַ������� 
  * @author ZH
  * @date   2015��1��06��
  * @note   ��
  *************************************************************************************  
  */
u16 GetBufferStringLenght(u8* Buffer, u16 MaxLenght)
{
	u16 i;
	
	for(i=0;i<MaxLenght;i++)
	{
		if(Buffer[i] == 0x00)
		{
			break;
		}
	}
	return i;
}

/**
  *************************************************************************************
  * @brief  �ӻ������е���ֵ
  * @param  buffer�����ݻ�����
  * @param  MaxLenght�����ݻ��������� 
  * @retval ��ֵ
  * @author ZH
  * @date   2015��1��06��
  * @note   ��
  *************************************************************************************  
  */
u32 GetDecValueFromBuffer(u8* Buffer, u16 MaxLenght)
{
	u8 i;
	u32 Value = 0;
	
	for(i=0;i<MaxLenght;i++)
	{
		if ((Buffer[i] < '0') || (Buffer[i] > '9'))
		{
			break;
		}
		Value *= 10;
		Value += (Buffer[i]-'0');
	}
	return Value;
}

/**
  *************************************************************************************
  * @brief  �ӷָ�����ȡ����
  * @param  Separator���ָ���
  * @param  SeparatorNumber���ָ������
  * @param  Buffer��ԭʼ����
  * @param  Bytes��ԭʼ���ݳ���
  * @param  StartIndex���õ���������Buffer�е���ʼ����ֵ
  * @param  DataBytes���õ������ݳ���  
  * @retval ��
  * @author ZH
  * @date   2015��1��06��
  * @note   ��123\rABCD\rGL\r��,����ȡ��"ABCD"��Separator SeparatorNumber �ֱ�Ϊ'\r',2
  *************************************************************************************  
  */
ErrorStatus GetDataFromSeparator(u8 Separator, u8 SeparatorNumber, u8* Buffer, u16 Bytes, u16* StartIndex, u16* DataBytes)
{
	u16 i;
	s32 LastIndex = -1;
	u8 Number = 0;
	
	for(i=0;i<Bytes;i++)
	{
		if(Separator == Buffer[i])
		{
			Number++;
			if(SeparatorNumber == Number)
			{
				*StartIndex = LastIndex+1;
				*DataBytes = i-LastIndex-1;
				return SUCCESS;
			}
			else
			{
				LastIndex = i;
			}
		}
	}
	return ERROR;//�ָ�������
}

/**
  *************************************************************************************
  * @brief  �ӷָ�����ȡ10������ֵ
  * @param  Separator���ָ���
  * @param  SeparatorNumber���ָ������
  * @param  Buffer��ԭʼ����
  * @param  Bytes��ԭʼ���ݳ���
  * @param  Value����ֵ
  * @retval ��
  * @author ZH
  * @date   2015��1��06��
  * @note   ��123\rABCD\rGL\r��,����ȡ��"123"��Separator SeparatorNumber �ֱ�Ϊ'\r',1
  *************************************************************************************  
  */
ErrorStatus GetDecValueFromSeparator(u8 Separator, u8 SeparatorNumber, u8* Buffer, u16 Bytes, u32* Value)
{
	u16 i;
	s32 LastIndex = -1;
	u8 Number = 0;
	u8 temp[20];
	
	for(i=0;i<Bytes;i++)
	{
		if(Separator == Buffer[i])
		{
			Number++;
			if(SeparatorNumber == Number)
			{
				memset(temp,0,20);
				memcpy(&temp[0], &Buffer[LastIndex+1], i-LastIndex-1);
				sscanf((const char*)&temp[0],"%uld",Value);
				return SUCCESS;
			}
			else
			{
				LastIndex = i;
			}
		}
	}
	return ERROR;//�ָ�������
}

/**
  *************************************************************************************
  * @brief  USB������Ӧ
  * @param  buffer_in�����յ�������
  * @param  buffer_out����Ӧ������
  * @retval ��
  * @author ZH
  * @date   2015��1��06��
  * @note   ��
  *************************************************************************************  
  */
void  GuestRespond(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 Cmd = 0, HostPack = 0;
//	static u16 GuestPack = 0, LastValue;
/*	u8 U8Value;
	u16 U16Value1, U16Value2;
	u32 U32Value;
	u8 Temp1[100], Temp2[100]*/;
//	u16 i,j;
	
	memset((char*)buffer_out, 0, sizeof(STR_COMM));
	
	if ((buffer_in->bytenum >= 5) && ((buffer_in->uch_buf[0] == device_info.name) || (buffer_in->uch_buf[0] == 0x00)))
	{
		Cmd = (buffer_in->uch_buf[1]<<8) + buffer_in->uch_buf[2];
		HostPack = (buffer_in->uch_buf[3]<<8) + buffer_in->uch_buf[4];
		switch(Cmd)
		{
			case 0x0200://������汾
				if(HostPack == 0xFFFF)
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);
					
					strcpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], VERSION_STRING);
					
					buffer_out->bytenum += strlen(VERSION_STRING);
					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
				}
				break;		               
			case 0xFFFF://����ָ����ڻ�ȡ�豸ͨ�ŵ�ַ
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5))
				{
					GetGeneralRespondData(buffer_out, Cmd, HostPack);//��������
				}				
				break;
			default:
				break;
		}
		if((buffer_in->uch_buf[0] == 0x00) && (Cmd != 0xFFFF))
		{
			buffer_out->bytenum = 0;
		}
	}
}


