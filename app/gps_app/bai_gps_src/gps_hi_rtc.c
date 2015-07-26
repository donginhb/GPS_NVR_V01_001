/*********************************************************************************
  * @�ļ����� :gps_hi_rtc.c
  * @�������� :���к�RTC��صĺ�����������
  * @��	   �� :������
  * @�������� :2015-7-15
  * @�ļ��汾 :0.01
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-15	�����ļ�
*********************************************************************************/
#ifdef __cplusplus
#if __cplusplus
	extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>

#include "gps_typedef.h"
#include "common_func.h"
#include "gps_hi_rtc.h"


rtc_time_t rtc;
static int s_rtc_fd =  0;


/***********************************************************
* Function:
* Description:
* Input:
* Input:
* Output:
* Return:
* Others:
***********************************************************/
__inline MYTIME buf_to_mytime( uint8_t *p )
{
	uint32_t ret;
	ret = (uint32_t)( ( *p++ ) << 26 );
	ret |= (uint32_t)( ( *p++ ) << 22 );
	ret |= (uint32_t)( ( *p++ ) << 17 );
	ret |= (uint32_t)( ( *p++ ) << 12 );
	ret |= (uint32_t)( ( *p++ ) << 6 );
	ret |= ( *p );
	return ret;
}

/*
   ��buf�л�ȡʱ����Ϣ
   �����0xFF ��ɵ�!!!!!!!����Դ�

 */
MYTIME mytime_from_hex( uint8_t* buf )
{
	MYTIME	ret = 0;
	uint8_t *p	= buf;
	if( *p == 0xFF ) /*������Ч������*/
	{
		return 0xFFFFFFFF;
	}
	ret = (uint32_t)( ( *p++ ) << 26 );
	ret |= (uint32_t)( ( *p++ ) << 22 );
	ret |= (uint32_t)( ( *p++ ) << 17 );
	ret |= (uint32_t)( ( *p++ ) << 12 );
	ret |= (uint32_t)( ( *p++ ) << 6 );
	ret |= ( *p );
	return ret;
}

/*ת��bcdʱ��*/
MYTIME mytime_from_bcd( uint8_t* buf )
{
	uint32_t year, month, day, hour, minute, sec;
	uint8_t *psrc = buf;
	year	= BCD2HEX( *psrc++ );
	month	= BCD2HEX( *psrc++ );
	day		= BCD2HEX( *psrc++ );
	hour	= BCD2HEX( *psrc++ );
	minute	= BCD2HEX( *psrc++ );
	sec		= BCD2HEX( *psrc );
	return MYDATETIME( year, month, day, hour, minute, sec );
}

/*ת��bcdʱ��*/
unsigned long utc_from_bcd( uint8_t* buf )
{
	uint32_t year, month, day, hour, minute, sec;
	uint8_t *psrc = buf;
	
	year	= BCD2HEX( *psrc++ );
	month	= BCD2HEX( *psrc++ );
	day		= BCD2HEX( *psrc++ );
	hour	= BCD2HEX( *psrc++ );
	minute	= BCD2HEX( *psrc++ );
	sec		= BCD2HEX( *psrc );
	return mytime_to_utc(MYDATETIME( year, month, day, hour, minute, sec ));
}

/*ת��Ϊʮ�����Ƶ�ʱ�� ���� 2013/07/18 => 0x0d 0x07 0x12*/
void mytime_to_hex( uint8_t* buf, MYTIME time )
{
	uint8_t *psrc = buf;
	*psrc++ = YEAR( time );
	*psrc++ = MONTH( time );
	*psrc++ = DAY( time );
	*psrc++ = HOUR( time );
	*psrc++ = MINUTE( time );
	*psrc	= SEC( time );
}

/*ת��Ϊbcd�ַ���Ϊ�Զ���ʱ�� ���� 0x13 0x07 0x12=>���� 13��7��12��*/
void mytime_to_bcd( uint8_t* buf, MYTIME time )
{
	uint8_t *psrc = buf;
	*psrc++ = HEX2BCD( YEAR( time ) );
	*psrc++ = HEX2BCD( MONTH( time ) );
	*psrc++ = HEX2BCD( DAY( time ) );
	*psrc++ = HEX2BCD( HOUR( time ) );
	*psrc++ = HEX2BCD( MINUTE( time ) );
	*psrc	= HEX2BCD( SEC( time ) );
}


