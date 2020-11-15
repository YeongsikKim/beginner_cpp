// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
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

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>
#include <WinSock2.h>
#include <WinUser.h>

// C�� ��Ÿ�� ��� �����Դϴ�.
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



#include "TCP_Chatting.h"
#include "TETRIS.h"
#include "waiting_room.h"


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
extern HWND			g_hReadyButton;
extern SOCKET		sock_room;

extern tag_Status	GameStatus;


//SOCKET Global variable
extern SOCKET hSock;
extern char cBuf[CHATSIZE];
extern HWND hEdit1, hEdit2, hOKbutton;
extern LVITEMA LI;


//About Bitmap
extern BITMAPINFOHEADER bi;
extern BITMAPFILEHEADER fh;
extern LPBITMAPINFO lpHeader;
extern LPVOID lpBody;
extern HDC hCompare;
extern HDC hTempDC;
extern int iSize;
extern HDC hMemDC;
extern HGDIOBJ old_obj;
extern HBITMAP hBitmap;

extern int iRecvSize;
extern LPVOID lpRecvBody;

extern LPVOID lpBMPFile;
extern int iFileSize;

extern ofstream stream;
extern ifstream streamSending;
extern ofstream streamTest;


extern map<SOCKET, LPPACKET_BODY> mPACKET;
extern map<SOCKET, LPPACKET_BODY>::iterator itPacket;

