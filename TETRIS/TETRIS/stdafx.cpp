// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// TETRIS.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

//TETRIS Global Variable
HINSTANCE		g_hInst;
HWND			g_hWndMain;
HWND			g_hList;
HWND			g_hRoomCreate;
HWND			g_hOKbutton2;
HWND			g_hRoomEdit;
HWND			g_hResetKey;
HWND			g_hReadyButton;
HWND			g_hChatWindow;
SOCKET			g_hSockRoom;

tag_Status		eGameStatus;



int	board[BW+2][BH+2];
int g_iWidth, g_iHeigth;
int g_iBrick, g_iRotation;
int g_iNumBrick;
int g_iScore;
int g_iBrickNum;
int iInterval;
HBITMAP hBit[11];



//Chatting Global Variable
SOCKET		g_hSock;
HWND		g_hButtonMakeRoom;
HWND		g_hChatting;
HWND		g_hOKbutton;
LVITEMA		g_tLI;


BITMAPINFOHEADER	g_tBmpInfoHeader;
BITMAPFILEHEADER	g_tBmpFileHeader;
LPBITMAPINFO		g_lpBmpHeader;

LPVOID		g_lpBody;
HDC			g_hCompare;
HDC			g_hTempDC;
int			g_iSize;
HDC			g_hMemDC;
HDC			g_hRecvMemDC;
HGDIOBJ		g_old_obj;
HBITMAP		g_hBitmap;



LPVOID lpBMPFile;
int iFileSize;
