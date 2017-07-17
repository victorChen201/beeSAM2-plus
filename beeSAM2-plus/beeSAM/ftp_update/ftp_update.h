#ifndef	__M_FILE_NAME_ftp_update_H__
#define __M_FILE_NAME_ftp_update_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<errno.h>


#define FTP_USERNAME_SIZE 32
#define FTP_PASSWORD_SIZE 32
#define FTP_SOU_FILENAME_SIZE 512
#define FTP_TAR_FILENAME_SIZE 512
#define FTP_COMMAND_TMP_SIZE  1024


// update message
typedef struct
{
	char ftp_username[FTP_USERNAME_SIZE];           // FTP username      				ex: www
	char ftp_password[FTP_PASSWORD_SIZE]; 		 	// FTP password  					ex: 123
	char ftp_sou_filename[FTP_SOU_FILENAME_SIZE];	// FTP upgrade link filename    	ex: 192.168.18.37/xxx/openwrt.bin
	char ftp_tar_Dir[FTP_TAR_FILENAME_SIZE];		// Save path dir					ex: ../update
	int  ftp_timeout;								// time out
}FtpupdateMes;


extern int ftp_upgrade(char ftp_username[FTP_USERNAME_SIZE],char ftp_password[FTP_PASSWORD_SIZE],char ftp_sou_filename[FTP_SOU_FILENAME_SIZE],char ftp_tar_Dir[FTP_TAR_FILENAME_SIZE],int ftp_timeout);


extern uint8_t ftp_update_listen_state();

#endif
