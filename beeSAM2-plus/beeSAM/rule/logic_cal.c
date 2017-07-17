#include "logic_cal.h"

static char LOGIC_POST[LOGIC_BUF_MAX] =
{ 0 };

void trans_to_polan (const char* log_expr)
{
	char stack[LOGIC_BUF_MAX] =
	{ 0 };
	char str[LOGIC_BUF_MAX] =
	{ 0 };
	char ch;
	int i, t, top = 0;
	memcpy(str + 1, log_expr, strlen(log_expr));

	t = 1;
	i = 1;
	ch = str[i];
	i++;

	while (ch != '#')
	{
		switch (ch)
		{
		case '|':
			//case '-':
			while (top != 0 && stack[top] != '(')
			{
				LOGIC_POST[t] = stack[top];
				top--;
				t++;
			}
			top++;
			stack[top] = ch;
			break;
		case '&':
			//case '/':
			while (stack[top] == '*' || stack[top] == '/')
			{
				LOGIC_POST[t] = stack[top];
				top--;
				t++;
			}
			top++;
			stack[top] = ch;
			break;
		case '(':
			top++;
			stack[top] = ch;
			break;
		case ')':
			while (stack[top] != '(')
			{
				LOGIC_POST[t] = stack[top];
				top--;
				t++;
			}
			top--;
			break;
		case ' ':
			break;
		default:
			while (isdigit(ch) || ch == '.')
			{
				LOGIC_POST[t] = ch;
				t++;
				ch = str[i];
				i++;
			}
			i--;
			LOGIC_POST[t] = ' ';	//数字之后要加空格，以避免和后面的数字连接在一起无法正确识别真正的位数
			t++;
		}
		ch = str[i];
		i++;
	}
	while (top != 0)
	{
		LOGIC_POST[t] = stack[top];
		t++;
		top--;
	}
	LOGIC_POST[t] = ' ';
}

int cal_value ()
{
	float stack[LOGIC_BUF_MAX];
	char ch, str[LOGIC_BUF_MAX];
	int i, t = 1, top = 0;
	ch = LOGIC_POST[t];
	t++;

	while (ch != ' ')
	{
		switch (ch)
		{
		case '|':
			stack[top - 1] = stack[top - 1] || stack[top];
			top--;
			break;
			/*case '-':
			 stack[top-1] = stack[top-1] - stack[top];
			 top--;
			 break;*/
		case '&':
			stack[top - 1] = stack[top - 1] && stack[top];
			top--;
			break;
			/*case '/':
			 if(stack[top]!= 0)
			 stack[top-1] = stack[top-1]/stack[top];
			 else
			 {
			 printf("\n\t闄ら浂閿欒锛乗n");
			 exit(0);
			 }
			 top--;
			 break;*/
		default:
			i = 0;
			while (isdigit(ch) || ch == '.')
			{
				str[i] = ch;
				i++;
				ch = LOGIC_POST[t];
				t++;
			}
			str[i] = '\0';
			top++;
			stack[top] = atof(str);
		}
		ch = LOGIC_POST[t];
		t++;
	}
	return (int) stack[top];
}

int is_legal_expr (const char* log_expr)
{
	unsigned int l_bro = 0;
	unsigned int r_bro = 0;

	while (*log_expr)
	{
		if ('(' == *log_expr)
		{
			r_bro++;
			log_expr++;
			continue;
		}

		if (')' == *log_expr)
		{
			l_bro++;
			log_expr++;
			continue;
		}

		if ('#' == *log_expr)
		{
			log_expr++;
			continue;
		}

		if (('0' <= *log_expr) && ('9' >= *log_expr))
		{
			log_expr++;
			continue;
		}

		if (('A' <= *log_expr) && ('F' >= *log_expr))
		{
			log_expr++;
			continue;
		}

		if ('&' == *log_expr)
		{
			log_expr++;
			continue;
		}
		if ('|' == *log_expr)
		{
			log_expr++;
			continue;
		}
		return LOG_EXPR_ERR;
	}
	if (l_bro != r_bro)
	{
		return LOG_EXPR_ERR;
	}
	return 0;
}

int Cov_To_Logic (const char* exptr, const unsigned char* buf, char* log_exptr, size_t len)
{
	unsigned int i = 0;
	unsigned int id = 0;
	unsigned int h_id = 1;
	while (*exptr)
	{
		if (i >= len)
		{
			return LOG_BUF_ERR;
		}
		if (('0' <= *exptr) && ('9' >= *exptr))
		{
			id = *exptr - '0' + id * h_id;
			h_id <<= 4;
			exptr++;
			continue;
		}
		if (('A' <= *exptr) && ('F' >= *exptr))
		{
			id = (*exptr - 'A' + 10) + id * h_id;
			h_id <<= 4;
			exptr++;
			continue;
		}

		if (0 == id)
		{
			*log_exptr++ = *exptr++;
			i++;
		}
		else
		{
			*log_exptr++ = buf[id] + '0';
			i++;
			*log_exptr++ = *exptr++;
			i++;
			id = 0;
			h_id = 1;
		}
	}

	if (id != 0)
	{
		*log_exptr++ = buf[id] + '0';

		id = 0;
		h_id = 1;
	}
	*log_exptr++ = '#';
	return 0;

}

int Get_Logic_Val (const char* log_expr, const unsigned char* logic_buf)
{
	int iRet = 0;
	char tmp[LOGIC_BUF_MAX] =
	{ 0 };
	iRet = is_legal_expr(log_expr);
	if (iRet < 0)
	{
		PRINT("[Get_Logic_Val] Call: is_legal_expr error :%d\r\n",iRet);
		return iRet;
	}
	iRet = Cov_To_Logic(log_expr, logic_buf, tmp, LOGIC_BUF_MAX);
	if (iRet < 0)
	{
		return iRet;
	}
	trans_to_polan(tmp);
	iRet = cal_value();
	memset(LOGIC_POST, 0, sizeof(LOGIC_POST));
	return iRet;

}

