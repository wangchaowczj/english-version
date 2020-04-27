#include "includes.h"

/**
  *************************************************************************************
  * @brief  �ж�оƬ�����Ƿ���Ҫ�޸�
  * @param  Serl_2 ������2 
  * @param  Serl_3 ������3   
  * @retval ���
  *      @arg 0 ����Ҫ�޸�
  *      @arg ���� ��Ҫ�޸�
  * @author ZH
  * @date   2018��3��30��
  * @note   ֻ����һС����
  *************************************************************************************  
  */
u8 DetCodeNeedPepairByNotComplete(u8 Serl_2, u8 Serl_3)
{
	if(Serl_2 == 0x5A)
	{
		return 1;
	}
	if(Serl_2 == 0xB4)
	{
		return 2;
	}	
	if((Serl_2 == 0x14) && (Serl_3 == 0x00))
	{
		return 3;
	}
	return 0;
}
/**
  *************************************************************************************
  * @brief  �ж�оƬ�����Ƿ���Ҫ�޸�
  * @param  DetInfo оƬ��������  
  * @retval ���
  *      @arg 0 ����Ҫ�޸�
  *      @arg 1 ��Ҫ�޸�
  * @author ZH
  * @date   2018��3��24��
  * @note   
  *************************************************************************************  
  */
u8 DetCodeNeedPepair(u8 Serl_2)
{
	u8 Value;

	/*
	SERL2[0] ==0 ��λ���ù̶�Ϊ0��
	Serl2[7:1]    != 7��b1000010  ��©
				  != 7��b0001010  ��λ
				  != 7��b0010010  ������ض�
				  != 7��b1111010  ׼����
				  != 7��b1011010  BIT
				  != 7��b1101010  ���
				  != 7��b0110010  ����
				  != 7��b1011010  EE����
				  != 7��b0101010  checkosc
				  != 7��b1010010  ʱ��ض�
				  != 7��b1111010  ״̬�ض�
	*/
	
	if(Serl_2&0x01)//������2��bit0λҪ��̶�Ϊ0
	{
		return 1;
	}
	Value = Serl_2>>1;
	switch(Value)
	{
		case 0x42://7��b1000010  ��©
		case 0x0A://7��b0001010  ��λ
		case 0x12://7��b0010010  ������ض�
		case 0x7A://7��b1111010  ׼����
		case 0x5A://7��b1011010  BIT
		case 0x6A://7��b1101010  ���
		case 0x32://7��b0110010  ����
//		case 0x5A://7��b1011010  EE����
		case 0x2A://7��b0101010  checkosc
		case 0x52://7��b1010010  ʱ��ض�
//		case 0x7A://7��b1111010  ״̬�ض�
			return 1;
//			break;
		default:
			break;
	}		
	return 0;
}


/**
  *************************************************************************************
  * @brief  �ж�оƬ�����Ƿ���Ҫ�޸�
  * @param  DetInfo оƬ��������  
  * @retval ���
  *      @arg 0 ����Ҫ�޸�
  *      @arg 1 ��Ҫ�޸�
  * @author ZH
  * @date   2018��3��24��
  * @note   
  *************************************************************************************  
  */
u8 DetPwdNeedPepair(DET_INFO *DetInfo)
{
	u8 Value;
	u16 DetPwd;

	/*
	Code1[0] =0; ��λ�̶�����Ϊ0
	Code1[7:1]  != 7��b1011010  BIT
	!= 7��b1101010  ���
	Code2[7:1] != 7��b0000000�� ����2
	*/	
	DetPwd = GetDetPwd(DetInfo->Serl_1, DetInfo->Serl_2, DetInfo->Serl_3,
		DetInfo->Factor, DetInfo->bat, DetInfo->year);	
	
	Value = DetPwd&0xFF;	
	if(Value&0x01)//����1��bit0λҪ��̶�Ϊ0
	{
		return 1;
	}	
	Value = Value>>1;//Code1[7:1]
	switch(Value)
	{
		case 0x5A://7��b1011010  BIT
		case 0x6A://7��b1101010  ���
			return 1;
		default:
			break;
	}	
	Value = DetPwd>>9;//Code2[7:1]
	switch(Value)
	{
		case 0x00://7��b0000000�� ����2
			return 1;
		default:
			break;
	}		
	return 0;	
}


/**
  *************************************************************************************
  * @brief  �޸�оƬ��ʱʱ��ֵ
  * @param  DelayClk ���޸�ǰ��ֵ  
  * @retval �޸�����ֵ
  * @author ZH
  * @date   2018��3��24��
  * @note   Ҫ��T_L [0] =0���������ʱ���м�8λT_M[15:8] ==8��h00ʱ��Ҫ��T_L[1] == 1��b1
  *************************************************************************************  
  */
u32 DetDelayClkPepair(u32 DelayClk)
{
	if(DelayClk&0x01)//Ҫ�����λ����Ϊ0
	{
		DelayClk &= 0xFFFFFFFE;
	}
	if(((DelayClk>>8)&0xFF) == 0)
	{
		DelayClk |= 0x02;
	}

	return DelayClk;
}

/**
  *************************************************************************************
  * @brief  �����Ƿ�ʹ�ù��˹���
  * @param  �� 
  * @retval ���
  *      @arg 0 ����Ҫ�޸�
  *      @arg 1 ��Ҫ�޸�
  * @date   2018��3��30��
  * @note   
  *************************************************************************************  
  */
u8 BlasterNeedPepair(void)
{
	//������������ʹ��������ǹ���
	if(memcmp(device_info.mac, "F34", 3) == 0)
	{
		return 0;
	}
	else if(memcmp(device_info.mac, "f34", 3) == 0)
	{
		return 0;
	}
	return 1;
}




