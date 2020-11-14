#pragma once

#include "Resource.h"

VOID QuitRoom();
VOID SendingBMP();
VOID ReadBinaryBMP(LPSTR pBody, int iBodySize);
VOID SettingBMPHeader();
VOID CreateReadyButton(HWND hWnd);