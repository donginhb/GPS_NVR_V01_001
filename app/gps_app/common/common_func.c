#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gps_typedef.h"



const unsigned char	tbl_hex_to_assic[] = "0123456789ABCDEF"; 	// 0x0-0xf���ַ����ұ�
const unsigned char tbl_assic_to_hex[24] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };


/*********************************************************************************
*��������:unsigned int Hex_To_Ascii( unsigned char* pDst, const unsigned char* pSrc, unsigned int nSrcLength )
*��������:��hex����ת��Ϊ���ڴ��д洢��16����ASSIC���ַ���
*��    ��:	pDst	:�洢ת��ASSIC���ַ����Ľ��
			pSrc	:ԭʼ����
			nSrcLength	:pSrc����
*��    ��:unsigned int �����ݣ���ʾת����ASSIC��pDst�ĳ���
*�� �� ֵ:	
*��    ��:������
*��������:2013-2-19
*---------------------------------------------------------------------------------
*�� �� ��:
*�޸�����:
*�޸�����:
*********************************************************************************/
unsigned int Hex_To_Ascii( unsigned char* pDst, const unsigned char* pSrc, unsigned int nSrcLength )
{
	unsigned int			i;

	for( i = 0; i < nSrcLength; i++ )
	{
		// �����4λ
		*pDst++ = tbl_hex_to_assic[*pSrc >> 4];

		// �����4λ
		*pDst++ = tbl_hex_to_assic[*pSrc & 0x0f];

		pSrc++;
	}

	// ����ַ����Ӹ�������
	*pDst = '\0';

	// ����Ŀ���ַ�������
	return ( nSrcLength << 1 );
}


/*********************************************************************************
*��������:unsigned int Ascii_To_Hex(unsigned char *dest_buf,char * src_buf,unsigned int max_rx_len)
*��������:��16����ASSIC���ַ���ת��Ϊ���ڴ��д洢��hex����
*��    ��:	dest_buf:�洢ת���Ľ��
			src_buf	:ASSIC���ַ���
			max_rx_len		:src_dest�����Խ��յĳ���
*��    ��:unsigned int �����ݣ���ʾת����src_dest�ĳ���
*�� �� ֵ:	
*��    ��:������
*��������:2013-2-19
*---------------------------------------------------------------------------------
*�� �� ��:
*�޸�����:
*�޸�����:
*********************************************************************************/
unsigned int Ascii_To_Hex(unsigned char *dest_buf,char * src_buf,unsigned int max_rx_len)
{
 char		c;
 char 		*p;
 unsigned int i,infolen;

 if((unsigned long)dest_buf == 0)
 	return 0;
 infolen = strlen(src_buf)/2;
 p = src_buf;
 for( i = 0; i < infolen; i++ )
 {
 	c		= tbl_assic_to_hex[*p++ - '0'] << 4;
 	c		|= tbl_assic_to_hex[*p++ - '0'];
 	dest_buf[i] = c;
	if(i>=max_rx_len)
		break;
 }
 return i;
}





/*********************************************************************************
*��������:unsigned long AssicBufToUL(char * buf,unsigned int num)
*��������:��10����ASSIC���ַ�����ָ����󳤶�Ϊnum���ַ���תΪunsigned long������
*��    ��:	* buf	:ASSIC���ַ���
			num		:�ַ�������󳤶�
*��    ��:unsigned long ������
*�� �� ֵ:	
*��    ��:������
*��������:2013-2-19
*---------------------------------------------------------------------------------
*�� �� ��:
*�޸�����:
*�޸�����:
*********************************************************************************/
unsigned long AssicBufToUL(char * buf,unsigned int num)
{
 unsigned char tempChar;
 unsigned int i;
 unsigned long retLong=0;
 
 for(i=0;i<num;i++)
 	{
 	tempChar=(unsigned char)buf[i];
	if((tempChar>='0')&&(tempChar<='9'))
		{
	 	retLong*=10;
		retLong+=tempChar-'0';
		}
	else
		{
		return retLong;
		}
 	}
 return retLong;
}

/*********************************************************************************
*��������:void printf_hex_data( const u8* pSrc, u16 nSrcLength )
*��������:��hex����ת��Ϊ���ڴ��д洢��16����ASSIC���ַ���Ȼ���ӡ����
*��    ��:	pSrc	:ԭʼ����
			nSrcLength	:pSrc����
*��    ��:none
*�� �� ֵ:	
*��    ��:������
*��������:2013-2-19
*---------------------------------------------------------------------------------
*�� �� ��:
*�޸�����:
*�޸�����:
*********************************************************************************/
void printf_hex_data( const unsigned char* pSrc, unsigned int nSrcLength )
{
 	char 			pDst[3];
	unsigned int	i;

	
	pDst[2]  = 0;
	for( i = 0; i < nSrcLength; i++ )
	{
		// �����4λ
		pDst[0] = tbl_hex_to_assic[*pSrc >> 4];

		// �����4λ
		pDst[1] = tbl_hex_to_assic[*pSrc & 0x0f];

		pSrc++;

		printf(pDst);
	}
}



