#ifndef _H_GPS_STORE_IO_
#define _H_GPS_STORE_IO_
/*********************************************************************************
  * @�ļ����� :gps_store.h
  * @�������� :���кʹ洢������Ӧ�ó�����صĺ�����������
  * @��	   �� :������
  * @�������� :2015-6-19
  * @�ļ��汾 :0.01
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-19	��������
*********************************************************************************/
#ifdef __cplusplus
#if __cplusplus
	extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include "hi_common.h"
#include "hi_comm_video.h"
#include <time.h>

#define DF_STORE_AV_PACK_SIZE			0x10000		///ÿ�δ��������Ƶ���Ĵ�С
#define DF_STORE_AV_PACK_SIZE_X2		0x20000		///ÿ�δ��������Ƶ���Ĵ�С * 2
#define DF_STORE_CHANNEL_MAX			5			///�������ͨ������
#define DF_STORE_AUDIO_FRAME_SIZE		168			///��Ƶ����ÿ���Ĵ�С
#define DF_STORE_AUDIO_FRAME_MAX		8			///��Ƶ�������֡������ֵ��СΪ6
#define DF_STORE_WRITE_INDEX_START		1024		///¼�����ݿ�ʼд���λ�ã���λΪ64K
#define DF_STORE_FOLDER_LIST_LEN		16			///�ļ����б���
#define DF_STORE_FILE_LIST_HEAD_LEN		64			///�ļ��б�ͷ����
#define DF_STORE_FILE_LIST_LEN			128			///�ļ��б���
#define DF_STORE_NVR_LIST_LEN			64			///nvr�ļ��б���Ϣ����
#define DF_STORE_NVR_LIST_HEAD_LEN		64			///nvr�ļ��б���Ϣͷ����

#define DF_STORE_MAGIC_FOLDER_LIST		"GP01"
#define DF_STORE_MAGIC_FILE_LIST_HEAD	"FILELIST01"
#define DF_STORE_MAGIC_FILE_LIST		"LIST0001"
#define DF_STORE_MAGIC_NVR_LIST_HEAD	"NVR001"
#define DF_STORE_MAGIC_NVR_LIST			"nvr001"



#define DF_STORE_FIFO_NAME				"/tmp/cmd_pipe"
#define DF_STORE_HDISK_AV				"/disk1"	///¼����Ϣ���ڵ��ļ�Ŀ¼
#define DF_STORE_FOLDER_LIST_NAME		"folder_list"
#define DF_STORE_FILE_LIST_NAME			"file_list"
#define DF_STORE_HDISK_DIR_NAME			"dev/sda4"	///¼����Ϣ���ڵ��ļ�Ŀ¼


/*********************************************************************************
  *ö������:en_store_style
  *��������:�洢����������
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-23	����
*********************************************************************************/
typedef enum _en_store_style
{
	STORE_STYLE_VIDEO			=0,			///�洢������Ƶ
	STORE_STYLE_AUDIO,						///�洢������Ƶ
	STORE_STYLE_VIDEO_AUDIO,				///�洢��������Ƶ�������
}en_store_style;


/*********************************************************************************
  *ö������:en_trige_style
  *��������:�洢���ݵĴ�����ʽ
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-23	����
*********************************************************************************/
typedef enum _en_trige_style
{
	STORE_TRIGE_NORMA			=0,		///����������Ĭ��ֵ
	STORE_TRIGE_ALRAM,					///��������
	STORE_TRIGE_EVENT,					///�ⲿ�¼�����
	STORE_TRIGE_TIMER,					///��ʱ������
	STORE_TRIGE_HUMAN,					///�ֶ�����
}en_trige_style;

