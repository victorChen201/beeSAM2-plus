#ifndef RULE_SYS_H
#define RULE_SYS_H

#include "common.h"

#if (defined __GNUC__)

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define rule_malloc( _a )   malloc( (_a) )
#define rule_delayms( _t )  usleep(((_t)*1000))
#define rule_free( _a )     free( (_a))
#elif (defined __ICCARM__)
#define rule_malloc( _a )   malloc( (_a) )
#define rule_delayms( _t )   msleep((_t))
#define rule_free( _a )     free( (_a))
#endif




#endif