/*ת��Ϊbcd�ַ���Ϊ�Զ���ʱ�� ���� 0x13 0x07 0x12=>���� 13��7��12��*/
void utc_to_bcd( uint8_t* buf, unsigned long  utc_time )
{
	uint8_t *psrc = buf; 
	MYTIME time;
	time=utc_to_mytime(utc_time);
	*psrc++ = HEX2BCD( YEAR( time ) );
	*psrc++ = HEX2BCD( MONTH( time ) );
	*psrc++ = HEX2BCD( DAY( time ) );
	*psrc++ = HEX2BCD( HOUR( time ) );
	*psrc++ = HEX2BCD( MINUTE( time ) );
	*psrc	= HEX2BCD( SEC( time ) );
}


/*********************************************************************************
  *��������:unsigned long mytime_to_utc(MYTIME	time)
  *��������:����ʽΪMYTIME��ʱ��ת��ΪUTCʱ��
  *��	��:	time	:MYTIMEʱ��
  *��	��:	none
  *�� �� ֵ:unsigned long����ʾ�����UTCʱ��
  *��	��:������
  *��������:2013-12-18
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
unsigned long mytime_to_utc(MYTIME	time)
{
	unsigned long	utc;
	unsigned int 	year;
	unsigned int 	month;
	unsigned int 	day;
	unsigned int 	hour;
	unsigned int 	minute;
	unsigned int 	sec;

	year	= YEAR(time)+2000;
	month	= MONTH(time);
	day		= DAY(time);
	hour	= HOUR(time);
	minute	= MINUTE(time);
	sec		= SEC(time);
	
	if( 0 >= (int)( month -= 2 ) )    /**//* 1..12 -> 11,12,1..10 */
	{
		month	+= 12;              /**//* Puts Feb last since it has leap day */
		year	-= 1;
	}
	utc = ( ( ( (unsigned long)( year / 4 - year / 100 + year / 400 + 367 * month / 12 + day ) +
					 year * 365 - 719499
					 ) * 24 + hour		/**//* now have hours */
				   ) * 60 + minute		   /**//* now have minutes */
				 ) * 60 + sec;			/**//* finally seconds */
	return utc;
}


/*********************************************************************************
  *��������:MYTIME utc_to_mytime(unsigned long utc)
  *��������:��utcʱ��ת��ΪMYTIMEʱ���ʽ
  *��	��:	utc	:UTCʱ��
  *��	��:	none
  *�� �� ֵ:MYTIMEʱ���ʽ��ʱ��
  *��	��:������
  *��������:2013-12-18
  *---------------------------------------------------------------------------------
  *�� �� ��:
  *�޸�����:
  *�޸�����:
*********************************************************************************/
MYTIME utc_to_mytime(unsigned long utc)
{
	int i,leapyear,day2;
	uint32_t day1;
	unsigned int 	year;
	unsigned int 	month;
	unsigned int 	day;
	unsigned int 	hour;
	unsigned int 	minute;
	unsigned int 	sec;
	MYTIME 			time;
	
	///10957   10988

	sec		= utc%60;
	minute	= utc%3600/60;
	hour	= utc%86400/3600;
	utc		/= 86400;
	if(utc<10957)
		utc	= 10967;
	year	= 2000 + (utc-10957)/1461*4;		///10957��ʾΪ2000��1��1�յ�UTC������1461��ʾ4��Ϊ��λ����������Ϊ��2000�꿪ʼ���㣬���Ե�һ��Ϊ366�죬����3��Ϊ365��
	day1=(utc-10957)%1461;
	if(day1 >= 366)
		{
		year++;
		day1 -= 366;
		year	+= day1/365;

		day1	%= 365;
		leapyear	= 0;
		}
	else
		{
		leapyear	= 1;
		}
	day2=0;
	for(i=1;i<=12;i++)
 	{
	 	day=Get_Month_Day(i,leapyear);
	 	day2+=day;
		//�����ǰ�µ�������С�ڼ���������õ����·�
		if(day2>day1)
		{
			day2-=day;
			break;
		}
 	}
	month=i;
	day=day1-day2+1;
	time	= MYDATETIME(year-2000, month, day, hour, minute, sec);
	return time;
}


