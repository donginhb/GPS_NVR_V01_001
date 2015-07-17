/*********************************************************************************
  * @�ļ����� :gps_disk_io.c
  * @�������� :����Ӳ�̺ʹ洢�̲���������������Ӧ�ú���
  * @��	   �� :������
  * @�������� :2015-6-17
  * @�ļ��汾 :0.01
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-17	����
*********************************************************************************/
#ifdef __cplusplus
#if __cplusplus
	extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "gps_typedef.h"
#include <fcntl.h>





/*********************************************************************************
  *��������:int disk_printf_dir(const char* pc_path)  
  *��������:�������ܣ���ӡpc_pathĿ¼�µ������ļ�,��Ŀ¼�Ϊ255�ֽ�
  *��	��: pc_path :Ҫ��ӡ��Ŀ¼·������������Ϊ255�ֽ�
  *��	��: none
  *�� �� ֵ:1	:��Ŀ¼�Ѿ����ڣ�0:��Ŀ¼�Ѿ�����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-17	����
*********************************************************************************/
int disk_printf_dir(const char* pc_path)
{
	DIR * pt_dir;
	struct dirent * pt_dirent;
	char c_tmp_buf[256];
	
	pt_dir = opendir(pc_path);
	
	if(pt_dir != NULL)
	{
		while((pt_dirent = readdir(pt_dir)) != NULL)
		{
			if(strcmp(pt_dirent->d_name,"..") && strcmp(pt_dirent->d_name,"."))
			{
				if(4 == pt_dirent->d_type)
				{
					memset(c_tmp_buf,0,sizeof(c_tmp_buf));
					strcpy(c_tmp_buf,pc_path);
					strcat(c_tmp_buf,"/");
					strcat(c_tmp_buf,pt_dirent->d_name);
					disk_printf_dir(c_tmp_buf);
				}
				else
				{
					printf("FIL_:%s/%s   d_type:%d\n",pc_path, pt_dirent->d_name,pt_dirent->d_type);
				}
			}
		}
		closedir(pt_dir);
	}
	printf("DIR&:%s\n", pc_path);
	return 0;
}  

/* 
struct dirent
{
	ino_t d_ino; 	//d_ino ��Ŀ¼������inode
	ff_t d_off; 	//d_off Ŀ¼�ļ���ͷ����Ŀ¼������λ��
	signed short int d_reclen; 	//d_reclen _name �ĳ���, ������NULL �ַ�
	unsigned char d_type; 		//d_type ��ָ���ļ�����
	har d_name[256];			// d_name �ļ���
};
*/
/*********************************************************************************
  *��������:int disk_remove_dir(const char* pc_path)  
  *��������:ɾ��һ��Ŀ¼(�ļ���)���ļ����ڲ��������ļ�����Ŀ¼�Ϊ255�ֽ�
  *��	��: pc_path :Ҫɾ����Ŀ¼·������������Ϊ255�ֽ�
  *��	��: none
  *�� �� ֵ:��0ֵ:��Ŀ¼�����ڻ�ɾ��ʧ�ܣ�0:ɾ���ɹ�
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-17	����
*********************************************************************************/
int disk_remove_dir(const char* pc_path)  
{
	DIR * pt_dir;
	struct dirent * pt_dirent;
	char c_tmp_buf[256];
	int  i_ret = 0;
	
	pt_dir = opendir(pc_path);
	
	if(pt_dir != NULL)
	{
		while((pt_dirent = readdir(pt_dir)) != NULL)
		{
			if(strcmp(pt_dirent->d_name,"..") && strcmp(pt_dirent->d_name,"."))
			{
				if(4 == pt_dirent->d_type)
				{
					memset(c_tmp_buf,0,sizeof(c_tmp_buf));
					strcpy(c_tmp_buf,pc_path);
					strcat(c_tmp_buf,"/");
					strcat(c_tmp_buf,pt_dirent->d_name);
					i_ret += disk_remove_dir(c_tmp_buf);
				}
				else
				{
					remove(pt_dirent->d_name);
					printf("FIL_REMOVE:%s/%s   d_type:%d\n",pc_path, pt_dirent->d_name,pt_dirent->d_type);
				}
			}
		}
		closedir(pt_dir);
	}
	else
	{
		i_ret = 1;
	}
	rmdir(pc_path);
	printf("DIR_REMOVE:%s\n", pc_path);
	return i_ret;
}  


