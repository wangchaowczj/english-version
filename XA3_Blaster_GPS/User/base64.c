#include "base64.h"


const unsigned char TableOfBase64[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" ;



/**************************************************************************************************************
*Base64编码方法:																							  
*第一个字符通过右移2位获得第一个目标字符的Base64表位置，根据这个数值取到表上相应的字符，就是第一个目标字符。 
*然后将第一个字符左移4位加上第二个字符右移4位，即获得第二个目标字符。										  
*再将第二个字符左移2位加上第三个字符右移6位，获得第三个目标字符。										      
*最后取第三个字符的右6位即获得第四个目标字符。																  
*原文字节数量不是3的整倍数时,不够的地方用全0来补足，转换时Base64编码用=号来代替。							  
*在以上的每一个步骤之后，再把结果与 0x3F 进行 AND 位操作，就可以得到编码后的字符了。						  
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
*函数功能：BASE64 解码实现                          
*输入参数：*ps: 源码地址，*pd: 解码地址，length :非4整数倍字符			
*返回值：	无												
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

	for(i=0;i<length;i++)		// 将源码数据由码表值转换为索引值
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

