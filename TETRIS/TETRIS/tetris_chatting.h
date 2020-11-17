#pragma once

#include "resource1.h"



ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK	DlgProc(HWND, UINT, WPARAM, LPARAM);

VOID				ProcessSocketMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
VOID				InitProc(HWND hDlg);


DWORD	WINAPI		ClientMain(LPVOID arg);


//
VOID ChattingReadFunction(HWND hDlg, WPARAM wParam, LPARAM lParam);
VOID SendChatting(char* cBuf);
VOID VictoryOnGame();