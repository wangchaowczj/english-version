/**
  *************************************************************************************
  * @file    SdCard.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014年9月10日
  * @brief   SD卡模块源文件
  *************************************************************************************
  */
#include "includes.h"
/**@defgroup PowerBoard
  *@{
  */
/**@defgroup SdCard
  *@{
  */
static u8 SdCardType;			/**<SD卡类型*/
static u32 SdCardDelay1sCount;  /**<延时1秒计数器*/

/**@defgroup SdCard_Functions
  *@{
  */
/**
  *************************************************************************************
  * @brief  SPI1配置
  * @param  无
  * @retval 无
  * @author ZH
  * @date   2014年9月11日
  * @note   目前暂时使用模拟SPI
  *************************************************************************************  
  */
void SdCardSpiConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;


	RCC_AHB1PeriphClockCmd(SDCARD_SPI_SCLK_PORT_RCC | SDCARD_SPI_MISO_PORT_RCC| 
		SDCARD_SPI_MOSI_PORT_RCC | SDCARD_SPI_CS_PORT_RCC, ENABLE);
		
	RCC_APB2PeriphClockCmd(SDCARD_SPI_RCC | RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_PinAFConfig(SDCARD_SPI_SCLK_PORT, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(SDCARD_SPI_MISO_PORT, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(SDCARD_SPI_MOSI_PORT, GPIO_PinSource7, GPIO_AF_SPI1);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		
	GPIO_InitStruct.GPIO_Pin = SDCARD_SPI_CS_PIN;
	GPIO_Init(SDCARD_SPI_CS_PORT, &GPIO_InitStruct);
	
	/* 配置SPI1引脚类型 */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStruct.GPIO_Pin = SDCARD_SPI_SCLK_PIN;
	GPIO_Init(SDCARD_SPI_SCLK_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = SDCARD_SPI_MISO_PIN;
	GPIO_Init(SDCARD_SPI_MISO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = SDCARD_SPI_MOSI_PIN;
	GPIO_Init(SDCARD_SPI_MOSI_PORT, &GPIO_InitStruct);	

	
	/*!< SPI1 configuration */
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;        // 在72Mhz的主频下，APB1为36Mhz(2分频)，波特率8分频后约4.5MHz
																	// 因为SD卡在SPI模式需要在100-400kHz的速度下初始化，即使设为
																	// 256分频，最大主频也不能超过102.4MHz。
																	// 设备暂时设SPI在工作时为2分频，SD初始化时为256分频

	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SDCARD_SPI, &SPI_InitStruct);

	/*!< Enable the sFLASH_SPI  */
	SPI_Cmd(SDCARD_SPI, ENABLE);
  //  SPI_InitTypeDef  SPI_InitStructure;

	SDCARD_SPI_CS_L();	

	SDCARD_SPI_CS_H();  
}	

/**
  *************************************************************************************
  * @brief  设置Spi1的速率
  * @param  baud：速率,取值如下：
  *         @arg @b SPI_BaudRatePrescaler_2
  *         @arg @b SPI_BaudRatePrescaler_4
  *         @arg @b SPI_BaudRatePrescaler_8
  *         @arg @b SPI_BaudRatePrescaler_16
  *         @arg @b SPI_BaudRatePrescaler_32
  *         @arg @b SPI_BaudRatePrescaler_64
  *         @arg @b SPI_BaudRatePrescaler_128
  *         @arg @b SPI_BaudRatePrescaler_256
  * @retval 无
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
void SdCardSpiSetBaud(u16 baud)
{
    u16 tmpreg = 0;
    
    tmpreg = SDCARD_SPI->CR1;
    tmpreg &= ~SPI_BaudRatePrescaler_BM;
    tmpreg |= baud;
    SDCARD_SPI->CR1 = tmpreg;
}
/**
  *************************************************************************************
  * @brief  获取Spi1的速率
  * @param  无
  * @retval SPI速率,值如下：
  *         @arg @b SPI_BaudRatePrescaler_2
  *         @arg @b SPI_BaudRatePrescaler_4
  *         @arg @b SPI_BaudRatePrescaler_8
  *         @arg @b SPI_BaudRatePrescaler_16
  *         @arg @b SPI_BaudRatePrescaler_32
  *         @arg @b SPI_BaudRatePrescaler_64
  *         @arg @b SPI_BaudRatePrescaler_128
  *         @arg @b SPI_BaudRatePrescaler_256
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
u16 SdCardSpiGetBaud(void)
{
    u16 tmpreg = 0;
    
    tmpreg = SDCARD_SPI->CR1;
    tmpreg &= SPI_BaudRatePrescaler_BM;

    return tmpreg;
}
/**
  *************************************************************************************
  * @brief  获取spi1时钟频率
  * @param  无
  * @retval spi1时钟频率
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
u32 SdCardSpiGetFrequency(void)
{
    uint32_t freq;    
    RCC_ClocksTypeDef rcc;
    uint16_t baud;

    RCC_GetClocksFreq(&rcc);   // 获得系统时钟频率
    
    freq = rcc.PCLK2_Frequency;
    baud = SdCardSpiGetBaud() >> SPI_BaudRatePrescaler_BP;
    freq = freq >> (baud + 1);
    
    return freq;
}
/**
  *************************************************************************************
  * @brief  SPI1发送并接收一个字节数据
  * @param  byte: 要发送的数据
  * @retval 接收到的数据
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
u8 SdCardSpiSendByte(u8 byte)
{
	while (SPI_I2S_GetFlagStatus(SDCARD_SPI, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SDCARD_SPI, byte);
	while (SPI_I2S_GetFlagStatus(SDCARD_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SDCARD_SPI);
}
/**
  *************************************************************************************
  * @brief  等待SD卡准备好 
  * @param  无
  * @retval 等待结果
  *         @arg @b ERROR   没有准备好
  *         @arg @b SUCCESS 已经准备好  
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus SdCardWaitReady(void)
{
	uint8_t res;
    uint32_t retry;

    retry = SdCardDelay1sCount >> 1;        // 需要延时500ms
    
	SdCardSpiSendByte(0xFF);
	do
	{
		res = SdCardSpiSendByte(0xFF);
	}	
	while ((res != 0xFF) && (--retry));

    if(0 != retry)
    {
	    return SUCCESS;
    }

    return ERROR;
}

/**
  *************************************************************************************
  * @brief  SD卡发送指令
  * @param[in]  cmd: 需要发送的指令
  * @param[in]  arg: 指令参数
  * @param[out] rtn: 指令执行完成后返回的状态
  * @param[in]  rlength: 返回状态长度
  * @retval 无
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
void SdCardSendCmd(u8 cmd, u32 arg, u8 *rtn, u8 rlength)
{
    uint8_t i;
    uint8_t retry;

	if (cmd & 0x80) 
	{	/* ACMD<n> is the command sequence of CMD55-CMD<n> */
		cmd &= 0x7F;
		SdCardSendCmd(SDCARD_CMD55, 0, rtn, rlength);
        if(*rtn > 1)
		{
			return;
		}			
	}

    SDCARD_SPI_CS_H();                               // CS引脚拉高
    SDCARD_SPI_CS_L();                                // CS引脚拉低
    if(SdCardWaitReady()==ERROR)
    {
        *rtn = 0xFF;
        return;
    }

    /* 发送指令 */
    SdCardSpiSendByte(cmd);
    SdCardSpiSendByte((uint8_t)(arg>>24));
    SdCardSpiSendByte((uint8_t)(arg>>16));
    SdCardSpiSendByte((uint8_t)(arg>>8));
    SdCardSpiSendByte((uint8_t)arg);
    if(SDCARD_CMD0 == cmd)
    {
		SdCardSpiSendByte(0x95);                   // 8位CRC校验，暂时没有实现所有数据都使用CRC校验
	}
    else if(SDCARD_CMD8 == cmd)
    {
		SdCardSpiSendByte(0x87);                   // 8位CRC校验，暂时没有实现所有数据都使用CRC校验
	}    
    else
    {
		SdCardSpiSendByte(0x01);                   // 8位CRC校验，其他指令没有校验
	}    

	/* Receive command response */
	if (SDCARD_CMD12 == cmd)
	{
		SdCardSpiSendByte(0xFF); /* Skip a stuff byte when stop reading */
	}		

    /* 等待SD卡返回 */
    if(rlength>0)
    {
        retry = 0;
        do
        {// 等待SD卡返回状态
            *rtn = SdCardSpiSendByte(0xFF);
            if(*rtn!=0xFF)
            {// SD卡状态返回成功
                for (i=0; i<rlength-1; i++)
                {// 保存SD卡返回的状态
                    rtn++;
                    *rtn = SdCardSpiSendByte(0xFF);
                }
                break; // 退出
            }
        } while(retry++<10);   // 指令发送出去到指令返回有1-8个等待时间，单位是8 clock cycles
    }
}

/**
  *************************************************************************************
  * @brief  SD卡接收数据块
  * @param[out] buff: 数据缓冲区首址
  * @param[in]  bytes: 数据长度
  * @retval 结果
  *         @arg @b ERROR   失败
  *         @arg @b SUCCESS 成功 
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus SdCardRcvDataBlock(uint8_t *buff, uint32_t bytes)
{
    uint8_t tmp[7];
    uint32_t retry;
    uint32_t i;
    ErrorStatus rtn = SUCCESS;

    retry = SdCardDelay1sCount >> 3;                  // 延时125ms
    
    do
    {
        tmp[0] = SdCardSpiSendByte(0xFF);

    }while((tmp[0]!=SDCARD_CMD17_TOKEN)&&(--retry));
    
    if(retry>0)
    {// 获得令牌成功
        for (i=0; i<bytes; i++)
        {
            *buff++ = SdCardSpiSendByte(0xFF);
        }
        SdCardSpiSendByte(0xFF);        // 读CRC校验,暂时不做处理
        SdCardSpiSendByte(0xFF);
    }
    else
    {// 获得令牌失败
        rtn = ERROR;
    }

    return rtn;
}




/**
  *************************************************************************************
  * @brief  SD卡读1块数据
  * @param[out] buff:  数据缓冲区
  * @param[in]  sector:开始扇区号
  * @param[in]  count: 扇区数
  * @retval 结果
  *         @arg @b ERROR   失败
  *         @arg @b SUCCESS 成功 
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus SdCardReadBlock(uint8_t *buff, uint32_t sector, uint32_t count)
{
    uint8_t tmp[7];
    ErrorStatus rtn = SUCCESS;

	if (!(SdCardType & SDCARD_TYPE_BLOCK)) 
	{
		sector *= 512;	/* Convert to byte address if needed */
	}
    if(count==1)
    {
        /* 发送CMD17 */
        SdCardSendCmd(SDCARD_CMD17, sector, tmp, 1);       // 发送读单块数据指令,把扇区号转化为字节地址，
        if(tmp[0]==SDCARD_R1_NOERR)
        {// CMD17指令执行正常
            rtn = SdCardRcvDataBlock(buff, 512);
        }
        else
        {// CMD17指令执行失败
            rtn = ERROR;
        }
    }
    else
    {
        /* 发送CMD18 */
        SdCardSendCmd(SDCARD_CMD18, sector, tmp, 1);       // 发送读多块数据指令,把扇区号转化为字节地址，
        do
        {// 循环读取多个块数据，直至读取完成或出现错误
            rtn = SdCardRcvDataBlock(buff, 512);
            if(rtn==ERROR)
            {
                break;
            }
            buff += 512;
        } while(--count);
        SdCardSendCmd(SDCARD_CMD12, 0x00, tmp, 1);     // 发送停止读取指令,例程中没有对停止指令做判断，不知道为什么，实际返回值也不对

    }

    SDCARD_SPI_CS_H();                               // CS引脚拉高
    SdCardSpiSendByte(0xFF);
    return rtn;
}

