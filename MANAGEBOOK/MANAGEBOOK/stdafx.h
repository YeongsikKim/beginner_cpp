#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <wchar.h>
#include <Windows.h>

// my_header
#include "pre_define.h"
#include "univ_list.h"
#include "student_list.h"
#include "book_list.h"

extern LPMANAGEBOOK_STUDENT			g_pHeadStudent;
extern LPMANAGEBOOK_STUDENT			g_pTailStudent;
extern LPMANAGEBOOK_UNIVERSITY		g_pHeadUniversity;
extern LPMANAGEBOOK_UNIVERSITY		g_pTailUniversity;
extern LPMANAGEBOOK_BOOK			g_pHeadBook;
extern LPMANAGEBOOK_BOOK			g_pTailBook;


VOID SaveInFile_University();
VOID SaveInFile_Student();
VOID SaveInFile_Book();
VOID LoadFromFile_University();
VOID LoadFromFile_Student();
VOID LoadFromFile_Book();

VOID AllNodeDelete();

