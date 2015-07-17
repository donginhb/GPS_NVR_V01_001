#ifndef _H_GPS_TYPEDEF_
#define _H_GPS_TYPEDEF_
/*********************************************************************************
  * @�ļ����� :gps_disk_io.h
  * @�������� :gps��Ŀ����Ҫ���������ȫ�ֽṹ�壬define����typedef ��������
  * @��	   �� :������
  * @�������� :2015-6-18
  * @�ļ��汾 :0.01
  *---------------------------------------------------------------------------------
  * @�޸���    �޸�ʱ��   �޸�����
  *  ������   2015-06-18  ��������
*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#ifdef __cplusplus
#if __cplusplus
	extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

//typedef		unsigned char			uint8_t;
//typedef		short unsigned int		uint16_t;
//typedef		unsigned long 			uint32_t;


typedef		unsigned long 			utc_time;


#ifndef FALSE
	#define FALSE	0
#endif
#ifndef TRUE
	#define TRUE	1
#endif




/*********************************************************************************
  *�� �� ��:gps_thread_param
  *��������:������һ���ṹ�����ͣ�����ʾ���ݸ�һ���̵߳Ĳ���
  *---------------------------------------------------------------------------------
  *�޸���    �޸�ʱ��   �޸�����
  *������   2015-06-08  ��������
*********************************************************************************/
typedef struct _st_gps_thread_param
{
     u8   thread_start;
     u32  thread_param;
}st_gps_thread_param;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef _H_GPS_TYPEDEF_ */

