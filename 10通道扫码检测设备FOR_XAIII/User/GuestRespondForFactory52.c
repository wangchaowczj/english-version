#include "includes.h"
#include "GuiCustomUtils.h"

extern void DetTestStart(void);

extern u16 HoldRegBuffer[200];
u8 NULLCodeExternal[] = "0000000000000" ;
/**
  *************************************************************************************
  * @brief  ���
  * @param[in]  buffer_in����������
  * @param[out]  buffer_out���������  
  * @retval �� 
  * @author ZH
  * @date   2018��6��11��
  * @note   ��ÿ�����ݵ�ǰ/������������ʱҲҪ�ܽ����ɹ�
  *************************************************************************************  
  */
static ErrorStatus UnPack(STR_COMM *buffer_in)
{
    u16 i,j,count;
    u8 CRC16[2];
    
	if(buffer_in->bytenum < 4)
	{
		return ERROR;
	}
    
    for(i=0;i<buffer_in->bytenum;i++)
    {
        if((buffer_in->uch_buf[i] == 0x01) || (buffer_in->uch_buf[i] == 0x02))
        {
            for(j=buffer_in->bytenum-1; j > (i+2); j--)
            {
                count = j-i+1;
                GetCrc16Value(0xFFFF, CRC16, &buffer_in->uch_buf[i], count-2);
                if((CRC16[1] == buffer_in->uch_buf[j-1]) && (CRC16[0] == buffer_in->uch_buf[j]))
                {
                    if(count < 4)
                    {
                        return ERROR;
                    }
                    buffer_in->bytenum = count;
                    memcpy(&buffer_in->uch_buf[0], &buffer_in->uch_buf[i], buffer_in->bytenum);
                    return SUCCESS;
                }                
            }
        }
    }
    return ERROR;
}
/**
  *************************************************************************************
  * @brief  ���õ���������Ȧֵ
  * @param[in]  buffer_in����������
  * @param[out]  buffer_out���������  
  * @retval �� 
  * @author ZH
  * @date   2018��6��11��
  * @note   ������0x05
  *************************************************************************************  
  */
static void SetSingleCoilValue(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 RegAdd = (buffer_in->uch_buf[2]<<8) + buffer_in->uch_buf[3];
	u16 Value = (buffer_in->uch_buf[4]<<8) + buffer_in->uch_buf[5];
	
	switch(RegAdd)
	{
		case 0x0300://�����źż������
            //0xFF00��ʾON��0x0000��ʾOFF
			if(Value == 0xFF00)
			{            
				buffer_out->bytenum = 6;
				memcpy(buffer_out->uch_buf, buffer_in->uch_buf, buffer_out->bytenum);				
				
                OPEN_IO_RELAY1();
			}
            else if(Value == 0x0000)
            {
                buffer_out->bytenum = 6;
				memcpy(buffer_out->uch_buf, buffer_in->uch_buf, buffer_out->bytenum);

                CLOSE_IO_RELAY1();                
            }
            else
            {
                buffer_out->bytenum = 0;
                buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[0];
                buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[1]+0x80;
                buffer_out->uch_buf[buffer_out->bytenum++] = 0x03; //�Ƿ���ֵ              
            }
			break;
		case 0x0301://��ʾ����ͨ���ļ�������Ѿ����ߣ��豸���Կ�ʼ��һ�μ��
            //0xFF00��ʾON
			if(Value == 0xFF00)
			{            
				buffer_out->bytenum = 6;
				memcpy(buffer_out->uch_buf, buffer_in->uch_buf, buffer_out->bytenum);				
				
                OpData.NetModeNext = OSTimeGet();
			}
            else
            {
                buffer_out->bytenum = 0;
                buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[0];
                buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[1]+0x80;
                buffer_out->uch_buf[buffer_out->bytenum++] = 0x03; //�Ƿ���ֵ              
            }
			break;            
		default:
			break;			
	}
}
/**
  *************************************************************************************
  * @brief  ������Ĵ�����ֵ
  * @param[in]  buffer_in����������
  * @param[out]  buffer_out���������  
  * @retval �� 
  * @author ZH
  * @date   2018��6��11��
  * @note   ������0x03
  *************************************************************************************  
  */
