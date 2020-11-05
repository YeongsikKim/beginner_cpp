// TESTProgram.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	char buf[100] = {0,};
	int iNum = 0;

	sprintf(buf, "11108");
	printf("%s\n", buf);
	printf("%s\n", (int)buf);
	printf("%d\n", buf);
	iNum = atoi(buf);
	printf("%d\n", iNum);
	return 0;
}

