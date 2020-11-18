#pragma once

#include "Resource.h"

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
VOID DrawScreen(HDC hdc);
VOID MakeNewBrick();
int GetAround(int x, int y, int b, int r);
BOOL MoveDown();
VOID TestFull();
VOID PrintTile(HDC hdc, int x, int y, int c);
VOID DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit);
VOID QuitRoom();
VOID SendingBMP();
VOID ReadBinaryBMP(LPSTR pBody, int iBodySize);
VOID SettingBMPHeader();
VOID CreateReadyButton(HWND hWnd);
VOID ClickedReadyButton();