/*********************************************************************************
  *ö������:en_hdisk_state
  *��������:Ӳ��״̬
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-13	����
*********************************************************************************/
typedef enum _en_hdisk_state
{
	STORE_HDISK_NONE			=0,		///û�м�⵽Ӳ��
	STORE_HDISK_FORMATING,				///Ӳ�����ڸ�ʽ��
	STORE_HDISK_FORMATED,				///Ӳ�̸�ʽ�����
	STORE_HDISK_MOUNTED,				///Ӳ�̹����ļ�ϵͳOK
	STORE_HDISK_ERROR,					///Ӳ�̴���
}en_hdisk_state;


/*********************************************************************************
  *�� �� ��:st_store_video
  *��������:�洢����Ƶ����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-23	����
*********************************************************************************/
typedef struct _st_store_video_param
{
	unsigned char				channel;			///ͨ����
	PIC_SIZE_E					e_pic_size;			///��Ƶ�ֱ��ʴ�С
	VIDEO_NORM_E				e_video_norm;		///��Ƶ��ʽ��PAL or NTSC
	PAYLOAD_TYPE_E				e_payload_type;		///Ҫ���ص��ļ���ʽ
	en_trige_style				e_trige_type;		///������ʽ
}st_store_video_param;


/*********************************************************************************
  *�� �� ��:st_store_video
  *��������:�洢����Ƶ����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-23	����
*********************************************************************************/
typedef struct _st_store_audio_param
{
	unsigned char				channel;			///ͨ����
	PAYLOAD_TYPE_E				e_payload_type;		///Ҫ���ص��ļ���ʽ
	en_trige_style				e_trige_type;		///������ʽ
}st_store_audio_param;

#pragma pack(1)

/*********************************************************************************
  *�� �� ��:st_store_video
  *��������:�洢����Ƶbuf��Ϣ
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-23	����
*********************************************************************************/
typedef struct _st_store_av_data
{
	unsigned char 				av_buf[DF_STORE_AV_PACK_SIZE_X2];	///����Ľ�������Ƶ���ݴ�ȡ��buf
	//unsigned char 				a_buf[DF_STORE_AUDIO_FRAME_MAX][DF_STORE_AUDIO_FRAME_SIZE];		///����Ľ�����Ƶ���ݴ�ȡ��buf,ÿ�δ���168���ֽڵ�����
	unsigned char 				v_buf[512];			///����Ľ�����Ƶ����������֡(����֡����ipb֡)�洢��buf
	unsigned long				av_buf_wr;			///��buf����д���λ��
	unsigned long				av_buf_rd;			///��buf�����ȡ��λ��
	unsigned long				av_len;				///buf�������Ч����
	//unsigned long				a_buf_wr;			///��buf����д���λ�ã���ά����λ�ã����Ϊ8
	//unsigned long				a_buf_rd;			///��buf�����ȡ��λ�ã���ά����λ�ã����Ϊ8
	//unsigned long				a_len;				///buf�������Ч���ݣ���ά���鳤�ȣ����Ϊ8
	unsigned long				v_buf_wr;			///��buf����д���λ��
	unsigned long				v_buf_rd;			///��buf�����ȡ��λ��
	unsigned long				v_len;				///buf�������Ч����
	unsigned char				iswrite;			///���ڽ���д����
	utc_time					time[2];			///Ҫ�洢���������ɵ�ʱ��,�ֱ��ʾǰ������64k�������������ʱ��
	unsigned char				frame_style[2];		///��ʾҪ�洢��֡���ͣ�BIT(1)��ʾ��64k��������I֡��BIT(7)��ʾ������洢����
}st_store_av_data;
 
 
 /*********************************************************************************
   *�� �� ��:st_store_video
   *��������:�洢����Ƶbuf��Ϣ
   *---------------------------------------------------------------------------------
   * @�޸���	 �޸�ʱ��	 �޸�����
   * ������ 	 2015-06-23  ����
 *********************************************************************************/
 typedef struct _st_store_a_data
 {
	 unsigned char				 a_buf[DF_STORE_AUDIO_FRAME_MAX][DF_STORE_AUDIO_FRAME_SIZE];	 ///����Ľ�����Ƶ���ݴ�ȡ��buf,ÿ�δ���168���ֽڵ�����
	 unsigned long				 a_buf_wr;			 ///��buf����д���λ�ã���ά����λ�ã����Ϊ8
	 unsigned long				 a_buf_rd;			 ///��buf�����ȡ��λ�ã���ά����λ�ã����Ϊ8
	 unsigned long				 a_len; 			 ///buf�������Ч���ݣ���ά���鳤�ȣ����Ϊ8
}st_store_a_data;



