#ifndef _LOGIC_CAL_H_
#define _LOGIC_CAL_H_

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "common.h"

#define LOGIC_BUF_MAX 100

int Get_Logic_Val (const char* log_expr, const unsigned char* logic_buf);

#endif

