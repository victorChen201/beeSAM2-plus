//#define	__M_FILE_NAME_ntp_H__	//���α�ͷ�ļ�
#ifndef	__M_FILE_NAME_ntp_H__
#define	__M_FILE_NAME_ntp_H__

#include <time.h>
#include "common.h"

/* NTP Э����ṹ*/
typedef struct _ntp_time
{
    unsigned int coarse;
    unsigned int fine;
} ntp_time;

struct ntp_packet
{
     unsigned char leap_ver_mode;
     unsigned char startum;
     char poll;
     char precision;

     int root_delay;
     int root_dispersion;
     int reference_identifier;
     ntp_time reference_timestamp;
     ntp_time originage_timestamp;
     ntp_time receive_timestamp;
     ntp_time transmit_timestamp;
};

extern int ntp_init (void);
extern int ntp_quit_wait (void);		//SSDP�˳��ȴ�����

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_ntp_H__".
