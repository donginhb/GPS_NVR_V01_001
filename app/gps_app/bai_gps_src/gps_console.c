/*********************************************************************************
  * @�ļ����� :gps_console.c
  * @�������� :�û������˻������Ľӿں�����������Ҫͨ������̨�����˻������Ĵ��붼������
  * @��	   �� :������
  * @�������� :2015-6-18
  * @�ļ��汾 :0.01
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-19	�����ļ�
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




#include "gps_typedef.h"
#include "common_func.h"
#include "gps_param.h"
#include "gps_disk_io.h"
#include "gps_store.h"
#include <gps_hi_rtc.h>


typedef int ( *DF_CONSOLE_PROC )( char *p, uint16_t len );


/*********************************************************************************
  *ö������:en_console_msg_style
  *��������:��ʾ����̨��Ϣ�Ĵ���ʽ
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
typedef enum _en_console_msg_style
{
	CONSOLE_MSG_NONE			=0,		///��������Ϣ������
	CONSOLE_MSG_STR,					///���ض�Ӧ���ַ���
	CONSOLE_MSG_FUNC, 					///�ô��������������
	CONSOLE_MSG_STR_FUNC, 				///���ض�Ӧ���ַ���,���ô��������������
}en_console_msg_style;


/*********************************************************************************
  *�� �� ��:st_console_msg
  *��������:��ʾһ������̨��Ϣ����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
typedef struct _st_console_msg
{
	char						*rx_str;		/*��������*/
	en_console_msg_style		type;           /*�ж��Ǵ����ַ����Ƚ�,��������Ӧ����*/
	DF_CONSOLE_PROC				proc_func;		/*������*/
	char						*tx_str;   		/*Ҫ�Ƚϵ��ַ���*/
}st_console_msg;


static int my_system2(const char * cmd) 
{ 
	FILE * fp; 
	int res; char buf[1024]; 
	if (cmd == NULL) 
	{ 
		printf("my_system cmd is NULL!\n");
		return -1;
	} 
	if ((fp = popen(cmd, "r") ) == NULL) 
	{ 
		perror("popen");
		printf("popen error: %s/n", strerror( 2 )); 
		return -1; 
	} 
	else
	{
		while(fgets(buf, sizeof(buf), fp)) 
		{ 
			printf("%s", buf); 
		} 
		if ( (res = pclose(fp)) == -1) 
		{ 
			printf("close popen file pointer fp error!\n"); 
			return res;
		} 
		else if (res == 0) 
		{
			return res;
		} 
		else 
		{ 
			printf("popen res is :%d\n", res); return res; 
		} 
	}
} 


int gps_send_cmd(char *cmd,char *rx_buf,int rx_max_len)
{
	int 		i,ret;
    int			bytes_read;
    int			pipe_fd;
    int			res;
	char 		name_gps_cmd[]="/tmp/gps_test_cmd_pipe";
	char 		buffer[128];
	
	//remove(name_gps_cmd);
    ret = mkfifo( name_gps_cmd, 0666 );
	/*
    if (ret == 0)
	{
		pipe_fd = open(name_gps_cmd, O_NONBLOCK | O_RDWR);
		
		printf("open=%d\n",pipe_fd);
	    if( pipe_fd==-1 )
	    {
	        printf("open pipe erro!");
	        goto RETT;
	    }
	} 
	else
    {
		printf("�����ܵ�ʧ��!\n");
		goto RETT;
    }
    */
	pipe_fd = open(name_gps_cmd, O_NONBLOCK | O_RDWR);
	sprintf(buffer,"%s > %s",cmd,name_gps_cmd);
	write(pipe_fd,"\n",1);
	//system(buffer);
	my_system2(buffer);
	write(pipe_fd,"\n",1);
	//usleep(10);
	bytes_read = 0;
	do
    {
    	memset(buffer,0,sizeof(buffer));
        res = read(pipe_fd, buffer, sizeof(buffer)-1);
        if(((void *)rx_buf != 0)&&(rx_max_len>1))
    	{
			for(i=0;i<res;i++)
			{
				rx_buf[bytes_read++] = buffer[i];
				if(bytes_read >= rx_max_len - 1)
				{
					rx_buf[bytes_read] = 0;
					goto RETT;
				}
			}
			rx_buf[bytes_read] = 0;
    	}
		else
		{
			printf(buffer);
		}
    }
    while(res > 0);
RETT:
	close( pipe_fd );
	return bytes_read;
}


