// TESTProgram.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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