/*********************************************************************************
  *��������:int console_param_load( char *p, uint16_t len )
  *��������:���¼������в���
  *��	��: p		:���ݵ��ַ���
  			len		:�ַ�������
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
/*used for convert time frome string to struct rtc_time_t*/
static int parse_string(char *string, rtc_time_t *p_tm)
{
	char *comma, *head;
	int value[10];
	int i;

	if (!string || !p_tm)
		return -1;

	if (!strchr(string, '/'))
		return -1;

	head = string;
	i = 0;
	comma = NULL;

	for(;;) {	
		comma = strchr(head, '/');

		if (!comma){
			value[i++] = atoi(head);
			break;
		}

		*comma = '\0';
		value[i++] = atoi(head);
		head = comma+1;	
	}
	
	if (i < 5)
		return -1;

	p_tm->year   = value[0];
	p_tm->month  = value[1];
	p_tm->date   = value[2];
	p_tm->hour   = value[3];
	p_tm->minute = value[4];
	p_tm->second = value[5];
	p_tm->weekday = 0;

	return 0;
}


/*********************************************************************************
  *��������:int rtc_manage_hi(const char *pstr, int commd)
  *��������:��ȡRTCʱ���������RTCʱ��
  *��	��: pstr	:
  			commd	:
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-15	����
*********************************************************************************/
int rtc_manage_hi(const char *pstr, int commd)
{
	//rtc_time_t tm;
	int ret =  - 1;
	int fd =  - 1;
	const char *dev_name = "/dev/hi_rtc";
	char string[50];
	fd = open(dev_name, O_RDWR);
	if (!fd)
	{
		printf("open %s failed\n", dev_name);
		return  - 1;
	}
	if(pstr!=NULL)
	{
		memset(string,0,sizeof(string));
		memcpy(string, pstr,strlen(pstr));
	}
	switch (commd)
	{
		case 1:
			ret = parse_string(string, &rtc);
			if (ret < 0)
			{
				printf("parse time param failed\n");
				goto err1;
			}
			printf("set time\n");
			#if 1	
			printf("year:%d\n", rtc.year);
			printf("month:%d\n", rtc.month);
			printf("date:%d\n", rtc.date);
			printf("hour:%d\n", rtc.hour);
			printf("minute:%d\n", rtc.minute);
			printf("second:%d\n", rtc.second);
			#endif
			ret = ioctl(fd, HI_RTC_SET_TIME, &rtc);
			if (ret < 0)
			{
				printf("ioctl: HI_RTC_SET_TIME failed\n");
				goto err1;
			}
			break;
		case 2:
			ret = ioctl(fd, HI_RTC_RD_TIME, &rtc);
			if (ret < 0)
			{
				printf("ioctl: HI_RTC_RD_TIME failed\n");
				goto err1;
			}
			printf("Current time value: \n");
			printf("year:%d\n", rtc.year);
			printf("month:%d\n", rtc.month);
			printf("date:%d\n", rtc.date);
			printf("hour:%d\n", rtc.hour);
			printf("minute:%d\n", rtc.minute);
			printf("second:%d\n", rtc.second);
			break;
	}
	return 3;
	err1:
    close(fd);

	return 0;
}


/*********************************************************************************
  *��������:int rtc_init_time(void)
  *��������:��ʼ��RTC���ڸú����п��Դ�RTC�豸
  *��	��: none
  *��	��: none
  *�� �� ֵ:	0:OK����0��ʾERROR
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-15	����
*********************************************************************************/
int rtc_init_time(void)
{
	//rtc_time_t tm;
	int ret =  - 1;
	const char *dev_name = "/dev/hi_rtc";
	
	s_rtc_fd = open(dev_name, O_RDWR);
	if (!s_rtc_fd)
	{
		printf("open %s failed\n", dev_name);
		return  -1;
	}
	return 0;
}



