#include "stdafx.h"

VOID CBook::NodeBookCreate()
{
	int iData_ID		= 0;
	WCHAR wData_Name[MAX_BOOK_NAME] = {0,};
	int iStatus			= Book_Return;

	LPMANAGEBOOK_BOOK pNew = NULL;
	pNew = new MANAGEBOOK_BOOK;
	ZeroMemory(pNew, sizeof(MANAGEBOOK_BOOK));

	printf("Please Insert Book ID : ");
	scanf_s("%d", &iData_ID, sizeof(int));

	printf("Please Insert Book Name : ");
	scanf_s("%s", wData_Name, sizeof(WCHAR[MAX_BOOK_NAME]));

	pNew->iBookNumber	= iData_ID;
	memcpy(pNew->wBookName, wData_Name, sizeof(WCHAR[MAX_BOOK_NAME]));
	pNew->iBookStatus	= iStatus;

	if ( g_pHeadBook == NULL )
	{
		g_pHeadBook = pNew;
		g_pTailBook = pNew;
	}
	else
	{
		LPMANAGEBOOK_BOOK p = g_pTailBook;
		p->pNext = pNew;
		pNew->pPrev = p;
		g_pTailBook = pNew;
	}
}



VOID CBook::NodeBookRevise(int iBookNum)
{
	LPMANAGEBOOK_BOOK p	= g_pHeadBook;
	if ( p == NULL )
	{
		return;
	}

	int iMenu		= 0;
	int iTemp		= 0;
	WCHAR wName[MAX_BOOK_NAME];

	while ( p != NULL )
	{
		if ( p->iBookNumber == iBookNum )
		{
			printf("Found..\n");
			break;
		}

		p = (LPMANAGEBOOK_BOOK)p->pNext;
	}

	if ( p == NULL )
	{
		printf("[ERROR] Can't find this Book\n");
		return;
	}

	printf("Which one of Book Number or Book Name do you want to change?\n\n");
	printf("1. Book Number\n");
	printf("2. Book Name\n\n");
	printf("Select : ");

	scanf_s("%d", &iMenu, sizeof(int));

	switch ( iMenu )
	{
	case 1: 
		{
			printf("New Book Number : ");
			scanf_s("%d", &iTemp, sizeof(int));

			p->iBookNumber = iTemp;
			break;
		}
	case 2:
		{
			printf("New Book Name : ");
			scanf_s("%s", wName, sizeof(wchar_t[MAX_BOOK_NAME]));

			memcpy(p->wBookName, wName, sizeof(wchar_t[MAX_BOOK_NAME]));
			break;
		}
	default: break;
	}
}


VOID CBook::NodeBookErase(int iBookNum)
{
	LPMANAGEBOOK_BOOK p	= g_pHeadBook;

	while ( p != NULL )
	{
		if ( p->iBookNumber == iBookNum )
		{
			printf("Founded...\n");
			break;
		}

		p = (LPMANAGEBOOK_BOOK)p->pNext;
	}

	if( p == NULL )
	{
		printf("[ERROR] Can not find data...\n");
		Sleep(2000);
		return;
	}

	if ( g_pHeadBook == NULL )
	{
		printf("[ERROR] NO DATA\n");
		Sleep(1000);
		return;
	}
	else if ( g_pHeadBook == g_pTailBook )
	{
		g_pHeadBook = NULL;
		g_pTailBook = NULL;
	}
	else if ( p == g_pHeadBook )
	{
		g_pHeadBook = (LPMANAGEBOOK_BOOK)p->pNext;
		LPMANAGEBOOK_BOOK(p->pNext)->pPrev = NULL;
	}
	else if ( p == g_pTailBook )
	{
		g_pTailBook = (LPMANAGEBOOK_BOOK)p->pPrev;
		LPMANAGEBOOK_BOOK(p->pPrev)->pNext = NULL;
	}
	else
	{
		LPMANAGEBOOK_BOOK(p->pPrev)->pNext = p->pNext;
		LPMANAGEBOOK_BOOK(p->pNext)->pPrev = p->pPrev;
	}

	delete p;
	p	= NULL;
}



VOID CBook::NodeBookView()
{
	LPMANAGEBOOK_BOOK p = g_pHeadBook;
	if ( p == NULL )
	{
		printf(" No data...\n");
		return;
	}
	printf("\tBook Number\t");
	printf("Name\t\t");
	printf("Status\n");
	printf("\n");

	while( p != NULL )
	{
		printf("\t%d\t\t", p->iBookNumber);
		printf("%s\t", p->wBookName);
		if ( p->iBookStatus == Book_Rental )
		{
			printf("Rental\t\t");
			printf("ID: %d\n", p->iRentalID);
		}
		else if ( p->iBookStatus == Book_Return )
		{
			printf("Exist\n");
		}

		p = (LPMANAGEBOOK_BOOK)p->pNext;
	}
}



VOID CBook::RentalService(int iStudentID)
{
	int iBookNum		= 0;
	system("cls");
	NodeBookView();

	printf("\n\n");
	printf("Select the Book Number\n");
	printf("If you go out here, Enter the value 0\n\n");
	printf("Select : ");

	scanf_s("%d", &iBookNum, sizeof(int));
	if ( iBookNum == 0 )
	{
		return;
	}

	LPMANAGEBOOK_BOOK p = g_pHeadBook;
	while ( p != NULL )
	{
		if ( p->iBookNumber == iBookNum )
		{
			printf("Founded...\n");
			if ( p->iBookStatus == Book_Rental )
			{
				printf("But This Book is NOT here..\n");
				Sleep(2000);
				return;
			}
			break;
		}
		p = (LPMANAGEBOOK_BOOK)p->pNext;
	}

	if ( p == NULL )
	{
		printf("[ERROR] Can't find the Book...");
		Sleep(2000);
		return;
	}

	p->iBookStatus	= Book_Rental;
	p->iRentalID	= iStudentID;
	printf("\n\nSuccessfully Rental is done\n");
	Sleep(2000);
}


VOID CBook::ReturnService()
{
	int iBookNum	= 0;
	system("cls");
	NodeBookView();

	printf("\n\n");
	printf("Input the Book Number to return : ");
	scanf_s("%d", &iBookNum, sizeof(int));

	LPMANAGEBOOK_BOOK p = g_pHeadBook;

	while ( p != NULL )
	{
		if ( iBookNum == p->iBookNumber )
		{
			printf("Founded ...\n");
			break;
		}
		p = (LPMANAGEBOOK_BOOK)p->pNext;
	}
	if ( p == NULL )
	{
		printf("[ERROR] Can't find the Book\n");
	}
	if ( p->iBookStatus == Book_Return )
	{
		printf("Already this book has be returned.\n");
		Sleep(2000);
		return;
	}

	p->iBookStatus = Book_Return;
	p->iRentalID = NULL;
}
