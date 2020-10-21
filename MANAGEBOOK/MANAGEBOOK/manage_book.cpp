#include "stdafx.h"

int _tmain()
{
	while( TRUE )
	{
		system("cls");

		int iMenuNum		= 0;
		int iMenuStudent	= 0;
		int iMenuUniv		= 0;
		int iMenuBook		= 0;
		int iStudentID		= 0;
		int iUnivCode		= 0;
		int iBookNum		= 0;
		int iExitNum		= 0;


		printf("\n\n");
		printf("Book Management Program\n");
		printf("Choose the menu\n\n");
		printf("1. Student\n");
		printf("2. University\n");
		printf("3. Book\n");
		printf("4. Save\n");
		printf("5. Load\n");
		printf("0. End this program\n");
		printf("\n\n");
		printf("Select : ");

		scanf_s("%d", &iMenuNum, sizeof(int));

		switch ( iMenuNum )
		{
		case STUDENT:
			{
				CStudent cStudent;
				system("cls");
				if ( g_pHeadUniversity == NULL )
				{
					printf("You NEED to enroll university info, first");
					Sleep(3000);
					continue;
				}

				cStudent.NodeStudentView();
				printf("\n\n\n");
				printf("1. Create\n");
				printf("2. Revise\n");
				printf("3. Delete\n");
				printf("0. Back\n");
				printf("\n\n");
				printf("Select : ");

				scanf_s("%d", &iMenuStudent, sizeof(int));

				switch ( iMenuStudent )
				{
				case CREATE: 
					{
						system("cls");
						cStudent.NodeStudentCreate();
						break;
					}
				case REVISE: 
					{
						system("cls");
						cStudent.NodeStudentView();

						printf("\n\n");
						printf("Select the ID : ");
						scanf_s("%d", &iStudentID, sizeof(int));
						cStudent.NodeStudentRevise(iStudentID);
						break;
					}
				case ERASE:
					{
						system("cls");
						cStudent.NodeStudentView();

						printf("\n\n");
						printf("Select : ");

						scanf_s("%d", &iStudentID, sizeof(int));
						cStudent.NodeStudentErase(iStudentID);
						break;
					}
				case BACK:
					{

						break;
					}
				}
				break;
			}
		case UNIVERSITY: 
			{
				CUniversity cUniversity;
				system("cls");
				cUniversity.NodeUniversityView();
				printf("\n\n");
				printf("1. Create\n");
				printf("2. Revise\n");
				printf("3. Delete\n");
				printf("0. Back\n");
				printf("\n\n");
				printf("Select : ");

				scanf_s("%d", &iMenuUniv, sizeof(int));

				switch ( iMenuUniv )
				{
				case CREATE: 
					{
						system("cls");
						cUniversity.NodeUniversityCreate();
						break;
					}
				case REVISE:
					{
						system("cls");
						cUniversity.NodeUniversityView();

						printf("\n\n");
						printf("Select the university code : ");
						scanf_s("%d", &iUnivCode, sizeof(int));
						cUniversity.NodeUniversityRevise(iUnivCode);
						break;
					}
				case ERASE:
					{
						system("cls");
						cUniversity.NodeUniversityView();

						printf("\n\n");
						printf("Select : ");
						scanf_s("%d", &iUnivCode, sizeof(int));

						cUniversity.NodeUniversityErase(iUnivCode);
						break;
					}
				case BACK:
					{
						break;
					}
				}
				break;
			}
		case BOOK: 
			{
				CBook cBook;
				system("cls");
				cBook.NodeBookView();

				printf("\n\n");
				printf("1. Create\n");
				printf("2. Revise\n");
				printf("3. Delete\n");
				printf("4. Rental\n");
				printf("5. Return\n");
				printf("0. Back\n");
				printf("\n\n");
				printf("Select : ");
				scanf_s("%d", &iMenuBook, sizeof(int));

				switch ( iMenuBook )
				{
				case CREATE: 
					{
						system("cls");
						cBook.NodeBookCreate();
						break;
					}
				case REVISE:
					{
						system("cls");
						cBook.NodeBookView();
						printf("\n\n");
						printf("Select the Book Number which you want to change : ");
						scanf_s("%d", &iBookNum, sizeof(int));

						cBook.NodeBookRevise(iBookNum);
						break;
					}
				case ERASE:
					{
						system("cls");
						cBook.NodeBookView();

						printf("\n\n");
						printf("Select : ");
						scanf_s("%d", &iBookNum, sizeof(int));

						cBook.NodeBookErase(iBookNum);
						break;
					}
				case RENTAL:
					{
						system("cls");
						if ( g_pHeadStudent == NULL )
						{
							printf("[ERROR] You have to create student.\n");
							Sleep(3000);
							break;
						}
						printf("Please Enter the your ID\n");
						printf("ID : ");

						scanf_s("%d", &iStudentID, sizeof(int));

						cBook.RentalService(iStudentID);
						break;
					}
				case RETURN:
					{
						cBook.ReturnService();
						break;
					}
				case BACK:
					{
						break;
					}
				}

				break;
			}
		case SAVE: 
			{
				system("cls");
				SaveInFile_University();
				SaveInFile_Student();
				SaveInFile_Book();
				printf("Successfully Saved...");
				Sleep(1000);
				break;
			}
		case LOAD: 
			{
				system("cls");

				int iSelNum			= 0;
				//Warning message + Delete All//
				printf("Are you sure to load?\n");
				printf("We have to RESET to Load\n");
				printf("\n");
				printf("1. Load\n");
				printf("2. Exit\n");
				printf("\n\n");
				printf("Select : ");

				scanf_s("%d", &iSelNum, sizeof(int));

				//Not Load
				if ( iSelNum == 2 )
				{
					break;
				}

				//All Node Delete in here before Load
				AllNodeDelete();


				LoadFromFile_University();
				LoadFromFile_Student();
				LoadFromFile_Book();

				Sleep(1500);
				break;
			}
		case EXIT: 
			{
				system("cls");
				printf("Are you sure END THIS PROGRAM?\n Please save the data\n");
				printf("1. Exit\n");
				printf("2. Back\n");
				printf("Select : ");

				scanf_s("%d", &iExitNum, sizeof(int));

				switch ( iExitNum )
				{
				case 1: 
					{
						AllNodeDelete();
						system("cls");
						printf("End this program\n");

						return 0;
					}
				case 2:
					{
						break;
					}
				}
			}
		default :
			{
				break;
			}
		}
	}
}


