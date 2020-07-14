#include "includes.h"
#include "GuiCustomUtils.h"

extern void DetTestStart(void);

extern u16 HoldRegBuffer[200];
u8 NULLCodeExternal[] = "0000000000000" ;
/**
  *************************************************************************************
  * @brief  解包
  * @param[in]  buffer_in：输入数据
  * @param[out]  buffer_out：输出数据  
  * @retval 无 
  * @author ZH
  * @date   2018年6月11日
  * @note   在每包数据的前/后有冗余数据时也要能解析成功
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
  * @brief  设置单个单个线圈值
  * @param[in]  buffer_in：输入数据
  * @param[out]  buffer_out：输出数据  
  * @retval 无 
  * @author ZH
  * @date   2018年6月11日
  * @note   功能码0x05
  *************************************************************************************  
  */
static void SetSingleCoilValue(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 RegAdd = (buffer_in->uch_buf[2]<<8) + buffer_in->uch_buf[3];
	u16 Value = (buffer_in->uch_buf[4]<<8) + buffer_in->uch_buf[5];
	
	switch(RegAdd)
	{
		case 0x0300://开关信号检测命令
            //0xFF00表示ON，0x0000表示OFF
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
                buffer_out->uch_buf[buffer_out->bytenum++] = 0x03; //非法数值              
            }
			break;
		case 0x0301://表示所有通道的检测数据已经读走，设备可以开始下一次检测
            //0xFF00表示ON
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
                buffer_out->uch_buf[buffer_out->bytenum++] = 0x03; //非法数值              
            }
			break;            
		default:
			break;			
	}
}
/**
  *************************************************************************************
  * @brief  读多个寄存器得值
  * @param[in]  buffer_in：输入数据
  * @param[out]  buffer_out：输出数据  
  * @retval 无 
  * @author ZH
  * @date   2018年6月11日
  * @note   功能码0x03
  *************************************************************************************  
  */