static void ReadMultRegValue(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 RegStartAdd = (buffer_in->uch_buf[2]<<8) + buffer_in->uch_buf[3];
	u16 RegCount = (buffer_in->uch_buf[4]<<8) + buffer_in->uch_buf[5];
	
    if((RegCount == 0) || (RegCount > 0x7D))//modbusЭ��Ҫ��
    {
        return;
    }
    
    if((RegStartAdd >= 0x0100) && ((RegStartAdd+RegCount) <= (0x0100+20*Channel.Count)))//ÿ��ͨ��ռ20���Ĵ���
    {
        u16 Ch;
        u8 add,Count;
        u32 Pwd32;
        u8 Buffer[8];
        DET_ERR_CODE Status;
        
        for(Ch=0; Ch< Channel.Count; Ch++)
        {
            add = Ch*20;
            //ÿ��ͨ����Ӧ������Ϊ20���Ĵ�����
            //ͨ����1���Ĵ���������� 1���Ĵ��������� 8���Ĵ��������� 3���Ĵ����������7���Ĵ�������20���Ĵ���
            
            //ͨ����
            HoldRegBuffer[add++] = Ch+1; 
            
            OSSchedLock();
            
            //�����
            Status = Channel.XA2_Info[Ch].Status;
            HoldRegBuffer[add++] = (u16)Status;
            
            //����           
            Buffer[0] = Channel.XA2_Info[Ch].Serl.U8Buffer[0];
            Buffer[1] = Channel.XA2_Info[Ch].Serl.U8Buffer[1];
            Buffer[2] = Channel.XA2_Info[Ch].Serl.U8Buffer[2];
            Buffer[3] = Channel.XA2_Info[Ch].Serl.U8Buffer[3];
            Buffer[4] = Channel.XA2_Info[Ch].Pwd.U8Buffer[0];
            Buffer[5] = Channel.XA2_Info[Ch].Pwd.U8Buffer[1];
			Buffer[6] = Channel.XA2_Info[Ch].Pwd.U8Buffer[2];
            Buffer[7] = Channel.XA2_Info[Ch].Pwd.U8Buffer[3];
            
            OSSchedUnlock();
            if ((Status != DET_PASS)
                && (Buffer[0] == 0) && (Buffer[1] == 0)&& (Buffer[2] == 0)&& (Buffer[3] == 0)&& 
                (Buffer[4] == 0)&& (Buffer[5] == 0)&& (Buffer[6] == 0)&& (Buffer[7] == 0))
            {//û�ж�������ʱ,���������ȫ����Ϊ0x00
                u8 i;
                
                for(i=0; i<11; i++)
                {
                    HoldRegBuffer[add++] = 0x00;
                }               
            }
            else
            {
                HoldRegBuffer[add++] = (Buffer[0]<<8)|Buffer[1];
                HoldRegBuffer[add++] = (Buffer[2]<<8)|Buffer[3];
                HoldRegBuffer[add++] = (Buffer[4]<<8)|Buffer[5];
				HoldRegBuffer[add++] = (Buffer[6]<<8)|Buffer[7];
                HoldRegBuffer[add++] = 0x00;
                HoldRegBuffer[add++] = 0x00;
                HoldRegBuffer[add++] = 0x00;
                HoldRegBuffer[add++] = 0x00;
                HoldRegBuffer[add++] = (0x08<<8)|GetCrc8Value(&Buffer[0], 8);//��Ч�ֽ���+crc8У��
                
                //����   �����ݵ�32λУ�鲻���׹�����         
                Pwd32 = CRC32Default(&Buffer[0], 8); 

                HoldRegBuffer[add++] = Pwd32>>16;
                HoldRegBuffer[add++] = Pwd32&0xFFFF;
                HoldRegBuffer[add++] = 0x00;            
            }
            
            //�����
            HoldRegBuffer[add++] = Channel.BarCode[Ch].CodeExternal[0];
            HoldRegBuffer[add++] = (Channel.BarCode[Ch].CodeExternal[1]<<8)|Channel.BarCode[Ch].CodeExternal[2];
            HoldRegBuffer[add++] = (Channel.BarCode[Ch].CodeExternal[3]<<8)|Channel.BarCode[Ch].CodeExternal[4];
            HoldRegBuffer[add++] = (Channel.BarCode[Ch].CodeExternal[5]<<8)|Channel.BarCode[Ch].CodeExternal[6];
            HoldRegBuffer[add++] = (Channel.BarCode[Ch].CodeExternal[7]<<8)|Channel.BarCode[Ch].CodeExternal[8];
            HoldRegBuffer[add++] = (Channel.BarCode[Ch].CodeExternal[9]<<8)|Channel.BarCode[Ch].CodeExternal[10];
            HoldRegBuffer[add++] = (Channel.BarCode[Ch].CodeExternal[11]<<8)|Channel.BarCode[Ch].CodeExternal[12];
        }
        buffer_out->bytenum = 2;
        memcpy(buffer_out->uch_buf, buffer_in->uch_buf, buffer_out->bytenum); 
        
        buffer_out->uch_buf[buffer_out->bytenum++] = RegCount*2;//��Ӧ���ֽ���        
        
        Count = 0;
        while(Count < RegCount)
        {
            add = RegStartAdd-0x100+Count;
            buffer_out->uch_buf[buffer_out->bytenum++] = HoldRegBuffer[add]>>8;
            buffer_out->uch_buf[buffer_out->bytenum++] = HoldRegBuffer[add]&0xFF;
            Count++;
        }      
    }
    
//	if ((RegStartAdd >= 0x0100) && (RegStartAdd <= (0x0100+20*9)) && (((RegStartAdd-0x0100)%20) == 0))//ÿ��ͨ��ռ20���Ĵ���
//	{//��ѯоƬ�����
//        u16 Ch = (RegStartAdd-0x0100)/20;
//        u16 Status;
//        u32 Pwd32;
//        
//        if(RegCount == 0x0014)
//        {
//            //��Ӧ���ݰ�˳��Ϊ
//            //ͨ����1���Ĵ���������� 1���Ĵ��������� 8���Ĵ��������� 3���Ĵ����������7���Ĵ�������20���Ĵ���
//            buffer_out->bytenum = 2;
//            memcpy(buffer_out->uch_buf, buffer_in->uch_buf, buffer_out->bytenum); 
//            
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x28;//��Ӧ���ֽ���
//            
//            //ͨ����
//            buffer_out->uch_buf[buffer_out->bytenum++] = (Ch+1)>>8;
//            buffer_out->uch_buf[buffer_out->bytenum++] = (Ch+1)&0xFF;
//            
//            //�����
//            Status = (u16)Channel.Info[Ch].Status;
//            buffer_out->uch_buf[buffer_out->bytenum++] = Status>>8;
//            buffer_out->uch_buf[buffer_out->bytenum++] = Status&0xFF;  
//            
//            //����
//            buffer_out->uch_buf[buffer_out->bytenum++] = Channel.Info[Ch].ser1;
//            buffer_out->uch_buf[buffer_out->bytenum++] = Channel.Info[Ch].ser2;
//            buffer_out->uch_buf[buffer_out->bytenum++] = Channel.Info[Ch].ser3;
//            buffer_out->uch_buf[buffer_out->bytenum++] = Channel.Info[Ch].fac;
//            buffer_out->uch_buf[buffer_out->bytenum++] = Channel.Info[Ch].bat;
//            buffer_out->uch_buf[buffer_out->bytenum++] = Channel.Info[Ch].year;            
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x00;
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x00;
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x00;
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x00;
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x00;
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x00;
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x00;
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x00;
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x06;//��Ч�ֽ���
//            buffer_out->uch_buf[buffer_out->bytenum++] = GetCrc8Value(&buffer_out->uch_buf[7], 6);
//            
//            //����
//            Pwd32 = CRC32Default(&buffer_out->uch_buf[7], 6);
//            
//            buffer_out->uch_buf[buffer_out->bytenum++] = (Pwd32>>24)&0xFF;
//            buffer_out->uch_buf[buffer_out->bytenum++] = (Pwd32>>16)&0xFF;
//            buffer_out->uch_buf[buffer_out->bytenum++] = (Pwd32>>8)&0xFF;
//            buffer_out->uch_buf[buffer_out->bytenum++] = Pwd32&0xFF;
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x00;
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x00;
//            
//            //�����
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x00;
//            memcpy(&buffer_out->uch_buf[buffer_out->bytenum], &Channel.BarCode[Ch].CodeExternal[0], 13);
//            buffer_out->bytenum += 13;                 
//        }
//    }               
}