/**
  *************************************************************************************
  * @brief  SD卡发送数据
  * @param[in] buff:  数据缓冲区
  * @param[in] token: 令牌
  * @retval 结果
  *         @arg @b ERROR   失败
  *         @arg @b SUCCESS 成功 
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus SdCardSendDataBlock(const uint8_t *buff, uint8_t token)
{
    uint8_t value;

	if (SdCardWaitReady() != SUCCESS)
	{
		return ERROR;
	}		
    
    value = SdCardSpiSendByte(token);

    if(token == SDCARD_CMD25_ST_TOKEN)
	{
		return SUCCESS;    // 如果是停止令牌，直接返回
	}		
    
    value = 0;
    do
    {
        SdCardSpiSendByte(*buff++);
        SdCardSpiSendByte(*buff++);
    } while(--value);    

    value = SdCardSpiSendByte(0xFF);
    value = SdCardSpiSendByte(0xFF);
    
    value = SdCardSpiSendByte(0xFF);   // 实际应该是在此返回状态，测试结果是有时在此返回
    if((value&0x1F)==SDCARD_DATA_RESP_ACCEPTED)
    {
        return SUCCESS;
    }

    return ERROR;
}

//==================================================================================================
//| 函数名称 | MMC_blockwrite 
//|----------|--------------------------------------------------------------------------------------
//| 函数功能 | 写数据
//|----------|--------------------------------------------------------------------------------------
//| 输入参数 | buff:数据缓冲区
//|          | sector:开始扇区号
//|          | count:扇区数
//|----------|--------------------------------------------------------------------------------------       
//| 返回参数 | 
//|----------|--------------------------------------------------------------------------------------       
//| 函数设计 | 编写人：    时间：2013-04-17  
//|----------|-------------------------------------------------------------------------------------- 
//|   备注   | 
//|----------|-------------------------------------------------------------------------------------- 
//| 修改记录 | 修改人：          时间：         修改内容： 
//==================================================================================================
ErrorStatus SdCardWriteBlock(const uint8_t *buff, uint32_t sector, uint32_t count)
{
    uint8_t tmp[7];
    ErrorStatus rtn = SUCCESS;

	if (!(SdCardType & SDCARD_TYPE_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

    if(count==1)
    {
        /* 发送CMD24 */
        SdCardSendCmd(SDCARD_CMD24, sector, tmp, 1);       // 发送写单块数据指令,把扇区号转化为字节地址，
        if(tmp[0]==SDCARD_R1_NOERR)
        {// CMD24指令执行正常
            rtn = SdCardSendDataBlock(buff, SDCARD_CMD17_TOKEN);
        }
        else
        {
            rtn = ERROR;
        }
    }
    else
    {
		if (SdCardType & SDCARD_TYPE_SDC)
		{
			SdCardSendCmd(SDCARD_ACMD23, count, tmp, 1);
		}			
        /* 发送CMD25 */
        SdCardSendCmd(SDCARD_CMD25, sector, tmp, 1);       // 发送写多块数据指令,把扇区号转化为字节地址，
        if(tmp[0]==SDCARD_R1_NOERR)
        {// CMD25指令执行正常
            do
            {
                rtn = SdCardSendDataBlock(buff, SDCARD_CMD25_TOKEN);
                if(rtn==ERROR)
                {
                    break;
                }
                buff += 512;
            } while(--count);
            rtn = SdCardSendDataBlock(tmp, SDCARD_CMD25_ST_TOKEN);         // 停止传送
        }
        else
        {
            rtn = ERROR;
        }
    }

    SDCARD_SPI_CS_H();                               // CS引脚拉高

    SdCardSpiSendByte(0xFF);

    return rtn;
}