/*********************************************************************************
  *��������:unsigned char sample_month_day(unsigned char uc_month,unsigned char uc_leapyear)
  *��������:�ú�������Ϊ���㵱�µ�����
  *��	��: uc_month	:Ҫ���м�����·�
  			uc_leapyear	:�Ƿ�Ϊ���꣬��Ϊ1��������Ϊ0
  *��	��: none
  *�� �� ֵ:���������������
  *---------------------------------------------------------------------------------
  *�޸���    �޸�ʱ��   �޸�����
  *������   2014-06-08  ��������
*********************************************************************************/
unsigned char Get_Month_Day(unsigned char month,unsigned char leapyear)
{
	unsigned char day;
	switch(month)
	{
		case 12 :
		{
	 		day=31;
			break;
		}
		case 11 :
		{
			day=30;
			break;
		}
		case 10 :
		{
			day=31;
			break;
		}
		case 9 :
		{
			day=30;
			break;
		}
		case 8 :
		{
			day=31;
			break;
		}
		case 7 :
		{
			day=31;
			break;
		}
		case 6 :
		{
			day=30;
			break;
		}
		case 5 :
		{
			day=31;
			break;
		}
		case 4 :
		{
			day=30;
			break;
		}
		case 3 :
		{
			day=31;
			break;
		}
		case 2 :
		{
			day=28;
			day+=leapyear;	
			break;
		}
		case 1 :
		{
			day=31;
			break;
		}
		default :
		{
			break;
		}
	}
	return day;
}



/*********************************************************************************
  *��������:void strtrim( unsigned char* s, unsigned char c )
  *��������:�ú�������Ϊȥ���ַ����е�ɾ����(0x08)�������������Ч�ַ��ƶ���ǰ�档
  *��	��: s		:Ҫ�޸ĵ��ַ�����ע�⣬���ַ��������ܱ��޸�
  *��	��: s		:�޸���ɵ��ַ���
  *�� �� ֵ:none
  *---------------------------------------------------------------------------------
  *�޸���    �޸�ʱ��   �޸�����
  *������   2015-06-18  ��������
*********************************************************************************/
void strproc( unsigned char* s )
{
	int 	i, j, len;

	if ( s == 0 )
	{
		return;
	}
	if ( *s == 0 )
	{
		return;
	}
	
	len = strlen( (const char*)s );
	for ( i = 0,j=0; i < len; i++ )
	{
		if ( s[i] == 0x08 )
		{
			if(j)
				j--;
		}
		else
		{
			s[j++] = s[i];
		}
		
	}
	s[j] = 0;
}



/*********************************************************************************
  *��������:void strtrim( unsigned char* s, unsigned char c )
  *��������:�ú�������Ϊȥ���ַ���s��ǰ��Ϊc���ַ������CΪ0���ʾɾ��ǰ��Ĳ��ɼ��ַ�
  *��	��: s		:Ҫ�޸ĵ��ַ�����ע�⣬���ַ��������ܱ��޸�
  			c		:�����ַ�
  *��	��: s		:�޸���ɵ��ַ���
  *�� �� ֵ:none
  *---------------------------------------------------------------------------------
  *�޸���    �޸�ʱ��   �޸�����
  *������   2015-06-18  ��������
*********************************************************************************/
void strtrim( unsigned char* s, unsigned char c )
{
	unsigned char	i, j, * p1, * p2;

	if ( s == 0 )
	{
		return;
	}

	// delete the trailing characters
	if ( *s == 0 )
	{
		return;
	}
	
	j = strlen( (const char*)s );
	p1 = s + j;
	for ( i = 0; i < j; i++ )
	{
		p1--;
		if( c == 0 )
		{
			if ( *p1 > 0x20 )
			{
				break;
			}
		}
		else
		{
			if ( *p1 != c )
			{
				break;
			}
		}
	}
	if ( i < j )
	{
		p1++;
	}
	*p1 = 0;	// null terminate the undesired trailing characters

	// delete the leading characters
	p1 = s;
	if ( *p1 == 0 )
	{
		return;
	}
	if( c == 0 )
	{
		for ( i = 0; *p1++ <= 0x20; i++ )
		{
			;
		}
	}
	else
	{
		for ( i = 0; *p1++ == c; i++ )
		{
			;
		}
	}
	if ( i > 0 )
	{
		p2 = s;
		p1--;
		for ( ; *p1 != 0; )
		{
			*p2++ = *p1++;
		}
		*p2 = 0;
	}
}



