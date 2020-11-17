#include <stdio.h>
#include <string.h>

int main()
{
	int i = 0;
	int j = 0;
	int iSize = 0;
	char* pName[5] = {0,};

	pName[0] = "Jung jae Une";
	pName[1] = "Han Woo Ryoung";
	pName[2] = "Byun Ji Ha";
	pName[3] = "Lee Do Geum";
	pName[4] = "Hong Jae Mok";

	iSize = sizeof(pName) / 4;

	printf("iSize = %d, sizeof(pName) = %d\n", iSize, sizeof(pName));

	for (i = 0; i < iSize; i++)
	{
		for (j = 0; j < strlen(pName[i]); j++)
		{
			while(*pName[i] ++ != ' ');
			puts(pName[i]);
			break;
		}
	}

	return 0;
}
