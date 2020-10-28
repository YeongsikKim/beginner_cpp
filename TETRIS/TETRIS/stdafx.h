// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>
#include <WinSock2.h>
#include <WinUser.h>

// C의 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdarg.h>
#include <stdio.h>
#include <commctrl.h>



#include "TCP_Chatting.h"
#include "TETRIS.h"
#include "waiting_room.h"


//WaitingRoom define
#define MAX_PEOPLE		3


//ChattingWnd.cpp define
#define MAX_LOADSTRING	100
#define BUFSIZE			512
#define WM_SOCKET		(WM_USER+1)
#define WM_SOCKET2		(WM_USER+2)


//TESTRIS.cpp define
#define BW				10
#define BH				20
#define TS				24
#define random(n) (rand()%n)


//Function of TETRIS
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
VOID DrawScreen(HDC hdc);
VOID MakeNewBrick();
int GetAround(int x, int y, int b, int r);
BOOL MoveDown();
VOID TestFull();
VOID PrintTile(HDC hdc, int x, int y, int c);
VOID DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit);


struct Point
{
	int x, y;
};
enum {EMPTY, BRICK, WALL = 10};
enum tag_Status {GAMEOVER, RUNNING, PAUSE};


//TETRIS Global Variable
extern HINSTANCE	g_hInst;								
extern HWND			hWndMain;
extern int			board[BW+2][BH+2];
extern int			nx, ny;
extern int			brick, rot;
extern int			nbrick;
extern int			score;
extern int			bricknum;
extern int			iInterval;
extern HBITMAP		hBit[11];
extern HWND			hList; 
extern HWND			hRoomCreate; 
extern HWND			hOKbutton2;
extern HWND			hEdit;
extern SOCKET		sock_room;


//SOCKET Global variable
extern SOCKET sock;
extern char buf[BUFSIZE+1];
extern HWND hEdit1, hEdit2, hOKbutton;