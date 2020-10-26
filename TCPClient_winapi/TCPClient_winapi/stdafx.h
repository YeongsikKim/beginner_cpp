// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>
#include <WinSock2.h>

// C�� ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdarg.h>
#include <stdio.h>



#define MAX_LOADSTRING	100
#define BUFSIZE			512
#define WM_SOCKET		(WM_USER+1)

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.


// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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
extern HINSTANCE hInst;								// ���� �ν��Ͻ��Դϴ�.
extern WCHAR szTitle[MAX_LOADSTRING];					// ���� ǥ���� �ؽ�Ʈ�Դϴ�.
extern WCHAR szWindowClass[MAX_LOADSTRING];			// �⺻ â Ŭ���� �̸��Դϴ�.