/*********************************************************************************
  *��������:int disk_create_dir(const char* pc_path)
  *��������:����һ��Ŀ¼(�ļ���)��༶Ŀ¼����Ŀ¼��·����������Ϊ255�ֽ�
  *��	��: pc_path	:Ҫ������Ŀ¼·������������Ϊ255�ֽ�
  *��	��: none
  *�� �� ֵ:1	:��Ŀ¼�Ѿ����ڣ�0:��Ŀ¼�Ѿ�����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-17	����
*********************************************************************************/
int disk_create_dir(const char* pc_path)
{
	if(-1 != access(pc_path,0))
		return 1;

	char c_tmp_buf[256];
	const char* pc_cur = pc_path;

	memset(c_tmp_buf,0,sizeof(c_tmp_buf));
	int pos=0;
	//printf("\n disk_create_dir=%s\n",pc_path);
	while(*pc_cur++!='\0')
	{
		c_tmp_buf[pos++] = *(pc_cur-1);

		if(*pc_cur=='/' || *pc_cur=='\0')
		{
			if(0!=access(c_tmp_buf,0)&&strlen(c_tmp_buf)>0)
			{
				mkdir(c_tmp_buf,S_IRWXU|S_IRWXG|S_IRWXO );
			}
		}
	}
	return 0;
}

/*********************************************************************************
  *��������:int disk_write(int fd, u64 addr, u8 *pdata , int len)
  *��������:����open��ʽ�򿪵��ļ���д������
  *��	��: fd		:Ҫд����豸�ľ��
  			addr	:Ҫд���λ�ã�������ļ��Ŀ�ʼ
  			pdata	:Ҫд�������
  			len		:Ҫд��ĳ���
  *��	��: none
  *�� �� ֵ:��0:д��ʧ�ܣ�0:д��ɹ�
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-17	����
*********************************************************************************/
int disk_write(int fd, s64 addr, u8 *pdata , int len)
{
    int i_ret;
    int i_size;
	
	if((fd == NULL)||( len == 0)||( pdata == NULL))
	{
        return -1;
    }
	
	lseek64(fd,addr,SEEK_SET);
	i_size=write(fd,pdata,len);
	if(i_size < 0)
	{
		printf("disk_hd_write: ERROR!");
        return -1;
	}
	return 0;
}

/*********************************************************************************
  *��������:int disk_read(int fd, u64 addr, u8 *pdata , int len)
  *��������:���Ѿ���open��ʽ�򿪵��ļ��ж�ȡ����
  *��	��: fd		:Ҫ��ȡ���豸�ľ��
  			addr	:Ҫ��ȡ��λ�ã�������ļ��Ŀ�ʼ
  			pdata	:Ҫ��ȡ�����ݴ�ŵ�λ��
  			len		:Ҫ��ȡ�ĳ���
  *��	��: none
  *�� �� ֵ:��0:��ȡʧ�ܣ�0:��ȡ�ɹ�
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-17	����
*********************************************************************************/
int disk_read(int fd, s64 addr, u8 *pdata , int len)
{
    int i_ret;
    int i_size;
	
	if((fd == NULL)||( len == 0)||( pdata == NULL))
	{
        return -1;
    }
	printf("disk_read_1\n");
	lseek64(fd,addr,SEEK_SET);
	printf("disk_read_2\n");
	i_size=read(fd,pdata,len);
	if(i_size < 0)
	{
		printf("disk_hd_read: ERROR!");
        return -1;
	}
	printf("disk_read_ok\n");
	return 0;
}

/***************************************************************************************************************/
//���ǵ�һ����ʽ���򣬿�ʼ�������Թ���_START
/***************************************************************************************************************/


