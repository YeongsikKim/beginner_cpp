// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//
#pragma warning(disable : 4996)
#pragma once
/*
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
*/
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
#include <string>
#include <wingdi.h>
#include <iostream>
#include <fstream>
#include <map>



#include "tetris_chatting.h"
#include "tetris_main.h"
#include "waiting_room.h"
#include "err_print.h"


#include "pre_define.h"

using namespace std;

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



//Packet
typedef struct _Packet_Header
{
	int iFlag;
	int iSize;
}PACKET_HEADER, *LPPACKET_HEADER;

typedef struct _Packet_Body
{
	char cData[BUFSIZE];
	int iCurRecv;
}PACKET_BODY, *LPPACKET_BODY;

//Socket info
typedef struct _SOCKETINFO
{
	SOCKET sock;
	char buf[BUFSIZE+1];
	int recvbytes;
	int sendbytes;
	BOOL recvdelayed;
}SOCKETINFO, *LPSOCKETINFO;

//Room info
typedef struct _ROOMINFO
{
	int iNum;
	int iPeopleIN;
	char cRoomName[NAMEBUF];
}ROOMINFO, *LPROOMINFO;


extern BOOL bRecvDelay;

//TETRIS Global Variable
extern HINSTANCE	g_hInst;								
extern HWND			g_hWndMain;
extern int			board[BW+2][BH+2];
extern int			g_iWidth, g_iHeigth;
extern int			g_iBrick, g_iRotation;
extern int			g_iNumBrick;
extern int			g_iScore;
extern int			g_iBrickNum;
extern int			iInterval;
extern HBITMAP		hBit[11];
extern HWND			g_hList; 
extern HWND			g_hRoomCreate; 
extern HWND			g_hOKbutton2;
extern HWND			g_hRoomEdit;
extern HWND			g_hReadyButton;
extern HWND			g_hResetKey;
extern HWND			g_hChatWindow;
extern SOCKET		g_hSockRoom;

extern tag_Status	eGameStatus;


//SOCKET Global variable
extern SOCKET g_hSock;
extern HWND g_hButtonMakeRoom, g_hChatting, g_hOKbutton;
extern LVITEMA g_tLI;


//About Bitmap
extern BITMAPINFOHEADER g_tBmpInfoHeader;
extern BITMAPFILEHEADER g_tBmpFileHeader;
extern LPBITMAPINFO g_lpBmpHeader;
extern LPVOID g_lpBody;
extern HDC g_hCompare;
extern HDC g_hTempDC;
extern int g_iSize;
extern HDC g_hMemDC;
extern HDC g_hRecvMemDC;
extern HGDIOBJ g_old_obj;
extern HBITMAP g_hBitmap;

extern int iRecvSize;
extern LPVOID lpRecvBody;

extern LPVOID lpBMPFile;
extern int iFileSize;