/*********************************************************************************
  *��������:unsigned long rtc_get_time(rtc_time_t *p_time)
  *��������:��ȡRTCʱ��
  *��	��: p_time	:Ҫ���ص�ʱ�䣬���ΪNULL��ֻ�к�������ֵ�ǵ�ǰʱ��
  *��	��: none
  *�� �� ֵ:UTCʱ�䣬���Ϊ0��ʾ��ȡ����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-15	����
*********************************************************************************/
unsigned long rtc_get_time(rtc_time_t *p_time)
{
	//rtc_time_t tm;
	int ret =  - 1;
	int fd =  - 1;
	const char *dev_name = "/dev/hi_rtc";
	rtc_time_t r_time;
	unsigned long	utc = 0;
	unsigned int 	year;
	unsigned int 	month;
	unsigned int 	day;
	unsigned int 	hour;
	unsigned int 	minute;
	unsigned int 	sec;

	if (!s_rtc_fd)
	{
		if(rtc_init_time())
		{
			return  0;
		}
	}
	if( (void *)p_time == 0 )
	{
		p_time = &r_time;
	}
	
	ret = ioctl(s_rtc_fd, HI_RTC_RD_TIME, p_time);
	if (ret < 0)
	{
		printf("ioctl: HI_RTC_RD_TIME failed\n");
		return  0;
	}
	year	= p_time->year;
	month	= p_time->month;
	day		= p_time->date;
	hour	= p_time->hour;
	minute	= p_time->minute;
	sec		= p_time->second;
	
	if( 0 >= (int)( month -= 2 ) )    /**//* 1..12 -> 11,12,1..10 */
	{
		month	+= 12;              /**//* Puts Feb last since it has leap day */
		year	-= 1;
	}
	utc = ( ( ( (unsigned long)( year / 4 - year / 100 + year / 400 + 367 * month / 12 + day ) +
				 year * 365 - 719499
				 ) * 24 + hour		/**//* now have hours */
			   ) * 60 + minute		   /**//* now have minutes */
			 ) * 60 + sec;			/**//* finally seconds */

	return utc;
}



/*********************************************************************************
  *��������:unsigned long rtc_set_time(rtc_time_t *p_time)
  *��������:����RTCʱ��
  *��	��: p_time	:����Ҫ���õ�ʱ��
  *��	��: none
  *�� �� ֵ:	0:OK����0��ʾERROR
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-15	����
*********************************************************************************/
int rtc_set_time(rtc_time_t *p_time)
{
	//rtc_time_t tm;
	int ret =  - 1;
	int fd =  - 1;
	const char *dev_name = "/dev/hi_rtc";
	
	if (!s_rtc_fd)
	{
		if(rtc_init_time())
		{
			return  -1;
		}
	}
	
	ret = ioctl(s_rtc_fd, HI_RTC_SET_TIME, p_time);
	if (ret < 0)
	{
		printf("ioctl: HI_RTC_SET_TIME failed\n");
		return  -1;
	}
	return 0;
}



/*********************************************************************************
  *��������:int rtc_set_time_utc(unsigned long uct_time)
  *��������:����RTCʱ�䣬����ʱ���ʽΪUTCʱ��
  *��	��: uct_time	:Ҫ���õ�ʱ�䣬��ʽΪutc��ʽ
  *��	��: none
  *�� �� ֵ:	0:OK����0��ʾERROR
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-15	����
*********************************************************************************/
int rtc_set_time_mytime(MYTIME my_time)
{
	rtc_time_t 	r_time;
	
	r_time.year = YEAR(my_time) + 2000;
	r_time.month= MONTH(my_time);
	r_time.date = DAY(my_time);
	r_time.hour = HOUR(my_time);
	r_time.minute= MINUTE(my_time);
	r_time.second= SEC(my_time);
	r_time.weekday = 0;

	/*
	printf( " rtc_set_time_mytime:%d-%d-%d-%d-%d-%d\n",
		r_time.year,
		r_time.month,
		r_time.date,
		r_time.hour,
		r_time.minute,
		r_time.second
		);
		*/
	return rtc_set_time(&r_time);
}



/*********************************************************************************
  *��������:int rtc_set_time_utc(unsigned long uct_time)
  *��������:����RTCʱ�䣬����ʱ���ʽΪUTCʱ��
  *��	��: uct_time	:Ҫ���õ�ʱ�䣬��ʽΪutc��ʽ
  *��	��: none
  *�� �� ֵ:	0:OK����0��ʾERROR
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-15	����
*********************************************************************************/
int rtc_set_time_utc(unsigned long uct_time)
{
	rtc_time_t 	r_time;
	MYTIME 		my_time;
	
	my_time = utc_to_mytime(uct_time);
	return rtc_set_time_mytime(my_time);
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

