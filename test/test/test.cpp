// test.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int iLen1		= 0;
	int iLen2		= 0;
	int iNum		= 0;
	char cbuf[10]	= {0,};

	strcpy(cbuf, "Hello");
	iLen1	= strlen(cbuf);
	printf("Length1 = %d\n", iLen1);

	cbuf[iLen1+1]	= '2';
	iLen2	= strlen(cbuf);
	printf("Length2	= %d\n",iLen2);

	iNum		= (int)cbuf[iLen1+1] - '0';

	printf("iNum	= %d\n", iNum);
	

	return 0;
}