/*********************************************************************************
  *�� �� ��:st_store_video
  *��������:�洢����Ƶ���ݼ�������Ϣ
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-23	����
*********************************************************************************/
typedef struct _st_store_av_manage
{
	en_store_style				style;				///Ҫ�洢����������
	st_store_video_param		param_video;		///�洢����Ƶ����
	st_store_audio_param		param_audio;		///��Ƶ����
	utc_time					time;				///Ҫ�洢���������ɵ�ʱ��
	unsigned long				tick;				///Ҫ�洢���������ɵ�tickֵ����λΪ10ms
	unsigned long				tick_validity;		///Ҫ�洢�����ݵ���Чʱ�䳤�ȣ���λΪ10ms��
	st_store_av_data			data_av;			///Ҫ�洢��¼������
	st_store_a_data				data_a;				///��Ƶ�����ݴ�buf
}st_store_av_manage;


/*********************************************************************************
  *�� �� ��:st_store_folder_list
  *��������:�洢¼�����ݵ��ļ����б���Ϣ,�̶�����Ϊ16�ֽ�
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-13	����
*********************************************************************************/
typedef struct _st_store_folder_list
{
	s8						head[4];			///8�ֽڣ��̶� "GP01"��
	u32						start_index;		///��ʼд�������е�λ��
	u16						folder_ID;			///�ļ���ID
	u8						time_bcd[6];		///�ļ�������
}st_store_folder_list;



/*********************************************************************************
  *�� �� ��:st_store_file_list_head
  *��������:�洢¼�����ݵ��ļ��б���Ϣͷ,�̶�����Ϊ64�ֽ�
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-13	����
*********************************************************************************/
typedef struct _st_store_file_list_head
{
	s8						head[10];			///8�ֽڣ��̶� "FILELIST01"��
	u8						time_start_bcd[6];	///�ļ���ʼʱ��	6�ֽ�BCD�루������ʱ���룩
	u8						time_end_bcd[6];	///�ļ�����ʱ��	6�ֽ�BCD�루������ʱ���룩
	u16						file_num;			///���ļ���	2�ֽڣ��ȵͺ�ߣ�
	u32						filelist_size;		///�ļ��ܴ�С	4�ֽڣ��ȵͺ�ߣ�����λΪ64K
	u32						write_index_start;	///��������д��Ŀ�ʼindexλ��
	u32						write_index_end;	///��������д���ĩβindexλ��
}st_store_file_list_head;

/*********************************************************************************
  *�� �� ��:st_store_file_list
  *��������:�洢¼�����ݵ��ļ��б���Ϣ,�̶�����Ϊ128�ֽ�
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-13	����
*********************************************************************************/
typedef struct _st_store_file_list
{
	s8						head[8];			///8�ֽڣ��̶� "LIST0001"��
	s8						file_name[64];		///�ļ�����	96�ֽڣ�����96�ֽں��油��0
	u16						file_len;			///�ļ����� 2�ֽڣ��ȵͺ�ߣ�����λΪ64K
	u8						time_start_bcd[6];	///�ļ���ʼʱ��	6�ֽ�BCD�루������ʱ���룩
	u8						time_end_bcd[6];	///�ļ�����ʱ��	6�ֽ�BCD�루������ʱ���룩
	u32						write_index_start;	///��������д��Ŀ�ʼindexλ��
	u32						write_index_end;	///��������д���ĩβindexλ��
	u8						channel;			///¼��ͨ��	1�ֽڣ�0��ʾΪ1��ͨ����
	u8						t_data[43];			///��33���ֽڶ༸���ֽ���Ϊ�˲����
}st_store_file_list;


