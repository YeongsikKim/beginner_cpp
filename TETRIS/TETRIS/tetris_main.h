#pragma once

#include "Resource.h"

VOID QuitRoom();
int SendingBMP();
VOID ReadBinaryBMP(LPSTR pBody, int iBodySize);
int SettingBMPHeader();
VOID CreateReadyButton(HWND hWnd);
VOID ClickedReadyButton();
VOID AllReadyIsDone(HWND hWnd);
VOID SendFunction(int iFlag, int iTotSize, LPVOID pBodyBuf);