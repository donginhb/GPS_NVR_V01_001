/*********************************************************************************
  * @�ļ����� :gps_store.c
  * @�������� :���кʹ洢������Ӧ�ó�����صĺ�����������
  * @��	   �� :������
  * @�������� :2015-6-19
  * @�ļ��汾 :0.01
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-19	����
*********************************************************************************/
#ifdef __cplusplus
#if __cplusplus
	extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
//#include "linux/time.h"
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "gps_typedef.h"
#include "common_func.h"
#include "gps_disk_io.h"
#include "gps_param.h"
#include "gps_store.h"
#include "gps_hi_rtc.h"


#define DF_STORE_AUDIO_DOUBLE		3			///��buf�е���Ƶ�������ڸ�ֵʱ����Ҫ��������


st_store_av_manage gt_store_av_manage[DF_STORE_CHANNEL_MAX];			///����Ľ�������Ƶ���ݴ�ȡ����Ľṹ��
static pthread_t gt_store_thread_pid;					///¼�����ݴ洢�߳�PID
int		gt_store_fifo_fd = 0;							///¼�����ݴ洢�����ܵ��ľ��
static st_gps_thread_param	gt_store_thread_para;		///���Խӿ��̲߳���
st_store_hdisk		gt_store_hdisk;						///Ӳ��״̬��Ϣ�����Ӳ�̲���
static utc_time		sgt_utc_now;
static en_hdisk_state		sgt_disk_status = STORE_HDISK_NONE;
static utc_time				sgt_disk_size	= 6250000;


/*********************************************************************************
  *��������:static int store_create_file_list_head( char *p_buf )
  *��������:�����ļ��б�ͷ��Ϣ
  *��	��: p_buf	:Ҫ�洢��buf
  *��	��: none
  *�� �� ֵ:0����ʾ�������أ�-1����ʾ�����˴���1��ʾ�洢����Ч������
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-22	����
*********************************************************************************/
static int store_create_file_list_head( char *p_buf )
{
	int len = 0;
	memset(p_buf, 0 ,DF_STORE_FILE_LIST_HEAD_LEN);
	strcpy(gt_store_hdisk.file_list_head.head,DF_STORE_MAGIC_FILE_LIST_HEAD);
	memcpy(p_buf, (u8 *)&gt_store_hdisk.file_list_head ,sizeof(st_store_file_list_head));
	return 0;
}



/*********************************************************************************
  *��������:static int store_create_file_list( st_store_channel_param * pt_store_channel,char *p_buf )
  *��������:�����ļ��б���Ϣ
  *��	��: p_buf	:Ҫ�洢��buf
  *��	��: none
  *�� �� ֵ:0����ʾ�������أ�-1����ʾ�����˴���1��ʾ�洢����Ч������
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-22	����
*********************************************************************************/
static int store_create_file_list( st_store_channel_param * pt_store_channel,char *p_buf )
{
	int len = 0;
	st_store_file_list * pt_file_list;
	pt_file_list =(st_store_file_list * )p_buf;

	memset(pt_file_list,0,DF_STORE_FILE_LIST_LEN);
	strcpy(pt_file_list->head,DF_STORE_MAGIC_FILE_LIST);
	strcpy(pt_file_list->file_name, pt_store_channel->write_file );
	pt_file_list->file_len = pt_store_channel->file_len;
	mytime_to_bcd(pt_file_list->time_start_bcd,utc_to_mytime( pt_store_channel->time_start));
	mytime_to_bcd(pt_file_list->time_end_bcd,utc_to_mytime( pt_store_channel->time_end));
	pt_file_list->.write_index_start = pt_store_channel->write_index_start;
	pt_file_list->write_index_end	= pt_store_channel->write_index_end;
	pt_file_list->channel 			= pt_store_channel->param_video.channel;
	
	return 0;
}





/*********************************************************************************
  *��������:static int store_create_nvr_list_head( u8 channel,char *p_buf )
  *��������:����nvr�б�ͷ��Ϣ
  *��	��: channel	:��Ӧ��¼��ͨ����0��ʾͨ��1
  			p_buf	:Ҫ�洢��buf
  *��	��: none
  *�� �� ֵ:0����ʾ�������أ�-1����ʾ�����˴���1��ʾ�洢����Ч������
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-22	����
*********************************************************************************/
static int store_create_nvr_list_head( u8 channel,char *p_buf )
{
	int len = 0;

	memset(p_buf,0,DF_STORE_NVR_LIST_HEAD_LEN);

	///����ͷ�̶�Ϊ"NVR001"
	strcpy(p_buf,DF_STORE_MAGIC_NVR_LIST_HEAD);
	len += 6;
	///�ļ���ʼʱ��	6�ֽ�BCD�루������ʱ���룩
	mytime_to_bcd(p_buf+len,utc_to_mytime( gt_store_hdisk.channel_param[channel].time_start ));
	len += 6;
	///�ļ�����ʱ��	6�ֽ�BCD�루������ʱ���룩
	mytime_to_bcd(p_buf+len,utc_to_mytime( gt_store_hdisk.channel_param[channel].time_end ));
	len += 6;
	///�ļ��ܴ�С	4�ֽڣ��ȵͺ�ߣ�����λΪ64K
	memcpy(p_buf+len,(u8 *)&(gt_store_hdisk.channel_param[channel].file_len),4);
	len += 4;
	///�ļ���Ϣ��Ӧд�����̿�ʼλ��	4�ֽ�
	memcpy(p_buf+len,(u8 *)&(gt_store_hdisk.channel_param[channel].write_index_start),4);
	len += 4;
	///�ļ���Ϣ��Ӧд�����̽���λ��	4�ֽ�
	memcpy(p_buf+len,(u8 *)&(gt_store_hdisk.channel_param[channel].write_index_end),4);
	len += 4;
	///¼��ͨ��	1�ֽڣ�0��ʾΪ1��ͨ����
	p_buf[len] = channel;
	len += 1;
	///��33�ֽ�
	len += 33;
	return 0;
}



/*********************************************************************************
  *��������:static int store_create_nvr_list( u8 channel,char *p_buf )
  *��������:����nvr�б���Ϣ
  *��	��: channel	:��Ӧ��¼��ͨ����0��ʾͨ��1
  			p_buf	:Ҫ�洢��buf
  *��	��: none
  *�� �� ֵ:0����ʾ�������أ�-1����ʾ�����˴���1��ʾ�洢����Ч������
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-22	����
*********************************************************************************/
static int store_create_nvr_list( u8 channel,char *p_buf )
{
	int len = 0;

	memset(p_buf,0,DF_STORE_NVR_LIST_LEN);

	///����ͷ�̶�Ϊ"nvr001"
	strcpy(p_buf,DF_STORE_MAGIC_NVR_LIST);
	len += 6;
	///���ݵ�ʱ��
	mytime_to_bcd(p_buf+len,utc_to_mytime( gt_store_hdisk.channel_param[channel].write_time));
	len += 6;
	///д�����̵�λ��
	memcpy(p_buf+len,(u8 *)&(gt_store_hdisk.channel_param[channel].write_index_end),4);
	len += 4;
	///¼��ͨ��	1�ֽڣ�0��ʾΪ1��ͨ����
	p_buf[len] = channel;
	len += 1;
	///�Ƿ���I֡ 1�ֽڣ�0��ʾû�У�1��ʾ�У�
	if(gt_store_hdisk.channel_param[channel].write_frame_style & (BIT(1)) )
	{
		p_buf[len] = 1;
	}
	len += 1;
	///��46�ֽ�
	len += 46;
	return 0;
}

