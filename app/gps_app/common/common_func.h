#ifndef _H_COMMON_FUNC
#define _H_COMMON_FUNC

#include <stdio.h>



#ifndef BIT
#define BIT( i ) ( (unsigned long)( 1 << i ) )
#endif

extern const unsigned char tbl_hex_to_assic[]; 		//���ڴ��д洢��16����������ת��ΪASSIC�ɼ��ַ����ݣ���Ҫ�õ��ı�
extern const unsigned char tbl_assic_to_hex[24];	//��ASSIC�ɼ��ַ�������ת��Ϊ���ڴ��д洢��16�������ݣ���Ҫ�õ��ı�


extern unsigned int Hex_To_Ascii( unsigned char* pDst, const unsigned char* pSrc, unsigned int nSrcLength );
extern unsigned int Ascii_To_Hex(unsigned char *dest_buf,char * src_buf,unsigned int max_rx_len);
extern unsigned long AssicBufToUL(char * buf,unsigned int num);
extern void printf_hex_data( const unsigned char* pSrc, unsigned int nSrcLength );
extern unsigned char Get_Month_Day(unsigned char month,unsigned char leapyear);
extern void strproc( unsigned char* s );
extern void strtrim( unsigned char* s, unsigned char c );
extern unsigned long tick_get( void );
extern uint16_t data_to_buf( uint8_t * pdest, uint32_t data, uint8_t width );
extern uint32_t buf_to_data( uint8_t * psrc, uint8_t width );
extern uint8_t HEX2BCD( uint8_t x );
extern uint8_t BCD2HEX( uint8_t x );
extern int my_system(const char * cmd);
extern void outprint_hex(uint8_t * descrip, char *instr, uint16_t inlen );


#endif