VOID SaveInFile_University()
{
	int iNameSize	= 0;
	HANDLE hUniv	= NULL;
	DWORD dwWrite	= 0;
	DWORD dwSeek	= 0;
	PBYTE pBuf		= NULL;
	BYTE bTempBuf[MAX_UNIVERSITY_NAME] = {0,};
	LPMANAGEBOOK_UNIVERSITY pUniv	= NULL;

	pBuf		= new BYTE[TEN_MEGABYTE];
	ZeroMemory(pBuf, TEN_MEGABYTE);

	hUniv		= CreateFile(_T("University_data.txt"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	pUniv		= g_pHeadUniversity;

	while ( pUniv != NULL )
	{
		pBuf[dwSeek]	= pUniv->iUniversityNumber;
		dwSeek			+= sizeof(int);

		memcpy(bTempBuf, pUniv->wUnivName, sizeof(BYTE[MAX_UNIVERSITY_NAME]));
		iNameSize	= 0;
		while ( bTempBuf[iNameSize] != NULL )
		{
			iNameSize++;
		}
		memcpy(pBuf + dwSeek, bTempBuf, iNameSize);
		while ( pBuf[dwSeek] != NULL )
		{
			dwSeek++;
		}
		dwSeek++;
		pBuf[dwSeek]		= (BYTE)'\n';
		dwSeek++;

		pUniv		= (LPMANAGEBOOK_UNIVERSITY) pUniv->pNext;
	}
	WriteFile(hUniv, pBuf, dwSeek, &dwWrite, NULL);
	CloseHandle(hUniv);

	delete pBuf;
	pBuf	= NULL;

}

VOID SaveInFile_Student()		//Save Student_data.txt
{
	int iNameSize	= 0;
	HANDLE hStudent	= NULL;
	DWORD dwWrite	= 0;
	DWORD dwSeek	= 0;
	PBYTE pBuf		= NULL;
	BYTE bTempBuf[MAX_STUDENT_NAME] = {0,};
	LPMANAGEBOOK_STUDENT pStudent = NULL;

	dwWrite	= 0;
	dwSeek	= 0;

	pBuf		= new BYTE[TEN_MEGABYTE];
	ZeroMemory(pBuf, TEN_MEGABYTE);

	hStudent		= CreateFile(_T("Student_data.txt"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	pStudent		= g_pHeadStudent;

	while ( pStudent != NULL )
	{
		pBuf[dwSeek]	= pStudent->iID;
		dwSeek			+= sizeof(int);

		memcpy(bTempBuf, pStudent->wName, MAX_STUDENT_NAME);
		iNameSize	= 0;
		while ( bTempBuf[iNameSize] != NULL )
		{
			iNameSize++;
		}
		memcpy(pBuf + dwSeek, bTempBuf, iNameSize);
		while ( pBuf[dwSeek] != NULL )
		{
			dwSeek++;
		}
		dwSeek++;

		memcpy(bTempBuf, pStudent->wUniversityName, MAX_STUDENT_NAME);
		iNameSize	= 0;
		while (bTempBuf[iNameSize] != NULL)
		{
			iNameSize++;
		}
		memcpy(pBuf + dwSeek, bTempBuf, iNameSize);

		dwSeek		+= iNameSize + 1;

		pBuf[dwSeek]		= (BYTE)'\n';
		dwSeek++;

		pStudent		= (LPMANAGEBOOK_STUDENT) pStudent->pNext;
	}
	WriteFile(hStudent, pBuf, dwSeek, &dwWrite, NULL);
	CloseHandle(hStudent);

	delete pBuf;
	pBuf	= NULL;

}

	

VOID SaveInFile_Book()			//Save Book_data.txt
{
	int iNameSize	= 0;
	HANDLE hBook	= NULL;
	DWORD dwWrite	= 0;
	DWORD dwSeek	= 0;
	PBYTE pBuf		= NULL;
	BYTE bTempBuf[MAX_BOOK_NAME] = {0,};
	LPMANAGEBOOK_BOOK pBook = NULL;

	dwWrite	= 0;
	dwSeek	= 0;

	pBuf		= new BYTE[TEN_MEGABYTE];
	ZeroMemory(pBuf, TEN_MEGABYTE);

	hBook		= CreateFile(_T("Book_data.txt"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	pBook		= g_pHeadBook;

	while ( pBook != NULL )
	{
		pBuf[dwSeek]	= pBook->iBookNumber;
		dwSeek			+= sizeof(int);

		memcpy(bTempBuf, pBook->wBookName, MAX_BOOK_NAME);
		iNameSize	= 0;
		while ( bTempBuf[iNameSize] != NULL )
		{
			iNameSize++;
		}
		
		memcpy(pBuf + dwSeek, bTempBuf, iNameSize);
		while ( pBuf[dwSeek] != NULL )
		{
			dwSeek++;
		}
		dwSeek++;

		pBuf[dwSeek]	= pBook->iBookStatus;
		dwSeek			+= sizeof(int);

		pBuf[dwSeek]	= pBook->iRentalID;
		dwSeek			+= sizeof(int);

		dwSeek++;

		pBuf[dwSeek]		= (BYTE)'\n';
		dwSeek++;

		pBook		= (LPMANAGEBOOK_BOOK) pBook->pNext;
	}
	WriteFile(hBook, pBuf, dwSeek, &dwWrite, NULL);
	CloseHandle(hBook);

	delete pBuf;
	pBuf	= NULL;

}

	

 // --> WINAPI
/*
CreatFile
ReadFile
WriteFile
CloseHandle
*/

VOID LoadFromFile_University()
{
	//Load University.txt
	int iNameSize		= 0;
	HANDLE hUniv		= NULL;
	DWORD dwRead		= 0;
	DWORD dwSeek		= 0;
	DWORD dwFileSize	= 0;
	PBYTE pBuf			= NULL;
	LPMANAGEBOOK_UNIVERSITY pUniv	= NULL;
	LPMANAGEBOOK_UNIVERSITY pTemp	= NULL;

	pBuf		= new BYTE[TEN_MEGABYTE];
	ZeroMemory(pBuf, TEN_MEGABYTE);

	hUniv		= CreateFile(_T("University_data.txt"), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	pUniv		= g_pHeadUniversity;

	dwFileSize	= GetFileSize(hUniv, NULL);
	
	ReadFile(hUniv, pBuf, dwFileSize, &dwRead, NULL);
	CloseHandle(hUniv);

	LPMANAGEBOOK_UNIVERSITY pNewUniv	= NULL;
	while (dwSeek < dwFileSize)
	{
		pNewUniv		= NULL;
		pNewUniv		= new MANAGEBOOK_UNIVERSITY;
		ZeroMemory(pNewUniv, sizeof(MANAGEBOOK_UNIVERSITY));

		memcpy(&(pNewUniv->iUniversityNumber), pBuf + dwSeek, sizeof(int));
		dwSeek		+= sizeof(int);
		iNameSize	= 0;
		while (pBuf[dwSeek + iNameSize] != NULL)
		{
			iNameSize++;
		}
	
		memcpy(pNewUniv->wUnivName, pBuf + dwSeek, iNameSize);
		dwSeek		+= iNameSize + 2;

		if (g_pHeadUniversity == NULL)
		{
			g_pHeadUniversity	= pNewUniv;
			g_pTailUniversity	= pNewUniv;
		}
		else
		{
			pTemp			= g_pTailUniversity;
			pTemp->pNext	= pNewUniv;
			pNewUniv->pPrev	= pTemp;
			g_pTailUniversity = pNewUniv;
		}
	}
	
	delete pUniv;
	pUniv	= NULL;
	pTemp	= NULL;
	
}


//Load Student
VOID LoadFromFile_Student()
{
	int iNameSize		= 0;
	HANDLE hStudent		= NULL;
	DWORD dwRead		= 0;
	DWORD dwSeek		= 0;
	DWORD dwFileSize	= 0;
	PBYTE pBuf			= NULL;
	LPMANAGEBOOK_STUDENT pStudent	= NULL;
	LPMANAGEBOOK_STUDENT pTemp		= NULL;


	pBuf		= new BYTE[TEN_MEGABYTE];
	ZeroMemory(pBuf, TEN_MEGABYTE);

	hStudent		= CreateFile(_T("Student_data.txt"), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);		////////////////////////////////
	pStudent		= g_pHeadStudent;

	dwFileSize	= GetFileSize(hStudent, NULL);
	
	ReadFile(hStudent, pBuf, dwFileSize, &dwRead, NULL);
	CloseHandle(hStudent);

	LPMANAGEBOOK_STUDENT pNewStudent	= NULL;
	while (dwSeek < dwFileSize)
	{
		pNewStudent		= NULL;
		pNewStudent		= new MANAGEBOOK_STUDENT;
		ZeroMemory(pNewStudent, sizeof(MANAGEBOOK_STUDENT));

		memcpy(&(pNewStudent->iID), pBuf + dwSeek, sizeof(int));
		dwSeek		+= sizeof(int);
		iNameSize	= 0;
		while (pBuf[dwSeek + iNameSize] != NULL)
		{
			iNameSize++;
		}
		
		memcpy(pNewStudent->wName, pBuf + dwSeek, iNameSize);
		dwSeek		+= iNameSize + 1;

		iNameSize	= 0;
		while (pBuf[dwSeek + iNameSize] != NULL)
		{
			iNameSize++;
		}
		memcpy(pNewStudent->wUniversityName, pBuf + dwSeek, iNameSize);
		dwSeek		+= iNameSize + 2;

		if (g_pHeadStudent == NULL)
		{
			g_pHeadStudent	= pNewStudent;
			g_pTailStudent	= pNewStudent;
		}
		else
		{
			pTemp			= g_pTailStudent;
			pTemp->pNext	= pNewStudent;
			pNewStudent->pPrev	= pTemp;
			g_pTailStudent = pNewStudent;
		}
	}

	delete pStudent;
	pStudent	= NULL;
	pTemp		= NULL;

}



VOID LoadFromFile_Book()
{
	int iNameSize		= 0;
	HANDLE hBook		= NULL;
	DWORD dwRead		= 0;
	DWORD dwSeek		= 0;
	DWORD dwFileSize	= 0;
	PBYTE pBuf			= NULL;
	LPMANAGEBOOK_BOOK pBook		= NULL;
	LPMANAGEBOOK_BOOK pTemp		= NULL;

	pBuf		= new BYTE[TEN_MEGABYTE];
	ZeroMemory(pBuf, TEN_MEGABYTE);

	hBook		= CreateFile(_T("Book_data.txt"), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);		////////////////////////////////
	pBook		= g_pHeadBook;

	dwFileSize	= GetFileSize(hBook, NULL);
	ReadFile(hBook, pBuf, dwFileSize, &dwRead, NULL);
	CloseHandle(hBook);

	LPMANAGEBOOK_BOOK pNewBook	= NULL;
	while (dwSeek < dwFileSize)
	{
		pNewBook		= NULL;
		pNewBook		= new MANAGEBOOK_BOOK;
		ZeroMemory(pNewBook, sizeof(MANAGEBOOK_BOOK));

		memcpy(&(pNewBook->iBookNumber), pBuf + dwSeek, sizeof(int));
		dwSeek		+= sizeof(int);
		iNameSize	= 0;
		while (pBuf[dwSeek + iNameSize] != NULL)
		{
			iNameSize++;
		}
		memcpy(pNewBook->wBookName, pBuf + dwSeek, iNameSize);
		dwSeek		+= iNameSize + 1;

		memcpy(&(pNewBook->iBookStatus), pBuf + dwSeek, sizeof(int));
		dwSeek		+= sizeof(int);

		memcpy(&(pNewBook->iRentalID), pBuf + dwSeek, sizeof(int));
		dwSeek		+= sizeof(int) + 2;

		if (g_pHeadBook == NULL)
		{
			g_pHeadBook	= pNewBook;
			g_pTailBook	= pNewBook;
		}
		else
		{
			pTemp			= g_pTailBook;
			pTemp->pNext	= pNewBook;
			pNewBook->pPrev	= pTemp;
			g_pTailBook = pNewBook;
		}
	}

	delete pBook;
	pBook	= NULL;
	pTemp	= NULL;

}


VOID AllNodeDelete()
{
	LPMANAGEBOOK_BOOK pDeleteBook			= g_pHeadBook;
	LPMANAGEBOOK_STUDENT pDeleteStudent		= g_pHeadStudent;
	LPMANAGEBOOK_UNIVERSITY pDeleteUniv		= g_pHeadUniversity;
	LPMANAGEBOOK_BOOK temp1					= NULL;
	LPMANAGEBOOK_STUDENT temp2				= NULL;
	LPMANAGEBOOK_UNIVERSITY temp3			= NULL;

	//Delete Book data
	while( pDeleteBook != NULL )
	{
		temp1		= pDeleteBook;
		pDeleteBook		= (LPMANAGEBOOK_BOOK)pDeleteBook->pNext;
		delete temp1;
	}
	g_pHeadBook			= NULL;
	g_pTailBook			= NULL;
	pDeleteBook			= NULL;

	//Delete Student data
	while( pDeleteStudent != NULL )
	{
		temp2		= pDeleteStudent;
		pDeleteStudent		= (LPMANAGEBOOK_STUDENT)pDeleteStudent->pNext;
		delete temp2;
	}
	g_pHeadStudent			= NULL;
	g_pTailStudent			= NULL;
	pDeleteStudent			= NULL;


	//Delete University data
	while( pDeleteUniv != NULL )
	{
		temp3		= pDeleteUniv;
		pDeleteUniv			= (LPMANAGEBOOK_UNIVERSITY)pDeleteUniv->pNext;
		delete temp3;
	}

	g_pHeadUniversity		= NULL;
	g_pTailUniversity		= NULL;
	pDeleteUniv				= NULL;
}