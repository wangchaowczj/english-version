/**
  *************************************************************************************
  * @file    Flash.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   Flash模块源文件
  *************************************************************************************
  */
#include "includes.h"
/**@defgroup Dkey
  *@{
  */
/**@defgroup Flash
  *@brief 延时模块
  *@{
  */

/**@defgroup Flash_Functions
  *@brief 函数
  *@{
  */
/**
  *************************************************************************************
  * @brief  Flash引脚配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2014年10月29日
  * @note   无
  *************************************************************************************  
  */
void FlashGpioConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(FLASH_WP_PORT_RCC | FLASH_SCLK_PORT_RCC| FLASH_MISO_PORT_RCC  | 
		FLASH_MOSI_PORT_RCC |FLASH_CS_PORT_RCC |FLASH_RST_PORT_RCC, ENABLE);

	
	//at45db 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_InitStruct.GPIO_Pin = FLASH_WP_PIN;
	GPIO_Init(FLASH_WP_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = FLASH_CS_PIN;
	GPIO_Init(FLASH_CS_PORT, &GPIO_InitStruct);
		
	GPIO_InitStruct.GPIO_Pin = FLASH_RST_PIN;
	GPIO_Init(FLASH_RST_PORT, &GPIO_InitStruct);
	
//	/* 配置SPI3引脚类型 */	
//		/*--JTAG-DP禁止,SW-DP允许, PA13, PA14用于调试仿真。PA15, PB3, PB4可用于SPI3--*/ 
//	AFIO->MAPR &= 0x00FFFFFF;
//	AFIO->MAPR |=0x02000000;
	/***管脚映射***/  
	GPIO_PinAFConfig(FLASH_MOSI_PORT, GPIO_PinSource3, GPIO_AF_SPI2);
	GPIO_PinAFConfig(FLASH_SCLK_PORT, GPIO_PinSource1, GPIO_AF_SPI2);
	GPIO_PinAFConfig(FLASH_MISO_PORT, GPIO_PinSource2, GPIO_AF_SPI2);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStruct.GPIO_Pin = FLASH_SCLK_PIN;
	GPIO_Init(FLASH_SCLK_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = FLASH_MISO_PIN;
	GPIO_Init(FLASH_MISO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = FLASH_MOSI_PIN;
	GPIO_Init(FLASH_MOSI_PORT, &GPIO_InitStruct);	

	FLASH_RESET_L();	

	FLASH_RESET_H();	
}
/**
  *************************************************************************************
  * @brief  Flash SPI配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2014年10月29日
  * @note   无
  *************************************************************************************  
  */
void FlashSPIConfig(void)
{
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB1PeriphClockCmd(FlashSPI_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	FLASH_CS_H();
    FLASH_WP_H();
	
	// SPI1配置
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(FlashSPI, &SPI_InitStructure);
	
	//使能SPI3
	SPI_Cmd(FlashSPI, ENABLE);
}

/**
  *************************************************************************************
  * @brief  Flash 发送并接收1字节数据
  * @param  byte 发送的数据
  * @retval 接收到的数据
  * @author ZH
  * @date   2014年10月29日
  * @note   无
  *************************************************************************************  
  */
static u8 FlashSendByte(u8 byte)
{
	while (SPI_I2S_GetFlagStatus(FlashSPI, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(FlashSPI, byte);
	while (SPI_I2S_GetFlagStatus(FlashSPI, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(FlashSPI);
}

/**
  *************************************************************************************
  * @brief  向Flash发送指令和数据，并接收返回数据
  * @param  md:需要执行的指令
  * @param  CmdLength:需要执行的指令大小 
  * @param  dir:指令执行方向，指明是读数据还是写数据
  *         @rag @b AT45DBX_READ: 读数据
  *         @rag @b AT45DBX_READ: 写数据  
  * @param  Data:存放数据的缓冲区
  * @param  bytes:数据字节数  
  * @retval 无
  * @author ZH
  * @date   2014年10月29日
  * @note   无
  *************************************************************************************  
  */
void FlashTransmmitData(u8 *Cmd, u8 CmdLength, AT45DBXDIR dir, u8* Data, u16 bytes)
{
    u16 i = 0;
    
    FLASH_CS_L();         // 选中AT45DBX芯片
    FLASH_WP_H();         // 取消写保护

    for(i=0; i<CmdLength; i++)
    {// 发送指令
        FlashSendByte(*(Cmd+i));
    }

    if(Data!=NULL)
    {// 如果有缓冲区，发送或接收数据
        if(dir==AT45DBX_READ)
        {// 读取数据
            for(i=0; i<bytes; i++)
            {
                *Data++=FlashSendByte(0x00);
            }
        }
        else if(dir==AT45DBX_WRITE)
        {// 写入数据
            for(i=0; i<bytes; i++)
            {
                FlashSendByte(*Data++);
            }
        }
    }

    FLASH_CS_H();      // 取消选中AT45DBX芯片
    FLASH_WP_L();       // 写保护
}

/**
  *************************************************************************************
  * @brief  Flash延期函数
  * @param  t：延时毫秒数 
  * @retval 无
  * @author ZH
  * @date   2014年10月29日
  * @note   无
  *************************************************************************************  
  */
static void FlashDelay(uint32_t t)
{
    uint32_t  i;
    uint32_t _1us_count = 0;
    RCC_ClocksTypeDef rcc;

    RCC_GetClocksFreq(&rcc);   // 获得系统时钟频率

    _1us_count = rcc.SYSCLK_Frequency/4000;   /* 1ms需要的时钟数 = 系统频率 / 1000(1ms的时钟数) / 4(for循环占用的指令数) */
    _1us_count = t * _1us_count;
    for(i = 0; i < _1us_count; i++)
    {
        ;
    }
}

/**
  *************************************************************************************
  * @brief  Flash硬件复位
  * @param  无 
  * @retval 无
  * @author ZH
  * @date   2014年10月29日
  * @note   无
  *************************************************************************************  
  */
static void  FlashHardwareRest(void)
{
    FLASH_RESET_L();
    FlashDelay(10);
    FLASH_RESET_H();
    FlashDelay(10);	
}

/**
  *************************************************************************************
  * @brief  获取Flash状态寄存器1
  * @param  无 
  * @retval 寄存器1的值
  * @author ZH
  * @date   2014年10月29日
  * @note   无
  *************************************************************************************  
  */
static u8 FlashStatusRegister1(void)
{
	u8 status;
    u8 cmd[] = {DFCMD_READ_STATUS/*读状态指令*/};
	
    FlashTransmmitData(cmd, sizeof(cmd), AT45DBX_READ, &status, 1);   // 向FLASH发送指令并保存返回数据
	
	return status;
} /* flash_at45dbx_status */

/**
  *************************************************************************************
  * @brief  等待Flash操作完成
  * @param  无 
  * @retval ErrorStatus 等待结果
  *         @arg @b SUCCESS 操作已经完成
  *         @arg @b ERROR 操作没有完成
  * @author ZH
  * @date   2014年10月29日
  * @note   无
  *************************************************************************************  
  */
static ErrorStatus FlashWaitReady(void)
{
	u16 i;
	u8  status;

	for(i = 0;i < 0xAFFF;i++)
	{		
        status = FlashStatusRegister1();
		if(status & AT45DBX_MASK_BUSY)
		{// 如果不忙则认为操作成功
		    return SUCCESS;
		}		
	}
    return ERROR;   // 数据操作失败
}


/**
  *************************************************************************************
  * @brief  Flash硬件初始化
  * @param  无 
  * @retval ErrorStatus 等待结果
  *         @arg @b SUCCESS 初始化成功
  *         @arg @b ERROR 初始化失败
  * @author ZH
  * @date   2014年10月29日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus FlashHardwareInit(void)
{
    u8 Status;
    
	FlashGpioConfig();
	FlashSPIConfig();
    FlashHardwareRest();                       // 复位at45dbx芯片

    Status = FlashStatusRegister1();           // 读取at45dbx的ID

	/*同时判断0和1可有效避免返回值为全0全1异常造成误判 */
	if((Status&0xBC) == 0xBC)
	{//根据手册28页状态寄存器1说明：bit7 为1表示空闲，bit5-2都为1
		if((Status&0x02) == 0x00)
		{//Sector Protection Status 为0表示没有打开保护，前边刚复位芯片，没有打开保护
			return SUCCESS;//
		}
	}
    return ERROR;
}

/**
  *************************************************************************************
  * @brief  Flash块擦除
  * @param  Block 块地址 
  * @retval ErrorStatus 擦除结果
  *         @arg @b SUCCESS 擦除成功
  *         @arg @b ERROR 擦除失败
  * @author ZH
  * @date   2014年10月29日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus FlashEraseBlock(u16 Block)
{
    u8 cmd[] = {DFCMD_BLOCK_ERASE/*读状态指令*/, 0x00, 0x00, 0x00};
	u16 Page;
	
    Page = Block<<3;  // 块调整为起始页地址，为与页操作保持一致
	Page <<= 1;//PA14 - PA3表示页地址	
    cmd[1] = (u8)(Page >> 8);
    cmd[2] = (u8)Page;
	cmd[3] = 0x00;
    FlashTransmmitData(cmd, sizeof(cmd), AT45DBX_READ, NULL, 0);   // 向FLASH发送指令并保存返回数据

    return FlashWaitReady();
}

/**
  *************************************************************************************
  * @brief  Flash擦除整个芯片
  * @param  Block 块地址 
  * @retval ErrorStatus 擦除结果
  *         @arg @b SUCCESS 擦除成功
  *         @arg @b ERROR 擦除失败
  * @author ZH
  * @date   2014年10月29日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus FlashEraseChip(void)
{
    u8 cmd[] = {DFCMD_CHIP_ERASE, 0x94, 0x80, 0x9A};
	u16 i;
    FlashTransmmitData(cmd, sizeof(cmd), AT45DBX_READ, NULL, 0);   // 向FLASH发送指令并保存返回数据

	for(i = 0;i<0xAFFF;i++)
	{
		if(ERROR != FlashWaitReady())
		{
			return SUCCESS;
		}	
	}

	return ERROR;
}

/**
  *************************************************************************************
  * @brief  Flash执行页指令
  * @param  code 需要执行的指令代码
  * @param  page 页地址  
  * @retval ErrorStatus 指令结果
  *         @arg @b SUCCESS 指令成功
  *         @arg @b ERROR 指令失败
  * @author ZH
  * @date   2014年10月29日
  * @note   无
  *************************************************************************************  
  */
static ErrorStatus FlashPageCmd(u8 code, u16 page)
{
    u8 cmd[4];
	u16 temp;
     
    temp = 	page<<1;
	
    cmd[0] = code;// 需要发送的页指令
    cmd[1] = (u8)(temp >> 8);// 页地址(PA14 - PA0）
    cmd[2] = (u8)temp;       // 页地址+1位无关
    cmd[3] = 0x00;           // 无关位

    FlashTransmmitData(cmd, sizeof(cmd), AT45DBX_READ, NULL, 0);   // 向FLASH发送指令并保存返回数据    
    return FlashWaitReady();
}

/**
  *************************************************************************************
  * @brief  擦除指定页
  * @param  page 页地址  
  * @retval ErrorStatus 擦除结果
  *         @arg @b SUCCESS 擦除成功
  *         @arg @b ERROR 擦除失败
  * @author ZH
  * @date   2014年10月29日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus FlashErasePage(u16 Page) 
{
    return FlashPageCmd(DFCMD_PAGE_ERASE, Page);
}

/**
  *************************************************************************************
  * @brief  从指定地址读取数据
  * @param  addr: 地址
  * @param  Data: 存放数据的缓冲区 
  * @param  Bytes: 需要读取的数据字节数
  * @retval 无
  * @author ZH
  * @date   2014年10月29日
  * @note   无
  *************************************************************************************  
  */
void FlashReadData(u32 Addr, u8 *Data, u16 Bytes)
{
    u8 cmd[5];
    u16 Page = Addr / FLASH_PAGE_BYTES;
    u16 offset =  Addr % FLASH_PAGE_BYTES;

	Page <<= 1;
	
    cmd[0] = DFCMD_CONT_READ_HF;
    cmd[1] = (u8)(Page >> 8);//前15位表示页地址，后9位表示内在起始地址
    cmd[2] = (u8)((Page&0xFF) + ((offset>>8)&0x01));
    cmd[3] = (u8)offset;
	cmd[4] = 0x00;
    FlashTransmmitData(cmd, sizeof(cmd), AT45DBX_READ, Data, Bytes);   // 向FLASH发送指令并保存返回数据
}

/**
  *************************************************************************************
  * @brief  写数据到指定地址
  * @param  Addr:地址
  * @param  Data:需要写入数据的缓冲区 
  * @param  Bytes:需要写入数据的字节数  
  * @retval ErrorStatus 写数据结果
  *         @arg @b SUCCESS 写数据成功
  *         @arg @b ERROR 写数据失败
  * @author ZH
  * @date   2014年10月29日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus FlashWriteData(u32 Addr, const u8 *Data, u16 Bytes)
{
    uint16_t cur_page;
    uint16_t cur_ofs;
    uint8_t cmd[] = {DFCMD_BUF1_WRITE, 0x00, 0x00, 0x00};
    uint16_t send_len;
    ErrorStatus status = ERROR;

	while(Bytes) 
	{
		cur_page = (uint16_t)(Addr / FLASH_PAGE_BYTES);
		cur_ofs  = (uint16_t)(Addr % FLASH_PAGE_BYTES);
		
		// 将页数据从flash移到buf1中
        status = FlashPageCmd(DFCMD_MM_PAGE_TO_B1_XFER,cur_page);           
        if(status == ERROR)                                          
		{// 指令出现错误
			break;  
		}
		
        //修改buf1中的数据
        cmd[2] = (uint8_t)(cur_ofs>>8);
        cmd[3] = (uint8_t)cur_ofs;
        send_len = 0;
		do 
		{
			Addr++;
            send_len++;
		} 
		while((--Bytes) && (++cur_ofs < FLASH_PAGE_BYTES));
        FlashTransmmitData(cmd, sizeof(cmd), AT45DBX_WRITE, (uint8_t *)Data, send_len);
        Data += send_len; // 调整发送数据指针

		// 将buf1中的数据移到cur_page页
        status = FlashPageCmd(DFCMD_BUF1_FLASH, cur_page);                  
        if(status==ERROR)
		{// 指令出现错误
			break;                                           
		}		
		status = FlashPageCmd(DFCMD_AUTO_REW_B1,cur_page);
        if(status == ERROR) 
		{// 指令出现错误
			break;    
		}
	} 
    return status;
} 
/**
  *@} Flash_Functions
  */
/**
  *@} Flash
  */
/**
  *@} Dkey
  */