/**
  *************************************************************************************
  * @brief  SD卡状态控制
  * @param[out] buff:  数据缓冲区
  * @param[in]  sector:开始扇区号
  * @param[in]  count: 扇区数
  * @retval 结果
  *         @arg @b ERROR   失败
  *         @arg @b SUCCESS 成功 
  * @author ZH
  * @date   2014年9月11日
  * @note   用于FATFS文件系统作大容量存储模式时使用
  *************************************************************************************  
  */
ErrorStatus SdCardIoCtrl(uint8_t cmd, uint8_t *buff)
{
    uint8_t tmp[7];
	ErrorStatus res = ERROR;
	uint8_t n, csd[16], *ptr = buff;
	uint16_t csize;

	if (cmd == CTRL_POWER) 
	{
		switch (*ptr) 
		{
			case 0:		/* Sub control code == 0 (POWER_OFF) */
				res = SUCCESS;
				break;
			case 1:		/* Sub control code == 1 (POWER_ON) */
				res = SUCCESS;
				break;
			case 2:		/* Sub control code == 2 (POWER_GET) */
				*(ptr+1) = 1;
				res = SUCCESS;
				break;
			default :
				res = ERROR;
				break;
		}
	}
	else 
	{
		switch (cmd) 
		{
			case CTRL_SYNC :		/* Make sure that no pending write process */
				SDCARD_SPI_CS_L();                                // CS引脚拉低
				if (SdCardWaitReady() != ERROR)
				{
					res = SUCCESS;
				}	
				break;
			case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
				SdCardSendCmd(SDCARD_CMD9, 0, tmp, 1);       // 发送写多块数据指令,把扇区号转化为字节地址，
				if(tmp[0]==SDCARD_R1_NOERR)
				{// CMD9指令执行正常
					if(SdCardRcvDataBlock(csd, 16) != ERROR)
					{
						if ((csd[0] >> 6) == 1) 
						{	/* SDC version 2.00 */
							csize = csd[9] + ((WORD)csd[8] << 8) + 1;
							*(uint32_t *)buff = (uint32_t)csize << 10;
						} 
						else 
						{					/* SDC version 1.XX or MMC*/
							n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
							csize = (csd[8] >> 6) + ((uint32_t)csd[7] << 2) + ((uint32_t)(csd[6] & 3) << 10) + 1;
							*(uint32_t*)buff = (uint32_t)csize << (n - 9);
						}
						res = SUCCESS;
					}
				}
				break;
			case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
				*(uint32_t*)buff = 512;
				res = SUCCESS;
				break;
			case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
				if (SdCardType & SDCARD_TYPE_SD2) 
				{	/* SDC version 2.00 */
					SdCardSendCmd(SDCARD_ACMD13, 0, tmp, 2);       // 发送写多块数据指令,把扇区号转化为字节地址，
					if(tmp[0]==SDCARD_R1_NOERR)
					{// ACMD13指令执行正常
						if(SdCardRcvDataBlock(csd, 16) == SUCCESS)
						{
							for (n = 64 - 16; n; n--)
							{
								SdCardSpiSendByte(0xFF);	/* Purge trailing data */
							}						
							*(uint32_t *)buff = 16UL << (csd[10] >> 4);
							res = SUCCESS;
						}
					}
				} 
				else 
				{					/* SDC version 1.XX or MMC */
					SdCardSendCmd(SDCARD_CMD9, 0, tmp, 1);       // 发送写多块数据指令,把扇区号转化为字节地址，
					if(tmp[0]==SDCARD_R1_NOERR)
					{// CMD13指令执行正常
						if(SdCardRcvDataBlock(csd, 16) == SUCCESS)
						{
							if (SdCardType & SDCARD_TYPE_SD1) 
							{	/* SDC version 1.XX */
								*(uint32_t *)buff = (((csd[10] & 63) << 1) + ((uint32_t)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
							} 
							else 
							{					/* MMC */
								*(uint32_t*)buff = ((uint32_t)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
							}
							res = SUCCESS;
						}
					}
				}
				break;
			case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
				*ptr = SdCardType;
				res = SUCCESS;
				break;
			case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
				SdCardSendCmd(SDCARD_CMD9, 0, tmp, 1);       // 发送写多块数据指令,把扇区号转化为字节地址，
				if(tmp[0] == SDCARD_R1_NOERR)
				{// CMD9指令执行正常
					if(SdCardRcvDataBlock(csd, 16)==SUCCESS)
					{
						res = SUCCESS;
					}
				}
				break;

			case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
				SdCardSendCmd(SDCARD_CMD10, 0, tmp, 1);       // 发送写多块数据指令,把扇区号转化为字节地址，
				if(tmp[0]==SDCARD_R1_NOERR)
				{// CMD10指令执行正常
					if(SdCardRcvDataBlock(csd, 16)== SUCCESS)
					{
						res = SUCCESS;
					}
				}
				break;
			case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
				SdCardSendCmd(SDCARD_CMD58, 0, tmp, 1);       // 发送写多块数据指令,把扇区号转化为字节地址，
				if(tmp[0]==SDCARD_R1_NOERR)
				{// CMD58指令执行正常
					for (n = 4; n; n--) 
					{
						*ptr++ = SdCardSpiSendByte(0xFF);
					}
					res = SUCCESS;
				}
				break;
			case MMC_GET_SDSTAT :	/* Receive SD status as a data block (64 bytes) */
				SdCardSendCmd(SDCARD_ACMD13, 0, tmp, 1);       // 发送写多块数据指令,把扇区号转化为字节地址，
				if(tmp[0]==SDCARD_R1_NOERR)
				{// ACMD13指令执行正常
					SdCardSpiSendByte(0xFF);
					if (SdCardRcvDataBlock(ptr, 64) == SUCCESS)
					{
						res = SUCCESS;
					}	
				}
				break;
			default:
				res = ERROR;
				break;
		}

        SDCARD_SPI_CS_H();                               // CS引脚拉高   
        SdCardSpiSendByte(0xFF);
	}

	return res;
}

/**
  *************************************************************************************
  * @brief  读SD卡的容量
  * @param  无
  * @retval SD卡的容量(字节为单位)，读失败时返回0
  * @author ZH
  * @date   2015年02月05日
  * @note   无
  *************************************************************************************  
  */
uint64_t SdCardGetCapacity(void)
{
	uint8_t tmp[7];
	uint8_t csd[16];
	uint32_t Capacity;
	uint16_t n;
	uint16_t csize; 
	uint64_t SdCardSize = 0;

	SdCardSendCmd(SDCARD_CMD9, 0, tmp, 1);       // 发送写多块数据指令,把扇区号转化为字节地址，
	if(tmp[0]==SDCARD_R1_NOERR)
	{// CMD9指令执行正常
		//读取SD卡CSD寄存器
		//CSD寄存器为128个位，即16个字节。
		//通过检测CSD寄存器的bit126是0还是1来判断。如果是0，即是V1.0版本的；如果是1，即是V2.0版本的
		if(SdCardRcvDataBlock(csd, 16) != ERROR)
		{
			if((csd[0]&0xC0)==0x40)//判断bit126是否为1
			{//V2.0卡的容量的计算
				csize = csd[9] + ((uint32_t)csd[8] << 8) + ((uint32_t)(csd[7] & 63) << 16) + 1;
				Capacity = csize << 9; 
			}
			else
			{//V1.0卡的容量的计算
				n = (csd[5] & 0x0F) + ((csd[10] & 0x80) >> 7) + ((csd[9] & 0x03) << 1) + 2;
				csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 0x03) << 10) + 1;
				Capacity = (uint32_t)csize << (n - 10);
			}
			SdCardSize = (uint64_t)Capacity * 1024UL;//由KB转换成字节		
		}
	}

	return SdCardSize;	
}
/**
  *************************************************************************************
  * @brief  SD卡上电
  * @param  无
  * @retval 上电结果
  *         @arg @b ERROR   失败
  *         @arg @b SUCCESS 成功 
  * @author ZH
  * @date   2014年9月11日
  * @note   无
  *************************************************************************************  
  */
ErrorStatus SdCardPowerOn(void)
{
    uint16_t baud;                                  // 备份SPI时钟频率使用，中间不可更改
    uint8_t tmp[7];
    uint32_t retry;
    uint8_t cdtype = 0x00;
    ErrorStatus rtn = ERROR;

	SdCardSpiConfig();//配置SPI
	
    baud = SdCardSpiGetBaud();
    SdCardSpiSetBaud(SPI_BaudRatePrescaler_256);        // 进入低速模式
    
    SDCARD_SPI_CS_H();                               // CS引脚拉高

    for(retry=0; retry<15; retry++)                 // MMC/SD的DI引脚拉高，并发送>74时钟脉冲
    {
        SdCardSpiSendByte(0xFF);
    }

    SdCardDelay1sCount = SdCardSpiGetFrequency() >> 3;  // 延时1s需要发送的字节数    
    
    /* 选中SD卡 */
    SDCARD_SPI_CS_L();                                // CS拉低

    SdCardSendCmd(SDCARD_CMD0, 0x00, tmp, 1);       // 发送指令0,并接收R1,复位进入IDLE模式

    if(tmp[0]==0x01)
    {// 指令0执行成功
        SdCardSendCmd(SDCARD_CMD8, 0x1AA, tmp, 5);  // 发送指令8,检查设备电压
        if(tmp[0]==0x01)                       // SDHC
        {
            if((tmp[3]==0x01)&&(tmp[4]==0xAA)) // 设备可以工作在2.7-3.3V
            {//ocr = 0x1AA
                /* 发送ACMD41 */
                retry = SdCardDelay1sCount;
                do
                {// ACMD41是组合指令
                    SdCardSendCmd(SDCARD_ACMD41, 1UL<<30, tmp, 1);
                } while((tmp[0]!=SDCARD_R1_NOERR)&&(--retry));   // R1返回成功
                if(retry)
                {
                    SdCardSendCmd(SDCARD_CMD58, 0x00, tmp, 5);
                    cdtype = (tmp[1] & 0x40) ? SDCARD_TYPE_SD2 | SDCARD_TYPE_BLOCK : SDCARD_TYPE_SD2;
                    rtn = SUCCESS;
                }
            }
        }
        else                                   // SDSC or MMC
        {
            cdtype = SDCARD_TYPE_SD1;
            retry = SdCardDelay1sCount;
            do
            {// ACMD41是组合指令
                SdCardSendCmd(SDCARD_ACMD41, 0x00, tmp, 1);
            } while((tmp[0]!=SDCARD_R1_NOERR)&&(--retry));   // R1返回成功
            if(!retry)
            {// ACMD41指令执行失败，尝试使用CMD1
                cdtype = SDCARD_TYPE_MMC;
                retry = retry;
                do
                {
                    SdCardSendCmd(SDCARD_CMD1, 0x00, tmp, 1);
                } while((tmp[0]!=SDCARD_R1_NOERR)&&(--retry));   // R1返回成功
            }
            if(retry)
            {
                SdCardSendCmd(SDCARD_CMD16, 512, tmp, 1);           // 设置扇区大小为512字节
                if(tmp[0]==0x00)
                {
                    rtn = SUCCESS;
                }
            }
            else
            {
                cdtype = 0x00;
            }
        }
    }

    SdCardType = cdtype;

    SDCARD_SPI_CS_H();                               // CS引脚拉高

    SdCardSpiSendByte(0xFF);

    SdCardSpiSetBaud(baud);                              // 恢复总线速度

    SdCardDelay1sCount = SdCardSpiGetFrequency() >> 3;  // 延时1s需要发送的字节数

    return rtn;
}

/**
 *@} SdCard_Functions
 */
/**
 *@} SdCard
 */
/**
 *@} PowerBoard
 */  