static void ReadMultRegValue(STR_COMM *buffer_in, STR_COMM *buffer_out)
{
	u16 RegStartAdd = (buffer_in->uch_buf[2]<<8) + buffer_in->uch_buf[3];
	u16 RegCount = (buffer_in->uch_buf[4]<<8) + buffer_in->uch_buf[5];
	
    if((RegCount == 0) || (RegCount > 0x7D))//modbus协议要求
    {
        return;
    }
    
    if((RegStartAdd >= 0x0100) && ((RegStartAdd+RegCount) <= (0x0100+20*Channel.Count)))//每个通道占20个寄存器
    {
        u16 Ch;
        u8 add,Count;
        u32 Pwd32;
        u8 Buffer[8];
        DET_ERR_CODE Status;
        
        for(Ch=0; Ch< Channel.Count; Ch++)
        {
            add = Ch*20;
            //每个通道对应的数据为20个寄存器：
            //通道号1个寄存器，检测结果 1个寄存器，内码 8个寄存器，密码 3个寄存器，外壳码7个寄存器，共20个寄存器
            
            //通道号
            HoldRegBuffer[add++] = Ch+1; 
            
            OSSchedLock();
            
            //检测结果
            Status = Channel.XA2_Info[Ch].Status;
            HoldRegBuffer[add++] = (u16)Status;
            
            //内码           
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
            {//没有读到内码时,内码和密码全设置为0x00
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
                HoldRegBuffer[add++] = (0x08<<8)|GetCrc8Value(&Buffer[0], 8);//有效字节数+crc8校验
                
                //密码   是数据的32位校验不是雷管密码         
                Pwd32 = CRC32Default(&Buffer[0], 8); 

                HoldRegBuffer[add++] = Pwd32>>16;
                HoldRegBuffer[add++] = Pwd32&0xFFFF;
                HoldRegBuffer[add++] = 0x00;            
            }
            
            //外壳码
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
        
        buffer_out->uch_buf[buffer_out->bytenum++] = RegCount*2;//响应的字节数        
        
        Count = 0;
        while(Count < RegCount)
        {
            add = RegStartAdd-0x100+Count;
            buffer_out->uch_buf[buffer_out->bytenum++] = HoldRegBuffer[add]>>8;
            buffer_out->uch_buf[buffer_out->bytenum++] = HoldRegBuffer[add]&0xFF;
            Count++;
        }      
    }
    
//	if ((RegStartAdd >= 0x0100) && (RegStartAdd <= (0x0100+20*9)) && (((RegStartAdd-0x0100)%20) == 0))//每个通道占20个寄存器
//	{//查询芯片检测结果
//        u16 Ch = (RegStartAdd-0x0100)/20;
//        u16 Status;
//        u32 Pwd32;
//        
//        if(RegCount == 0x0014)
//        {
//            //响应数据按顺序为
//            //通道号1个寄存器，检测结果 1个寄存器，内码 8个寄存器，密码 3个寄存器，外壳码7个寄存器，共20个寄存器
//            buffer_out->bytenum = 2;
//            memcpy(buffer_out->uch_buf, buffer_in->uch_buf, buffer_out->bytenum); 
//            
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x28;//响应的字节数
//            
//            //通道号
//            buffer_out->uch_buf[buffer_out->bytenum++] = (Ch+1)>>8;
//            buffer_out->uch_buf[buffer_out->bytenum++] = (Ch+1)&0xFF;
//            
//            //检测结果
//            Status = (u16)Channel.Info[Ch].Status;
//            buffer_out->uch_buf[buffer_out->bytenum++] = Status>>8;
//            buffer_out->uch_buf[buffer_out->bytenum++] = Status&0xFF;  
//            
//            //内码
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
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x06;//有效字节数
//            buffer_out->uch_buf[buffer_out->bytenum++] = GetCrc8Value(&buffer_out->uch_buf[7], 6);
//            
//            //密码
//            Pwd32 = CRC32Default(&buffer_out->uch_buf[7], 6);
//            
//            buffer_out->uch_buf[buffer_out->bytenum++] = (Pwd32>>24)&0xFF;
//            buffer_out->uch_buf[buffer_out->bytenum++] = (Pwd32>>16)&0xFF;
//            buffer_out->uch_buf[buffer_out->bytenum++] = (Pwd32>>8)&0xFF;
//            buffer_out->uch_buf[buffer_out->bytenum++] = Pwd32&0xFF;
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x00;
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x00;
//            
//            //外壳码
//            buffer_out->uch_buf[buffer_out->bytenum++] = 0x00;
//            memcpy(&buffer_out->uch_buf[buffer_out->bytenum], &Channel.BarCode[Ch].CodeExternal[0], 13);
//            buffer_out->bytenum += 13;                 
//        }
//    }               
}

/**
  *************************************************************************************
  * @brief  设置多个寄存器得值
  * @param[in]  buffer_in：输入数据
  * @param[out]  buffer_out：输出数据  
  * @retval 无 
  * @author ZH
  * @date   2018年9月5日
  * @note   功能码0x10
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
		case 0x0000://启动检测命令
			if((RegCount == (7*Channel.Count)) && (Bytes == (2*RegCount)))
			{
                u8 Ch;

                if(OpData.TestStatus == TEST_WAIT_START)
                {
					if((UserParam.DetCodeWrite == 1) && (UserParam.Stage == FINISHED))//规定成品写码
					{
						for(Ch=0; Ch<Channel.Count;Ch++)
						{
							memcpy(&Channel.BarCode[Ch].CodeExternal[0], &buffer_in->uch_buf[8+14*Ch], 13); 
							memset(&Channel.BarCode[Ch].DetCode[0], 0, 8);
							if(ERROR != XA2_DetOutside2Inside(UserParam.DetType,&Channel.BarCode[Ch].CodeExternal[0], &TempDetSel.U32Value,&TempDetPwd.U32Value))//把外码转换成内码
							{
	//							sprintf((char*)Temp1, "%08x%08x",TempSerl,TempPwd);//???
	//							sprintf((char*)Temp1, "%s%s",&TempDetSel.U8Buffer[0],&TempDetPwd.U8Buffer[0]);
								memcpy(&Channel.BarCode[Ch].DetCode[0], &TempDetSel.U8Buffer[0], 4);//把外码转换成内码
								memcpy(&Channel.BarCode[Ch].DetCode[4], &TempDetPwd.U8Buffer[0], 4);//把外码转换成内码
								Channel.BarCode[Ch].Status = BARCODE_NEW;
								Channel.BarCode[Ch].Result = BARCODE_PASS;
							}
	   
						}
					}
					else if((UserParam.DetCodeWrite == 0) && (UserParam.Stage == SEMIFINISHED) && (UserParam.CheckDetCode == 0))//规定半成品不写码
					{
						for(Ch=0; Ch<Channel.Count;Ch++)
						{
							memcpy(&Channel.BarCode[Ch].CodeExternal[0], &buffer_in->uch_buf[8+14*Ch], 13); 
							if(strcmp((char*)&Channel.BarCode[Ch].CodeExternal[0],"0000000000000") != 0)//半成品发送管码为全0
							{//设置错误，半成品检但是有非全0管码
								buffer_out->bytenum = 0;
								buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[0];
								buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[1]+0x80;
								buffer_out->uch_buf[buffer_out->bytenum++] = 0x03; 
								return;	
							}
						}
					}
					else
					{//设置错误，非（半成品不写不检、成品写码）
						buffer_out->bytenum = 0;
						buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[0];
						buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[1]+0x80;
						buffer_out->uch_buf[buffer_out->bytenum++] = 0x03; 
						return;						
					}
                    DetTestStart();//开始检测雷管
                    
                    buffer_out->bytenum = 6;
                    memcpy(buffer_out->uch_buf, buffer_in->uch_buf, buffer_out->bytenum);//数据原样返回
                }
                else if(OpData.TestStatus == TEST_DOING)//有可能出现重试
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
                    memcpy(buffer_out->uch_buf, buffer_in->uch_buf, buffer_out->bytenum);//数据原样返回
                }
                else
                {
                    buffer_out->bytenum = 0;
                    buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[0];
                    buffer_out->uch_buf[buffer_out->bytenum++] = buffer_in->uch_buf[1]+0x80;
                    buffer_out->uch_buf[buffer_out->bytenum++] = 0x06; //从设备忙                
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
  * @brief  串口响应函数
  * @param[in]  buffer_in：输入数据
  * @param[out]  buffer_out：输出数据  
  * @retval 无 
  * @author ZH
  * @date   2018年6月11日
  * @note   针对的是广东韶化
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
		case 0x05://设置单个线圈, 标准MODBUS协议格式
			SetSingleCoilValue(buffer_in,  buffer_out);
			break;
		case 0x03://读寄存器, 标准MODBUS协议格式
			ReadMultRegValue(buffer_in,  buffer_out);			
			break;			
		case 0x10://设置多个寄存器, 非标准MODBUS协议格式
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
