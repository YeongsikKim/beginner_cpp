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

#define WIN32_LEAN_AND_MEAN
// Windows header
#include <windows.h>
#include <WinSock2.h>
#include <WinUser.h>

// C header
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdarg.h>
#include <stdio.h>
#include <commctrl.h>
#include <string>
#include <wingdi.h>

// C++ header
#include <map>


// User Header
#include "pre_define.h"
#include "ts_socket.h"
#include "ts_main.h"
#include "ts_waiting_room.h"
#include "err_debug.h"
#include "ts_paint.h"

using namespace std;


//ts_tetris Global Variable
extern HINSTANCE	g_hInst;								
extern HWND			g_hWndMain;
extern int			board[BW+2][BH+2];
extern int			g_iWidth, g_iHeigth;
extern int			g_iBrick, g_iRot;
extern int			g_iNBrick;
extern int			g_iScore;
extern int			g_iBrickNum;
extern int			g_iInterval;
extern int			g_iComboStack;
extern HBITMAP		g_tBit[11];
extern HWND			g_hList; 
extern HWND			g_hRoomCreate; 
extern HWND			g_hOKbutton2;
extern HWND			g_hEdit;
extern HWND			g_hReadyButton;
extern SOCKET		g_hWRSock;
extern tag_Status	GameStatus;
extern RECT			g_tGameRec;
extern RECT			g_tRecvRec;


//ts_socket Global variable
extern SOCKET g_hSock;
extern char g_cBuf[SMALLBUF];
extern HWND g_hChatViewEdit, g_hChatInputEdit, g_hOkbutton;
extern LVITEMA g_tLVItem;


//About Bitmap
extern BITMAPINFOHEADER g_tBitmap_InfoHeader;
extern BITMAPFILEHEADER g_tBitmap_FileHeader;
extern LPBITMAPINFO		g_pBitmap_Info;
extern LPVOID			g_pBody;
extern HDC				g_hCompare;
extern HDC				g_hTempDC;
extern int				g_iSize;
extern HDC				g_hMemDC;
extern HDC				g_hRecvMemDC;
extern HGDIOBJ			g_old_obj;
extern HBITMAP			g_hBitmap;

extern int				g_iRecvSize;
extern LPVOID			g_pRecvBody;

extern LPVOID			g_pBMPFile;
extern int				g_iFileSize;


//ts_waiting_room global variable
extern LPNMITEMACTIVATE g_lpNIA;
extern int	g_iSaveRoomNumber;
extern BOOL g_bWhether_CreateRoom;

extern map<SOCKET, LPPACKET_BODY> mPACKET;
extern map<SOCKET, LPPACKET_BODY>::iterator itPacket;

