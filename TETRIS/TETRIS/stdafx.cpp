// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// TETRIS.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"

//TETRIS Global Variable
HINSTANCE		g_hInst;								
HWND			hWndMain;
HWND			hList, hRoomCreate, hOKbutton2, hEdit;
SOCKET			sock_room;




int	board[BW+2][BH+2];
int nx, ny;
int brick, rot;
int nbrick;
int score;
int bricknum;
int iInterval;
HBITMAP hBit[11];



//Chatting Global Variable
SOCKET		sock;
char		buf[BUFSIZE+1];
HWND		hEdit1, hEdit2, hOKbutton;
LVITEMA		LI;




BITMAPINFOHEADER bi;
LPBITMAPINFO lpHeader;

extern LPVOID lpBody = NULL;
extern HDC hCompare = NULL;
extern HDC hTempDC = NULL;
extern int iSize = 0;
extern HDC hMemDC = NULL;
extern HGDIOBJ old_obj = NULL;
extern HBITMAP hBitmap = NULL;