/*********************************************************************************
  *��������:void strtrim( unsigned char* s, unsigned char c )
  *��������:�ú�������Ϊ��ȡϵͳtickֵ����λΪ10ms����ϵͳ������ʼһֱ�ۼ�
  *��	��: none
  *��	��: none
  *�� �� ֵ:ϵͳtickֵ
  *---------------------------------------------------------------------------------
  *�޸���    �޸�ʱ��   �޸�����
  *������   2015-06-18  ��������
*********************************************************************************/
unsigned long tick_get( void )
{
	return 0;
}




/*********************************************************************************
  *��������:uint16_t data_to_buf( uint8_t * pdest, uint32_t data, uint8_t width )
  *��������:����ͬ���͵����ݴ���buf�У�������buf��Ϊ���ģʽ
  *��	��:	pdest:  ������ݵ�buffer
   data:	������ݵ�ԭʼ����
   width:	��ŵ�ԭʼ����ռ�õ�buf�ֽ���
  *��	��:
  *�� �� ֵ:������ֽ���
  *��	��:������
  *��������:2013-06-5
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
uint16_t data_to_buf( uint8_t * pdest, uint32_t data, uint8_t width )
{
	uint8_t *buf;
	buf = pdest;

	switch( width )
	{
		case 1:
			*buf++ = data & 0xff;
			break;
		case 2:
			*buf++	= data >> 8;
			*buf++	= data & 0xff;
			break;
		case 4:
			*buf++	= data >> 24;
			*buf++	= data >> 16;
			*buf++	= data >> 8;
			*buf++	= data & 0xff;
			break;
	}
	return width;
}

/*********************************************************************************
  *��������:uint16_t buf_to_data( uint8_t * psrc, uint8_t width )
  *��������:����ͬ���͵����ݴ�buf��ȡ������������buf��Ϊ���ģʽ
  *��	��:	psrc:   ������ݵ�buffer
   width:	��ŵ�ԭʼ����ռ�õ�buf�ֽ���
  *��	��:	 none
  *�� �� ֵ:uint32_t ���ش洢������
  *��	��:������
  *��������:2013-06-5
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
uint32_t buf_to_data( uint8_t * psrc, uint8_t width )
{
	uint8_t		i;
	uint32_t	outData = 0;

	for( i = 0; i < width; i++ )
	{
		outData <<= 8;
		outData += *psrc++;
	}
	return outData;
}

uint8_t HEX2BCD( uint8_t x )
{
return ( ( ( x ) / 10 ) << 4 | ( ( x ) % 10 ) );
}


uint8_t BCD2HEX( uint8_t x )
{
return ( ( ( ( x ) >> 4 ) * 10 ) + ( ( x ) & 0x0f ) );
}


/************************************************************
 * @file
 * @brief: app use it call the shell command 
 * @cmd  :shell command
 * @author wxg
 * @date 2015-06-12
 * @version 0.1
 * @return if this thread sucess ,retun $?
 */

int my_system(const char * cmd) 
{
#if 0
	FILE * fp;
	char *result_buf;
	extern int errno;
	int res,result;
	result_buf =NULL;
	//char buf[1024]; 
	if (cmd == NULL) 
	{ 
		JT808_PRT("my_system cmd is NULL!\n");
	 	return -1;
	} 
	if ((fp = popen(cmd, "r") ) == NULL) 
	{ 
		perror("popen");
	 	JT808_PRT("popen error: %s/n", strerror(errno)); 
		return -1; 
	} 
	else
	{
		JT808_PRT("���%s��\r\n", cmd);
		result_buf = (char*)malloc( sizeof(char)*4096 );
		if(result_buf ==NULL)
		{
			return 0;
		}
	    while(fgets(result_buf, sizeof(result_buf), fp) != NULL)
	    {
	        printf("%s", result_buf);
	    }
	}
	if(result_buf !=NULL)
	{
		free(result_buf);
	}
	if ( (res = pclose(fp)) == -1) 
	{ 
		JT808_PRT("close popen file pointer fp error!\n"); 
		return res;
	}  
	else 
	{ 
		result =WEXITSTATUS(res); 
		JT808_PRT("popen res is :%d %d\n", res,result); 
		return result; 
	} 
#endif
}


void outprint_hex(uint8_t * descrip, char *instr, uint16_t inlen )
{
	uint32_t  i=0;
	uint8_t *THstr = NULL;
	THstr=(uint8_t*)malloc(sizeof(uint8_t)*inlen+1);
	memcpy(THstr,instr,inlen);
	printf("\n%s: (%d)>>%x\n",descrip,inlen,now_time());
	for( i=0;i<inlen;i++)
	{
	printf("%02X ",THstr[i]);
	if((i+1)%16==0)
	{
	printf("\n");
	}
	}

	printf("\n");
	free(THstr);
}