/*********************************************************************************
  *��������:static int store_create_nvr_name(st_store_channel_param * pt_store_channel,char *p_buf )
  *��������:����Ҫ�洢����Ϣ�ļ��ĵ��ļ�����
  *��	��: pt_store_channel	:
  *��	��: none
  *�� �� ֵ:0����ʾ�������أ�-1����ʾ�����˴���1��ʾ�洢����Ч������
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-22	����
*********************************************************************************/
static int store_create_nvr_name(st_store_channel_param * pt_store_channel,char *p_buf )
{
	u8 i;
	MYTIME	my_time1,my_time2;				///
	
	my_time1 = utc_to_mytime(pt_store_channel->time_start );
	my_time2 = utc_to_mytime(pt_store_channel->time_end );
	sprintf(p_buf,"%s\0000-%02d%02d%02d-%02d%02d%02d-%02d%02d%02d-%d-%d.nvr",
		gt_store_hdisk.write_folder,
		YEAR(my_time1),MONTH(my_time1),DAY(my_time1),
		HOUR(my_time1),MINUTE(my_time1),SEC(my_time1),
		HOUR(my_time2),MINUTE(my_time2),SEC(my_time2),
		pt_store_channel->param_video.e_trige_type,
		pt_store_channel->param_video.channel									
		);
	return 0;
}


/*********************************************************************************
  *��������:static int store_create_foldernvr_name(char *p_buf, utc_time time1 )
  *��������:����Ҫ�洢����Ϣ�ļ��е�����
  *��	��: pt_store_channel	:
  *��	��: none
  *�� �� ֵ:0����ʾ�������أ�-1����ʾ�����˴���1��ʾ�洢����Ч������
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-22	����
*********************************************************************************/
static int store_create_foldernvr_name(char *p_buf, utc_time time1 )
{
	u8 i;
	MYTIME	my_time1,my_time2;				///
	char c_buf[16];
	
	///�����ļ�������
	utc_to_bcd(c_buf, time1);
	sprintf(p_buf,"%s/%04d-%02d-%02d",DF_STORE_HDISK_AV,c_buf[0]+2000,c_buf[1],c_buf[2]);
	return 0;
}