/******************************************************************************
* function    : main()
* Description : video venc sample
******************************************************************************/
int disk_pro_test01(int argc, char *argv[])
{
    s32 s32Ret;
   	int i, strt, ch_out;
	int cmd = 4;  
	long long sector = 0;  
    char buf[512];  
    int fd,size,len;
	char *buf_wr="Hello! I'm writing to this file!";
	
    cmd = atoi(argv[1]);
    sector = atoi(argv[2]);
	printf("sector=%d",sector);
	sector *= 512;
	//getchar();
    /*0���ɹ���-1��ʧ��*/  
    ////////////////////////////////////////
 
	if(cmd&& (argc<4))
	{
    	printf("Please input param3( write string data. )!\n");
        return FALSE;
    }
	
	fd = open("/dev/sda2", O_RDWR );
	if(fd < 0)
	{
		printf("open: ERROR!");
		 return FALSE;
	}
	else
		printf("open file OK: %d\n",fd);
	/*
	while(sector)
	{
		if(sector>0xFFFFFFFF)
		{
			lseek64(fd,0xFFFFFFFF,SEEK_CUR);
			sector -= 0xFFFFFFFF;
		}
		else
		{
			lseek64(fd,sector,SEEK_CUR);
			sector = 0;
		}
	}
	*/
	lseek64(fd,sector,SEEK_SET);
	if(cmd)
	{
		size=write(fd,argv[3],strlen(argv[3]));
		if(size < 0)
		{
			printf("wtrite: ERROR!");
			close(fd);
			return FALSE;	
		}
	}
	else
	{
		memset(buf,0,sizeof(buf));
		size=read(fd,buf,128);
		if(size < 0)
		{
			printf("read: ERROR!");
			close(fd);
			return FALSE;
		}
		printf("Read OK\n");  
		printf_hex_data(buf,128);
	    printf("STRING=%s",buf);
	}
	//len = strlen(buf_wr);
	//buf_wr[10] = '\0';
	//if((size = write( fd, buf_wr, len)) < 0)
	//perror("write:");
	//exit(1);
	close(fd);
    ////////////////////////////////////////
    return FALSE;
}

/*********************************************************************************
  *��������:int console_disk_test( char *p, uint16_t len )
  *��������:���Գ���
  *��	��: p		:���ݵ��ַ���
  			len		:�ַ�������
  *��	��: none
  *�� �� ֵ:	0:OK  ��0:����
  *---------------------------------------------------------------------------------
  * @�޸���		�޸�ʱ��   	�޸�����
  * ������		2015-06-18	����
*********************************************************************************/
int console_disk_test( char *p, int len )
{
	char c_buf[64];
	char * pc_str;
	static int fd = 0;
	static int i_address1,i_address2;
	int size;
 	int i;
	static char *pc_argv1[]={"0","0","1000","1234567890aaaaaabbbbbbbccccccc"};
	static char *pc_argv2[]={"0","1","1000","1234567890aaaaaabbbbbbbccccccc"};
	
	if(*p == 't')
	{
		if( *(p+1) == '0')
		{
			disk_pro_test01(4,pc_argv1);
		}
		else
		{
			disk_pro_test01(4,pc_argv2);
		}
		printf("test_disk_t\n");
		return;
	}
	
	if(*p == 'o')
	{
		i_address1 = 0xFFFFFF;
		i_address2 = 0xFFFFFF;
		fd = open("/dev/sda2", O_RDWR );
		if(fd < 0)
		{
			printf("open: ERROR!");
			fd = 0;
			return 1;
		}
		else
		{
			printf("open disk OK: %d\n",fd);
		}
	}
	
	if(fd == NULL)
	{
		printf("ERROR:file not open!\n");
		return 1;
	}
	
	if(strlen(p) < 2)
	{
		printf("ERROR:input is to short!\n");
		return 1;
	}
	
	if(*p == 'c')
	{
		i = close(fd);
		if( i )
		{
			printf("ERROR:close = %d",i);
			return 1;
		}
		printf("disk close: %d\n",fd);
		i_address1 = 0xFFFFFF;
		i_address2 = 0xFFFFFF;
		fd = 0;
	}
	
	else if(*p == 'w')
	{
		if( *(p+1) == '0')
		{
			disk_write(fd,i_address1,"TEST1234567890",strlen("TEST1234567890"));
			i_address1 += strlen("TEST1234567890");
		}
		else
		{
			disk_write(fd,i_address1,p,strlen(p));
			i_address1 += strlen(p);
		}
	}
	
	else if(*p == 'r')
	{
		memset(c_buf,0,sizeof(c_buf));
		if( *(p+1) == '0')
		{
			disk_read(fd,i_address2,c_buf,14);
			i_address2 += 14;
		}
		else
		{
			disk_read(fd,0xFFFFFF,c_buf,32);
		}
		printf("str:%s\n",c_buf);
	}
	
	return 0;
	
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
