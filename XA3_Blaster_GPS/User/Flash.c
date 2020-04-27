/**
  *************************************************************************************
  * @file    Flash.c
  * @author  ZH
  * @version V1.0.0
  * @date    2014��9��10��
  * @brief   Flashģ��Դ�ļ�
  *************************************************************************************
  */
#include "includes.h"
/**@defgroup Dkey
  *@{
  */
/**@defgroup Flash
  *@brief ��ʱģ��
  *@{
  */

/**@defgroup Flash_Functions
  *@brief ����
  *@{
  */
/**
  *************************************************************************************
  * @brief  Flash��������
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��10��29��
  * @note   ��
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
	
//	/* ����SPI3�������� */	
//		/*--JTAG-DP��ֹ,SW-DP����, PA13, PA14���ڵ��Է��档PA15, PB3, PB4������SPI3--*/ 
//	AFIO->MAPR &= 0x00FFFFFF;
//	AFIO->MAPR |=0x02000000;
	/***�ܽ�ӳ��***/  
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
  * @brief  Flash SPI����
  * @param  ��
  * @retval ��
  * @author ZH
  * @date   2014��10��29��
  * @note   ��
  *************************************************************************************  
  */
void FlashSPIConfig(void)
{
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB1PeriphClockCmd(FlashSPI_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	FLASH_CS_H();
    FLASH_WP_H();
	
	// SPI1����
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
	
	//ʹ��SPI3
	SPI_Cmd(FlashSPI, ENABLE);
}

/**
  *************************************************************************************
  * @brief  Flash ���Ͳ�����1�ֽ�����
  * @param  byte ���͵�����
  * @retval ���յ�������
  * @author ZH
  * @date   2014��10��29��
  * @note   ��
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
  * @brief  ��Flash����ָ������ݣ������շ�������
  * @param  md:��Ҫִ�е�ָ��
  * @param  CmdLength:��Ҫִ�е�ָ���С 
  * @param  dir:ָ��ִ�з���ָ���Ƕ����ݻ���д����
  *         @rag @b AT45DBX_READ: ������
  *         @rag @b AT45DBX_READ: д����  
  * @param  Data:������ݵĻ�����
  * @param  bytes:�����ֽ���  
  * @retval ��
  * @author ZH
  * @date   2014��10��29��
  * @note   ��
  *************************************************************************************  
  */
void FlashTransmmitData(u8 *Cmd, u8 CmdLength, AT45DBXDIR dir, u8* Data, u16 bytes)
{
    u16 i = 0;
    
    FLASH_CS_L();         // ѡ��AT45DBXоƬ
    FLASH_WP_H();         // ȡ��д����

    for(i=0; i<CmdLength; i++)
    {// ����ָ��
        FlashSendByte(*(Cmd+i));
    }

    if(Data!=NULL)
    {// ����л����������ͻ��������
        if(dir==AT45DBX_READ)
        {// ��ȡ����
            for(i=0; i<bytes; i++)
            {
                *Data++=FlashSendByte(0x00);
            }
        }
        else if(dir==AT45DBX_WRITE)
        {// д������
            for(i=0; i<bytes; i++)
            {
                FlashSendByte(*Data++);
            }
        }
    }

    FLASH_CS_H();      // ȡ��ѡ��AT45DBXоƬ
    FLASH_WP_L();       // д����
}

/**
  *************************************************************************************
  * @brief  Flash���ں���
  * @param  t����ʱ������ 
  * @retval ��
  * @author ZH
  * @date   2014��10��29��
  * @note   ��
  *************************************************************************************  
  */
static void FlashDelay(uint32_t t)
{
    uint32_t  i;
    uint32_t _1us_count = 0;
    RCC_ClocksTypeDef rcc;

    RCC_GetClocksFreq(&rcc);   // ���ϵͳʱ��Ƶ��

    _1us_count = rcc.SYSCLK_Frequency/4000;   /* 1ms��Ҫ��ʱ���� = ϵͳƵ�� / 1000(1ms��ʱ����) / 4(forѭ��ռ�õ�ָ����) */
    _1us_count = t * _1us_count;
    for(i = 0; i < _1us_count; i++)
    {
        ;
    }
}

/**
  *************************************************************************************
  * @brief  FlashӲ����λ
  * @param  �� 
  * @retval ��
  * @author ZH
  * @date   2014��10��29��
  * @note   ��
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
  * @brief  ��ȡFlash״̬�Ĵ���1
  * @param  �� 
  * @retval �Ĵ���1��ֵ
  * @author ZH
  * @date   2014��10��29��
  * @note   ��
  *************************************************************************************  
  */
static u8 FlashStatusRegister1(void)
{
	u8 status;
    u8 cmd[] = {DFCMD_READ_STATUS/*��״ָ̬��*/};
	
    FlashTransmmitData(cmd, sizeof(cmd), AT45DBX_READ, &status, 1);   // ��FLASH����ָ����淵������
	
	return status;
} /* flash_at45dbx_status */

/**
  *************************************************************************************
  * @brief  �ȴ�Flash�������
  * @param  �� 
  * @retval ErrorStatus �ȴ����
  *         @arg @b SUCCESS �����Ѿ����
  *         @arg @b ERROR ����û�����
  * @author ZH
  * @date   2014��10��29��
  * @note   ��
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
		{// �����æ����Ϊ�����ɹ�
		    return SUCCESS;
		}		
	}
    return ERROR;   // ���ݲ���ʧ��
}


/**
  *************************************************************************************
  * @brief  FlashӲ����ʼ��
  * @param  �� 
  * @retval ErrorStatus �ȴ����
  *         @arg @b SUCCESS ��ʼ���ɹ�
  *         @arg @b ERROR ��ʼ��ʧ��
  * @author ZH
  * @date   2014��10��29��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus FlashHardwareInit(void)
{
    u8 Status;
    
	FlashGpioConfig();
	FlashSPIConfig();
    FlashHardwareRest();                       // ��λat45dbxоƬ

    Status = FlashStatusRegister1();           // ��ȡat45dbx��ID

	/*ͬʱ�ж�0��1����Ч���ⷵ��ֵΪȫ0ȫ1�쳣������� */
	if((Status&0xBC) == 0xBC)
	{//�����ֲ�28ҳ״̬�Ĵ���1˵����bit7 Ϊ1��ʾ���У�bit5-2��Ϊ1
		if((Status&0x02) == 0x00)
		{//Sector Protection Status Ϊ0��ʾû�д򿪱�����ǰ�߸ո�λоƬ��û�д򿪱���
			return SUCCESS;//
		}
	}
    return ERROR;
}