/*********************************************************************************
  *��������:void console_cmd( char *p, uint16_t len )
  *��������:��������
  *��	��: p		:���ݵ��ַ���
  			len		:�ַ�������
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
void console_cmd( char *p, uint16_t len )
{
	char buf[1024];
	gps_send_cmd(p,(char *)0,0);
	/*
	if(gps_send_cmd(p,buf,sizeof(buf)))
	{
		printf("\n%s",buf);
	}
	*/
}


/*********************************************************************************
  *��������:void console_rtc_set( char *p, uint16_t len )
  *��������:����RTCʱ���ʽΪ:
  *��	��: p		:���ݵ��ַ���
  			len		:�ַ�������
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
void console_rtc_set( char *p, uint16_t len )
{
	u8 uc_buf[8];
	
	Ascii_To_Hex(uc_buf,p,6);
	rtc_set_time_mytime(mytime_from_bcd(uc_buf));
}


/*********************************************************************************
  *��������:void console_rtc_read( char *p, uint16_t len )
  *��������:��ȡRTCʱ�䲢��ӡ������̨
  *��	��: p		:���ݵ��ַ���
  			len		:�ַ�������
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
void console_rtc_read( char *p, uint16_t len )
{
	u32 uct_time;
	MYTIME my_time;
	
	uct_time = rtc_get_time(0);
	my_time = utc_to_mytime(uct_time);
	printf( "%02d-%02d-%02d %02d:%02d:%02d",
	         YEAR( my_time ),
	         MONTH( my_time ),
	         DAY( my_time ) ,
	         HOUR( my_time ),
	         MINUTE( my_time ),
	         SEC( my_time ) 
	         );
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
int console_param_load( char *p, uint16_t len )
{
	//pt_para->thread_start = FALSE;
	return param_load();
}

/*********************************************************************************
  *��������:int console_param_out( char *p, uint16_t len );
  *��������:����̨���������������
  *��	��: p		:���ݵ��ַ���
  			len		:�ַ�������
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
int console_param_out( char *p, uint16_t len )
{
	printf("id_is:%s,len=%d",p,len);
	param_out(p);
	return 0;
}


/*********************************************************************************
  *��������:int console_param_out( char *p, uint16_t len );
  *��������:����̨���������������
  *��	��: p		:���ݵ��ַ���
  			len		:�ַ�������
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
int console_param_set( char *p, uint16_t len )
{
	int i,j;
	int i_ret = 0;
	char c_sectbuf[64];
	char c_idbuf[64];
	char c_val_buf[64];

	for( i = 0,j=0; i < len; i++ )
	{
		if(  p[i] == ',' )
			break;
		if( j >= ( sizeof(c_sectbuf) - 1 ))
			break;
		c_sectbuf[j++] = p[i];
	}
	c_sectbuf[j] = 0;
	i++;
	for( j=0; i < len; i++ )
	{
		if(  p[i] == ',' )
			break;
		if( j >= ( sizeof(c_idbuf) - 1 ))
			break;
		c_idbuf[j++] = p[i];
	}
	c_idbuf[j] = 0;
	i++;
	for( j=0; i < len; i++ )
	{
		if( j >= ( sizeof(c_val_buf) - 1 ))
			break;
		c_val_buf[j++] = p[i];
	}
	c_val_buf[j] = 0;
	
	strtrim(c_sectbuf,' ');
	strtrim(c_idbuf,' ');
	strtrim(c_val_buf,' ');
	printf("SET VAL. SECT=%s, ID=%s, VAL=%s\n",c_sectbuf,c_idbuf,c_val_buf);
	//i_ret = param_set(c_idbuf,c_val_buf);
	
	i_ret = param_set_ex(c_sectbuf,c_idbuf,c_val_buf);
	if(i_ret)
	{
		printf("param_set_ERROR!");
	}
	i_ret += param_load();
	return i_ret;
}



/*********************************************************************************
  *��������:int console_param_out( char *p, uint16_t len );
  *��������:����̨���������������
  *��	��: p		:���ݵ��ַ���
  			len		:�ַ�������
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
int console_param_set2( char *p, uint16_t len )
{
	int i,j;
	int i_ret = 0;
	char c_sectbuf[64];
	char c_idbuf[64];
	char c_val_buf[64];

	for( i = 0,j=0; i < len; i++ )
	{
		if(  p[i] == ',' )
			break;
		if( j >= ( sizeof(c_sectbuf) - 1 ))
			break;
		c_sectbuf[j++] = p[i];
	}
	c_sectbuf[j] = 0;

	if(p[1] == '0')
	{
		gps_param.id_0x0001 = 8001;
		gps_param.id_0x0002 = 8002;
		gps_param.id_0x0003 = 8003;
		gps_param.id_0x0004 = 8004;
		gps_param.id_0x0005 = 8005;
		gt_vdr_param.id_0x0001 = 8001;
		gt_vdr_param.id_0x0002 = 8002;
		gt_vdr_param.id_0x0003 = 8003;
		gt_vdr_param.id_0x0004 = 8004;
		gt_vdr_param.id_0x0005 = 8005;
		printf("SET VAL_1\n");
	}
	else	// if(p[1] == '1')
	{
		gps_param.id_0x0001 = 8011;
		gps_param.id_0x0002 = 8012;
		gps_param.id_0x0003 = 8013;
		gps_param.id_0x0004 = 8014;
		gps_param.id_0x0005 = 8015;
		gt_vdr_param.id_0x0001 = 8101;
		gt_vdr_param.id_0x0002 = 8102;
		gt_vdr_param.id_0x0003 = 8103;
		gt_vdr_param.id_0x0004 = 8104;
		gt_vdr_param.id_0x0005 = 8105;
		printf("SET VAL_2\n");
	}
	if(param_save_ex(""))
	{
		printf("SET ERROR!\n");
	}
	return i_ret;
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
int store_fifo_test( char *p, uint16_t len )
{
    int i_size;
	char buf[128];
	//pt_para->thread_start = FALSE;
	if(gt_store_fifo_fd>0)
	{
		i_size=write(gt_store_fifo_fd,p,len);
		if(i_size < 0)
		{
			printf("FIFO_write: ERROR!\n");
	        return -1;
		}
		printf("FIFO_write ok!\n");
		return 0;
	}
	printf("FIFO_write: ERROR_2!\n");
}


/*********************************************************************************
  *��������:void* console_app_main(void *p)
  *��������:����̨ͨ�ſ��Ƴ��򣬸ú�����һ���̵߳����к��������ܱ����ã�ֻ�ܴ����߳�ʱ
  			��Ϊ�̵߳���ں�����
  *��	��: p		:���ݵĲ������ò��������ͱ���Ϊ	st_gps_thread_param
  *��	��: none
  *�� �� ֵ:�̶�����Ϊ	NULL
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
void* console_proc(void *p)
{
    int i;
    char menuin[128];
    char menuin_lower[128];
    st_gps_thread_param	*pt_para;
	static const st_console_msg gt_console_msg[] =
	{
		//{ "exit",			CONSOLE_MSG_STR_FUNC,	NULL,				"�˳��߳�",				},		///
		{ "712",			CONSOLE_MSG_STR,		NULL,				"www.712.cn",			},		///
		{ "ip",				CONSOLE_MSG_STR_FUNC,	NULL,				"192.168.2.20",			},		///
		{ "param_load",		CONSOLE_MSG_STR_FUNC,	console_param_load,	"���ز���",				},		///
		{ "param_out",		CONSOLE_MSG_STR_FUNC,	console_param_out,	"�������",				},		///
		{ "param_set:",		CONSOLE_MSG_STR_FUNC,	console_param_set,	"���ò���",				},		///
		{ "param_set2:", 	CONSOLE_MSG_STR_FUNC,	console_param_set2,	"���ò���", 			},		///
		{ "disk:",			CONSOLE_MSG_STR_FUNC,	console_disk_test,	"���̲���",				},		///
		{ "store:",			CONSOLE_MSG_STR_FUNC,	store_fifo_test,	"FIFO����",				},		///
		{ "cmd:",			CONSOLE_MSG_STR_FUNC,	console_cmd,		"shell cmd",			},		///
		{ "rtc_set:",		CONSOLE_MSG_STR_FUNC,	console_rtc_set,	"rtc_set:120715133759",	},		///
		{ "rtc_read",		CONSOLE_MSG_STR_FUNC,	console_rtc_read,	"��ȡRTCʱ��",			},		///
		{ "1234",			CONSOLE_MSG_STR_FUNC,	console_rtc_read,				"5678",		},		///
	};
	u32 uct_time;
	MYTIME my_time;
	

	
	pt_para = (st_gps_thread_param*)p;
	printf("pt_para->thread_param=%c\n",pt_para->thread_param);

    while( pt_para->thread_start)
	{
		/*
		sleep(1);
		if(gi_pipe_fd_test>0)
		{
			i=write(gi_pipe_fd_test,"test",4);
			if(i < 0)
			{
				printf("FIFO_write: ERROR!");
		        continue;
			}
			printf("FIFO_write ok!");
			continue;
		}
		printf("FIFO_write: ERROR_2!");
		continue;
		*/
		printf(">> ");
		fflush(stdout);
		memset(menuin,0,sizeof(menuin));
		
		if (fgets(menuin, sizeof(menuin), stdin) == NULL) 
		{
		    if (1)
			{
				puts("Cannot switch back to console from file redirection");
				menuin[0] = 'q';
				menuin[1] = '\0';
		    }
			else
			{
				puts("Switched back to console from file redirection");
				continue;
		    }
		}
		///ȥ���س����м����в��ɼ��ַ�
		strtrim(menuin,0);
		///���������ַ����ò��費����
		strproc(menuin);
		///
		if(strlen(menuin) == 0)
		{
			continue;
		}
		///�Ƿ���Ҫ��ӡ����
		if (pt_para->thread_param == '1') 
		{
		    printf("%s,  len=%d\n HEX:", menuin,strlen(menuin));
			//printf_hex_data(menuin,strlen(menuin));
		    //printf("\n");
		}
		for(i=0;i<sizeof(menuin);i++)
		{
			menuin_lower[i] = tolower( menuin[i] );
		}
		///�˳�
		if( strncmp(menuin_lower, "exit" , 4) == 0 )
		{
			printf("�˳��߳�\n");
			pt_para->thread_start = FALSE;
			return NULL;
		}
		///�˳�
		if( strncmp(menuin_lower, "help" , 4) == 0 )
		{
			//{ 	"store:",			CONSOLE_MSG_STR_FUNC,	store_fifo_test,	"FIFO����",				},		///
			printf(		"       ����:               ����:\n");
			for( i = 0; i < sizeof( gt_console_msg ) / sizeof( st_console_msg ); i++ )
			{
				memset(menuin,' ',sizeof(menuin));
				memcpy(menuin+2,gt_console_msg[i].rx_str,strlen(gt_console_msg[i].rx_str));
				menuin[22] = ':';
				memcpy(menuin+22,gt_console_msg[i].tx_str,strlen(gt_console_msg[i].tx_str));
				menuin[22+strlen(gt_console_msg[i].tx_str)] = 0;
				printf("(%03d)%s\n",i+1,menuin);
			}
			continue;
		}
		///���ѭ���ж�
		for( i = 0; i < sizeof( gt_console_msg ) / sizeof( st_console_msg ); i++ )
		{
			if( strncmp(menuin_lower, gt_console_msg[i].rx_str, strlen(gt_console_msg[i].rx_str)) == 0)
			{
				if((gt_console_msg[i].type == CONSOLE_MSG_STR ) || (gt_console_msg[i].type == CONSOLE_MSG_STR_FUNC))
				{
					uct_time = rtc_get_time(0);
					my_time = utc_to_mytime(uct_time);
					printf( "%02d:%02d:%02d--> ",
					         HOUR( my_time ),
					         MINUTE( my_time ),
					         SEC( my_time ) 
					         );
					printf("%s\n", gt_console_msg[i].tx_str);
				}
				if((gt_console_msg[i].type == CONSOLE_MSG_FUNC) || (gt_console_msg[i].type == CONSOLE_MSG_STR_FUNC))
				{
					if(gt_console_msg[i].proc_func)
					{
						gt_console_msg[i].proc_func(menuin+strlen(gt_console_msg[i].rx_str),strlen(menuin)-strlen(gt_console_msg[i].rx_str));
					}
				}
				usleep(3);
				break;
			}
		}
		///���û���ҵ���Ч�����������������󣬱����жϸ���������Ч����
		if(i == sizeof( gt_console_msg ) / sizeof( st_console_msg ))
		{
			uct_time = rtc_get_time(0);
			my_time = utc_to_mytime(uct_time);
			printf( "%02d-%02d-%02d %02d:%02d:%02d--> ",
			         YEAR( my_time ) ,
			         MONTH( my_time ) ,
			         DAY( my_time ) ,
			         HOUR( my_time ) ,
			         MINUTE( my_time ),
			         SEC( my_time ) 
			         );
			printf("Invalid input: %s\n", menuin);
		}
		/*
		switch (menuin[0]) 
		{	
			case 'i':
			{
			    if (menuin[1] == 'p') 
				{
					printf("Your input is IP.\n", menuin);
			    }
				break;
			}
			case '7':
			{
			    if ((menuin[1] == '1') &&  (menuin[2] == '2') )
				{
					printf("Your input is 712.\n", menuin);
			    }
				break;
			}
			case 'e':
			{
			    if ((menuin[1] == 'x') &&  (menuin[2] == 'i')  &&  (menuin[3] == 't') )
				{
					printf("Your input is exit.\n");
					pt_para->thread_start = FALSE;
			    }
				break;
			}
			default:
			{
			    if (menuin[0] != '\n' && menuin[0] != '\r') 
				{
					printf("Invalid input: %s\n", menuin);
			    }
			    break;
			}
		}
		*/
    }
	return NULL;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