/*********************************************************************************
  *��������:static int store_remove_oldest_folder(void)
  *��������:¼��洢���ˣ���Ҫɾ�����ϵ����ݺ������Ϣʱ���øú���
  *��	��: none
  *��	��: none
  *�� �� ֵ:0����ʾ�������أ�-1����ʾ�����˴���1��ʾ�洢����Ч������
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
static int store_remove_oldest_folder(void)
{
	int	i,j,k;
	int max_id,min_id;
	FILE *	floder_fd,floder_fd_temp;
	s8	 	c_buf[128];
	s8	 	c_buf_name1[64];
	s8	 	c_buf_name2[64];
	u8		uc_file_buf[DF_STORE_FOLDER_LIST_LEN*128];		///��buf���ȱ���Ϊ DF_STORE_FOLDER_LIST_LEN ��������
	int	i_init_folder;
	int	oldest_folder_is_find = 0;
	int filesize,len,read_len;
	st_store_folder_list t_folder_list;
	st_store_folder_list* pt_folder_list;

	if(sgt_disk_status != STORE_HDISK_MOUNTED )
		return -1;

	min_id = 65535;
	max_id = 0;

	///�����ļ�������ļ������ھʹ��������ھʹ�
	sprintf(c_buf_name1,"%s/%s",DF_STORE_HDISK_AV,DF_STORE_FOLDER_LIST_NAME);
	floder_fd = fopen(c_buf_name1, "a+");
	if( floder_fd == 0 )
	{
		gt_store_hdisk.disk_state = STORE_HDISK_ERROR;
		SAMPLE_PRT("open file err\n");
		return -1;
	}
	
	///������ʱ�ļ�
	sprintf(c_buf_name2,"%s/%s_temp",DF_STORE_HDISK_AV,DF_STORE_FOLDER_LIST_NAME);
	floder_fd_temp= fopen(c_buf_name2, "w+");
	if( floder_fd_temp == 0 )
	{
		gt_store_hdisk.disk_state = STORE_HDISK_ERROR;
		SAMPLE_PRT("open file err\n");
		return -1;
	}
	
	i_init_folder = 1;
	///�鿴�ļ�����
	fseek(floder_fd, 0, SEEK_END);
	filesize = ftell(floder_fd);
	if( filesize >= DF_STORE_FOLDER_LIST_LEN )
	{
		/////////////////////////////////////////////////////////////////////////////
		fseek(floder_fd, 0, SEEK_SET);
		len = filesize;
		read_len= 0;

		///��folderlist���ҵ����ݵĿ�ʼλ�úͽ���λ�ã����ҵ����������ļ���
		for( i = 0; i < filesize/DF_STORE_FOLDER_LIST_LEN; i++)
		{
			if(read_len == 0)
			{
				if( len > sizeof(uc_file_buf) )
				{
					read_len = sizeof(uc_file_buf);
				}
				else
				{
					read_len = len;
				}
				len -= read_len;
				fread(uc_file_buf,read_len,1,floder_fd);

				///����������ļ��У�������ɾ��
				j = 0;
				if(oldest_folder_is_find < 2)
				{
					for(j=0; j<read_len; j+=DF_STORE_FOLDER_LIST_LEN )
					{
						pt_folder_list = (st_store_folder_list*)&uc_file_buf[j];
						if( memcmp( pt_folder_list->head, DF_STORE_MAGIC_FOLDER_LIST, sizeof(DF_STORE_MAGIC_FOLDER_LIST) ) == 0 )
						{
							++oldest_folder_is_find;
							///�ҵ��ĵ�һ���ļ���ɾ����
							if(oldest_folder_is_find == 1)
							{
								store_create_foldernvr_name(c_buf,utc_from_bcd(pt_folder_list->time_bcd));
								disk_remove_dir(c_buf);
							}
							///�ڶ����ļ��е�����Ϊ��������
							else if(oldest_folder_is_find == 2)
							{
								gt_store_hdisk.read_index = pt_folder_list->start_index;
								break;
							}
						}
					}
				}
				if(oldest_folder_is_find >= 2)
				{
					fwrite(uc_file_buf+j,read_len-j,1,floder_fd_temp);
				}
			}
		}
		/////////////////////////////////////////////////////////////////////////////
	}
	if( oldest_folder_is_find < 2 )
	{
		gt_store_hdisk.read_index = DF_STORE_WRITE_INDEX_START;
		gt_store_hdisk.write_index= DF_STORE_WRITE_INDEX_START;
	}
	fclose(floder_fd);
	fclose(floder_fd_temp);
	remove(c_buf_name1);
	///�������ļ�����
	rename( c_buf_name2, c_buf_name1 );
	return 0;
}


/*********************************************************************************
  *��������:static int store_close_file_list(void)
  *��������:��file_list�ļ���Ϣ���б���͹رգ�ͬʱ�����ļ��б��ļ�
  *��	��: close_channel	����Ҫ�����ͨ����ţ�bit0��ʾͨ��0��bit1��ʾͨ��1��........
  *��	��: none
  *�� �� ֵ:0����ʾ�������أ�-1����ʾ�����˴���1��ʾ�洢����Ч������
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
static int store_close_file_list(void)
{
	u8	 uc_buf[128];
	
	if(gt_store_hdisk.write_file_list_fd)
	{
		///����nvr�ļ�listͷ		
		store_create_file_list_head((char *)uc_buf);
		fseek(gt_store_hdisk.write_file_list_fd,0,SEEK_SET);
		fwrite(uc_buf,DF_STORE_FILE_LIST_HEAD_LEN,1,gt_store_hdisk.write_file_list_fd);
		fclose(gt_store_hdisk.write_file_list_fd);
		gt_store_hdisk.write_file_list_fd = 0;
		return 1;
	}
	return 0;
}



/*********************************************************************************
  *��������:static int store_close_channel_nvr(u16 close_channel)
  *��������:��ָ��ͨ����¼���ļ���Ϣ���б���͹رգ�ͬʱ�����ļ��б��ļ�
  *��	��: close_channel	����Ҫ�����ͨ����ţ�bit0��ʾͨ��0��bit1��ʾͨ��1��........
  *��	��: none
  *�� �� ֵ:0����ʾ�������أ�-1����ʾ�����˴���1��ʾ�洢����Ч������
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
static int store_close_channel_nvr(u16 close_channel)
{
	u8	i;
	s8	c_buf[128];
	u8	uc_buf[128];
	u32	ul_len,ul_temp1;
	
	for(i=0; i<DF_STORE_CHANNEL_MAX; i++)
	{
		if( close_channel & (BIT( i )) )
		{
			if( gt_store_hdisk.channel_param[i].file_len )
			{
				if(( gt_store_hdisk.channel_param[i].write_file_fd ) && (gt_store_hdisk.write_file_list_fd))
				{
					///����nvr�ļ�listͷ
					store_create_nvr_list_head(i,uc_buf);

					///д��ͷ��Ϣ
					fseek(gt_store_hdisk.channel_param[i].write_file_fd,0,SEEK_SET);
					fwrite(uc_buf,DF_STORE_NVR_LIST_HEAD_LEN,1,gt_store_hdisk.channel_param[i].write_file_fd);
					///�ر��ļ�������վ��
					fclose( gt_store_hdisk.channel_param[i].write_file_fd );
					gt_store_hdisk.channel_param[i].write_file_fd = 0;
					
					///���ļ���������
					store_create_nvr_name(&gt_store_hdisk.channel_param[i],c_buf);
					///�������ļ�����
					rename( gt_store_hdisk.channel_param[i].write_file, c_buf );
					strcpy( gt_store_hdisk.channel_param[i].write_file, c_buf );

					///����ļ��б�����
					store_create_file_list(&gt_store_hdisk.channel_param[i],uc_buf);
					
					///���ļ���Ϣд���б��ļ���
					//ul_temp1 = gt_store_hdisk.file_list_head.file_num;
					//ul_temp1 = DF_STORE_FILE_LIST_LEN * ul_temp1 + DF_STORE_FILE_LIST_HEAD_LEN;
					//disk_write(gt_store_hdisk.write_file_list_fd,ul_temp1,uc_buf,DF_STORE_FILE_LIST_LEN);
					fwrite(uc_buf,DF_STORE_FILE_LIST_LEN,1,gt_store_hdisk.write_file_list_fd);
					///�ı��ļ��б�ͷ��Ϣ������
					gt_store_hdisk.file_list_head.file_num++;
					gt_store_hdisk.file_list_head.filelist_size += gt_store_hdisk.channel_param[i].file_len;
					if(utc_from_bcd(gt_store_hdisk.file_list_head.time_end_bcd) < gt_store_hdisk.channel_param[i].time_end)
						utc_to_bcd(gt_store_hdisk.file_list_head.time_end_bcd ,gt_store_hdisk.channel_param[i].time_end);
					if(utc_from_bcd(gt_store_hdisk.file_list_head.time_start_bcd) == 0)
						utc_to_bcd(gt_store_hdisk.file_list_head.time_start_bcd ,gt_store_hdisk.channel_param[i].time_start);
				}
			}
		}
	}
	return 0;
}


/*********************************************************************************
  *��������:static FILE * store_search_file_list_head( char * list_name,st_store_file_list_head * p_list_head )
  *��������:����ָ����·���ļ��µ�file_list����ͷ��Ϣ�������p_list_head�е�����
  *��	��: list_name	��file_list�ļ�·������
  			p_list_head	��Ҫ���������ݰ�ͷָ��
  *��	��: none
  *�� �� ֵ:0����ʾʧ�ܣ���0��ʾ�ɹ�������file_list�ļ�
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-07-24	����
*********************************************************************************/
static FILE * store_search_file_list_head( char * list_name,st_store_file_list_head * p_list_head )
{
	u8 		i;
	u32 	ul_len,ul_temp1;
	u32		filesize;
	int		i_init_file_list;
	st_store_av_manage *p_av_manage;
	st_store_file_list	ft_file_list;
	FILE *	file_list_fd = 0;
	
	file_list_fd = fopen(list_name, "a+");
	if( file_list_fd == 0 )
	{
		SAMPLE_PRT("open file err\n");
		return 0;
	}
	memset(p_list_head,0,sizeof(st_store_file_list_head));
	
	i_init_file_list = 1;
	///�鿴�ļ����ȣ�����filelistͷ�������ȡͷ
	fseek( file_list_fd, 0, SEEK_END );  
	filesize = ftell( file_list_fd );
	if( filesize > DF_STORE_FILE_LIST_HEAD_LEN )
	{
		fseek( file_list_fd, 0, SEEK_SET );
		fread( (u8 *)p_list_head, DF_STORE_FILE_LIST_HEAD_LEN, 1, file_list_fd );
		///������ݰ�ͷ����Ҫ������Ҫ��ʼ�����ļ�������Ĳ���ֱ�����漴��
		if(memcmp(p_list_head->head,DF_STORE_MAGIC_FILE_LIST_HEAD,sizeof(DF_STORE_MAGIC_FILE_LIST_HEAD)) == 0)
		{
			if( p_list_head->file_num != ( filesize - DF_STORE_FILE_LIST_HEAD_LEN ) / DF_STORE_FILE_LIST_LEN )
			{
				if( (filesize-DF_STORE_FILE_LIST_HEAD_LEN)%DF_STORE_FILE_LIST_LEN == 0 )
				{
					fseek(file_list_fd, filesize-DF_STORE_FILE_LIST_LEN, SEEK_SET);
					fread( (u8 *)&ft_file_list, DF_STORE_FILE_LIST_LEN, 1, file_list_fd );
					if(memcmp(ft_file_list.head,DF_STORE_MAGIC_FILE_LIST,sizeof(DF_STORE_MAGIC_FILE_LIST)) == 0)
					{
						p_list_head->write_index_end 	= ft_file_list.write_index_end;
						p_list_head->filelist_size 		= ( p_list_head->write_index_end + gt_store_hdisk.disk_size_av - DF_STORE_WRITE_INDEX_START- p_list_head->write_index_start + 1)%(gt_store_hdisk.disk_size_av - DF_STORE_WRITE_INDEX_START);
						p_list_head->file_num			= ( filesize - DF_STORE_FILE_LIST_HEAD_LEN ) / DF_STORE_FILE_LIST_LEN;
						memcpy(p_list_head->time_end_bcd,ft_file_list.time_end_bcd,6);
					}
				}
			}
			i_init_file_list = 0;
		}
	}
	fseek(file_list_fd, 0, SEEK_END); 
	strcpy(p_list_head->head,DF_STORE_MAGIC_FILE_LIST_HEAD);
	return file_list_fd;
}