/**
  *************************************************************************************
  * @brief  Flash�����
  * @param  Block ���ַ 
  * @retval ErrorStatus �������
  *         @arg @b SUCCESS �����ɹ�
  *         @arg @b ERROR ����ʧ��
  * @author ZH
  * @date   2014��10��29��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus FlashEraseBlock(u16 Block)
{
    u8 cmd[] = {DFCMD_BLOCK_ERASE/*��״ָ̬��*/, 0x00, 0x00, 0x00};
	u16 Page;
	
    Page = Block<<3;  // �����Ϊ��ʼҳ��ַ��Ϊ��ҳ��������һ��
	Page <<= 1;//PA14 - PA3��ʾҳ��ַ	
    cmd[1] = (u8)(Page >> 8);
    cmd[2] = (u8)Page;
	cmd[3] = 0x00;
    FlashTransmmitData(cmd, sizeof(cmd), AT45DBX_READ, NULL, 0);   // ��FLASH����ָ����淵������

    return FlashWaitReady();
}

/**
  *************************************************************************************
  * @brief  Flash��������оƬ
  * @param  Block ���ַ 
  * @retval ErrorStatus �������
  *         @arg @b SUCCESS �����ɹ�
  *         @arg @b ERROR ����ʧ��
  * @author ZH
  * @date   2014��10��29��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus FlashEraseChip(void)
{
    u8 cmd[] = {DFCMD_CHIP_ERASE, 0x94, 0x80, 0x9A};
	u16 i;
    FlashTransmmitData(cmd, sizeof(cmd), AT45DBX_READ, NULL, 0);   // ��FLASH����ָ����淵������

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
  * @brief  Flashִ��ҳָ��
  * @param  code ��Ҫִ�е�ָ�����
  * @param  page ҳ��ַ  
  * @retval ErrorStatus ָ����
  *         @arg @b SUCCESS ָ��ɹ�
  *         @arg @b ERROR ָ��ʧ��
  * @author ZH
  * @date   2014��10��29��
  * @note   ��
  *************************************************************************************  
  */
