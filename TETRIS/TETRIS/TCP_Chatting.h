#pragma once

#include "resource1.h"



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


