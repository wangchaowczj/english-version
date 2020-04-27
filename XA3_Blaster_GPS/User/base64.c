#include "base64.h"


const unsigned char TableOfBase64[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" ;



/**************************************************************************************************************
*Base64���뷽��:																							  
*��һ���ַ�ͨ������2λ��õ�һ��Ŀ���ַ���Base64��λ�ã����������ֵȡ��������Ӧ���ַ������ǵ�һ��Ŀ���ַ��� 
*Ȼ�󽫵�һ���ַ�����4λ���ϵڶ����ַ�����4λ������õڶ���Ŀ���ַ���										  
*�ٽ��ڶ����ַ�����2λ���ϵ������ַ�����6λ����õ�����Ŀ���ַ���										      
*���ȡ�������ַ�����6λ����õ��ĸ�Ŀ���ַ���																  
*ԭ���ֽ���������3��������ʱ,�����ĵط���ȫ0�����㣬ת��ʱBase64������=�������档							  
*�����ϵ�ÿһ������֮���ٰѽ���� 0x3F ���� AND λ�������Ϳ��Եõ��������ַ��ˡ�						  
**************************************************************************************************************/
unsigned int Base64Encode(unsigned char *ps, unsigned char *pd, unsigned int length)
{
	unsigned int i, encodetime, lastlength ;
	
	if(length == 0)
	{
		return 0;
	}
	encodetime = length/3;
	lastlength = length%3;
	for(i = 0;i<encodetime;i++)	
	{
		*pd = TableOfBase64[((*ps)>>2) & 0x3f];
		*(pd + 1) = TableOfBase64[(((*ps)<<4) + ((*(ps+1))>>4)) & 0x3f];
		*(pd + 2) = TableOfBase64[(((*(ps+1))<<2) + ((*(ps+2))>>6)) & 0x3f];
		*(pd + 3) = TableOfBase64[(*(ps+2)) & 0x3f];			
		pd += 4;
		ps += 3;		
	}
	
	if(lastlength==1)
	{
		*pd = TableOfBase64[((*ps)>>2) & 0x3f];
		*(pd + 1) = TableOfBase64[((*ps)<<4) & 0x3f];
		*(pd + 2) = '=';
		*(pd + 3) = '=';	
	}
	else if(lastlength==2)
	{
		*pd = TableOfBase64[((*ps)>>2) & 0x3f];
		*(pd + 1) = TableOfBase64[(((*ps)<<4) + ((*(ps+1))>>4)) & 0x3f];
		*(pd + 2) = TableOfBase64[((*(ps+1))<<2) & 0x3f];
		*(pd + 3) = '=';	
	}
	
	return encodetime*4 + ((length%3)==0?0:4);
}

static unsigned char search_index(unsigned char asc_dat)
{
    unsigned char index ;

	for(index=0; index<64; index++)
	{
	    if(asc_dat==TableOfBase64[index])
		{
		    return index ;
		}
	}

	return 0xFF ;
}


/******************************************************
*�������ܣ�BASE64 ����ʵ��                          
*���������*ps: Դ���ַ��*pd: �����ַ��length :��4�������ַ�			
*����ֵ��	��												
******************************************************/
unsigned char Base64Decode(unsigned char *ps, unsigned char *pd, unsigned int length)
{
    unsigned int i, encodetime, lastlength ;
	unsigned char value;

	if(length%4)
	{
		return 1;
	}
	
	if((*(ps+length-1))=='=')
	{
	    length = length - 1 ;
	}
	if((*(ps+length-2))=='=')
	{
	    length = length - 1 ;
	}
	encodetime = length / 4 ;
	lastlength = length % 4 ;

	for(i=0;i<length;i++)		// ��Դ�����������ֵת��Ϊ����ֵ
	{
		value = search_index(*(ps+i)) ;
		if(value == 0xFF)
		{
			return 1;
		}
		*(ps+i) = value ;	
	}
	
	for(i = 0;i<encodetime;i++)	
	{
		*pd = (*ps<<2) | (*(ps+1)>>4) ;
		*(pd + 1) = (*(ps+1)<<4) | (*(ps+2)>>2) ;
		*(pd + 2) = (*(ps+2)<<6) | *(ps+3) ;		
		pd += 3 ;
		ps += 4 ;		
	}

	if(lastlength==2)
	{
		*pd = (*ps<<2) | (*(ps+1)>>4) ;
	}
	else if(lastlength==3)
	{
		*pd = (*ps<<2) | (*(ps+1)>>4) ;
		*(pd + 1) = (*(ps+1)<<4) | (*(ps+2)>>2) ;	
	}
	else
	{
	    
	}
	return 0;
}