/*********************************************************************************
  *��������:static int store_save_channel_nvr(u16 save_channel)
  *��������:��ָ��ͨ����¼���ļ����б���
  *��	��: save_channel	����Ҫ�����ͨ����ţ�bit0��ʾͨ��0��bit1��ʾͨ��1��........
  *��	��: none
  *�� �� ֵ:0����ʾ�������أ�-1����ʾ�����˴���1��ʾ�洢����Ч������
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
static int store_save_channel_nvr(u16 save_channel)
{
	u8 		i;
	u8	 	uc_buf[128];
	s8	 	c_buf[128];
	u32 	ul_len,ul_temp1;
	u32		filesize;
	int		i_init_file_list;
	st_store_av_manage *p_av_manage;

	///��ǰ�ļ����Ѿ����������򷵻ش���
	if( strlen(gt_store_hdisk.write_folder) == 0 )
	{
		SAMPLE_PRT("folder err\n");
		return -1;
	}
	for(i=0; i<DF_STORE_CHANNEL_MAX; i++)
	{
		if( save_channel & (BIT( i )) )
		{
			p_av_manage = &gt_store_av_manage[i];
			///���д��nvr���ݵ��ļ������ڣ�����Ҫ�������ļ�
			if( gt_store_hdisk.channel_param[i].write_file_fd == 0 )
			{
				///�������
				memset((u8 *)&gt_store_hdisk.channel_param[i],0,sizeof(st_store_channel_param));
				memcpy((u8 *)&gt_store_hdisk.channel_param[i].style, (u8 *)&p_av_manage->style,sizeof(en_store_style)+sizeof(st_store_video_param)+sizeof(st_store_audio_param));
				gt_store_hdisk.channel_param[i].time_start 	= gt_store_hdisk.channel_param[i].write_time;
				gt_store_hdisk.channel_param[i].time_end	= gt_store_hdisk.channel_param[i].write_time;
				gt_store_hdisk.channel_param[i].write_index_start 	= gt_store_hdisk.write_index;
				gt_store_hdisk.channel_param[i].write_index_end		= gt_store_hdisk.write_index;
				///�����ļ�����
				store_create_nvr_name( &gt_store_hdisk.channel_param[i], gt_store_hdisk.channel_param[i].write_file );

				///�����ļ�
				gt_store_hdisk.channel_param[i].write_file_fd = fopen(gt_store_hdisk.channel_param[i].write_file, "w+");
				if( gt_store_hdisk.channel_param[i].write_file_fd == 0 )
				{
					gt_store_hdisk.disk_state = STORE_HDISK_ERROR;
					SAMPLE_PRT("open file err\n");
					return -1;
				}
				///д��nvr�ļ��б�ͷ��Ϣ
				store_create_nvr_list_head(i,uc_buf);
				fwrite(uc_buf,DF_STORE_NVR_LIST_HEAD_LEN,1,gt_store_hdisk.channel_param[i].write_file_fd);
			}
			///���д��filelist���ļ������ڣ���Ҫ��������ʼ�����ļ�
			if( gt_store_hdisk.write_file_list_fd == 0 )
			{
				///�����ļ�������ļ������ھʹ��������ھʹ�
				sprintf(c_buf,"%s/%s",gt_store_hdisk.write_folder,DF_STORE_FILE_LIST_NAME);
				gt_store_hdisk.write_file_list_fd = store_search_file_list_head(c_buf,&gt_store_hdisk.file_list_head);
				if(gt_store_hdisk.write_file_list_fd == 0)
				{
					gt_store_hdisk.disk_state = STORE_HDISK_ERROR;
					SAMPLE_PRT("open file err\n");
					return -1;
				}
				///���д����б�ͷ����
				if(gt_store_hdisk.file_list_head.write_index_start == 0)
				{
					utc_to_bcd(gt_store_hdisk.file_list_head.time_start_bcd, gt_store_hdisk.channel_param[i].time_start);
					utc_to_bcd(gt_store_hdisk.file_list_head.time_end_bcd, gt_store_hdisk.channel_param[i].time_end);
					gt_store_hdisk.file_list_head.write_index_start = gt_store_hdisk.channel_param[i].write_index_start;
					gt_store_hdisk.file_list_head.write_index_end	= gt_store_hdisk.channel_param[i].write_index_end;
					store_create_file_list_head((char *)uc_buf);
					fwrite(uc_buf,DF_STORE_FILE_LIST_HEAD_LEN,1,gt_store_hdisk.write_file_list_fd);
				}
			}
			///���Ҫд���nvr�б���Ϣ
			gt_store_hdisk.channel_param[i].file_len++;
			gt_store_hdisk.channel_param[i].time_end			= gt_store_hdisk.channel_param[i].write_time;
			gt_store_hdisk.channel_param[i].write_index_end		= gt_store_hdisk.write_index;
			store_create_nvr_list(i,uc_buf);
			fwrite(uc_buf,DF_STORE_NVR_LIST_LEN,1,gt_store_hdisk.channel_param[i].write_file_fd);
		}
	}
	return 0;
}



/*********************************************************************************
  *��������:int store_av_init(void)
  *��������:��ʼ��Ӳ�̴洢��ز���
  *��	��: none
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
int store_nvr_init(void)
{
	int	i,j,k,i_index;
	int max_id,min_id;
	FILE *	floder_fd;
	s8	 	c_buf[128];
	u8		uc_file_buf[DF_STORE_FOLDER_LIST_LEN*128];		///��buf���ȱ���Ϊ DF_STORE_FOLDER_LIST_LEN ��������
	int	i_init_folder;
	int filesize,len,read_len;
	en_hdisk_state		disk_state;
	st_store_folder_list t_folder_list;
	st_store_folder_list* pt_folder_list;
	
	if(gt_store_hdisk.disk_state != STORE_HDISK_MOUNTED )
		return -1;
	disk_state = gt_store_hdisk.disk_state;
	memset(gt_store_hdisk, 0, sizeof(gt_store_hdisk));
	gt_store_hdisk.disk_state = disk_state;
	gt_store_hdisk.disk_size_av = sgt_disk_size;

	min_id = 65535;
	max_id = 0;

	///�����ļ�������ļ������ھʹ��������ھʹ�
	sprintf(c_buf,"%s/%s",DF_STORE_HDISK_AV,DF_STORE_FOLDER_LIST_NAME);
	floder_fd = fopen(c_buf, "a+");
	if( floder_fd == 0 )
	{
		gt_store_hdisk.disk_state = STORE_HDISK_ERROR;
		SAMPLE_PRT("open file err\n");
		return -1;
	}
	
	i_init_folder = 1;
	///�鿴�ļ�����
	fseek(floder_fd, 0, SEEK_END);
	filesize = ftell(floder_fd);
	if( filesize >= DF_STORE_FOLDER_LIST_LEN )
	{
		i_init_folder = 0;
		/*
		///�������folderlist���ҵ����ݵĿ�ʼλ��
		fseek(floder_fd, 0, SEEK_SET);
		fread((u8 *)&t_folder_list,DF_STORE_FOLDER_LIST_LEN,1,floder_fd);
		if(memcmp(t_folder_list.head,"GP01",4) == 0)
		{
			if(t_folder_list.start_index == DF_STORE_WRITE_INDEX_START)
				gt_store_hdisk.read_index = gt_store_hdisk.disk_size_av;
			else
				gt_store_hdisk.read_index--;
		}
		///������folderlist���ҵ����ݵĽ���λ�ã������������ļ���
		fseek(floder_fd, filesize - DF_STORE_FOLDER_LIST_LEN, SEEK_SET);
		fread((u8 *)&t_folder_list,DF_STORE_FOLDER_LIST_LEN,1,floder_fd);
		if(memcmp(t_folder_list.head,"GP01",4) == 0)
		{
			gt_store_hdisk.write_index = t_folder_list.start_index;
			gt_store_hdisk.write_folder_day = utc_from_bcd(t_folder_list.time_bcd);
		}
		*/
		/////start ��δ�����������ε���ͬ���ܣ�ֻ�������˱����������ܼ�⵽�ļ�����
		fseek(floder_fd, 0, SEEK_SET);
		
		len = filesize;
		read_len= 0;

		///��folderlist���ҵ����ݵĿ�ʼλ�úͽ���λ�ã����ҵ����������ļ���
		for( i = 0; i < filesize/DF_STORE_FOLDER_LIST_LEN; i++)
		{
			if(read_len == 0)
			{
				if( len > sizeof(uc_file_buf) )
				{
					read_len = sizeof(uc_file_buf);
				}
				else
				{
					read_len = len;
				}
				len -= read_len;
				fread(uc_file_buf,read_len,1,floder_fd);
				i_index = 0;
			}
			if( read_len )
			{
				pt_folder_list = (st_store_folder_list*)&uc_file_buf[i_index*DF_STORE_FOLDER_LIST_LEN];
				if( memcmp( pt_folder_list->head, DF_STORE_MAGIC_FOLDER_LIST, sizeof(DF_STORE_MAGIC_FOLDER_LIST) ) == 0 )
				{
					if(pt_folder_list->folder_ID < min_id)
						min_id = pt_folder_list->folder_ID;
					if(pt_folder_list->folder_ID > max_id)
					{
						max_id = pt_folder_list->folder_ID;
						gt_store_hdisk.write_folder_day = utc_from_bcd(pt_folder_list->time_bcd);
					}
					if( i == 0 )
					{
						gt_store_hdisk.read_index = pt_folder_list->start_index;
					}
					gt_store_hdisk.write_index = pt_folder_list->start_index;
				}
				else
				{
					gt_store_hdisk.disk_state = STORE_HDISK_ERROR;
					SAMPLE_PRT("folder_list_error!\n");
					return -1;
				}
				read_len -= DF_STORE_FOLDER_LIST_LEN;
				i_index++;
			}
		}
		//////end

		///����ҵ������һ�죬��������һ��������ҵ����д���indexλ��
		if( gt_store_hdisk.write_folder_day )
		{
			///�����ļ�������
			store_create_foldernvr_name(gt_store_hdisk.write_folder,gt_store_hdisk.write_folder_day);
			
			///�����ļ����ڲ���file_list�ļ����ƣ�Ȼ��򿪲�������Ϣ���������file_listͷ�ṹ����Ϣ
			sprintf(c_buf,"%s/%s",gt_store_hdisk.write_folder,DF_STORE_FILE_LIST_NAME);
			gt_store_hdisk.write_file_list_fd = store_search_file_list_head(c_buf,&gt_store_hdisk.file_list_head);
			if(gt_store_hdisk.write_file_list_fd == 0)
			{
				gt_store_hdisk.disk_state = STORE_HDISK_ERROR;
				SAMPLE_PRT("open file err\n");
				return -1;
			}
			///�ҵ�����������λ�ã���¼�� gt_store_hdisk.write_index 
			if( gt_store_hdisk.file_list_head.write_index_end )
			{
				gt_store_hdisk.write_index = gt_store_hdisk.file_list_head.write_index_end;
				gt_store_hdisk.write_index++;
				if(gt_store_hdisk.write_index > gt_store_hdisk.disk_size_av)
					gt_store_hdisk.write_index = DF_STORE_WRITE_INDEX_START;
			}
		}
	}
	///���д����б�ͷ����
	if(i_init_folder)
	{
		gt_store_hdisk.write_index = DF_STORE_WRITE_INDEX_START;
		gt_store_hdisk.read_index  = DF_STORE_WRITE_INDEX_START;
		gt_store_hdisk.write_folder_day = sgt_utc_now / 86400 * 86400;
		store_create_foldernvr_name(gt_store_hdisk.write_folder,gt_store_hdisk.write_folder_day);
		disk_create_dir(gt_store_hdisk.write_folder);
	}
	fclose(floder_fd);

	///������
	if(0 == gt_store_hdisk.write_hdisk_fd)
	{
		gt_store_hdisk.write_hdisk_fd = open(DF_STORE_HDISK_DIR_NAME, O_RDWR );
		if(gt_store_hdisk.write_hdisk_fd == 0)
		{
			gt_store_hdisk.disk_state = STORE_HDISK_ERROR;
			SAMPLE_PRT("open file err\n");
			return -1;
		}
	}
	return 0;
}


