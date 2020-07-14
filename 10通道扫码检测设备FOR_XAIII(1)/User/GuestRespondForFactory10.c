#include "includes.h"
#include "GuiCustomUtils.h"
extern void DetTestStart(void);


/**
  *************************************************************************************
  * @brief  ������Ӧ����
  * @param[in]  buffer_in����������
  * @param[out]  buffer_out���������  
  * @retval �� 
  * @author ZH
  * @date   2019��3��13��
  * @note   ��Ե������ɺ���
  *************************************************************************************  
  */
void RespondForFactory10(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 Cmd = 0, HostPack = 0;
//	static u16 GuestPack = 0, LastValue;
//	u8 U8Value;
	u16 U16Value1, /*U16Value2, */U16Value3, U16Value4;
	u32 U32Value;
	u8 Temp1[100]/*, Temp2[100]*/;
//	u32 TempSer,TempPwd;
	U32_UNION TempDetSel,TempDetPwd;
	
	memset((char*)buffer_out, 0, sizeof(STR_COMM));
	
	if ((buffer_in->bytenum >= 5) && ((buffer_in->uch_buf[0] == DeviceInfo.name) || (buffer_in->uch_buf[0] == 0x00)))
	{
		Cmd = (buffer_in->uch_buf[1]<<8) + buffer_in->uch_buf[2];
		HostPack = (buffer_in->uch_buf[3]<<8) + buffer_in->uch_buf[4];
		switch(Cmd)
		{
        	case 0x0010://���豸��Ϣ
				if(HostPack == 0xFFFF)
                {
                    GetGeneralRespondData(buffer_out, Cmd, HostPack);
                    
                    //�豸��š�����汾��Ӳ���汾��ͨ��������⻷�ڡ��Ƿ�д��
                    U16Value1 = GetBufferStringLenght(&DeviceInfo.ID[0], sizeof(DeviceInfo.ID));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&DeviceInfo.ID[0], U16Value1);
					buffer_out->bytenum += U16Value1;
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;	
                    
                    strcpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], VERSION_STRING);
					buffer_out->bytenum += strlen(VERSION_STRING);					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                    
                    U16Value1 = GetBufferStringLenght(&DeviceInfo.HW[0], sizeof(DeviceInfo.HW));
					strncpy((char*)&buffer_out->uch_buf[buffer_out->bytenum], (char*)&DeviceInfo.HW[0], U16Value1);					
					buffer_out->bytenum += U16Value1;					
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                    
                    //Ҫ��10ͨ������Ʒ�죬д��
                    buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
						"%d\r%d\r%d\r%d\r", Channel.Count, UserParam.Stage, UserParam.DetCodeWrite, UserParam.DetType);	 
                }
                break;
			case 0x0011://�������
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5) && (UserParam.Stage == FINISHED))
				{
                    U8 Ch, Count;
                    
					GetGeneralRespondData(buffer_out, Cmd, HostPack);                                           
                        
                    if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                        buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                        break;
                    }
                    if (U32Value > Channel.Count)
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                        buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                        break;
                    }
                    Count = U32Value;
                    if(OpData.TestStatus == TEST_WAIT_START)
                    {
                        for(Ch=0; Ch<Channel.Count;Ch++)
                        {
                            if(Ch < Count)
                            {
                                if(ERROR == GetDataFromSeparator('\r', 2+Ch, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U16Value3,&U16Value4))
                                {
                                    buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                                    break;
                                }
                                if(U16Value4 != 13)
                                {
                                    buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                                    break;
                                }
                                
                                if(ERROR == XA2_DetOutside2Inside(UserParam.DetType,&buffer_in->uch_buf[5+U16Value3], &TempDetSel.U32Value,&TempDetPwd.U32Value))//������ת��������
                                {
                                    buffer_out->uch_buf[buffer_out->bytenum++] = '9';
                                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                                    break;                           
                                }
                                memcpy(&Channel.BarCode[Ch].CodeExternal[0], &buffer_in->uch_buf[5+U16Value3], 13); 
//								sprintf((char*)Temp1, "%08x%08x",TempSer,TempPwd);//???
//                                memcpy(&Channel.BarCode[Ch].DetCode[0], Temp1, 8);//������ת��������
								memcpy(&Channel.BarCode[Ch].DetCode[0], &TempDetSel.U8Buffer[0], 4);//������ת��������
								memcpy(&Channel.BarCode[Ch].DetCode[4], &TempDetPwd.U8Buffer[0], 4);//������ת��������
                                Channel.BarCode[Ch].Status = BARCODE_NEW;
                                Channel.BarCode[Ch].Result = BARCODE_PASS;                                
                            }
                            else
                            {
                                memset(&Channel.BarCode[Ch].CodeExternal[0], 0, 13); 
                                memset(&Channel.BarCode[Ch].DetCode[0], 0, 6);
                                Channel.BarCode[Ch].Status = BARCODE_IDLE;
                            }
                        }
                        if(buffer_out->bytenum <= 5)
                        {
                            buffer_out->uch_buf[buffer_out->bytenum++] = '1';					
                            buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;  

                            DetTestStart();//��ʼ����׹�                        
                        }                   
                    }
                    else if(OpData.TestStatus == TEST_DOING)//�п��ܳ�������
                    {                       
                        for(Ch=0; Ch<Channel.Count;Ch++)
                        {
                            if(Ch < Count)
                            {
                                if(ERROR == GetDataFromSeparator('\r', 2+Ch, &buffer_in->uch_buf[5],buffer_in->bytenum-5, &U16Value3,&U16Value4))
                                {
                                    buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                                    break;
                                }
                                if(U16Value4 != 13)
                                {
                                    buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                                    break;
                                }                        
                                if(memcmp(&Channel.BarCode[Ch].CodeExternal[0], &buffer_in->uch_buf[5+U16Value3], 13) != 0) 
                                {
                                    buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                                    break;
                                }                            
                            }
                            else
                            {
                                if (Channel.BarCode[Ch].Status != BARCODE_IDLE)
                                {
                                    buffer_out->uch_buf[buffer_out->bytenum++] = '2';
                                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                                    break;                                
                                }
                            }
                        }
                        if(buffer_out->bytenum <= 5)
                        {
                            buffer_out->uch_buf[buffer_out->bytenum++] = '1';					
                            buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;                     
                        }
                    }
                    else
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '8';					
                        buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;                      
                    }
				}
                else if ((HostPack == 0xFFFF) && (buffer_in->bytenum == 5) && (UserParam.Stage == SEMIFINISHED))
                {
                    u8 Ch;
                    
                    GetGeneralRespondData(buffer_out, Cmd, HostPack);
                    
                    for(Ch=0; Ch<Channel.Count;Ch++)
                    {
                        memset(&Channel.BarCode[Ch].CodeExternal[0], 0, 13); 
                        memset(&Channel.BarCode[Ch].DetCode[0], 0, 6);
                        Channel.BarCode[Ch].Status = BARCODE_IDLE;                    
                    }
                    if(OpData.TestStatus == TEST_WAIT_START)
                    {
                        DetTestStart();//��ʼ����׹�
                        buffer_out->uch_buf[buffer_out->bytenum++] = '1';
                    }
                    else if(OpData.TestStatus == TEST_DOING)
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '1';
                    }
                    else
                    {
                        buffer_out->uch_buf[buffer_out->bytenum++] = '2';					
                    }
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;                      
                }
				break;
			case 0x0012://�����״̬
				if ((HostPack == 0xFFFF) && (buffer_in->bytenum > 5)) 
				{
                    U8 Ch;
                    DET_ERR_CODE Status;
                    u32 freq;                //Ƶ��,��λHz
                    u8 type;                 //оƬ����
                    u16 lv_current;          //��ѹ���������ֵ
                    u16 lv_reverse_current;  //��ѹ�·������ֵ
                    u16 hv_current;          //��ѹ���������ֵ
                    u16 hv_reverse_current;	 //��ѹ�·������ֵ  
                    
					GetGeneralRespondData(buffer_out, Cmd, HostPack);
					if(ERROR == GetDecValueFromSeparator('\r',1,&buffer_in->uch_buf[5],buffer_in->bytenum-5, &U32Value))
					{
						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;
					}
                    
                    if (U32Value >= Channel.Count)
                    {
 						buffer_out->uch_buf[buffer_out->bytenum++] = '2';
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
						break;                       
                    }
 					buffer_out->uch_buf[buffer_out->bytenum++] = '1';
					buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                    
                    Ch = U32Value;
                    
                    OSSchedLock();
                    //�����
                    Status = Channel.XA2_Info[Ch].Status;
                    
                    //����           
                    Temp1[0] = Channel.XA2_Info[Ch].Serl.U8Buffer[0];
                    Temp1[1] = Channel.XA2_Info[Ch].Serl.U8Buffer[1];
                    Temp1[2] = Channel.XA2_Info[Ch].Serl.U8Buffer[2];
                    Temp1[3] = Channel.XA2_Info[Ch].Serl.U8Buffer[3];
                    Temp1[4] = Channel.XA2_Info[Ch].Pwd.U8Buffer[0];
                    Temp1[5] = Channel.XA2_Info[Ch].Pwd.U8Buffer[1];
					Temp1[6] = Channel.XA2_Info[Ch].Pwd.U8Buffer[2];
					Temp1[7] = Channel.XA2_Info[Ch].Pwd.U8Buffer[3];

                    
                    freq = Channel.XA2_Info[Ch].freq;                //Ƶ��,��λHz
					Temp1[8] = Channel.XA2_Info[Ch].Pwd.U8Buffer[1] & 0x03;
					Temp1[9] = Channel.XA2_Info[Ch].Pwd.U8Buffer[2] & 0xE0;
                    type = (Temp1[8]<<3) + (Temp1[9]>>5);                 //оƬ����
                    lv_current = Channel.XA2_Info[Ch].lv_current;          //��ѹ���������ֵ
                    lv_reverse_current = Channel.XA2_Info[Ch].lv_reverse_current;  //��ѹ�·������ֵ
                    hv_current = Channel.XA2_Info[Ch].hv_current;          //��ѹ���������ֵ
                    hv_reverse_current = Channel.XA2_Info[Ch].hv_reverse_current;	 //��ѹ�·������ֵ
                    
                    OSSchedUnlock();
                    
                    //״̬+����+����+����+Ƶ��+оƬ����+��ѹ���������+��ѹ�·������+��ѹ���������+��ѹ�·������
                    buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], "%d\r", (U8)Status);	  
                    
                    memcpy(&buffer_out->uch_buf[buffer_out->bytenum], &Channel.BarCode[Ch].CodeExternal[0], 13); 
                    buffer_out->bytenum += 13;
                    
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x0D;
                    
                    buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
                    "%02X%02X%02X%02X%02X%02X%02X%02X\r", Temp1[0], Temp1[1], Temp1[2], Temp1[3], Temp1[4], Temp1[5], Temp1[6], Temp1[7]);

                    buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
                    "%02X%02X\r", DET_PWD_FIX_VALUE>>8, DET_PWD_FIX_VALUE&0xFF);

                     buffer_out->bytenum += sprintf((char*)&buffer_out->uch_buf[buffer_out->bytenum], 
                        "%d\r%d\r%d\r%d\r%d\r%d\r", 
                        freq, type, lv_current, lv_reverse_current, hv_current,hv_reverse_current);                   
                    
				}
				break;                 
 			default:
				break;
		}
		if(buffer_in->uch_buf[0] == 0x00)
		{
			buffer_out->bytenum = 0;
		}
	}               
}

