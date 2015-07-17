#ifndef _H_GPS_HI_RTC_
#define _H_GPS_HI_RTC_
/*********************************************************************************
  * @�ļ����� :gps_console.c
  * @�������� :�û������˻������Ľӿں�����������Ҫͨ������̨�����˻������Ĵ��붼������
  * @��	   �� :������
  * @�������� :2015-6-18
  * @�ļ��汾 :0.01
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gps_typedef.h"
#include "hi_rtc.h"


#ifdef __cplusplus
#if __cplusplus
	extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


///MYTIME��ʽ��ʱ������ʾ��ʱ��Ϊ2063-12-31 23:59:59
typedef uint32_t MYTIME;			

#define MYDATETIME( year, month, day, hour, minute, sec ) \
    ( (uint32_t)( ( year ) << 26 ) | \
      (uint32_t)( ( month ) << 22 ) | \
      (uint32_t)( ( day ) << 17 ) | \
      (uint32_t)( ( hour ) << 12 ) | \
      (uint32_t)( ( minute ) << 6 ) | ( sec ) )

#define YEAR( datetime )	( ( datetime >> 26 ) & 0x3F )
#define MONTH( datetime )	( ( datetime >> 22 ) & 0xF )
#define DAY( datetime )		( ( datetime >> 17 ) & 0x1F )
#define HOUR( datetime )	( ( datetime >> 12 ) & 0x1F )
#define MINUTE( datetime )	( ( datetime >> 6 ) & 0x3F )
#define SEC( datetime )		( datetime & 0x3F )


extern MYTIME mytime_from_hex( uint8_t* buf );
/*ת��bcdʱ��*/
extern MYTIME mytime_from_bcd( uint8_t* buf );
/*ת��Ϊʮ�����Ƶ�ʱ�� ���� 2013/07/18 => 0x0d 0x07 0x12*/
extern void mytime_to_hex( uint8_t* buf, MYTIME time );
/*ת��Ϊbcd�ַ���Ϊ�Զ���ʱ�� ���� 0x13 0x07 0x12=>���� 13��7��12��*/
extern void mytime_to_bcd( uint8_t* buf, MYTIME time );
extern unsigned long mytime_to_utc(MYTIME	time);
extern MYTIME utc_to_mytime(unsigned long utc);
extern int rtc_init_time(void);
extern unsigned long rtc_get_time(rtc_time_t *p_time);
extern int rtc_set_time(rtc_time_t *p_time);
extern int rtc_set_time_mytime(MYTIME my_time);
extern int rtc_set_time_utc(unsigned long uct_time);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef _H_GPS_HI_RTC_ */