/*********************************************************************************
  *��������:static int store_save_proc(void)
  *��������:���ݴ洢�������ú���ʵ��Ӳ�����ݵĴ洢��
  *��	��: none
  *��	��: none
  *�� �� ֵ:0����ʾ�������أ�-1����ʾ�����˴���1��ʾ�洢����Ч������
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
static int store_save_proc(void)
{
	int i,j,k;
	st_store_av_manage *p_av_manage;
	s64	s64_write_addr;
	int	new_file;
	int	save_channel_file;
	int	close_file;
	u8	 uc_buf[128];
	u32 	ul_len,ul_temp1;
	
	if(gt_store_hdisk.disk_state != STORE_HDISK_MOUNTED )
	{
		return -1;
	}

	if(gt_store_hdisk.write_folder_day == 0)
	{
		if(store_nvr_init() != 0)
			return -1;
	}
	if(gt_store_hdisk.write_folder_day != sgt_utc_now / 86400 * 86400)
	{
		store_close_channel_nvr( 0xFFFF );
		store_close_file_list();
		gt_store_hdisk.write_folder_day = sgt_utc_now / 86400 * 86400;
		store_create_foldernvr_name(gt_store_hdisk.write_folder,gt_store_hdisk.write_folder_day);
		disk_create_dir(gt_store_hdisk.write_folder);
	}
	for(i=0;i<DF_STORE_CHANNEL_MAX;i++)
	{
		p_av_manage = &gt_store_av_manage[i];
		save_channel_file  = 0;
		close_file = 0;
		///��ѯ�Ƿ�����Ҫд������ݣ��еĻ�ֱ����д��
		for(j=0;j<2;j++)
		{
			if( p_av_manage->data_av.frame_style[j] & (BIT(7)) )
			{
				p_av_manage->data_av.frame_style[j] &= ~(BIT(7));
				p_av_manage->data_av.av_buf_rd = DF_STORE_AV_PACK_SIZE * ( 1 - j );
				p_av_manage->data_av.av_len %= DF_STORE_AV_PACK_SIZE;
				gt_store_hdisk.channel_param[i].write_frame_style = p_av_manage->data_av.frame_style[j];
				gt_store_hdisk.channel_param[i].write_time = p_av_manage->data_av.time[j];
				if(gt_store_hdisk.write_hdisk_fd)
				{
					///д��Ӳ�����ݣ�������д��λ����Ϣ
					s64_write_addr = gt_store_hdisk.write_index;
					s64_write_addr *= DF_STORE_AV_PACK_SIZE;
					disk_write(gt_store_hdisk.write_hdisk_fd, s64_write_addr, p_av_manage->data_av.av_buf + j * DF_STORE_AV_PACK_SIZE, DF_STORE_AV_PACK_SIZE);
					
					///�����ǰ����ĺ�֮ǰ��¼���ʽ����ͬ���򱣴��ϸ�ʽ��Ϣ
					if( memcmp((u8 *)&gt_store_hdisk.channel_param[i].style, (u8 *)&p_av_manage->style,sizeof(en_store_style)+sizeof(st_store_video_param)+sizeof(st_store_audio_param)) )
					{
						///�����ϸ�ʽ¼����Ϣ
						store_close_channel_nvr( BIT(i) );
					}

					///��鲢д���µĴ洢��Ϣ
					store_save_channel_nvr( BIT(i) );

					///���nvr�����Ƿ�����(�����ļ���ʱ�䣬���С����)��������ˣ��򱣴�nvr�ļ���������file_list�ļ�
					if(gt_store_hdisk.write_limit_mode)
					{
						if( gt_store_hdisk.channel_param[i].file_len >= gt_store_hdisk.write_size_max )
						{
							store_close_channel_nvr( BIT(i) );
						}
					}
					else
					{
						if( gt_store_hdisk.channel_param[i].time_end - gt_store_hdisk.channel_param[i].time_start >= gt_store_hdisk.write_time_max )
						{
							store_close_channel_nvr( BIT(i) );
						}
					}

					///����д��λ�ü�1��Ȼ���ж��Ƿ�Խ��
					gt_store_hdisk.write_index++;
					if(	gt_store_hdisk.write_index > gt_store_hdisk.disk_size_av )
					{
						gt_store_hdisk.write_index = DF_STORE_WRITE_INDEX_START;
					}
					if(	gt_store_hdisk.write_index == gt_store_hdisk.read_index)
					{
						store_remove_oldest_folder();
					}
					break;
				}
			}
		}
	}
	
	return 0;
}


/*********************************************************************************
  *��������:void* store_thread_proc(void *p)
  *��������:���ݴ洢�̣߳����߳�ʵ��Ӳ�����ݵĴ洢��
  *��	��: p		:���ݵĲ������ò��������ͱ���Ϊ	st_gps_thread_param
  *��	��: none
  *�� �� ֵ:�̶�����Ϊ	NULL
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
void* store_thread_proc(void *p)
{
	fd_set			readfds, testfds;
    int				i;
    int				pipe_fd;
    int				res;
	char 			buffer[64];
	static 	int		bytes_read = 0;
    struct timeval TimeoutVal;
	
    st_gps_thread_param	*pt_para;
	
	pt_para = (st_gps_thread_param*)p;

	pipe_fd = gt_store_fifo_fd;
	
    FD_ZERO(&readfds);
    FD_SET(pipe_fd, &readfds);

    while( pt_para->thread_start)
	{
        testfds = readfds;
		///TimeoutVal.tv_usec	= 0;
		TimeoutVal.tv_usec	= 100000;
		TimeoutVal.tv_sec	= 0;
		res = select(pipe_fd+1, &testfds, (fd_set *)0, (fd_set *)0, &TimeoutVal);
		//printf( "S" );
		//fflush(stdout);
        if(res < 0)
        {
            perror("server5");
            return;
        }
        sgt_utc_now = rtc_get_time(0);
        if( FD_ISSET(pipe_fd,&testfds) )
        {
            do
            {
            	memset(buffer,0,sizeof(buffer));
                res = read(pipe_fd, buffer, sizeof(buffer)-1);
                bytes_read += res;
				if( res > 0 )
	                printf("FIFO:%s\n", buffer );
            }
            while(res > 0);
        }
    }
}


/*********************************************************************************
  *��������:int store_thread_Start(char c_val)
  *��������:�����洢¼���߳�
  *��	��: none
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
int store_thread_Start(char c_val)
{
	s32 i_ret;
	
	gt_store_thread_para.thread_start = TRUE;
	gt_store_thread_para.thread_param = c_val;
    i_ret = pthread_create(&gt_store_thread_pid, 0, store_thread_proc, (void*)&gt_store_thread_para);
	return i_ret;
}


/*********************************************************************************
  *��������:int store_fifo_write( char *p)
  *��������:���߳�store�������ܵ������ַ���
  *��	��: p		:���ݵ��ַ���
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
int store_fifo_write( char *p)
{
    int i_size;
	//pt_para->thread_start = FALSE;
	if(gt_store_fifo_fd>0)
	{
		i_size=write(gt_store_fifo_fd,p,strlen(p));
		if(i_size < 0)
		{
			//printf("%s: ERROR1!\n" __FUNCTION__);
			printf("%s: ERROR1!\n", "store_fifo_write");
	        return -1;
		}
		//printf("%s:send->%s\n" __FUNCTION__,p);
		printf("%s:send->%s\n", "store_fifo_write",p);
		return 0;
	}
	//printf("%s: ERROR2!\n" __FUNCTION__);
	printf("%s: ERROR2!\n", "store_fifo_write");
	return 1;
}


/*********************************************************************************
  *��������:int store_creat_fifo(void)
  *��������:����һ�������ܵ�����������¼��洢�߳�
  *��	��: none
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
int store_creat_fifo(void)
{
	int ret;
    int                pipe_fd;
    //ret = mkfifo( FIFO_NAME, S_IFIFO | 0666 );
    remove(DF_STORE_FIFO_NAME);
    ret = mkfifo( DF_STORE_FIFO_NAME, 0666 );
    if (ret == 0)
	{
		printf("�ɹ������ܵ�!\n");
		pipe_fd = open(DF_STORE_FIFO_NAME, O_NONBLOCK | O_RDWR);
		
		printf("open=%d\n",pipe_fd);
	    if( pipe_fd==-1 )
	    {
	        printf("open pipe erro!");
	        return 2;
	    }
		else
		{
			printf("open pipe ok!");
		}
		gt_store_fifo_fd = pipe_fd;
		return store_thread_Start(1);
	} 
	else
    {
		printf("�����ܵ�ʧ��!\n");
		gt_store_fifo_fd = 0;
		return 1;
    }
}


/*********************************************************************************
  *��������:int store_creat_fifo(void)
  *��������:����¼��洢�������ܵ������ҽ���¼��洢�̣߳�����ɾ���ܵ�����
  *��	��: none
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
int store_destroy_fifo(void)
{
	int i_ret = 0;
	if(gt_store_fifo_fd)
	{
		gt_store_thread_para.thread_start = FALSE;
		usleep(10000);
	    pthread_join(gt_store_thread_pid, 0);
		close( gt_store_fifo_fd );
		gt_store_fifo_fd = 0;
		printf(" �߳̽�����_store!\n");
	}
	remove(DF_STORE_FIFO_NAME);
	return i_ret;
}


/*********************************************************************************
  *��������:int store_av_init(void)
  *��������:��ʼ��¼����صĲ���������
  *��	��: none
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
int store_av_init(void)
{
	memset(gt_store_av_manage, 0, sizeof(gt_store_av_manage));
}


/*********************************************************************************
  *��������:int store_init(void)
  *��������:��ʼ��¼�񲿷֣���ʼ¼�����Ҫ���øú���
  *��	��: none
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
int store_init(void)
{
	store_av_init( );
	memset(gt_store_hdisk, 0, sizeof(gt_store_hdisk));
	store_creat_fifo( );
}


/*********************************************************************************
  *��������:int store_end(void)
  *��������:����¼�񣬳����˳�ʱ����Ҫ���øú���
  *��	��: none
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
int store_end(void)
{
	store_av_init( );
	store_destroy_fifo( );
}



/*********************************************************************************
  *��������:static int store_save_av_data( st_store_buf *pt_store_buf, u8 * p_data,uint32_t len )
  *��������:�洢���ݵ�����Ƶbuf�У��ú���ֻ����store_save_video�����ڲ�����
  *��	��: pt_store_buf:д���Ŀ��buf
  			p		:д�������
  			len		:д��ĳ���
  *��	��: none
  *�� �� ֵ:	0:����д�룬 1:��ʾд��buf��������ͷ����д�룬2:��ǰ�벿��д�뵽�˺�벿��
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	��������
*********************************************************************************/
static int store_save_av_data( st_store_av_data *pt_store_buf, u8 * p_data,uint32_t len )
{
	u32	i;
	u32 ul_len;
	int i_ret = 0;
	u32	start_wr;

	if((!len)||(len>sizeof(pt_store_buf->av_buf)))
	{
		//printf("ERROR:%s ,len = %d", __FUNCTION__ ,len );
		printf("ERROR:%s ,len = %d", __func__ ,len );
		return 0;
	}
	start_wr = pt_store_buf->av_buf_wr;

	///��������˳��ȣ�����Ҫ��д��һ���֣�Ȼ���bufͷ������д�롣
	if(pt_store_buf->av_buf_wr + len > sizeof(pt_store_buf->av_buf))
	{
		ul_len = sizeof(pt_store_buf->av_buf) - pt_store_buf->av_buf_wr;
		memcpy( &( pt_store_buf->av_buf[pt_store_buf->av_buf_wr]), p_data, ul_len );
		len -= ul_len;
		p_data += ul_len;
		pt_store_buf->av_len	+= ul_len;
		pt_store_buf->av_buf_wr = 0;
	}
	///
	if(len)
	{
		memcpy(&(pt_store_buf->av_buf[pt_store_buf->av_buf_wr]),p_data,len);
		pt_store_buf->av_len	+= len;
		pt_store_buf->av_buf_wr += len;
	}
	///����Ƿ�д���µ�64K����ǰ���д��
	if((start_wr >= DF_STORE_AV_PACK_SIZE)&&(pt_store_buf->av_buf_wr < DF_STORE_AV_PACK_SIZE))
	{
		pt_store_buf->time[0] = 0;
		pt_store_buf->frame_style[0] = 0;
		///���û�м�⵽I֡(д��I֡�󣬸�ֵframe_styleΪ2)������Ϊ����p b֡
		pt_store_buf->frame_style[1] |= BIT(7) + BIT(0);
		return 1;
	}
	///����Ƿ�д���µ�64K���򣬺���д��
	else if((start_wr < DF_STORE_AV_PACK_SIZE)&&(pt_store_buf->av_buf_wr >= DF_STORE_AV_PACK_SIZE))
	{
		pt_store_buf->time[1] = 0;
		pt_store_buf->frame_style[1] = 0;
		///���û�м�⵽I֡(д��I֡�󣬸�ֵframe_styleΪ2)������Ϊ����p b֡
		pt_store_buf->frame_style[0] |= BIT(7) + BIT(0);
		return 2;
	}
	else
		return 0;
}