/**
  *************************************************************************************
  * @brief  ���ö���Ĵ�����ֵ
  * @param[in]  buffer_in����������
  * @param[out]  buffer_out���������  
  * @retval �� 
  * @author ZH
  * @date   2018��9��5��
  * @note   ������0x10
  *************************************************************************************  
  */
static void SetMultRegValue(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 RegStartAdd = (buffer_in->uch_buf[2]<<8) + buffer_in->uch_buf[3];
	u16 RegCount = (buffer_in->uch_buf[4]<<8) + buffer_in->uch_buf[5];
	u16 Bytes = buffer_in->uch_buf[6];	
//	u32 TempSerl,TempPwd;
//	u8 Temp1[100]/*, Temp2[100]*/;
	U32_UNION TempDetSel,TempDetPwd;

	
	switch(RegStartAdd)
	{
		case 0x0000://�����������
			if((RegCount == (7*Channel.Count)) && (Bytes == (2*RegCount)))
			{
                u8 Ch;

                if(OpData.TestStatus == TEST_WAIT_START)
                {
					if((UserParam.DetCodeWrite == 1) && (UserParam.Stage == FINISHED))//�涨��Ʒд��
					{
						for(Ch=0; Ch<Channel.Count;Ch++)
						{
							memcpy(&Channel.BarCode[Ch].CodeExternal[0], &buffer_in->uch_buf[8+14*Ch], 13); 
							memset(&Channel.BarCode[Ch].DetCode[0], 0, 8);
							if(ERROR != XA2_DetOutside2Inside(UserParam.DetType,&Channel.BarCode[Ch].CodeExternal[0], &TempDetSel.U32Value,&TempDetPwd.U32Value))//������ת��������
							{
	//							sprintf((char*)Temp1, "%08x%08x",TempSerl,TempPwd);//???
	//							sprintf((char*)Temp1, "%s%s",&TempDetSel.U8Buffer[0],&TempDetPwd.U8Buffer[0]);
								memcpy(&Channel.BarCode[Ch].DetCode[0], &TempDetSel.U8Buffer[0], 4);//������ת��������
								memcpy(&Channel.BarCode[Ch].DetCode[4], &TempDetPwd.U8Buffer[0], 4);//������ת��������
								Channel.BarCode[Ch].Status = BARCODE_NEW;
								Channel.BarCode[Ch].Result = BARCODE_PASS;
							}
	   
						}
					}
					else if((UserParam.DetCodeWrite == 0) && (UserParam.Stage == SEMIFINISHED) && (UserParam.CheckDetCode == 0))//�涨���Ʒ��д��
					{
						for(Ch=0; Ch<Channel.Count;Ch++)
						{
							memcpy(&Channel.BarCode[Ch].CodeExternal[0], &buffer_in->uch_buf[8+14*Ch], 13); 
							if(strcmp((char*)&Channel.BarCode[Ch].CodeExternal[0],"0000000000000") != 0)//���Ʒ���͹���Ϊȫ0
							{//���ô��󣬰��Ʒ�쵫���з�ȫ0����
								buffer_out->bytenum = 0;
								buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[0];
								buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[1]+0x80;
								buffer_out->uch_buf[buffer_out->bytenum++] = 0x03; 
								return;	
							}
						}
					}
					else
					{//���ô��󣬷ǣ����Ʒ��д���졢��Ʒд�룩
						buffer_out->bytenum = 0;
						buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[0];
						buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[1]+0x80;
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x03; 
						return;						
					}
                    DetTestStart();//��ʼ����׹�
                    
                    buffer_out->bytenum = 6;
                    memcpy(buffer_out->uch_buf, buffer_in->uch_buf, buffer_out->bytenum);//����ԭ������
                }
                else if(OpData.TestStatus == TEST_DOING)//�п��ܳ�������
                {
                    for(Ch=0; Ch<Channel.Count;Ch++)
                    {
                        if(memcmp(&Channel.BarCode[Ch].CodeExternal[0], &buffer_in->uch_buf[8+14*Ch], 13) != 0) 
                        {
                            buffer_out->bytenum = 0;
                            buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[0];
                            buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[1]+0x80;
                            buffer_out->uch_buf[buffer_out->bytenum++] = 0x03;
                            return;
                        }
                    }
                    buffer_out->bytenum = 6;
                    memcpy(buffer_out->uch_buf, buffer_in->uch_buf, buffer_out->bytenum);//����ԭ������
                }
                else
                {
                    buffer_out->bytenum = 0;
                    buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[0];
                    buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[1]+0x80;
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x06; //���豸æ                
                }
			}
            else
            {
                buffer_out->bytenum = 0;
                buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[0];
                buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[1]+0x80;
                buffer_out->uch_buf[buffer_out->bytenum++] = 0x03;                
            }
			break;
			
	}
}

