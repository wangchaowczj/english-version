#include "includes.h"

/**
  *************************************************************************************
  * @brief  ͨ�����������豸������汾
  * @param  add��Ҫ�����豸��ַ
  * @param  Version��������
  * @param  MaxLength������������  
  * @retval ���
  * @author ZH
  * @date   2016��7��013��
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetBlueSlaveVersion(u8 add, u8* Version, u8 MaxLength)
{
	u32 c_time;
	u16 StartIndex = 0;
	u16 Bytes = 0;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//��������ֽ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x00;//��������ֽ�
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
			
			
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum >= 9)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02)  && 
								(BlueTempBuffer.uch_buf[2] == 0x00) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{
								if(ERROR == GetDataFromSeparator('\r', 1, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5,&StartIndex, &Bytes))
								{
									return ERROR;
								}
								if(Bytes > MaxLength)
								{
									return ERROR;
								}
								memset(Version, 0, MaxLength);
								memcpy(Version, &BlueTempBuffer.uch_buf[5+StartIndex], Bytes);
								return SUCCESS;						
							}			
						}
						return ERROR;
					}
					break;				
				}
				OSTimeDly(2);
			}		
		}	
	}
	return ERROR;
}
/**
  *************************************************************************************
  * @brief  ͨ�����������豸���׹�����������
  * @param  add��Ҫ�����豸��ַ
  * @param  RegNumber���׹�����
  * @param  ErrNumber����������
  * @param  Type���׹�����
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetBlueSlaveDetNumberAndType(u8 add, u16* RegNumber, u16* ErrNumber, u8* Type)
{
	u32 c_time;
	u32 U32Value;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//��������ֽ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x2E;//��������ֽ�
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
			
			
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum >= 9)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02)  && 
								(BlueTempBuffer.uch_buf[2] == 0x2E) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{
								if(ERROR == GetDecValueFromSeparator('\r', 1, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5,&U32Value))
								{
									return ERROR;
								}
								*RegNumber = (u16)U32Value;
								if(ERROR == GetDecValueFromSeparator('\r', 2, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5,&U32Value))
								{
									return ERROR;
								}
								*ErrNumber = (u16)U32Value;
								if(ERROR == GetDecValueFromSeparator('\r', 3, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5,&U32Value))
								{
									return ERROR;
								}
								*Type = (u8)U32Value;							
								return SUCCESS;						
							}			
						}
						return ERROR;
					}
					break;				
				}
				OSTimeDly(2);
			}		
		}	
	}
	return ERROR;	
}

/**
  *************************************************************************************
  * @brief  ͨ�����������豸���׹�����
  * @param  add��Ҫ�����豸��ַ
  * @param  StartHole����ʼ�׹�����
  * @param  Number��Ҫ��������
  * @param  Data����������
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetBlueSlaveDetData(u8 add, u16 StartIndex, u8 Number)
{
	u32 c_time;
	u16 Index, Bytes; 
	u32 U32Value;
	u8 Retry = 3;
	u16 DetIndex;
	u8 Buffer[20];
	u8 U8Value;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//��������ֽ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x2D;//��������ֽ�
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], "%d\r%d\r", StartIndex, Number);
			
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);

			
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum >= 9)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02)  && 
								(BlueTempBuffer.uch_buf[2] == 0x2D) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{
								if(ERROR == GetDecValueFromSeparator('\r', 1, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5, &U32Value))
								{
									return ERROR;
								}
								if(U32Value != 1)
								{
									return ERROR;
								}
								
								
								U8Value = 0;
								while(U8Value < Number)
								{
									//�׹�����\�׺�\Ƶ��\����\״̬��
									if(ERROR == GetDataFromSeparator('\r', 2+6*U8Value, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5, &Index, &Bytes))
									{
										return ERROR;
									}
									if(Bytes != 16)
									{
										return ERROR;
									}
									HexStrToCharStr(&BlueTempBuffer.uch_buf[5+Index], Bytes, Buffer);
									
									DetIndex = GetEmptyDetIndex();
									
									Det.Info[DetIndex].Serl.U8Buffer[0] = Buffer[0];
									Det.Info[DetIndex].Serl.U8Buffer[1] = Buffer[1];
									Det.Info[DetIndex].Serl.U8Buffer[2] = Buffer[2];
									Det.Info[DetIndex].Serl.U8Buffer[3] = Buffer[3];
									Det.Info[DetIndex].Pwd.U8Buffer[0] = Buffer[4];
									Det.Info[DetIndex].Pwd.U8Buffer[1] = Buffer[5];
									Det.Info[DetIndex].Pwd.U8Buffer[2] = Buffer[6];
									Det.Info[DetIndex].Pwd.U8Buffer[3] = Buffer[7];
									if(ERROR == GetDecValueFromSeparator('\r', 3+6*U8Value, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5, &U32Value))
									{
										return ERROR;
									}
									Det.Info[DetIndex].Hole.Name = (u16)U32Value;
									if(ERROR == GetDecValueFromSeparator('\r', 4+6*U8Value, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5, &U32Value))
									{
										return ERROR;
									}
									Det.Info[DetIndex].Hole.Inside = (u8)U32Value;
									
									if(ERROR == GetDecValueFromSeparator('\r', 5+6*U8Value, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5, &U32Value))
									{
										return ERROR;
									}
									Det.Info[DetIndex].Freq.U32Value = U32Value;	
									
									if(ERROR == GetDecValueFromSeparator('\r', 6+6*U8Value, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5, &U32Value))
									{
										return ERROR;
									}
									Det.Info[DetIndex].Delay = U32Value;	
									
									if(ERROR == GetDecValueFromSeparator('\r', 7+6*U8Value, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5, &U32Value))
									{
										return ERROR;
									}
									Det.Info[DetIndex].Err = (u8)U32Value;	

									U8Value++;
								}
								
						
								return SUCCESS;						
							}			
						}
						return ERROR;
					}
					break;
				}
				OSTimeDly(2);
			}		
		}
	}
	return ERROR;	
}
/**
  *************************************************************************************
  * @brief  ͨ�����������豸���׹�����
  * @param  add��Ҫ�����豸��ַ
  * @param  StartHole����ʼ�׹�����
  * @param  Number��Ҫ��������
  * @param  Data����������
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
u8 WriteBlueSlaveDetData(u8 add, u16 LocalIndex, u16 SlaveIndex, u8 Number)
{
	u32 c_time;
	u8 Retry = 3;
	u8 U8Value;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//��������ֽ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x2C;//��������ֽ�
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], "%d\r", SlaveIndex);
			U8Value = 0;
			while(U8Value < Number)
			{
				//�׹�����\�׺�\Ƶ��\����\״̬��
				BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], 
					"%02X%02X%02X%02X%02X%02X%02X%02X\r%u\r%u\r%u\r%u\r%u\r", 
					Det.Info[LocalIndex + U8Value].Serl.U8Buffer[0], Det.Info[LocalIndex + U8Value].Serl.U8Buffer[1], 
                    Det.Info[LocalIndex + U8Value].Serl.U8Buffer[2], Det.Info[LocalIndex + U8Value].Serl.U8Buffer[3],
					Det.Info[LocalIndex + U8Value].Pwd.U8Buffer[0], Det.Info[LocalIndex + U8Value].Pwd.U8Buffer[1], 
                    Det.Info[LocalIndex + U8Value].Pwd.U8Buffer[2], Det.Info[LocalIndex + U8Value].Pwd.U8Buffer[3],
					Det.Info[LocalIndex + U8Value].Hole.Name, Det.Info[LocalIndex + U8Value].Hole.Inside,
					Det.Info[LocalIndex + U8Value].Freq.U32Value, Det.Info[LocalIndex + U8Value].Delay,
					Det.Info[LocalIndex + U8Value].Err);
					
				U8Value++;
			}		
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum == 7)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02) && 
								(BlueTempBuffer.uch_buf[2] == 0x2C) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{					
								if(BlueTempBuffer.uch_buf[5] == '1')
								{
									return 0;	
								}
								if(BlueTempBuffer.uch_buf[5] == '3')
								{//���غ��׹�
									return 1;	
								}					
							}			
						}
						return 2;
					}
					break;
				}
				OSTimeDly(2);
			}			
		}
	}
	return 3;	
}
/**
  *************************************************************************************
  * @brief  ͨ�����������豸��ÿ��������׹���
  * @param  add��Ҫ�����豸��ַ
  * @param  StartHole����ʼ�׹�����
  * @param  Number��Ҫ��������
  * @param  Data����������
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetBlueSlaveHoleNumberData(u8 add, u16 StartHoleIndex, u16 Count)
{
	u32 c_time;
	u8 Retry = 3;

	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//��������ֽ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x2F;//��������ֽ�
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], 
				"%u\r%u\r", StartHoleIndex,Count);
				
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum == (5+Count))
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02)  && 
								(BlueTempBuffer.uch_buf[2] == 0x2F) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF))
							{
								memcpy(&Det.HoleNumber[StartHoleIndex], &BlueTempBuffer.uch_buf[5], Count);
								
								return SUCCESS;						
							}			
						}
						return ERROR;
					}
					break;				
				}
				OSTimeDly(2);
			}		
		}	
	}
	return ERROR;	
}

/**
  *************************************************************************************
  * @brief  ͨ�����������豸��ÿ��������׹���
  * @param  add��Ҫ�����豸��ַ
  * @param  StartHole����ʼ�׹�����
  * @param  Number��Ҫ��������
  * @param  Data����������
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
ErrorStatus WriteBlueSlaveHoleNumberData(u8 add, u16 StartHoleIndex, u16 Count)
{
	u32 c_time;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//��������ֽ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x30;//��������ֽ�
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		

			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], 
					"%u\r%u\r", StartHoleIndex, Count);	
					
			memcpy(&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], &Det.HoleNumber[StartHoleIndex], Count);
			BlueTempBuffer.bytenum += Count;
			
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum == 7)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02) && 
								(BlueTempBuffer.uch_buf[2] == 0x30) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[5] == '1') &&
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{					
								return SUCCESS;						
							}			
						}
						return ERROR;
					}
					break;				
				}
				OSTimeDly(2);
			}		
		}
	}
	return ERROR;	
}

/**
  *************************************************************************************
  * @brief  ͨ�����������豸��ÿ��������׹���
  * @param  add��Ҫ�����豸��ַ
  * @param  StartHole����ʼ�׹�����
  * @param  Number��Ҫ��������
  * @param  Data����������
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
ErrorStatus WriteBlueSlaveHoleNumberDataSpecifiedHoleRange(u8 add, u16 StartHole, u16 EndHole)
{
	u32 c_time;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//��������ֽ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x3B;//��������ֽ�
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
			
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], 
					"%u\r%u\r", StartHole,EndHole);			
			
			memcpy(&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], &Det.HoleNumber[StartHole-1], EndHole-StartHole+1);
			BlueTempBuffer.bytenum += EndHole-StartHole+1;
			
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum == 7)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02) && 
								(BlueTempBuffer.uch_buf[2] == 0x3B) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[5] == '1') &&
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{					
								return SUCCESS;						
							}			
						}
						return ERROR;
					}
					break;				
				}
				OSTimeDly(2);
			}		
		}
	}
	return ERROR;	
}

/**
  *************************************************************************************
  * @brief  ͨ������ɾ�����豸���׹���Ϣ
  * @param  add��Ҫ�����豸��ַ
  * @param  StartHole����ʼ�׹�����
  * @param  Number��Ҫ��������
  * @param  Data����������
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
ErrorStatus DeleteBlueSlaveDetData(u8 add)
{
	u32 c_time;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//��������ֽ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x2B;//��������ֽ�
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
			
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum == 7)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02) && 
								(BlueTempBuffer.uch_buf[2] == 0x2B) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[5] == '1') &&
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{					
								return SUCCESS;						
							}			
						}
						return ERROR;
					}
					break;				
				}
				OSTimeDly(2);
			}		
		}
	}
	return ERROR;	
}
/**
  *************************************************************************************
  * @brief  ͨ������ɾ�����豸���׹���Ϣ
  * @param  add��Ҫ�����豸��ַ
  * @param  StartHole����ʼ��
  * @param  EndHole����ֹ��
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
ErrorStatus DeleteBlueSlaveDetDataSpecifiedHoleRange(u8 add, u16 StartHole, u16 EndHole)
{
	u32 c_time;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//��������ֽ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x3A;//��������ֽ�
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], 
				"%u\r%u\r",  StartHole, EndHole);	
				
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum == 7)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02) && 
								(BlueTempBuffer.uch_buf[2] == 0x3A) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[5] == '1') &&
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{					
								return SUCCESS;						
							}			
						}
						return ERROR;
					}
					break;				
				}
				OSTimeDly(2);
			}		
		}
	}
	return ERROR;	
}
/**
  *************************************************************************************
  * @brief  ͨ������ɾ�����豸���׹���Ϣ
  * @param  add��Ҫ�����豸��ַ
  * @param  StartHole����ʼ�׹�����
  * @param  Number��Ҫ��������
  * @param  Data����������
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
ErrorStatus SaveBlueSlaveDetAndHoleData(u8 add)
{
	u32 c_time;
	u8 Retry = 3;

	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//��������ֽ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x31;//��������ֽ�
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
			
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+2500/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum == 7)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02) && 
								(BlueTempBuffer.uch_buf[2] == 0x31) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[5] == '1') &&
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{					
								return SUCCESS;						
							}			
						}
						return ERROR;
					}
					break;				
				}
				OSTimeDly(2);
			}		
		}	
	}
	return ERROR;	
}
/**
  *************************************************************************************
  * @brief  ͨ�������������豸�ָ��׹���Ϣ
  * @param  add��Ҫ�����豸��ַ
  * @retval ��� 0ͨ��ʧ�ܣ�1�ɹ���2��������ʧ�ܣ�3���ʹ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */

u8 BlueReadDetInfoBackUp(u8 add)
{
	ErrorStatus Result;
	u16 RegNumber, ErrNumber;
	u16 StartIndex = 0,i;
	u8 U8Value;

	Result = GetBlueSlaveDetNumberAndType(add, &RegNumber, &ErrNumber, &U8Value);
	if(Result == ERROR)
	{
		return 0;
	}

	memset((char*)&Det.Info[0], 0, sizeof(Det.Info));
	memset((char*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
	Det.ErrorCount = 0;
	Det.RegCount = 0;
	
	U8Value =  RegNumber/5;
	while(U8Value--)
	{
		Result = GetBlueSlaveDetData(add, StartIndex, 5);//ÿ�ζ�5���׹�
		if(Result == ERROR)
		{
			return 0;
		}
		StartIndex += 5;
	}
	if(RegNumber%5)
	{
		Result = GetBlueSlaveDetData(add, StartIndex, RegNumber%5);//ÿ�ζ�5���׹�
		if(Result == ERROR)
		{
			return 0;
		}	
	}
	for(i=0;i< DET_MAX_SIZE;i++)
	{
		if(ERROR != GetDetRegStatus(i))
		{
			if(ERROR == CheckDetType(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value))
			{
				memset((char*)&Det.Info[0],0,sizeof(Det.Info));
				memset((char*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
				Det.ErrorCount = 0;
				Det.RegCount = 0;
				return 3;//���Ͳ�����Ҫ��
			}			
		}
		if(Det.Info[i].Delay > Det.DlyMax)
		{
			Det.Info[i].Delay =  Det.DlyMax;
		}
	}

	//�������׹���
	Result = GetBlueSlaveHoleNumberData(add, 0, DET_MAX_SIZE);
	if(Result == ERROR)
	{
		return 0;
	}
	if(ERROR == SaveAllDetInfo(0))
	{
		return 2;
	}
	return 1;
}
/**
  *************************************************************************************
  * @brief  ͨ���������׹���Ϣ���ݵ������豸
  * @param  add��Ҫ�����豸��ַ
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
ErrorStatus BlueDetInfoBackUp(u8 add)
{
	ErrorStatus Result;
	u16 StartIndex = 0;
	u8 U8Value, U8Result;	
	
	//��ɾ��ԭ���׹�
	Result = DeleteBlueSlaveDetData(add);
	if(Result == ERROR)
	{
		return ERROR;
	}
	//�����׹����뼰��������
	U8Value =  Det.RegCount/5;
	while(U8Value--)
	{
		U8Result = WriteBlueSlaveDetData(add, StartIndex, StartIndex, 5);//ÿ�ζ�5���׹�
		if(U8Result != 0)
		{
			return ERROR;
		}
		StartIndex += 5;
	}
	if(Det.RegCount%5)
	{
		U8Result = WriteBlueSlaveDetData(add, StartIndex, StartIndex, Det.RegCount%5);//ÿ�ζ�5���׹�
		if(U8Result != 0)
		{
			return ERROR;
		}	
	}
	//����ÿ���׵�����׹���
	Result = WriteBlueSlaveHoleNumberData(add, 0, DET_MAX_SIZE);
	if(Result == ERROR)
	{
		return ERROR;
	}
	//���ƴӻ���������
	Result = SaveBlueSlaveDetAndHoleData(add);
	if(Result == ERROR)
	{
		return ERROR;
	}	
	return SUCCESS;
}

/**
  *************************************************************************************
  * @brief  ͨ�����������豸���׹�����������
  * @param  add��Ҫ�����豸��ַ
  * @param  StartHole����ʼ�׺�
  * @param  EndHole����ֹ�׺�  
  * @param  RegNumber���׹�����
  * @retval ���
  * @author ZH
  * @date   2016��11��28��
  * @note   
  *************************************************************************************  
  */
ErrorStatus GetBlueSlaveDetNumberSpecifiedHoleRange(u8 add, u16 StartHole, u16 EndHole, u16* RegNumber, u16* StartIndex)
{
	u32 c_time;
	u32 U32Value;
	u8 Retry = 3;
	
	while(Retry--)
	{
		if (BLUE_NOT_CONNECT != GET_BLUE_CONNECT_STATUS())
		{
			BlueTempBuffer.bytenum = 0;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = add;
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x02;//��������ֽ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0x39;//��������ֽ�
				
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����Ÿ��ֽڣ�����Ҫ�ְ�
			BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum++] = 0xFF;//�ְ����ŵ��ֽڣ�����Ҫ�ְ�		
			
			BlueTempBuffer.bytenum += sprintf((char*)&BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum], 
					"%u\r%u\r", StartHole,EndHole);	
					
			BlueData.status = DECODING;
			Pack(&BlueTempBuffer, &BlueData);//���ݴ��
			BlueTempBuffer = BlueData;
			BlueData.bytenum = 0;
			BlueData.status = RECIEVING;		
			BlueSendData(&BlueTempBuffer.uch_buf[0], BlueTempBuffer.bytenum);
			
			c_time = OSTimeGet();//����δ�ʱ��Ϊ1KHZ,�豸��������49.7����п������
			while(OSTimeGet() < (c_time+1000/(1000/OS_TICKS_PER_SEC)))
			{
				if(BlueData.status == RECIEVED)
				{
					if(Unpack(&BlueData, &BlueTempBuffer) != ERROR)
					{
						if (BlueTempBuffer.bytenum > 9)
						{
							if ((BlueTempBuffer.uch_buf[0] == add) && 
								(BlueTempBuffer.uch_buf[1] == 0x02)  && 
								(BlueTempBuffer.uch_buf[2] == 0x39) && 
								(BlueTempBuffer.uch_buf[3] == 0xFF) && 
								(BlueTempBuffer.uch_buf[4] == 0xFF) && 
								(BlueTempBuffer.uch_buf[BlueTempBuffer.bytenum-1] == 0x0D))
							{
								if(ERROR == GetDecValueFromSeparator('\r', 1, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5,&U32Value))
								{
									return ERROR;
								}
								if(U32Value != 1)
								{
									return ERROR;
								}
								if(ERROR == GetDecValueFromSeparator('\r', 2, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5,&U32Value))
								{
									return ERROR;
								}			
								*RegNumber = U32Value;
								if(ERROR == GetDecValueFromSeparator('\r', 3, &BlueTempBuffer.uch_buf[5],BlueTempBuffer.bytenum-5,&U32Value))
								{
									return ERROR;
								}								
								*StartIndex = U32Value;	
								
								return SUCCESS;						
							}			
						}
						return ERROR;
					}
					break;				
				}
				OSTimeDly(2);
			}		
		}	
	}
	return ERROR;	
}

/**
  *************************************************************************************
  * @brief  ͨ�������������豸�����׹���Ϣ
  * @param  add��Ҫ�����豸��ַ
  * @retval ��� 0ͨ��ʧ�ܣ�1�ɹ���2��������ʧ�ܣ�3���ʹ���, 4�����豸û������,5������
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
u8 BlueReadDetInfoInsert(u8 add, u16 StartHole, u16 EndHole)
{
	ErrorStatus Result;
	u16 RegNumber;
	u16 StartIndex = 0,i;
	u8 U8Value;
	
	Result = GetBlueSlaveDetNumberSpecifiedHoleRange(add, StartHole, EndHole, &RegNumber, &StartIndex);
	if(Result == ERROR)
	{
		return 0;
	}
	if(RegNumber == 0)
	{
		return 4;
	}
	//ɾ��ָ���׷�Χ���׹�	
	for(i=0; i<DET_MAX_SIZE; i++)
	{
		if (GetDetRegStatus(i) != ERROR)
		{
			if ((Det.Info[i].Hole.Name >= StartHole) && (Det.Info[i].Hole.Name <= EndHole))
			{
				Det.Info[i].Serl.U32Value = 0x00;
				Det.Info[i].Pwd.U32Value = 0x00;			
			}		
		}
	}
	GetDetNum();
	AllRegDetRankBaseOnHoleInfo();
	
	U8Value =  RegNumber/5;
	while(U8Value--)
	{
		Result = GetBlueSlaveDetData(add, StartIndex, 5);//ÿ�ζ�5���׹�
		if(Result == ERROR)
		{
			return 0;
		}
		StartIndex += 5;
	}
	if(RegNumber%5)
	{
		Result = GetBlueSlaveDetData(add, StartIndex, RegNumber%5);//ÿ�ζ�5���׹�
		if(Result == ERROR)
		{
			return 0;
		}	
	}
	for(i=0;i< DET_MAX_SIZE;i++)
	{
		if(ERROR != GetDetRegStatus(i))
		{
			if(ERROR == CheckDetType(Det.Info[i].Serl.U32Value, Det.Info[i].Pwd.U32Value))
			{
				memset((char*)&Det.Info[0],0,sizeof(Det.Info));
				memset((char*)&Det.HoleNumber[0], 0x01, sizeof(Det.HoleNumber));
				Det.ErrorCount = 0;
				Det.RegCount = 0;
				return 3;//���Ͳ�����Ҫ��
			}			
		}
		if(Det.Info[i].Delay > Det.DlyMax)
		{
			Det.Info[i].Delay =  Det.DlyMax;
		}
	}
	if(DetInfoHasSame())
	{
		memset((char*)&Det.Info[0],0,sizeof(Det.Info));
		Det.ErrorCount = 0;
		Det.RegCount = 0;
		return 5;//����ͬ����	
	}
	//�������׹���
	Result = GetBlueSlaveHoleNumberData(add, StartHole-1, EndHole-StartHole+1);
	if(Result == ERROR)
	{
		return 0;
	}	
	if(ERROR == SaveAllDetInfo(0))
	{
		return 2;
	}
	return 1;
}
/**
  *************************************************************************************
  * @brief  ͨ���������׹���Ϣ���ݵ������豸
  * @param  add��Ҫ�����豸��ַ
  * @retval ���
  * @author ZH
  * @date   2015��4��03��
  * @note   
  *************************************************************************************  
  */
u8 BlueDetInfoInsert(u8 add, u16 StartHole, u16 EndHole)
{
	ErrorStatus Result;
	u16 StartIndex = 0, Count, SlaveIndex;
	u16 ErrNumber;
	u8 U8Value, U8Result;	
	HOLE Start;
	HOLE End;
	
	Start.Name = StartHole;
	Start.Inside = 1;
	End.Name = EndHole;
	End.Inside = NET_PARAM_MAX;	
	
	Count = GetCountSpecifiedHoleRange(&Start, &End);
	if(Count == 0)
	{
		return 0;
	}
	//��ɾ��ԭ���׹�
	Result = DeleteBlueSlaveDetDataSpecifiedHoleRange(add, StartHole, EndHole);
	if(Result == ERROR)
	{
		return 1;
	}
	
	Result = GetBlueSlaveDetNumberAndType(add, &SlaveIndex, &ErrNumber, &U8Value);
	if(Result == ERROR)
	{
		return 1;
	}
	
	StartIndex = GetHoleIndexSpecifiedStartHole(&Start, 1);
	
	//�����׹����뼰��������
	U8Value =  Count/5;
	while(U8Value--)
	{
		U8Result = WriteBlueSlaveDetData(add, StartIndex, SlaveIndex, 5);//ÿ�ζ�5���׹�
		if(U8Result != 0)
		{
			if(U8Result == 1)
			{
				return 3; //���غ��׹�
			}
			return 1;
		}
		StartIndex += 5;
		SlaveIndex += 5;
	}
	if(Count%5)
	{
		U8Result = WriteBlueSlaveDetData(add, StartIndex, SlaveIndex, Count%5);//ÿ�ζ�5���׹�
		if(U8Result != 0)
		{
			if(U8Result == 1)
			{
				return 3; //���غ��׹�
			}
			return 1;
		}	
	}
	//����ÿ���׵�����׹���
	Result = WriteBlueSlaveHoleNumberData(add, StartHole-1, EndHole-StartHole+1);
	if(Result == ERROR)
	{
		return 1;
	}
	//���ƴӻ���������
	Result = SaveBlueSlaveDetAndHoleData(add);
	if(Result == ERROR)
	{
		return 1;
	}	
	return 2;
}
