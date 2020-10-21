#pragma once

#define STUDENT				1
#define UNIVERSITY			2
#define BOOK				3
#define SAVE				4
#define LOAD				5
#define EXIT				0
#define MAX_STUDENT_NAME	50
#define MAX_UNIVERSITY_NAME 50
#define MAX_BOOK_NAME		100
#define	Book_Rental			0
#define Book_Return			1

// Define About Menu
#define CREATE				1
#define REVISE				2
#define ERASE				3
#define RENTAL				4
#define RETURN				5
#define BACK				0
#define TEN_MEGABYTE		1024*1024*10


typedef struct _managebook_student
{
	int		iID;
	WCHAR	wName[MAX_STUDENT_NAME];
	WCHAR	wUniversityName[MAX_UNIVERSITY_NAME];
	void *	pNext;
	void *	pPrev;
}MANAGEBOOK_STUDENT, *LPMANAGEBOOK_STUDENT;

typedef struct _university
{
	int		iUniversityNumber;
	WCHAR	wUnivName[MAX_UNIVERSITY_NAME];
	void *	pNext;
	void *	pPrev;
}MANAGEBOOK_UNIVERSITY, *LPMANAGEBOOK_UNIVERSITY;

typedef struct _book
{
	int		iBookNumber;
	int		iBookStatus;
	int		iRentalID;
	WCHAR	wBookName[MAX_BOOK_NAME];
	void *	pNext;
	void *	pPrev;
}MANAGEBOOK_BOOK, *LPMANAGEBOOK_BOOK;