/**
  *************************************************************************************
  * @brief  ������Ӧ����
  * @param[in]  buffer_in����������
  * @param[out]  buffer_out���������  
  * @retval �� 
  * @author ZH
  * @date   2018��6��11��
  * @note   ��Ե��ǹ㶫�ػ�
  *************************************************************************************  
  */
void RespondForFactory52(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u8 Cmd;
	u8 Buffer[20];
	
	buffer_out->bytenum = 0;
	
	if(UnPack(buffer_in) == ERROR)
    {
        return ;
    }	
	Cmd = buffer_in->uch_buf[1];
	
	switch(Cmd)
	{
		case 0x05://���õ�����Ȧ, ��׼MODBUSЭ���ʽ
			SetSingleCoilValue(buffer_in,  buffer_out);
			break;
		case 0x03://���Ĵ���, ��׼MODBUSЭ���ʽ
			ReadMultRegValue(buffer_in,  buffer_out);			
			break;			
		case 0x10://���ö���Ĵ���, �Ǳ�׼MODBUSЭ���ʽ
			SetMultRegValue(buffer_in,  buffer_out);			
			break;
		default:
			break;
	}
	if(buffer_out->bytenum != 0)
	{
		GetCrc16Value(0xFFFF, Buffer, &buffer_out->uch_buf[0], buffer_out->bytenum);
		buffer_out->uch_buf[buffer_out->bytenum++] = Buffer[1];
		buffer_out->uch_buf[buffer_out->bytenum++] = Buffer[0];
	}
}