/*********************************************************************************
  *�� �� ��:st_store_channel_param
  *��������:���̴洢�е���ͨ���Ĳ���
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-13	����
*********************************************************************************/
typedef struct _st_store_channel_param
{
	en_store_style				style;				///Ҫ�洢����������
	st_store_video_param		param_video;		///�洢����Ƶ����
	st_store_audio_param		param_audio;		///��Ƶ����
	char						write_file[128];	///����д����ļ�������
	FILE *						write_file_fd;		///����д����ļ��ľ��
	unsigned long				file_len;			///���ļ���Ӧ�����ݵ��ܳ��ȣ���λΪ64K
	utc_time					time_start;			///д�뿪ʼʱ��
	utc_time					time_end;			///д�����ʱ��
	u32							write_index_start;	///������д��Ŀ�ʼindexλ��
	u32							write_index_end;	///������д���ĩβindexλ��
	unsigned char				write_frame_style;	///���ڴ洢�����֡���ͣ�BIT(1)��ʾ��64k��������I֡��BIT(7)��ʾ������洢����
	utc_time					write_time;				///���ڴ洢���������ʱ��
}st_store_channel_param;


/*********************************************************************************
  *�� �� ��:st_store_hdisk
  *��������:Ӳ�̴洢��ز���
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-13	����
*********************************************************************************/
typedef struct _st_store_hdisk
{
	en_hdisk_state				disk_state;				///Ӳ�̵�ǰ��״̬
	unsigned long				disk_size_av;			///Ӳ�̿��Դ洢��¼���̵Ĵ�С����λΪ65536�ֽڣ���64K
	unsigned long				write_index;			///Ӳ�̵�ǰ����д���λ�ã���λΪ64K
	unsigned long				read_index;				///Ӳ�̵�ǰ����д���λ�ã���λΪ64K
	unsigned char				write_limit_mode;		///Ӳ�̴洢�������Ƶ�ģʽ0��ʾ����ʱ�䳤�����ƴ洢��1��ʾ���մ�С���ƴ洢
	unsigned long				write_size_max;			///Ӳ�̴洢¼�񵥸��ļ������ֵ����λΪ64K
	unsigned long				write_time_max;			///Ӳ�̴洢¼�񵥸��ļ���ʱ���ֵ����λΪ��s
	int 						write_hdisk_fd;			///����д��ľ��
	FILE * 						write_folder_list_fd;	///�ļ���Ϣ�ļ����б��Ӧ���ļ����
	FILE *						write_file_list_fd;		///����д����ļ��б�ľ��
	utc_time					write_folder_day;		///��ǰ����д����ļ��ж�Ӧ��ʱ�䣬��ʱ��Сʱ�����ӣ����Ϊ0
	char						write_folder[32];		///��ǰ����д����ļ���λ��
	st_store_file_list_head		file_list_head;			///����洢�����ݵ�����Ϣ
	st_store_channel_param		channel_param[DF_STORE_CHANNEL_MAX];	///�洢��ÿ��ͨ���Ĳ���
}st_store_hdisk;
#pragma pack()



extern int					gt_store_fifo_fd;
extern st_store_hdisk		gt_store_hdisk;			///Ӳ��״̬��Ϣ�����Ӳ�̲���


int store_fifo_write( char *p);
int store_init(void);
int store_end(void);
int store_save_audio( st_store_audio_param *pt_audio_param, u8 * p_data,u32 len );
int store_save_video( st_store_video_param *tp_video_param, u8 * p_data,u32 len );


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef _H_GPS_STORE_IO_ */
