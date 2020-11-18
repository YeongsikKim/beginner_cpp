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

<<<<<<< HEAD

#include "tetris_chatting.h"
#include "tetris_main.h"
#include "waiting_room.h"
#include "err_print.h"
=======
// C++ header
#include <map>
>>>>>>> 3a1b7f01a4904cab97bf0eb6573d60835fc783bf


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
<<<<<<< HEAD
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


=======
extern int			g_iBrick, g_iRot;
extern int			g_iNBrick;
extern int			g_iScore;
extern int			g_iBrickNum;
extern int			g_iInterval;
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
>>>>>>> 3a1b7f01a4904cab97bf0eb6573d60835fc783bf



