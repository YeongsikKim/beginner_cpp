// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// TETRIS.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

//ts_main Global Variable
HINSTANCE		g_hInst;
HWND			g_hWndMain;
HWND			g_hList;
HWND			g_hRoomCreate;
HWND			g_hOKbutton2;
HWND			g_hEdit;
HWND			g_hReadyButton;
RECT			g_tGameRec;
RECT			g_tRecvRec;

tag_Status		GameStatus;

int	board[BW+2][BH+2];
int g_iWidth;
int g_iHeigth;
int g_iBrick;
int g_iRot;
int g_iNBrick;
int g_iScore;
int g_iBrickNum;
int g_iInterval;
HBITMAP g_tBit[11];



//ts_socket Global Variable
SOCKET		g_hSock;
SOCKET		g_hWRSock;
char		g_cBuf[SMALLBUF];
HWND		g_hChatViewEdit;
HWND		g_hChatInputEdit;
HWND		g_hOkbutton;
LVITEMA		g_tLVItem;



BITMAPINFOHEADER	g_tBitmap_InfoHeader;
BITMAPFILEHEADER	g_tBitmap_FileHeader;
LPBITMAPINFO		g_pBitmap_Info;

LPVOID	g_pBody;
HDC		g_hCompare;
HDC		g_hTempDC;
int		g_iSize;
HDC		g_hMemDC;
HDC		g_hRecvMemDC;
HGDIOBJ g_old_obj;
HBITMAP g_hBitmap;

int		g_iRecvSize;
LPVOID	g_pRecvBody;


LPVOID	g_pBMPFile;
int		g_iFileSize;




//ts_waiting_room global variable
LPNMITEMACTIVATE g_lpNIA;
int	g_iSaveRoomNumber;
BOOL g_bWhether_CreateRoom;

map<SOCKET, LPPACKET_BODY> mPACKET;
map<SOCKET, LPPACKET_BODY>::iterator itPacket;