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


#define FIFO_NAME						"/tmp/cmd_pipe"



#define DF_STORE_AUDIO_DOUBLE		3			///��buf�е���Ƶ�������ڸ�ֵʱ����Ҫ��������


st_store_av_manage gt_store_av_manage[DF_STORE_CHANNEL_MAX];			///����Ľ�������Ƶ���ݴ�ȡ����Ľṹ��
st_store_av_data gt_store_mem[DF_STORE_CHANNEL_MAX];			///����Ľ�������Ƶ���ݴ�ȡ��buf
static pthread_t gt_store_thread_pid;					///¼�����ݴ洢�߳�PID
int		gt_store_fifo_fd = 0;							///¼�����ݴ洢�����ܵ��ľ��
static st_gps_thread_param	gt_store_thread_para;		///���Խӿ��̲߳���
st_store_hdisk		gt_store_hdisk;			///Ӳ��״̬��Ϣ�����Ӳ�̲���





/*********************************************************************************
  *��������:static int store_remove_oldest_files(void)
  *��������:¼��洢���ˣ���Ҫɾ�����ϵ����ݺ������Ϣʱ���øú���
  *��	��: none
  *��	��: none
  *�� �� ֵ:0����ʾ�������أ�-1����ʾ�����˴���1��ʾ�洢����Ч������
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
static int store_remove_oldest_files(void)
{
	return 0;
}




/*********************************************************************************
  *��������:static int store_remove_oldest_files(void)
  *��������:��ָ��ͨ����¼���ļ����б���
  *��	��: uc_channel	����Ҫ�����ͨ����ţ�bit0��ʾͨ��0��bit1��ʾͨ��1��........
  *��	��: none
  *�� �� ֵ:0����ʾ�������أ�-1����ʾ�����˴���1��ʾ�洢����Ч������
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
static int store_close_channel_nvr(u8 close_channel)
{
	u8 i;
	MYTIME	my_time1,my_time2;				///
	char folder_name[32];
	char file_name[64];
	u8	 uc_buf[128];
	s8	 c_buf[128];
	u32 	ul_len,ul_temp1;
	
	for(i=0; i<DF_STORE_CHANNEL_MAX; i++)
	{
		if( close_channel & (BIT( i )) )
		{
			if( gt_store_hdisk.channel_param[i].write_index )
			{
				if(( gt_store_hdisk.channel_param[i].write_file_fd ) && (gt_store_hdisk.write_file_list_fd))
				{
					///�ر��ļ�������վ��
					close( gt_store_hdisk.channel_param[i].write_file_fd );
					gt_store_hdisk.channel_param[i].write_file_fd = 0;

					///���ļ���������
					my_time1 = utc_to_mytime(gt_store_hdisk.channel_param[i].time_start );
					my_time2 = utc_to_mytime(gt_store_hdisk.channel_param[i].time_end );
					sprintf(c_buf,"%s\0000-%02d%02d%02d-%02d%02d%02d-%02d%02d%02d-%d-%d",
						gt_store_hdisk.write_folder,
						YEAR(my_time1),MONTH(my_time1),DAY(my_time1),
						HOUR(my_time1),MINUTE(my_time1),SEC(my_time1),
						HOUR(my_time2),MINUTE(my_time2),SEC(my_time2),
						gt_store_hdisk.channel_param[i].param_video.e_trige_type,
						gt_store_hdisk.channel_param[i].param_video.channel									
						);
					///�������ļ�����
					rename( gt_store_hdisk.channel_param[i].write_file, c_buf);

					///����ļ��б�����
					memset(uc_buf,0,sizeof(uc_buf));
					strcpy(uc_buf,"LIST");
					ul_len = 8;
					strcpy(uc_buf+8, c_buf);
					ul_len += 96;
					uc_buf[ul_len++] = (u8)(gt_store_hdisk.channel_param[i].write_index);
					uc_buf[ul_len++] = (u8)(gt_store_hdisk.channel_param[i].write_index >> 8);
					ul_temp1 = gt_store_hdisk.channel_param[i].time_start%86400;
					memcpy(&uc_buf[ul_len],(u8 *)&ul_temp1,4);
					ul_len += 4;
					ul_temp1 = gt_store_hdisk.channel_param[i].time_end%86400;
					memcpy(&uc_buf[ul_len],(u8 *)&ul_temp1,4);
					ul_len += 4;
					uc_buf[ul_len++] = (u8)(gt_store_hdisk.channel_param[i].param_video.channel);
					ul_len = 128;

					///���ļ���Ϣд���б��ļ���
					ul_temp1 = gt_store_hdisk.file_list_head.file_num;
					ul_temp1 = DF_STORE_FILE_LIST_LEN * ul_temp1 + DF_STORE_FILE_LIST_HEAD_LEN;
					disk_write(gt_store_hdisk.write_file_list_fd,ul_temp1,uc_buf,DF_STORE_FILE_LIST_LEN);

					///�ı��ļ��б�ͷ��Ϣ������
					gt_store_hdisk.file_list_head.file_num++;
					gt_store_hdisk.file_list_head.filelist_size += gt_store_hdisk.channel_param[i].write_index;
					if(gt_store_hdisk.file_list_head.time_end < gt_store_hdisk.channel_param[i].time_end)
						gt_store_hdisk.file_list_head.time_end = gt_store_hdisk.channel_param[i].time_end;
					if(gt_store_hdisk.file_list_head.time_start == 0)
						gt_store_hdisk.file_list_head.time_start = gt_store_hdisk.channel_param[i].time_start;
					
				}
			}
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
	int	close_file;
	MYTIME	my_time1,my_time2;				///
	char folder_name[32];
	char file_name[64];
	u8	 uc_buf[128];
	s8	 c_buf[128];
	u32 	ul_len,ul_temp1;
	
	if(gt_store_hdisk.disk_state != STORE_HDISK_MOUNTED )
	{
		return -1;
	}

	for(i=0;i<DF_STORE_CHANNEL_MAX;i++)
	{
		p_av_manage = &gt_store_av_manage[i];
		new_file  = 0;
		close_file = 0;
		///��ѯ�Ƿ�����Ҫд������ݣ��еĻ�ֱ����д��
		for(j=0;j<2;j++)
		{
			if(p_av_manage->data_av.frame_style[j])
			{
				p_av_manage->data_av.frame_style[j] = 0;
				p_av_manage->data_av.av_buf_rd = 0x10000 * ( 1 - j );
				p_av_manage->data_av.av_len %= 0x10000;
				if(gt_store_hdisk.write_hdisk_fd)
				{
					///д��Ӳ�����ݣ�������д��λ����Ϣ
					s64_write_addr = gt_store_hdisk.write_index;
					s64_write_addr *= 0x10000;
					disk_write(gt_store_hdisk.write_hdisk_fd, s64_write_addr, p_av_manage->data_av.av_buf + j * 0x10000, 0x10000);
					
					gt_store_hdisk.write_index++;
					if(	gt_store_hdisk.write_index >= gt_store_hdisk.disk_size_av )
					{
						gt_store_hdisk.write_index = DF_STORE_WRITE_INDEX_START;
						/////////////////////////////////����������ɾ�������¼����Ϣ�Ĵ���start
						store_remove_oldest_files();
						/////////////////////////////////����������ɾ�������¼����Ϣ�Ĵ���end
					}
					///���ɵ�ǰʱ���Ӧ�����������ļ���
					my_time1 = utc_to_mytime(p_av_manage->data_av.time[j]);
					
					sprintf(folder_name,"%s\%04d-%02d-%02d",DF_STORE_HDISK_AV,
						YEAR(my_time1)+2000,MONTH(my_time1),DAY(my_time1));
					if(strcmp(gt_store_hdisk.write_folder,folder_name))
					{
						disk_create_dir(folder_name);
						//strcpy(gt_store_hdisk.channel_param[i].write_folder,folder_name);
						new_file |= 2;
					}
					
					///��鲢д����Ϣ�����ֵ�ǰ����ĺ�֮ǰ�Ĳ���ͬ������Ҫ���������µ�¼����Ϣ�ļ�
					if( memcmp((u8 *)&gt_store_hdisk.channel_param[i].style, (u8 *)&p_av_manage->style,sizeof(en_store_style)+sizeof(st_store_video_param)+sizeof(st_store_audio_param)) )
					{
						new_file |= 1;
					}

					if( new_file )
					{
						if( gt_store_hdisk.channel_param[i].write_index )
						{
							if( gt_store_hdisk.channel_param[i].write_file_fd )
							{
								///�ر��ļ�������վ��
								close( gt_store_hdisk.channel_param[i].write_file_fd );
								gt_store_hdisk.channel_param[i].write_file_fd = 0;

								///���ļ���������
								my_time1 = utc_to_mytime(gt_store_hdisk.channel_param[i].time_start );
								my_time2 = utc_to_mytime(gt_store_hdisk.channel_param[i].time_end );
								sprintf(c_buf,"%s\0000-%02d%02d%02d-%02d%02d%02d-%02d%02d%02d-%d-%d",
									gt_store_hdisk.write_folder,
									YEAR(my_time1),MONTH(my_time1),DAY(my_time1),
									HOUR(my_time1),MINUTE(my_time1),SEC(my_time1),
									HOUR(my_time2),MINUTE(my_time2),SEC(my_time2),
									gt_store_hdisk.channel_param[i].param_video.e_trige_type,
									gt_store_hdisk.channel_param[i].param_video.channel									
									);
								///�������ļ�����
								rename( gt_store_hdisk.channel_param[i].write_file, c_buf);

								///����ļ��б�����
								memset(uc_buf,0,sizeof(uc_buf));
								strcpy(uc_buf,"LIST");
								ul_len = 8;
								strcpy(uc_buf+8, c_buf);
								ul_len += 96;
								uc_buf[ul_len++] = (u8)(gt_store_hdisk.channel_param[i].write_index);
								uc_buf[ul_len++] = (u8)(gt_store_hdisk.channel_param[i].write_index >> 8);
								ul_temp1 = gt_store_hdisk.channel_param[i].time_start%86400;
								memcpy(&uc_buf[ul_len],(u8 *)&ul_temp1,4);
								ul_len += 4;
								ul_temp1 = gt_store_hdisk.channel_param[i].time_end%86400;
								memcpy(&uc_buf[ul_len],(u8 *)&ul_temp1,4);
								ul_len += 4;
								uc_buf[ul_len++] = (u8)(gt_store_hdisk.channel_param[i].param_video.channel);
								ul_len = 128;
							}
						}
					}

					///Ҫ����
					if(!new_file)
					{
						gt_store_hdisk.channel_param[i].write_index++;
						gt_store_hdisk.channel_param[i].time_end = p_av_manage->data_av.time[j];

						///
						disk_write(gt_store_hdisk.channel_param[i].write_file_fd, s64_write_addr, p_av_manage->data_av.av_buf + j * 0x10000, 64);
						if(gt_store_hdisk.write_limit_mode)
						{
							if( gt_store_hdisk.channel_param[i].write_index >= gt_store_hdisk.write_size_max )
							{
								close_file = 1;
							}
						}
						else
						{
							if( gt_store_hdisk.channel_param[i].time_end - gt_store_hdisk.channel_param[i].time_start >= gt_store_hdisk.write_time_max )
							{
								close_file = 1;
							}
						}
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
    remove(FIFO_NAME);
    ret = mkfifo( FIFO_NAME, 0666 );
    if (ret == 0)
	{
		printf("�ɹ������ܵ�!\n");
		pipe_fd = open(FIFO_NAME, O_NONBLOCK | O_RDWR);
		
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
	remove(FIFO_NAME);
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
		if(pt_store_buf->frame_style[1] == 0)
		{
			pt_store_buf->frame_style[1] = 1;
		}
		return 1;
	}
	///����Ƿ�д���µ�64K���򣬺���д��
	else if((start_wr < DF_STORE_AV_PACK_SIZE)&&(pt_store_buf->av_buf_wr >= DF_STORE_AV_PACK_SIZE))
	{
		pt_store_buf->time[1] = 0;
		pt_store_buf->frame_style[1] = 0;
		///���û�м�⵽I֡(д��I֡�󣬸�ֵframe_styleΪ2)������Ϊ����p b֡
		if(pt_store_buf->frame_style[0] == 0)
		{
			pt_store_buf->frame_style[0] = 1;
		}
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
	utc_time			uct_time1;				///Ҫ�洢���������ɵ�ʱ��
	MYTIME				my_time1;				///
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
	
	uct_time1 = rtc_get_time(0);
	my_time1 = utc_to_mytime(uct_time1);
	if(my_time1)
	{
		printf("\n time:  %d-%d-%d %d:%d:%d",YEAR(my_time1)+2000,
				MONTH(my_time1),
				DAY(my_time1),
				HOUR(my_time1),
				MINUTE(my_time1),
				SEC(my_time1)
				);
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
	utc_time			uct_time1;				///Ҫ�洢���������ɵ�ʱ��
	MYTIME				my_time1;				///
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
	
	uct_time1 = rtc_get_time(0);
	my_time1 = utc_to_mytime(uct_time1);
	if(my_time1)
	{
		printf("\n time:  %d-%d-%d %d:%d:%d",YEAR(my_time1)+2000,
				MONTH(my_time1),
				DAY(my_time1),
				HOUR(my_time1),
				MINUTE(my_time1),
				SEC(my_time1)
				);
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

			if(my_time1)
			{
				uc_buf[ul_len++] = YEAR(my_time1);
				uc_buf[ul_len++] = MONTH(my_time1);
				uc_buf[ul_len++] = DAY(my_time1);
				uc_buf[ul_len++] = HOUR(my_time1);
				uc_buf[ul_len++] = MINUTE(my_time1);
				uc_buf[ul_len++] = SEC(my_time1);
			}
			else
			{
				ul_len += 6;
			}
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
			if(uc_ip_frame == 2)
			{
				if(0 == pt_store_data->frame_style[uc_proc_index])
				{
					pt_store_data->frame_style[uc_proc_index] = 2;
					pt_store_data->time[uc_proc_index] = uct_time1;
				}
			}
			else if( 0 == pt_store_data->time[uc_proc_index])
			{
				pt_store_data->time[uc_proc_index] = uct_time1;
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
