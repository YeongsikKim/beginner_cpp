#include "stdafx.h"


VOID CStudent::NodeStudentCreate()
{
	int iData_ID				= 0;
	int iUnivCode				= 0;
	LPMANAGEBOOK_STUDENT pNew	= NULL;

	WCHAR wData_Name[MAX_STUDENT_NAME]		= {0,};

	pNew = new MANAGEBOOK_STUDENT;
	ZeroMemory(pNew, sizeof(MANAGEBOOK_STUDENT));

	printf("Please Insert your ID : ");
	scanf_s("%d", &iData_ID, sizeof(int));

	printf("Please Insert your Name : ");
	scanf_s("%s", wData_Name, sizeof(WCHAR[MAX_STUDENT_NAME]));

	CUniversity cUniversity;
	cUniversity.NodeUniversityView();
	printf("\n Selected University code : ");
	scanf_s("%d", &iUnivCode, sizeof(int));

	LPMANAGEBOOK_UNIVERSITY pHeadUniv = g_pHeadUniversity;

	while ( TRUE )
	{
		if ( pHeadUniv->iUniversityNumber == iUnivCode )
		{
			break;
		}

		if ( pHeadUniv == NULL )
		{
			printf("[ERROR] It is WRONG value !!\n");
			pHeadUniv = g_pHeadUniversity;
		}

		pHeadUniv	= LPMANAGEBOOK_UNIVERSITY(pHeadUniv->pNext);
	}

	pNew->iID	= iData_ID;
	memcpy(pNew->wName, wData_Name, sizeof(WCHAR[MAX_STUDENT_NAME]));
	memcpy(pNew->wUniversityName, pHeadUniv->wUnivName, sizeof(WCHAR[MAX_STUDENT_NAME]));

	if ( g_pHeadStudent == NULL )
	{
		g_pHeadStudent = pNew;
		g_pTailStudent = pNew;
		printf("Successfully saved\n");
		return;
	}

	LPMANAGEBOOK_STUDENT repeated_p = g_pHeadStudent;

	while ( repeated_p != NULL )
	{
		if ( iData_ID == repeated_p->iID )
		{
			printf("[ERROR] repeated value...\n");
			ZeroMemory(pNew, sizeof(MANAGEBOOK_STUDENT));
			delete pNew;
			Sleep(1000);
			return;
		}
		repeated_p = (LPMANAGEBOOK_STUDENT)repeated_p->pNext;
	}

	LPMANAGEBOOK_STUDENT p = g_pTailStudent;
	p->pNext		= pNew;
	pNew->pPrev		= p;
	g_pTailStudent	= pNew;
	printf("Successfully saved\n");

	printf("\n\n\n");


#ifdef DEBUG
	LPMANAGEBOOK_STUDENT DEBUG_p = g_pHeadStudent;

	while ( DEBUG_p != NULL )
	{
		printf("%d\n", DEBUG_p->iID);
		DEBUG_p = (LPMANAGEBOOK_STUDENT)DEBUG_p->pNext;
	}

#endif

}




VOID CStudent::NodeStudentRevise(int ID)
{
	LPMANAGEBOOK_STUDENT p	= g_pHeadStudent;
	if ( p == NULL )
	{
		return;
	}

	int iMenu		= 0;
	int iId			= 0;
	wchar_t wName[MAX_STUDENT_NAME];

	while ( p != NULL )
	{
		if ( p->iID == ID )
		{
			printf("Found..\n");
			break;
		}

		p = (LPMANAGEBOOK_STUDENT)p->pNext;
	}

	if ( p == NULL )
	{
		printf("Can't find this ID\n");
		return;
	}

	printf("Which one of ID or Name do you want to change?\n\n");
	printf("1. ID\n");
	printf("2. Name\n\n");
	printf("Select : ");

	scanf_s("%d", &iMenu, sizeof(int));

	switch ( iMenu )
	{
	case 1: 
		{
			printf("New ID : ");
			scanf_s("%d", &iId, sizeof(int));

			LPMANAGEBOOK_STUDENT repeated_p = g_pHeadStudent;
			while ( repeated_p != NULL )
			{
				if ( iId == repeated_p->iID )
				{
					printf("[ERROR] repeated value...\n");
					Sleep(1000);
					return;
				}
				repeated_p = (LPMANAGEBOOK_STUDENT)repeated_p->pNext;
			}


			p->iID = iId;
			break;
		}
	case 2:
		{
			printf("New Name : ");
			scanf_s("%s", wName, sizeof(wchar_t[MAX_STUDENT_NAME]));

			memcpy(p->wName, wName, sizeof(wchar_t[MAX_STUDENT_NAME]));
			break;
		}
	default: break;
	}
}

VOID CStudent::NodeStudentErase(int ID)
{
	LPMANAGEBOOK_STUDENT p	= g_pHeadStudent;

	while ( p != NULL )
	{
		if ( p->iID == ID )
		{
			printf("Founded...\n");
			break;
		}

		p = (LPMANAGEBOOK_STUDENT)p->pNext;
	}

	if( p == NULL )
	{
		printf("[ERROR] Can not find data...\n");
		Sleep(2000);
		return;
	}

	if ( g_pHeadStudent == NULL )
	{
		printf("[ERROR] NO DATA\n");
		Sleep(1000);
		return;
	}
	else if ( g_pHeadStudent == g_pTailStudent )
	{
		g_pHeadStudent	= NULL;
		g_pTailStudent	= NULL;
	}
	else if ( p == g_pHeadStudent )
	{
		g_pHeadStudent = (LPMANAGEBOOK_STUDENT)p->pNext;
		LPMANAGEBOOK_STUDENT(p->pNext)->pPrev = NULL;
	}
	else if ( p == g_pTailStudent )
	{
		g_pTailStudent = (LPMANAGEBOOK_STUDENT)p->pPrev;
		LPMANAGEBOOK_STUDENT(p->pPrev)->pNext = NULL;
	}
	else
	{
		LPMANAGEBOOK_STUDENT(p->pPrev)->pNext = p->pNext;
		LPMANAGEBOOK_STUDENT(p->pNext)->pPrev = p->pPrev;
	}

	delete p;
	p		= NULL;
}



VOID CStudent::NodeStudentView()
{
	LPMANAGEBOOK_STUDENT p	= g_pHeadStudent;
	if ( p == NULL )
	{
		printf(" No data...\n");
		return;
	}

	printf("\tID\t\t");
	printf("Name    \t\t");
	printf("Univ\n");
	printf("\n");

	while( p != NULL )
	{
		printf("\t%d\t\t", p->iID);
		printf("%s  \t", p->wName);
		printf("%s\n", p->wUniversityName);

		p = (LPMANAGEBOOK_STUDENT)p->pNext;
	}
}