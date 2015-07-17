#ifndef _H_GPS_DISK_IO_
#define _H_GPS_DISK_IO_
/*********************************************************************************
  * @�ļ����� :gps_disk_io.h
  * @�������� :����Ӳ�̺ʹ洢�̲���������������Ӧ�ú����������ļ�
  * @��	   �� :������
  * @�������� :2015-6-17
  * @�ļ��汾 :0.01
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-17	����
*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef __cplusplus
#if __cplusplus
	extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


int console_disk_test( char *p, unsigned int len );

int disk_printf_dir(const char* pc_path);
int disk_remove_dir(const char* pc_path);
int disk_create_dir(const char* pc_path);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef _H_GPS_DISK_IO_ */
