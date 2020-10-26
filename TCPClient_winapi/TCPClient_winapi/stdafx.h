// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>
#include <WinSock2.h>

// C의 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdarg.h>
#include <stdio.h>



#define MAX_LOADSTRING	100
#define BUFSIZE			512
#define WM_SOCKET		(WM_USER+1)

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.


// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK	DlgProc(HWND, UINT, WPARAM, LPARAM);
VOID				DisplayText(char *fmt, ...);
VOID				ProcessSocketMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
VOID				InitProc(HWND hDlg);


DWORD	WINAPI		ClientMain(LPVOID arg);

VOID				err_quit(char *msg);
VOID				err_display(char *msg);

int recvn(SOCKET s, char *buf, int len, int flags);



extern SOCKET sock;
extern char buf[BUFSIZE+1];
extern HWND hEdit1, hEdit2, hOKbutton;
extern HANDLE hReadEvent, hWriteEvent;
extern HINSTANCE hInst;								// 현재 인스턴스입니다.
extern WCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
extern WCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.