/*********************************************************************************
  *��������:int store_save_audio( st_store_audio_param *t_audio_param, u8 * p_data,uint32_t len )
  *��������:�洢��Ƶ���ݽӿں���
  *��	��: pt_audio_param:��Ƶ���ݲ���
  			p_data		:����ָ��buf
  			len			:���ݳ���
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	��������
*********************************************************************************/
int store_save_audio( st_store_audio_param *pt_audio_param, u8 * p_data,u32 len )
{
	st_store_audio_param  *pt_a_param;
	st_store_a_data	*pt_store_data;
	u32	i;
	u8	uc_channel;
	u32 ul_len;

	
	uc_channel = pt_audio_param->channel;
	uc_channel %= DF_STORE_CHANNEL_MAX;

	pt_a_param = &(gt_store_av_manage[uc_channel].param_audio);

	if(memcmp(pt_a_param,pt_audio_param,sizeof(st_store_audio_param)) != 0)
	{
		memset((u8 *)&(gt_store_av_manage[uc_channel].data_a),0,sizeof(st_store_a_data));
		memcpy(pt_a_param,pt_audio_param,sizeof(st_store_audio_param));
	}
	
	
	if((p_data[0] == 0x00) && (p_data[1] == 0x01) && (p_data[2] == 0x52) && (p_data[3] == 0x00) && ( len == 168 ))
	{
		pt_store_data = &(gt_store_av_manage[uc_channel].data_a);
		
		memcpy(pt_store_data->a_buf[pt_store_data->a_buf_wr], p_data, DF_STORE_AUDIO_FRAME_SIZE);
		pt_store_data->a_buf_wr++;
		pt_store_data->a_buf_wr = (pt_store_data->a_buf_wr + 1) % DF_STORE_AUDIO_FRAME_MAX;
		///��ǰ�ܵ���Ƶ������
		pt_store_data->a_len = ( pt_store_data->a_buf_wr + DF_STORE_AUDIO_FRAME_MAX - pt_store_data->a_buf_rd ) % DF_STORE_AUDIO_FRAME_MAX;
		return 0;
	}
	
	return 1;
}