static ErrorStatus FlashPageCmd(u8 code, u16 page)
{
    u8 cmd[4];
	u16 temp;
     
    temp = 	page<<1;
	
    cmd[0] = code;// ��Ҫ���͵�ҳָ��
    cmd[1] = (u8)(temp >> 8);// ҳ��ַ(PA14 - PA0��
    cmd[2] = (u8)temp;       // ҳ��ַ+1λ�޹�
    cmd[3] = 0x00;           // �޹�λ

    FlashTransmmitData(cmd, sizeof(cmd), AT45DBX_READ, NULL, 0);   // ��FLASH����ָ����淵������    
    return FlashWaitReady();
}

/**
  *************************************************************************************
  * @brief  ����ָ��ҳ
  * @param  page ҳ��ַ  
  * @retval ErrorStatus �������
  *         @arg @b SUCCESS �����ɹ�
  *         @arg @b ERROR ����ʧ��
  * @author ZH
  * @date   2014��10��29��
  * @note   ��
  *************************************************************************************  
  */
ErrorStatus FlashErasePage(u16 Page) 
{
    return FlashPageCmd(DFCMD_PAGE_ERASE, Page);
}

/**
  *************************************************************************************
  * @brief  ��ָ����ַ��ȡ����
  * @param  addr: ��ַ
  * @param  Data: ������ݵĻ����� 
  * @param  Bytes: ��Ҫ��ȡ�������ֽ���
  * @retval ��
  * @author ZH
  * @date   2014��10��29��
  * @note   ��
  *************************************************************************************  
  */
void FlashReadData(u32 Addr, u8 *Data, u16 Bytes)
{
    u8 cmd[5];
    u16 Page = Addr / FLASH_PAGE_BYTES;
    u16 offset =  Addr % FLASH_PAGE_BYTES;

	Page <<= 1;
	
    cmd[0] = DFCMD_CONT_READ_HF;
    cmd[1] = (u8)(Page >> 8);//ǰ15λ��ʾҳ��ַ����9λ��ʾ������ʼ��ַ
    cmd[2] = (u8)((Page&0xFF) + ((offset>>8)&0x01));
    cmd[3] = (u8)offset;
	cmd[4] = 0x00;
    FlashTransmmitData(cmd, sizeof(cmd), AT45DBX_READ, Data, Bytes);   // ��FLASH����ָ����淵������
}

/**
  *************************************************************************************
  * @brief  д���ݵ�ָ����ַ
  * @param  Addr:��ַ
  * @param  Data:��Ҫд�����ݵĻ����� 
  * @param  Bytes:��Ҫд�����ݵ��ֽ���  
  * @retval ErrorStatus д���ݽ��
  *         @arg @b SUCCESS д���ݳɹ�
  *         @arg @b ERROR д����ʧ��
  * @author ZH
  * @date   2014��10��29��
  * @note   ��
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
		
		// ��ҳ���ݴ�flash�Ƶ�buf1��
        status = FlashPageCmd(DFCMD_MM_PAGE_TO_B1_XFER,cur_page);           
        if(status == ERROR)                                          
		{// ָ����ִ���
			break;  
		}
		
        //�޸�buf1�е�����
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
        Data += send_len; // ������������ָ��

		// ��buf1�е������Ƶ�cur_pageҳ
        status = FlashPageCmd(DFCMD_BUF1_FLASH, cur_page);                  
        if(status==ERROR)
		{// ָ����ִ���
			break;                                           
		}		
		status = FlashPageCmd(DFCMD_AUTO_REW_B1,cur_page);
        if(status == ERROR) 
		{// ָ����ִ���
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

