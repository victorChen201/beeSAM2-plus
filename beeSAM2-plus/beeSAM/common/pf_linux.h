//#define	__M_FILE_NAME_pf_linux_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_pf_linux_H__
#define	__M_FILE_NAME_pf_linux_H__

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define _GNU_SOURCE

#define _XOPEN_SOURCE		500
//#define _POSIX_C_SOURCE		199309
#define _POSIX_C_SOURCE		200809

#include <features.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <netdb.h>
#include <dirent.h>
//#include <ifaddrs.h>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#ifndef UNUSED
#define UNUSED(_v)          (void)(_v)
#endif

#ifndef TRUE
#define TRUE	(1==1)
#endif
#ifndef FALSE
#define FALSE	(1==0)
#endif

#ifndef bool
typedef int		bool;
#endif

#ifndef true
#define true	(1==1)
#endif
#ifndef false
#define false	(1==0)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
typedef time_t T_SysTime;

#endif	//end of "#ifndef __M_FILE_NAME_pf_linux_H__".