/*********************************************************************************
  *��������:int store_save_video( st_store_video_param *tp_video_param, u8 * p_data,u32 len )
  *��������:�洢��Ƶ���ݽӿں���
  *��	��: tp_video_param:��Ƶ���ݲ���
  			p_data		:����ָ��buf
  			len			:���ݳ���
  *��	��: none
  *�� �� ֵ:	0:��������,	1:��ʾ64K�洢���������Խ����ݴ���Ӳ����	2:���δ�������ݸ�ʽ���ϴεĲ���ͬ
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	��������
*********************************************************************************/
int store_save_video( st_store_video_param *tp_video_param, u8 * p_data,u32 len )
{
	st_store_video_param  *pt_v_param;
	st_store_av_data	*pt_store_data;
	st_store_a_data		*pt_store_a_data;
	u32	i;
	u8  uc_ip_frame = 0;
	u8	uc_channel;
	u8	uc_proc_index = 0;
	static u8	uc_buf[256];
	u32 ul_av_len,ul_a_len,ul_v_len,ul_len;
	u32 ul_val;
	int i_ret = 0;

	
	uc_channel = tp_video_param->channel;
	uc_channel %= DF_STORE_CHANNEL_MAX;

	pt_v_param = &(gt_store_av_manage[uc_channel].param_video);

	if(memcmp(pt_v_param,tp_video_param,sizeof(st_store_video_param)) != 0)
	{
		memset((u8 *)&(gt_store_av_manage[uc_channel].data_av),0,sizeof(st_store_av_data));
		memcpy(pt_v_param,tp_video_param,sizeof(st_store_video_param));
		i_ret += 100;
	}
	
	if((p_data[0] == 0x00) && (p_data[1] == 0x00) && (p_data[2] == 0x00) && (p_data[3] == 0x01))
	{
		if( ( p_data[4] & 0x31 ) == 0x05 )
		{
			///�ҵ���p֡
			uc_ip_frame = 1;
		}
		else if(( p_data[4] & 0x31) == 0x01 )
		{
			///�ҵ���I֡
			uc_ip_frame = 2;
		}
		else
		{
			///�ҵ���������Ƶ֡
			uc_ip_frame = 3;
		}
	}
	//pt_store_data = &gt_store_mem[uc_channel];
	pt_store_data = &(gt_store_av_manage[uc_channel].data_av);
	pt_store_a_data = &(gt_store_av_manage[uc_channel].data_a);
	if( uc_ip_frame )
	{
		if(uc_ip_frame == 3)		///��֡��Ϣ����Ҫ�ݴ���v_buf��
		{
			memcpy(pt_store_data->v_buf + pt_store_data->v_buf_wr,p_data,len);
			pt_store_data->v_buf_wr += len;
		}
		else						///����֡��ֱ�Ӵ洢���������ݴ��֡��Ϣд��
		{
			pt_store_data->iswrite = 1;		///����д��buf
			///������Ƶ�ܳ���
			ul_v_len = pt_store_data->v_buf_wr + len;
			///������Ƶ�ܳ���
			//ul_a_len = pt_store_a_data->a_buf_wr;
			//ul_a_len = pt_store_a_data->a_len;
			if(pt_store_a_data->a_len > DF_STORE_AUDIO_DOUBLE)
			{
				ul_a_len = DF_STORE_AUDIO_FRAME_SIZE * 2;
			}
			else
			{
				ul_a_len = DF_STORE_AUDIO_FRAME_SIZE;
			}
			///��������Ƶ�ܳ���
			ul_av_len = ul_v_len + ul_a_len;
			///�����ͷ��Ϣ���֣��ò����ݶ�Ϊ16�ֽڶ���
			ul_len = ((pt_store_data->av_buf_wr + 15) & 0xFFFFFFF0) - pt_store_data->av_buf_wr;
			memset(uc_buf, 0, sizeof(uc_buf));
			i_ret += store_save_av_data( pt_store_data, uc_buf, ul_len );
			///����ͷ��Ϣ
			sprintf(uc_buf,"GPSAV00%d",uc_ip_frame);		///��ͷ�̶�����"GPSAV00"
			ul_len = 8;
			uc_buf[ul_len++] 	= (u8)ul_av_len;			///�ܳ���
			uc_buf[ul_len++] 	= (u8)(ul_av_len>>8);
			uc_buf[ul_len++] 	= (u8)(ul_av_len>>16);
			uc_buf[ul_len++] 	= (u8)ul_v_len;				///��Ƶ�ܳ���
			uc_buf[ul_len++] 	= (u8)(ul_v_len>>8);
			uc_buf[ul_len++] 	= (u8)(ul_v_len>>16);

			utc_to_bcd(uc_buf, sgt_utc_now);
			ul_len += 6;
			
			uc_buf[ul_len++] = gt_store_av_manage[uc_channel].param_video.channel;
			uc_buf[ul_len++] = gt_store_av_manage[uc_channel].param_video.e_pic_size;
			uc_buf[ul_len++] = gt_store_av_manage[uc_channel].param_video.e_video_norm;
			ul_val = (u32)gt_store_av_manage[uc_channel].param_video.e_payload_type;
			uc_buf[ul_len++] 	= (u8)ul_val;				///Ҫ���ص��ļ���ʽ
			uc_buf[ul_len++] 	= (u8)(ul_val>>8);
			uc_buf[ul_len++] = gt_store_av_manage[uc_channel].param_video.e_trige_type;			
			ul_val = (u32)gt_store_av_manage[uc_channel].param_audio.e_payload_type;
			uc_buf[ul_len++] 	= (u8)ul_val;				///Ҫ���ص��ļ���ʽ
			uc_buf[ul_len++] 	= (u8)(ul_val>>8);
			ul_len += 2;
			uc_buf[ul_len++] 	= 'E';						///��β�ַ�'E'
			uc_buf[ul_len++] 	= 'D';						///��β�ַ�'D'	
			///д���ͷ��Ϣ,����Ϊ16�ֽ�������
			i_ret += store_save_av_data( pt_store_data, uc_buf, ((ul_len + 15) & 0xFFF0) );
			///д����Ƶ����
			i_ret += store_save_av_data( pt_store_data, pt_store_data->v_buf, pt_store_data->v_len);
			i_ret += store_save_av_data( pt_store_data, p_data, len );
			///д����Ƶ����
			if(pt_store_a_data->a_buf_rd == pt_store_a_data->a_buf_wr)	///����Ƶ����
			{
				pt_store_a_data->a_len = 0;
				i = (pt_store_a_data->a_buf_wr + 2)%DF_STORE_AUDIO_FRAME_MAX;
				memset(pt_store_a_data->a_buf[i], 0, DF_STORE_AUDIO_FRAME_SIZE);
				i_ret += store_save_av_data( pt_store_data, pt_store_a_data->a_buf[i], DF_STORE_AUDIO_FRAME_SIZE);
			}
			else			///����Ƶ����
			{
				///��ǰ�ܵ���Ƶ������
				pt_store_a_data->a_len = ( pt_store_a_data->a_buf_wr + DF_STORE_AUDIO_FRAME_MAX - pt_store_a_data->a_buf_rd ) % DF_STORE_AUDIO_FRAME_MAX;
				///�������⵽��ǰ��Ƶ��̫�࣬��Ҫ��������������֤��3�����еİ���д��η��ʣ���ֹ��д��ͻ
				if( pt_store_a_data->a_len > ( DF_STORE_AUDIO_FRAME_MAX - 3 ) )
				{
					i = ( pt_store_a_data->a_buf_wr + 3 ) % DF_STORE_AUDIO_FRAME_MAX;
				}
				else
				{
					i = pt_store_a_data->a_buf_rd % DF_STORE_AUDIO_FRAME_MAX;
				}
				while(ul_a_len)
				{
					ul_a_len -= DF_STORE_AUDIO_FRAME_SIZE;
					i_ret += store_save_av_data( pt_store_data, pt_store_a_data->a_buf[i++], DF_STORE_AUDIO_FRAME_SIZE);
					i %=  DF_STORE_AUDIO_FRAME_MAX;
				}
				pt_store_a_data->a_buf_rd = i;
			}

			///д��ʱ��
			if(pt_store_data->av_buf_wr > DF_STORE_AV_PACK_SIZE)
			{
				uc_proc_index = 1;
			}
			else
			{
				uc_proc_index = 0;
			}
			///��ǰ����д���������I֡�����Ǹ����ݶ�����I֡
			if(uc_ip_frame == 2)
			{
				pt_store_data->frame_style[uc_proc_index] |= BIT(1);
				pt_store_data->time[uc_proc_index] = sgt_utc_now;
			}
			else if( 0 == pt_store_data->time[uc_proc_index])
			{
				pt_store_data->time[uc_proc_index] = sgt_utc_now;
			}
			///���д��״̬��Ϣ
			pt_store_data->v_len	= 0;
			pt_store_data->iswrite = 0;		///д�����
		}
	}
	if(i_ret >= 100)
		return 2;
	else if(i_ret)
	{
		
		return 1;
	}
	else
		return 0;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
