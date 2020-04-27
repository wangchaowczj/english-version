#include "includes.h"

DET_COMM det_comm;


//==================================================================================================
//| 函数名称 | DetSendConfig
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 配置GPIO
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 |	无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 	无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-09-10 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void DetSendConfig(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/***配置TIM8时钟，用于发送命令***/
  	RCC_APB2PeriphClockCmd(DET_SEND_TIMER_RCC, ENABLE);
	
	/***时钟配置***/
	RCC_AHB1PeriphClockCmd(DET_RECEIVE_RES_PORT_RCC | BLAST_SIGNAL_SEND_PORT_RCC | DET_CMD_SEND_PORT_RCC | 
	DOUBLE_KEY_SW_PORT_RCC|ADJUST_G_PORT_RCC|DOUBLE_KEY1_SIM_SW_PORT_RCC|DOUBLE_KEY2_SIM_SW_PORT_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 

	/***发送与接收状态切换管脚**/
	GPIO_InitStructure.GPIO_Pin = DET_RECEIVE_RES_PIN; 
	GPIO_Init(DET_RECEIVE_RES_PORT, &GPIO_InitStructure);
	
	/***雷管命令发送管脚**/
	GPIO_InitStructure.GPIO_Pin = BLAST_SIGNAL_SEND_PIN;  
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	GPIO_Init(BLAST_SIGNAL_SEND_PORT, &GPIO_InitStructure);
	
	/***雷管命令发送管脚**/
	GPIO_InitStructure.GPIO_Pin = DET_CMD_SEND_PIN;  
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	GPIO_Init(DET_CMD_SEND_PORT, &GPIO_InitStructure);
		
	/***作为起爆器时起爆双键电子控制IO**/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
	GPIO_InitStructure.GPIO_Pin = DOUBLE_KEY_SW_PIN;  
	GPIO_Init(DOUBLE_KEY_SW_PORT, &GPIO_InitStructure);	

	/***作为从机时模拟双键控制使能开关**/
	GPIO_InitStructure.GPIO_Pin = DOUBLE_KEY1_SIM_SW_PIN;   
	GPIO_Init(DOUBLE_KEY1_SIM_SW_PORT, &GPIO_InitStructure);

	/***作为从机时模拟双键控制使能开关**/
	GPIO_InitStructure.GPIO_Pin = DOUBLE_KEY2_SIM_SW_PIN;   
	GPIO_Init(DOUBLE_KEY2_SIM_SW_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; 
	
	/***比较器复位管脚**/
	GPIO_InitStructure.GPIO_Pin = ADJUST_G_PIN; 
	GPIO_Init(ADJUST_G_PORT, &GPIO_InitStructure);	
	
	CLOSE_DOUBLE_KEY_SW();
	CLOSE_DOUBLE_KEY1_SIM_SW();
	CLOSE_DOUBLE_KEY2_SIM_SW();
	
	/***雷管命令发送管脚初始为高电平**/
	DET_COM_SEND_PIN_INIT();
	

	TIM_TimeBaseStructure.TIM_Prescaler = 0;//设置预分频：不预分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseStructure.TIM_Period = DET_SEND_TIMER_50US_VALUE;   //当定时器从0计数到16799，即为16800次，为一个定时周期
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//设置时钟分频系数：不分频
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x00;
	TIM_TimeBaseInit(DET_SEND_TIMER, &TIM_TimeBaseStructure);

	/***暂不启动定时器***/
	TIM_Cmd(DET_SEND_TIMER, DISABLE);
	
	/***允许发送命令**/
	CLOSE_DOUBLE_KEY_SW();
	
	
	/***起爆信号初始电平**/
	BLAST_SIGNAL_PIN_INIT();	
	
	/***接收电阻默认为打开**/
	OPEN_DET_RECEIVE_RES();
	
	/***接收比较器默认为高**/
	ADJUST_G_RESET();
	
	/***发送定时器中断配置***/
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

//==================================================================================================
//| 函数名称 | DetReceiverConfig
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 配置GPIO,PC.6 PC.7作为接收雷管信号
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 |	无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 	无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-09-10 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void DetReceiverConfig(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	 /* Enable clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource6);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource7);	
		
	/* Configure PC6 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/*外部下降沿中断*/
	EXTI_InitStructure.EXTI_Line = EXTI_Line6; //FALL
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
			
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;	//RISE
	EXTI_Init(&EXTI_InitStructure);

	TIM_TimeBaseStructure.TIM_Prescaler = 0;//设置预分频：不预分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseStructure.TIM_Period = 0xFFFFFFFF; //TIME5和TIME2都是32位定时器 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//设置时钟分频系数：不分频
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x00;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM2, DISABLE);	//需要时再启动
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	//FALL,RISE信号，外部下降沿中断
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	//定时器中断，配合接收FALL和RISE信号
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//需要比UART3高，可抢占
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);		
}

//==================================================================================================
//| 函数名称 | DetTransmitStart
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 启动雷管命令发送功能
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------        
//| 函数设计 | 编写人：郑海    时间：2013-09-10  
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   |   
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
static void DetTransmitStart(void)
{
	det_comm.send_index = 0;
	det_comm.receive_state = 0;
	det_comm.blast_signal_flag = 0;
	det_comm.enable_receive = 0;
	
	/***雷管命令发送管脚初始为高电平**/
	DET_COM_SEND_PIN_INIT();
	CLOSE_DET_RECEIVE_RES();//短路接收电阻
	TIM_SetCounter(DET_SEND_TIMER,0);//
	TIM_ClearFlag(DET_SEND_TIMER, TIM_FLAG_Update);
	TIM_ITConfig(DET_SEND_TIMER, TIM_IT_Update, ENABLE);	
	TIM_SetAutoreload(DET_SEND_TIMER, WAVE_WIDTH_100US);
	TIM_Cmd(DET_SEND_TIMER, ENABLE);	
	
	//等待发送完成
	while(det_comm.send_index != 0xFFFF);
	System168MDelay100us(1);	//延时100us
	OPEN_DET_RECEIVE_RES();
}

//==================================================================================================
//| 函数名称 | SendDetBlastSignal 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 发送起爆信号
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-08-25 
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void SendDetBlastSignal(void)
{
	/**配置定时器**/
	TIM_SetCounter(TIM2, 0);	
	TIM_SetAutoreload(TIM2, BLAST_SIGNAL_SEND_TIMER_10_2MS_VALUE);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	
	/***雷管命令发送管脚初始为高电平**/
	DET_COM_SEND_PIN_INIT();
	
	/***起爆信号初始电平**/
	BLAST_SIGNAL_PIN_INIT();
	
	/**启动定时器**/
	det_comm.blast_signal_flag = 1;
	det_comm.blast_signal_toggle_count = 0;
	TIM_Cmd(TIM2, ENABLE);	
	
	/***等待发送完成***/
	while(det_comm.blast_signal_flag == 1)
	{//通过对端口电压进行测量，确认起爆信号已经发出
		if(BlastSingleMonitor.Count < BLAST_SINGLE_MONITOR_BUFFER_SIZE)
		{
			BlastSingleMonitor.LinABAdc[BlastSingleMonitor.Count++] = GetLinABAdcValue(); 
			System168MDelay1ms(1);		
		}
	}
	/***起爆信号初始电平**/
	BLAST_SIGNAL_PIN_INIT();	
	//Op_Data.DoubleKeySwitch = 0;//不允许按下双键	
	//CLOSE_DOUBLE_KEY_SW();
}

//==================================================================================================
//| 函数名称 | GetBitNum
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 根据总计数值及每位所占的计数值计算出总位数
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | sum_count:总计数值
//|          | bit_count:每位所占的计数值
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 总位数
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-09-10  
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   |   
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
static u8 GetBitNum(u32 sum_count, u32 bit_count)
{
	u8 bit_num = 0;
	u32 temp = 0;

	bit_num = (u8)(sum_count/bit_count);
	temp = sum_count%bit_count;
	if((temp*2) > bit_count)  //最后的余数超过一半时认为是完整的1位
	{
		bit_num++;	
	}
	return bit_num;	
}

//==================================================================================================
//| 函数名称 | DetCmdCode
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 把雷管命令字节编码成方便发送的2进制数据
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | cmd_ptr:需要发送的数据缓冲区首址
//|          | length:需要发送的数据缓冲区长度
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：郑海    时间：2013-09-10  
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 因每个命令的起始位和同步字节都一样，因此不作编写，在发送时进行自行处理    
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void DetCmdCode(const u8* cmd_ptr, u8 length)
{
	u8 i,j,temp,verify_bit=0;

	det_comm.send_length = 0;
	det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_100US; 

	for(i=0;i<9;i++)
	{
		det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_50US;
		det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_50US;	
	}

	for(i=0;i<length;i++)
	{
		temp = *cmd_ptr++ ;
		verify_bit = 0;
		for(j=0;j<8;j++)
		{
			if(temp&0x01) //先发送低位
			{
				det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_100US;
				det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_100US;
				verify_bit++;
			}
			else
			{			 
				det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_50US;
				det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_50US;
			}
			temp>>=1;
		}
		if(verify_bit&0x01)	//偶校验
		{
			 det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_100US;
			 det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_100US;
		}
		else
		{
			 det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_50US;
			 det_comm.send_data[det_comm.send_length++] = WAVE_WIDTH_50US;
		}
	}
}

//==================================================================================================
//| 函数名称 | DetTransmit
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 对数据进行编码后发送
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | cmd_ptr:需要发送的数据缓冲区首址
//|          | length:需要发送的数据缓冲区长度
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 无
//|----------|--------------------------------------------------------------------------------------        
//| 函数设计 | 编写人：郑海    时间：2013-09-10  
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   |      
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
void DetTransmit(const u8* cmd_ptr, const u8 length)
{
	/***雷管命令发送管脚初始为高电平**/
	DET_COM_SEND_PIN_INIT();
	System168MDelay1ms(10);//延时10ms
	DetCmdCode(cmd_ptr, length);//把命令字节编码成方便发送的数据	
	SET_CMD_SEND_STATE();//设置为发送状态
	DetTransmitStart();//启动发送
}

//==================================================================================================
//| 函数名称 | DetReceiver 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 雷管返回数据接收函数 
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | receive_ptr:接收缓冲区首址
//|          | mode: bit7表示是否需要测频,
//|          | Bit0-3表示需要接收数据的长度(不包括芯片频率数据的4字节数据)
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | EB_DET_TRANSCEIVER_SUCCESS-数据接收成功
//|          | EB_DET_TRANSCEIVER_FAIL-数据接收失败
//|----------|--------------------------------------------------------------------------------------        
//| 函数设计 | 编写人：    时间：2013-08-25  
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 1、有干扰脉冲的情况下，可能造成接收失败
//|          | 2、脉冲长度不能长于定时器所能表示的长度
//|          | 3、此函数依赖雷管信号引起的外部中断触发
//|          | 4、需要测频时必需先通过写延期指令写入0x0F0F0F
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
u8 DetReceiver(u8* receive_ptr, u8 mode)
{
	u32 temp_buffer[70];
	u8 bit_buffer[70];
	u8 receive_buffer[10];
	u32 det_bit_clk = 0; //16个CLK对应的计数值
	u32 det_4bit_clk = 0;//64个CLK对应的计数值
	u8 i,j,wave_cnt = 0;
	u32 freq = 0;  //芯片频率
	u8 sum_num = 0,mask_bit = 0;//用于把时间数据转换为字节数据
	double freq_buffer = 0;

	/**命令发送完成后，芯片127个时钟后返回波形***/
	TIM_SetCounter(TIM2, 0);

	/**在接收到2个RISE下降沿后重新计算超时时间**/
	TIM_SetAutoreload(TIM2, TIME2_DET_165CLK);//保证至少足够接收到2个RISE信号
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_Cmd(TIM2, ENABLE);	//启动定时器

	det_comm.receive_bytes = mode & 0x0F;  //要接收到返回字节，不包括同步头

	det_comm.receive_edg_max_num = (det_comm.receive_bytes+1)*4;
	det_comm.receive_rise_length = 0;
	det_comm.receive_fall_length = 0; //实际捕获的下降沿个数
	memset(det_comm.rise_receive, 0, sizeof(det_comm.rise_receive));
	memset(det_comm.fall_receive, 0, sizeof(det_comm.fall_receive));
	
	SET_CMD_RECEIVE_STATE();//设置为接收状态
	ADJUST_G_RESET();//让比较器复位一下
	System168MDelay100us(4);	//延时400us


	EXTI_ClearITPendingBit(EXTI_Line6);
	EXTI_ClearITPendingBit(EXTI_Line7);
	
	det_comm.enable_receive = 1;//启动FALL、RISE下降沿中断
	while(det_comm.receive_state == 0);//等待接收完成
	det_comm.enable_receive = 0;
	//SET_LV_6_7V(); //切换到正常状态
	SET_CMD_SEND_STATE();//接收电阻处于被短路状态，提高端口电压
	
	if(det_comm.receive_rise_length != det_comm.receive_fall_length)
	{
	   if(det_comm.receive_fall_length > det_comm.receive_rise_length)//正常波形FALL的下降沿个数不可能比RISE的个数多
	   {
			return EB_DET_TRANSCEIVER_FAIL;			
	   }
	   if((det_comm.receive_rise_length - det_comm.receive_fall_length)>1)//正常波形RISE的下降沿个数最多比FALL的个数多1
	   {
			return EB_DET_TRANSCEIVER_FAIL;			
	   }
	}

	if((det_comm.receive_rise_length>=3)&&(det_comm.receive_fall_length>=3)) //至少已经检测到3个同步头
	{
		/*对得到的数据进行必需的检查*/
		for(i=0; i < det_comm.receive_rise_length-1; i++)
		{
			if(det_comm.rise_receive[i] >= det_comm.rise_receive[i+1]) //确保为顺序递增
			{
				return EB_DET_TRANSCEIVER_FAIL;	//数据异常
			}
		}
		for(i=0; i < det_comm.receive_fall_length-1; i++)
		{
			if(det_comm.fall_receive[i]>=det_comm.fall_receive[i+1])//确保为顺序递增
			{
				return EB_DET_TRANSCEIVER_FAIL;	//数据异常
			}
			if(det_comm.rise_receive[i] >= det_comm.fall_receive[i])//确保先接收到上升沿再接收到下降沿，交替接收
			{
				return EB_DET_TRANSCEIVER_FAIL;	//数据异常
			} 
			if(det_comm.rise_receive[i+1] <= det_comm.fall_receive[i])//确保先接收到上升沿再接收到下降沿，交替接收
			{
				return EB_DET_TRANSCEIVER_FAIL;	//数据异常
			}
		}

		temp_buffer[0] = det_comm.rise_receive[1] - det_comm.rise_receive[0];
		temp_buffer[1] = det_comm.rise_receive[2] - det_comm.rise_receive[1];
		temp_buffer[2] = det_comm.fall_receive[1] - det_comm.fall_receive[0];
		temp_buffer[3] = det_comm.fall_receive[2] - det_comm.fall_receive[1];

		if((U32DataCMP(temp_buffer,4)) > 1000)//得到的同步头数据中，32个CLK相差5000个计数(约59us)以上,保证不小于2周期
		{
			return EB_DET_TRANSCEIVER_FAIL;	//同步头数据不稳定
		}
		//根据同步头计算出每位对应的计数值及芯片频率()
		temp_buffer[0] = (temp_buffer[0]+temp_buffer[1]+temp_buffer[2]+temp_buffer[3])/4;//32个CLK对应的计数值
		det_bit_clk = temp_buffer[0]/2;//返回波形中每位对应的计数值(对应16个芯片周期)
		freq_buffer = (double) TIME3_FREQ/((double)(temp_buffer[0]/(double)32));//芯片频率，单位HZ
		freq = (u32)(freq_buffer+0.5);//芯片频率，单位HZ(四舍五入)
		if((freq<DET_RECEIVE_FREQ_MIN)||(freq>DET_RECEIVE_FREQ_MAX))//40K-120K
		{
			return EB_DET_TRANSCEIVER_FAIL;	//同步头数据不稳定
		}
		
		//相当于把2路信号还原回1路信号
		for(i=0;i < det_comm.receive_fall_length;i++)
		{
			temp_buffer[wave_cnt++] = det_comm.fall_receive[i] - det_comm.rise_receive[i];//对应高电平
			if((i == (det_comm.receive_fall_length-1)) &&
				(det_comm.receive_fall_length == det_comm.receive_rise_length))
			{				
				break;
			}			
			temp_buffer[wave_cnt++] = det_comm.rise_receive[i+1] - det_comm.fall_receive[i];//对应低电平
		}
		
		//在数据的最后如果持续低电平或者高电平超时，那么需要对数据以0或者1填充
		if(det_comm.receive_rise_length>det_comm.receive_fall_length)
		{
			//最后接收到的是1，以1填充
			memset(receive_buffer,0xFF,sizeof(receive_buffer));
		}
		else
		{	//最后接收到的是0，以0填充
			memset(receive_buffer, 0, sizeof(receive_buffer));
		}
		for(i=0; i < wave_cnt; i++)
		{   //把时钟数据转换为字节数据，同步头也按1个字节数据进行转换
			bit_buffer[i] = GetBitNum(temp_buffer[i], det_bit_clk);//转换为0或1的位个数
			if(i&1)
			{//对应低电平
				for(j=0; j<bit_buffer[i]; j++)
				{
					mask_bit = ~(1<<(sum_num%8));	//芯片先返回低位
					receive_buffer[sum_num/8] &= mask_bit;	 
					sum_num++;//已经接收转换的位数
				}
			}
			else
			{//对应高电平
				for(j=0; j<bit_buffer[i]; j++)
				{
					mask_bit = (1<<(sum_num%8));			//芯片先返回低位
					receive_buffer[sum_num/8] |= mask_bit;	 
					sum_num++;//已经接收转换的位数
				}
			}
		}				
		
		if((receive_buffer[0]&0x3F)!=0x15)//验证3个同步头是否正确
		{
			return EB_DET_TRANSCEIVER_FAIL;	//同步头数据错误
		}
		if(mode & 0x80)	 //测频
		{
			if((receive_buffer[2]==0x0F) && (receive_buffer[3]==0x0F) && (receive_buffer[4]==0x0F))
			{
				sum_num = 0;
				for(i=0; i<wave_cnt; i++)
				{
					sum_num += bit_buffer[i];
					if(sum_num == 20)//同步头占8位，状态字节8位，延期1的前半字节4位共20位
					{
						//测频时先通过写延期写入0x0F0F0F
						if(((i+4) < wave_cnt) && 
							(4 == GetBitNum(temp_buffer[i+1], det_bit_clk)) && 
							(4 == GetBitNum(temp_buffer[i+2], det_bit_clk)) && 
							(4 == GetBitNum(temp_buffer[i+3], det_bit_clk)) && 
							(4 == GetBitNum(temp_buffer[i+4], det_bit_clk)))
						{
							//64个CLK对应的计数值
							det_4bit_clk = (temp_buffer[i+1] + temp_buffer[i+2] + temp_buffer[i+3] + temp_buffer[i+4])/4;
							freq_buffer = (double) TIME3_FREQ/((double)(det_4bit_clk/(double)64));//芯片频率，单位HZ
							freq = (u32)(freq_buffer+.55);//芯片频率，单位HZ(四舍五入)
							memcpy((u8*)receive_ptr, (u8*)&freq, sizeof(freq));
							return EB_DET_TRANSCEIVER_SUCCESS;	
						}
						else
						{
							break;
						}					
					}
					else if(sum_num > 20)
					{
						return EB_DET_TRANSCEIVER_FAIL;	//数据错误
					}
				}				
			}
			return EB_DET_TRANSCEIVER_FAIL;	//数据错误
		}
		else
		{
			memcpy((u8*)receive_ptr,(u8*)&receive_buffer[1],det_comm.receive_bytes);
			return EB_DET_TRANSCEIVER_SUCCESS;	
		}						
	}

	return EB_DET_TRANSCEIVER_FAIL;	//同步头数不足3个	
}
