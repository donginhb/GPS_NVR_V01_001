#ifndef _JT808_GPS_H_
#define _JT808_GPS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define BIT_STATUS_ACC		0x00000001	///0��ACC �أ�1�� ACC ��
#define BIT_STATUS_FIXED	0x00000002	///0��δ��λ��1����λ
#define BIT_STATUS_NS		0x00000004	///0����γ��1����γ
#define BIT_STATUS_EW		0x00000008  ///0��������1������
#define BIT_STATUS_SERVICE	0x00000010  ///0����Ӫ״̬��1��ͣ��״̬
#define BIT_STATUS_ENCRYPT	0x00000020	///0����γ��δ�����ܲ�����ܣ�1����γ���Ѿ����ܲ������

#define BIT_STATUS_EMPTY	0x00000100	///
#define BIT_STATUS_FULL		0x00000200

#define BIT_STATUS_OIL		0x00000400  ///0��������·������1��������·�Ͽ�
#define BIT_STATUS_ELEC		0x00000800  ///0��������·������1��������·�Ͽ�
#define BIT_STATUS_DOORLOCK 0x00001000  ///0�����Ž�����1�����ż���
#define BIT_STATUS_DOOR1	0x00002000	///0���� 1 �أ�1���� 1 ����ǰ�ţ�
#define BIT_STATUS_DOOR2	0x00004000	///0���� 2 �أ�1���� 2 �������ţ�
#define BIT_STATUS_DOOR3	0x00008000	///0���� 3 �أ�1���� 3 �������ţ�
#define BIT_STATUS_DOOR4	0x00010000	///0���� 4 �أ�1���� 4 ������ʻϯ�ţ�
#define BIT_STATUS_DOOR5	0x00020000	///0���� 5 �أ�1���� 5 �����Զ��壩
#define BIT_STATUS_GPS		0x00040000	///0��δʹ�� GPS ���ǽ��ж�λ��1��ʹ�� GPS ���ǽ��ж�λ
#define BIT_STATUS_BD		0x00080000	///0��δʹ�ñ������ǽ��ж�λ��1��ʹ�ñ������ǽ��ж�λ
#define BIT_STATUS_GLONASS	0x00100000	///0��δʹ�� GLONASS ���ǽ��ж�λ��1��ʹ�� GLONASS ���ǽ��ж�λ
#define BIT_STATUS_GALILEO	0x00200000	///0��δʹ�� Galileo ���ǽ��ж�λ��1��ʹ�� Galileo ���ǽ��ж�λ

#define BIT_ALARM_EMG				0x00000001  
#define BIT_ALARM_OVERSPEED			0x00000002  
#define BIT_ALARM_OVERTIME			0x00000004  
#define BIT_ALARM_DANGER			0x00000008  
#define BIT_ALARM_GPS_ERR			0x00000010  
#define BIT_ALARM_GPS_OPEN			0x00000020  
#define BIT_ALARM_GPS_SHORT			0x00000040  
#define BIT_ALARM_LOW_PWR			0x00000080  
#define BIT_ALARM_LOST_PWR			0x00000100  
#define BIT_ALARM_FAULT_LCD			0x00000200  
#define BIT_ALARM_FAULT_TTS			0x00000400  
#define BIT_ALARM_FAULT_CAM			0x00000800  
#define BIT_ALARM_FAULT_ICCARD		0x00001000  
#define BIT_ALARM_PRE_OVERSPEED		0x00002000  
#define BIT_ALARM_PRE_OVERTIME		0x00004000  
#define BIT_ALARM_TODAY_OVERTIME	0x00040000  
#define BIT_ALARM_STOP_OVERTIME		0x00080000  
#define BIT_ALARM_DEVIATE			0x00800000  
#define BIT_ALARM_VSS				0x01000000  
#define BIT_ALARM_OIL				0x02000000  
#define BIT_ALARM_STOLEN			0x04000000  
#define BIT_ALARM_IGNITION			0x08000000  
#define BIT_ALARM_MOVE				0x10000000  
#define BIT_ALARM_COLLIDE			0x20000000  
#define BIT_ALARM_TILT				0x40000000  
#define BIT_ALARM_DOOR_OPEN			0x80000000  

