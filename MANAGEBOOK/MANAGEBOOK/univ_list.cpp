#include "stdafx.h"


VOID CUniversity::NodeUniversityCreate()
{
	int iData_ID		= 0;
	WCHAR wData_Name[MAX_UNIVERSITY_NAME] = {0,};

	LPMANAGEBOOK_UNIVERSITY pNew = NULL;
	pNew = new MANAGEBOOK_UNIVERSITY;
	ZeroMemory(pNew, sizeof(MANAGEBOOK_UNIVERSITY));

	printf("Please Insert University Number : ");
	scanf_s("%d", &iData_ID, sizeof(int));

	//Repeated Univ ID value blocked
	LPMANAGEBOOK_UNIVERSITY pRepeateUniv = g_pHeadUniversity;
	while ( pRepeateUniv != NULL )
	{
		if ( iData_ID == pRepeateUniv->iUniversityNumber )
		{
			printf("[ERROR] Repeated value...\n");
			Sleep(2000);
			return;
		}
		pRepeateUniv		= (LPMANAGEBOOK_UNIVERSITY)pRepeateUniv->pNext;
	}

	printf("Please Insert University Name : ");
	scanf_s("%s", wData_Name, sizeof(WCHAR[MAX_UNIVERSITY_NAME]));
	
	//Repeated Univ Name value blocked
	pRepeateUniv = g_pHeadUniversity;
	while ( pRepeateUniv != NULL )
	{
		if ( strcmp( (const char*) pRepeateUniv->wUnivName , (const char*) wData_Name ) == 0 )
		{
			printf("[ERROR] Repeated University Name...\n");
			Sleep(2000);
			return;
		}
		pRepeateUniv		= (LPMANAGEBOOK_UNIVERSITY) pRepeateUniv->pNext;
	}

	pNew->iUniversityNumber	= iData_ID;
	memcpy(pNew->wUnivName, wData_Name, sizeof(WCHAR[MAX_UNIVERSITY_NAME]));

	if ( g_pHeadUniversity == NULL )
	{
		g_pHeadUniversity = pNew;
		g_pTailUniversity = pNew;
		return;
	}

	LPMANAGEBOOK_UNIVERSITY p = g_pTailUniversity;
	p->pNext = pNew;
	pNew->pPrev = p;
	g_pTailUniversity = pNew;

}


VOID CUniversity::NodeUniversityRevise(int iCode)
{
	LPMANAGEBOOK_UNIVERSITY p	= g_pHeadUniversity;
	if ( p == NULL )
	{
		return;
	}

	int iMenu		= 0;
	int iTemp		= 0;
	wchar_t wName[MAX_UNIVERSITY_NAME];

	while ( p != NULL )
	{
		if ( p->iUniversityNumber == iCode )
		{
			printf("Found..\n");
			break;
		}

		p = (LPMANAGEBOOK_UNIVERSITY)p->pNext;
	}

	if ( p == NULL )
	{
		printf("[ERROR] Can't find this ID\n");
		return;
	}

	printf("Which one of University Code or University Name do you want to change?\n\n");
	printf("1. Code\n");
	printf("2. Name\n\n");
	printf("Select : ");

	scanf_s("%d", &iMenu, sizeof(int));

	switch ( iMenu )
	{
	case 1: 
		{
			printf("New Code : ");
			scanf_s("%d", &iTemp, sizeof(int));

			p->iUniversityNumber = iTemp;
			break;
		}
	case 2:
		{
			printf("New Name : ");
			scanf_s("%s", wName, sizeof(wchar_t[MAX_UNIVERSITY_NAME]));

			memcpy(p->wUnivName, wName, sizeof(wchar_t[MAX_UNIVERSITY_NAME]));
			break;
		}
	default: break;
	}
}

VOID CUniversity::NodeUniversityErase(int iUnivNum)
{
	LPMANAGEBOOK_UNIVERSITY p	= g_pHeadUniversity;

	while ( p != NULL )
	{
		if ( p->iUniversityNumber == iUnivNum )
		{
			printf("Founded...\n");
			break;
		}

		p = (LPMANAGEBOOK_UNIVERSITY)p->pNext;
	}

	if( p == NULL )
	{
		printf("[ERROR] Can not find data...\n");
		Sleep(2000);
		return;
	}

	if ( g_pHeadUniversity == NULL )
	{
		printf("[ERROR] NO DATA\n");
		Sleep(1000);
		return;
	}
	else if ( g_pHeadUniversity == g_pTailUniversity )
	{
		g_pHeadUniversity = NULL;
		g_pTailUniversity = NULL;
	}
	else if ( p == g_pHeadUniversity )
	{
		g_pHeadUniversity = (LPMANAGEBOOK_UNIVERSITY)p->pNext;
		LPMANAGEBOOK_UNIVERSITY(p->pNext)->pPrev = NULL;
	}
	else if ( p == g_pTailUniversity )
	{
		g_pTailUniversity = (LPMANAGEBOOK_UNIVERSITY)p->pPrev;
		LPMANAGEBOOK_UNIVERSITY(p->pPrev)->pNext = NULL;
	}
	else
	{
		LPMANAGEBOOK_UNIVERSITY(p->pPrev)->pNext = p->pNext;
		LPMANAGEBOOK_UNIVERSITY(p->pNext)->pPrev = p->pPrev;
	}

	delete p;
	p = NULL;
}

VOID CUniversity::NodeUniversityView()
{
	LPMANAGEBOOK_UNIVERSITY p	= g_pHeadUniversity;
	if ( p == NULL )
	{
		printf(" No data...\n");
		return;
	}

	printf("\tUniv Number\t");
	printf("Name\t\t");
	printf("\n");

	while( p != NULL )
	{
		printf("\t%d\t\t", p->iUniversityNumber);
		printf("%s\n", p->wUnivName);

		p = (LPMANAGEBOOK_UNIVERSITY)p->pNext;
	}
}