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
BITMAPFILEHEADER fh;
LPBITMAPINFO lpHeader;

LPVOID lpBody;
HDC hCompare;
HDC hTempDC;
int iSize;
HDC hMemDC;
HGDIOBJ old_obj;
HBITMAP hBitmap;

int iRecvSize;
LPVOID lpRecvBody;