#if 0
typedef struct _gps_baseinfo
{
	uint32_t	alarm;
	uint32_t	status;
	uint32_t	latitude;                       /*γ��*/
	uint32_t	longitude;                      /*����*/
	uint16_t	altitude;
	uint16_t	speed_10x;                      /*�Ե��ٶ� 0.1KMH*/
	uint16_t	cog;                            /*�ԵؽǶ�*/
	uint8_t		datetime[6];                    /*BCD��ʽ*/
	uint8_t		mileage_id;                     /*������Ϣ_���ID*/
	uint8_t		mileage_len;                    /*������Ϣ_��̳���*/
	uint32_t	mileage;                   		/*������Ϣ_���*/
	uint8_t		csq_id;                     	/*������Ϣ_����ͨ����ID*/
	uint8_t		csq_len;                    	/*������Ϣ_����ͨ��������*/
	uint8_t		csq;                   			/*������Ϣ_����ͨ�����ź�ǿ��*/
	uint8_t		NoSV_id;                     	/*������Ϣ_��λ��������ID*/
	uint8_t		NoSV_len;                    	/*������Ϣ_��λ������������*/
	uint8_t		NoSV;                   		/*������Ϣ_��λ��������*/
}gps_baseinfo;
#endif

typedef struct _gps_baseinfo
{
	uint32_t	alarm;
	uint32_t	status;
	uint32_t	latitude;                       /*γ��*/
	uint32_t	longitude;                      /*����*/
	uint16_t	altitude;
	uint16_t	speed_10x;                      /*�Ե��ٶ� 0.1KMH*/
	uint16_t	cog;                            /*�ԵؽǶ�*/
	uint8_t		datetime[6];                    /*BCD��ʽ*/
	uint8_t		mileage_id;                     /*������Ϣ_���ID*/
	uint8_t		mileage_len;                    /*������Ϣ_��̳���*/
	uint32_t	mileage;                   		/*������Ϣ_���*/
	uint8_t     oil_id;
	uint8_t		oil_len;
	uint16_t	oil;
	uint8_t		pulse_id;
	uint8_t		pulse_len;
	uint16_t	pulse;
	uint8_t		manalarm_id;
	uint8_t		manalarm_len;
	uint16_t	manalarm;
	uint8_t		exspeed_id;
	uint8_t		exspeed_len;
	uint8_t		exspeed[5];
	uint8_t		region_id;
	uint8_t		region_len;
	uint8_t		region[6];
	uint8_t		road_id;
	uint8_t		road_len;
	uint8_t		road[7];
	uint8_t		carsignal_id;                    
	uint8_t		carsignal_len;                    
	uint32_t	carsignal;
	uint8_t     io_id;
	uint8_t		io_len;
	uint16_t	io;
	uint8_t		analog_id;                    
	uint8_t		analog_len;                    
	uint32_t	analog;
	uint8_t		csq_id;                     	/*������Ϣ_����ͨ����ID*/
	uint8_t		csq_len;                    	/*������Ϣ_����ͨ��������*/
	uint8_t		csq;                   			/*������Ϣ_����ͨ�����ź�ǿ��*/
	uint8_t		NoSV_id;                     	/*������Ϣ_��λ��������ID*/
	uint8_t		NoSV_len;                    	/*������Ϣ_��λ������������*/
	uint8_t		NoSV;                   		/*������Ϣ_��λ��������*/
	uint8_t     data[];
}gps_baseinfo;

typedef struct _gps_info_save
{
	uint32_t	gpsnum;
	uint8_t		flag;
	uint8_t		status;
	gps_baseinfo data;
}gps_info_save;


typedef struct _gps_contrl_
{
	int read_offset;
	int write_offset;
	int backflag;
}gps_contrl;


int gps_data_filled(gps_baseinfo *pstr);
int gps_thread_join(void);
int gps_thread(void);
int gps_write(uint8_t *pstr);
int jt808_tx_gpsdata(void);

#ifdef __cplusplus
}; //end of extern "C" {
#endif
#endif
