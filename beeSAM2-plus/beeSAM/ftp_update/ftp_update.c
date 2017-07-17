#include "common.h"
#include "ftp_update.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define FTP_UPDATE_DBG

#ifndef FTP_UPDATE_PUTC
#ifdef FTP_UPDATE_DBG
#define FTP_UPDATE_PUTC(Char)			PUTC(Char)
#else
#define FTP_UPDATE_PUTC(Char)
#endif
#endif

#ifndef FTP_UPDATE_PUTS
#ifdef FTP_UPDATE_DBG
#define FTP_UPDATE_PUTS(Str, Len)		PUTS(Str, Len)
#else
#define FTP_UPDATE_PUTS(Str, Len)
#endif
#endif

#ifndef FTP_UPDATE_PUTASCC
#ifdef FTP_UPDATE_DBG
#define FTP_UPDATE_PUTASCC(Char)			PUTASCC(Char)
#else
#define FTP_UPDATE_PUTASCC(Char)
#endif
#endif

#ifndef FTP_UPDATE_PUTASCS
#ifdef FTP_UPDATE_DBG
#define FTP_UPDATE_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define FTP_UPDATE_PUTASCS(Str, Len)
#endif
#endif

#ifndef FTP_UPDATE_PRINT
#ifdef FTP_UPDATE_DBG
#define FTP_UPDATE_PRINT(Fmt, Arg...)	PRINT(Fmt, ##Arg)
#else
#define FTP_UPDATE_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef FTP_UPDATE_DYN_PUTC
#ifdef FTP_UPDATE_DBG
#define FTP_UPDATE_DYN_PUTC(Dbg, Char)			DYN_PUTC((M_DBG_MODULE_FTP_UPDATE|Dbg), Char)
#else
#define FTP_UPDATE_DYN_PUTC(Dbg, Char)
#endif
#endif

#ifndef FTP_UPDATE_DYN_PUTS
#ifdef FTP_UPDATE_DBG
#define FTP_UPDATE_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_FTP_UPDATE|Dbg), Str, Len)
#else
#define FTP_UPDATE_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef FTP_UPDATE_DYN_PUTASCC
#ifdef FTP_UPDATE_DBG
#define FTP_UPDATE_DYN_PUTASCC(Dbg, Char)		DYN_PUTASCC((M_DBG_MODULE_FTP_UPDATE|Dbg), Char)
#else
#define FTP_UPDATE_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef FTP_UPDATE_DYN_PUTASCS
#ifdef FTP_UPDATE_DBG
#define FTP_UPDATE_DYN_PUTASCS(Dbg, Str, Len)	DYN_PUTASCS((M_DBG_MODULE_FTP_UPDATE|Dbg), Str, Len)
#else
#define FTP_UPDATE_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef FTP_UPDATE_DYN_PRINT
#ifdef FTP_UPDATE_DBG
#define FTP_UPDATE_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_FTP_UPDATE|Dbg), Fmt, ##Arg)
#else
#define FTP_UPDATE_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*

static FtpupdateMes FtpudateInfo;
static pthread_t s_sFtpUpdateThread;
uint8_t ftp_update_state;//  upgrade state 0 -- none   1 --- busying  2 --- ok

int ftp_update()
{
	int iRet;
	char tmp[FTP_COMMAND_TMP_SIZE];

	//wget ftp://yejianda:666888Ab@192.168.1.101/x/openwrt-ar71xx-generic-el-mini-squashfs-sysupgrade.bin  -P ../
	sprintf(tmp,"wget -c ftp://%s:%s@%s -P %s -T %d -t 1000",FtpudateInfo.ftp_username,FtpudateInfo.ftp_password,FtpudateInfo.ftp_sou_filename,FtpudateInfo.ftp_tar_Dir,FtpudateInfo.ftp_timeout);
	FTP_UPDATE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ftp_update] To start the download [%s]\r\n",FtpudateInfo.ftp_sou_filename);
	FTP_UPDATE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ftp_update] Please wait just a few minutes\r\n");
	FTP_UPDATE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "\r\n");
	iRet = system(tmp);
	if(iRet < 0 )
	{
		FTP_UPDATE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ftp_update] FTPGET Download is fail!!!!\r\n");
		ftp_update_state = 0;

		iRet = pthread_join(s_sFtpUpdateThread, NULL);
		/*
		if (iRet != 0)
		{
			FTP_UPDATE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ftp_update] Joining thread 'ftp_update' fail, errno:%d\r\n", errno);
			//abort();
		}
		else
		{
			FTP_UPDATE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ftp_update]  pthread exit\r\n");
		}*/
		return -1;
	}
	if(0 == iRet)
	{
		FTP_UPDATE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ftp_update] FTPGET Download is finish [%s] \r\n",FtpudateInfo.ftp_sou_filename);
		ftp_update_state = 2;

		iRet = pthread_join(s_sFtpUpdateThread, NULL);
		return 0;
	}
	return 0;

}



uint8_t ftp_update_listen_state()
{
	return ftp_update_state;
}

int ftp_upgrade(char ftp_username[],char ftp_password[],char ftp_sou_filename[],
                char ftp_tar_Dir[],int ftp_timeout)
{
	int iRet;
	FtpupdateMes *FtpInfo;

	FtpInfo=&FtpudateInfo;

	strcpy(FtpInfo->ftp_username,ftp_username);
	strcpy(FtpInfo->ftp_password,ftp_password);
	strcpy(FtpInfo->ftp_sou_filename,ftp_sou_filename);
	strcpy(FtpInfo->ftp_tar_Dir,ftp_tar_Dir);
	FtpInfo->ftp_timeout = ftp_timeout;

	ftp_update_state = 1;
	iRet = pthread_create(&s_sFtpUpdateThread, NULL, ftp_update, NULL);
	if (iRet < 0)
	{
		FTP_UPDATE_DYN_PRINT(M_DBG_LEVEL_DEBUG, "[ftp_update]Create thread 'ftp_update' fail, errno:%d.\r\n", errno);
		return -1;
	}
	return iRet;
}






