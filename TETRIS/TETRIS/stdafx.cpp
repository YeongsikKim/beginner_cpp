// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// TETRIS.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

//TETRIS Global Variable
HINSTANCE		g_hInst;								
HWND			hWndMain;
HWND			hList, hRoomCreate, hOKbutton2, hEdit;
HWND			g_hReadyButton;
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
SOCKET		hSock;
char		cBuf[CHATSIZE];
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


LPVOID lpBMPFile;
int iFileSize;


ofstream stream;
ifstream streamSending;
ofstream streamTest;


map<SOCKET, LPPACKET_BODY> mPACKET;
map<SOCKET, LPPACKET_BODY>::iterator itPacket;