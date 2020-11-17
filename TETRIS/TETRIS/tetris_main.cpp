// TETRIS.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"



LPCTSTR			lpszClass = TEXT("Tetris3");
Point			g_tShape[][4][4]=
{
	{{0,0,1,0,2,0,-1,0},{0,0,0,1,0,-1,0,-2},{0,0,1,0,2,0,-1,0},{0,0,0,1,0,-1,0,-2}},
	{{0,0,1,0,0,1,1,1,},{0,0,1,0,0,1,1,1},{0,0,1,0,0,1,1,1},{0,0,1,0,0,1,1,1}},
	{{0,0,-1,0,0,-1,1,-1},{0,0,0,1,-1,0,-1,-1},{0,0,-1,0,0,-1,1,-1},{0,0,0,1,-1,0,-1,-1}},
	{{0,0,-1,-1,0,-1,1,0},{0,0,-1,0,-1,1,0,-1},{0,0,-1,-1,0,-1,1,0},{0,0,-1,0,-1,1,0,-1}},
	{{0,0,-1,0,1,0,-1,-1},{0,0,0,-1,0,1,-1,1},{0,0,-1,0,1,0,1,1},{0,0,0,-1,0,1,1,-1}},
	{{0,0,1,0,-1,0,1,-1},{0,0,0,1,0,-1,-1,-1},{0,0,1,0,-1,0,-1,1},{0,0,0,-1,0,1,1,1}},
	{{0,0,-1,0,1,0,0,1},{0,0,0,-1,0,1,1,0},{0,0,-1,0,1,0,0,-1},{0,0,-1,0,0,-1,0,1}},
	{{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}},
	{{0,0,0,0,0,-1,1,0},{0,0,0,0,-1,0,0,-1},{0,0,0,0,0,1,-1,0},{0,0,0,0,0,1,1,0}},
};


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	
	MSG			iMessage;
	HACCEL		hAccelTable;

	g_hInst	= hInstance;

	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DlgProc_Waiting);

	//Window Create
	MyRegisterClass(g_hInst);

	//Window Load
	if (!InitInstance (g_hInst, nCmdShow))
	{
		return FALSE;
	}

	//Set Accelerators
	hAccelTable = LoadAccelerators(g_hInst, MAKEINTRESOURCE(IDC_TETRIS));


	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	
	while (GetMessage(&iMessage, NULL, 0, 0))
	{
		if (!TranslateAccelerator(iMessage.hwnd, hAccelTable, &iMessage))
		{
			TranslateMessage(&iMessage);
			DispatchMessage(&iMessage);
		}
	}
	QuitRoom();

	//Close Packet
	itPacket = mPACKET.find(g_hSock);
	delete itPacket->second;
	mPACKET.erase(itPacket);

	//Close Socket
	if (g_hSock == INVALID_SOCKET)
	{
		printf("It is Invalid Socket\n");
	}
	closesocket(g_hSock);
	WSACleanup();
	
	return (int) iMessage.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX tWindowClass;

	tWindowClass.cbSize = sizeof(WNDCLASSEX);

	tWindowClass.style			= 0;
	tWindowClass.lpfnWndProc	= WndProc;
	tWindowClass.cbClsExtra		= 0;
	tWindowClass.cbWndExtra		= 0;
	tWindowClass.hInstance		= hInstance;
	tWindowClass.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TETRIS));
	tWindowClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	tWindowClass.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	tWindowClass.lpszMenuName	= MAKEINTRESOURCE(IDC_TETRIS);
	tWindowClass.lpszClassName	= lpszClass;
	tWindowClass.hIconSm		= LoadIcon(tWindowClass.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&tWindowClass);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   g_hInst = hInstance; 
   hWnd = CreateWindow(lpszClass, lpszClass, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
	   CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, NULL, (HMENU)NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

HDC hSendDC;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i		= 0;
	int iTempRotation	= 0;
	int iWidth		= 0;
	int iHeight		= 0;
	int iRetval = 0;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT crt;
	

	switch (message)
	{
	case WM_CREATE:
		g_hWndMain	= hWnd;
		SetRect(&crt, 0, 0, (BW+12)*TS + (BW+2)*TS + 20, (BH+2)*TS);
		AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, TRUE);
		SetWindowPos(g_hWndMain, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top, SWP_NOMOVE | SWP_NOZORDER);

		//Create Ready Button
		CreateReadyButton(hWnd);

		eGameStatus	= GAMEOVER;
		srand(GetTickCount());
		for (i = 0; i<11; i++)
		{
			hBit[i]	= LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1+i));
		}
		return 0;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		
		switch (wmId)
		{
		case IDM_GAME_START:
			if (eGameStatus != GAMEOVER)
			{
				break;
			}
			for (iWidth = 0; iWidth<BW + 2; iWidth++)
			{
				for (iHeight = 0; iHeight<BH + 2; iHeight++)
				{
					board[iWidth][iHeight] = (iHeight==0 || iHeight==BH+1 || iWidth==0 || iWidth==BW+1)?WALL:EMPTY;
				}
			}
			g_iScore		= 0;
			g_iBrickNum	= 0;
			eGameStatus	= RUNNING;
			g_iNumBrick		= random(sizeof(g_tShape)/sizeof(g_tShape[0]));
			MakeNewBrick();
			iInterval	= TIMER_TYPE_DOWN_DELAY;
			SetTimer(hWnd, TIMER_TYPE_DOWN, iInterval, NULL);
			SetTimer(hWnd, TIMER_TYPE_CAPTURE, TIMER_TYPE_CAPTURE_DELAY, NULL);
			break;

		case IDM_GAME_PAUSE:
			if (eGameStatus == RUNNING)
			{
				eGameStatus	= PAUSE;
				KillTimer(hWnd, TIMER_TYPE_DOWN);
				KillTimer(hWnd, TIMER_TYPE_CAPTURE);
			}
			else if (eGameStatus == PAUSE)
			{
				eGameStatus	= RUNNING;
				SetTimer(hWnd, TIMER_TYPE_DOWN, iInterval, NULL);
				SetTimer(hWnd, TIMER_TYPE_CAPTURE, TIMER_TYPE_CAPTURE_DELAY, NULL);
			}
			break;

		case IDM_GAME_EXIT:
//			QuitRoom();
			DestroyWindow(hWnd);
			break;

		case MENU_READYBUTTON:
			ClickedReadyButton();

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_TIMER:
		{
			switch(wParam)
			{
			case TIMER_TYPE_DOWN : 
				{
					if (MoveDown() == TRUE)
					{
						MakeNewBrick();
					}
				}
				break;
			case TIMER_TYPE_CAPTURE : 
				{
					SendMessage(hWnd, WM_VSTETRIS, 0 ,0);
				}
				break;
			}
		}
		break;

	case WM_KEYDOWN:
		if (eGameStatus != RUNNING || g_iBrick == -1)
		{
			return 0;
		}

		switch (wParam)
		{
		case VK_LEFT:
			if (GetAround(g_iWidth - 1, g_iHeigth, g_iBrick, g_iRotation) == EMPTY)
			{
				g_iWidth--;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;

		case VK_RIGHT:
			if (GetAround(g_iWidth + 1, g_iHeigth, g_iBrick, g_iRotation) == EMPTY)
			{
				g_iWidth++;
				InvalidateRect(hWnd, NULL, FALSE);			
			}
			break;

		case VK_UP:
			iTempRotation	= (g_iRotation == 3? 0 : g_iRotation + 1);
			if (GetAround(g_iWidth, g_iHeigth, g_iBrick, iTempRotation) == EMPTY)
			{
				g_iRotation	= iTempRotation;
				InvalidateRect(hWnd, NULL, FALSE);				
			}
			break;

		case VK_DOWN:
			if (MoveDown() == TRUE)
			{
				MakeNewBrick();			
			}
			break;
		case VK_SPACE:
			while(MoveDown() == FALSE){;}
			MakeNewBrick();
			break;
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawScreen(hdc);
		EndPaint(hWnd, &ps);
		break;

	case WM_VSTETRIS:
		iRetval = SendingBMP();
		if ( iRetval == ERR_TS_INVALID_HANDLE )
		{
			err_quit("File Opne Error");
		}
		break;

	case WM_DESTROY:
		KillTimer(g_hWndMain, TIMER_TYPE_DOWN);
		KillTimer(g_hWndMain, TIMER_TYPE_CAPTURE);

		for ( i = 0; i < 11; i++ )
		{
			DeleteObject(hBit[i]);
		}
		
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


VOID DrawScreen(HDC hdc)
{
	int iRet	= 0;
	int iWidth	= 0;
	int iHeigth	= 0;
	int i		= 0;
	WCHAR wStr[128] = {0,};

	//Draw the WALL
	for (iWidth = 0; iWidth<BW+1; iWidth++)
	{
		PrintTile(hdc, iWidth, 0, WALL);
		PrintTile(hdc, iWidth, BH+1, WALL);
	}
	for (iHeigth = 0; iHeigth<BH+2; iHeigth++)
	{
		PrintTile(hdc, 0, iHeigth, WALL);
		PrintTile(hdc, BW+1, iHeigth, WALL);
	}

	for (iWidth = 1; iWidth<BW+1; iWidth++)
	{
		for (iHeigth = 1; iHeigth<BH+1; iHeigth++)
		{
			PrintTile(hdc, iWidth, iHeigth, board[iWidth][iHeigth]);
		}
	}

	//draw the moving brick
	if (eGameStatus != GAMEOVER && g_iBrick != -1)
	{
		for (i = 0; i<4; i++)
		{
			PrintTile(hdc, g_iWidth+g_tShape[g_iBrick][g_iRotation][i].x, g_iHeigth+g_tShape[g_iBrick][g_iRotation][i].y, g_iBrick+1);
		}
	}
	
	//draw the next brick
	for (iWidth=BW+3; iWidth<=BW+11; iWidth++)
	{
		for (iHeigth=BH-5; iHeigth<=BH+1; iHeigth++)
		{
			if (iWidth==BW+3 || iWidth==BW+11 || iHeigth==BH-5 || iHeigth==BH+1)
			{
				PrintTile(hdc, iWidth, iHeigth, WALL);
			}
			else
			{
				PrintTile(hdc, iWidth, iHeigth, 0);
			}
		}
	}
	if (eGameStatus != GAMEOVER)
	{
		for (i = 0; i<4; i++)
		{
			PrintTile(hdc, BW+7+g_tShape[g_iNumBrick][0][i].x, BH-2+g_tShape[g_iNumBrick][0][i].y, g_iNumBrick+1);
		}
	}

	wcscpy(wStr, TEXT("Tetris Ver 1.2"));
	TextOut(hdc, (BW+4)*TS, 30, wStr, lstrlen(wStr));
	wsprintf(wStr, TEXT("Score : %d   "), g_iScore);
	TextOut(hdc, (BW+4)*TS, 60, wStr, lstrlen(wStr));
	wsprintf(wStr, TEXT("Bricks: %d   "), g_iBrickNum);
	TextOut(hdc, (BW+4)*TS, 80, wStr, lstrlen(wStr));

	iRet = SettingBMPHeader();
	if (iRet == ERR_TS_INVALID_HANDLE)
	{
		
	}
}

VOID MakeNewBrick()
{
	g_iBrick			= g_iNumBrick;
	g_iNumBrick			= random(sizeof(g_tShape)/sizeof(g_tShape[0]));
	g_iWidth			= BW/2;
	g_iHeigth			= 3;
	g_iRotation			= 0;
	//Game over Send
	int iFlag = 0;
	int iTotSize = 0;


	g_iBrickNum++;

	InvalidateRect(g_hWndMain, NULL, FALSE);

	if (GetAround(g_iWidth, g_iHeigth, g_iBrick, g_iRotation) != EMPTY)
	{
		KillTimer(g_hWndMain, TIMER_TYPE_DOWN);
		KillTimer(g_hWndMain, TIMER_TYPE_CAPTURE);
		eGameStatus = GAMEOVER;

		//Sending "I am game over"
		iFlag = WSABUFFER_END;
		iTotSize = sizeof(PACKET_HEADER);
		SendFunction(iFlag, iTotSize, NULL);

		MessageBox(g_hWndMain, TEXT("GameOver... Do you want to play, again?"), TEXT("NOTICE"), MB_OK);
		SetWindowText(g_hReadyButton, TEXT("Ready"));
		ShowWindow(g_hReadyButton, SW_SHOW);
	}	
}


int GetAround(int x, int y, int b, int r)
{
	int i, k = EMPTY;
	
	for (i = 0; i<4; i++)
	{
		k = max(k, board[x+g_tShape[b][r][i].x][y+g_tShape[b][r][i].y]);
	}
	return k;
}

BOOL MoveDown()
{
	if (GetAround(g_iWidth, g_iHeigth+1, g_iBrick, g_iRotation) != EMPTY)
	{
		TestFull();
		return TRUE;
	}

	g_iHeigth++;

	InvalidateRect(g_hWndMain, NULL, FALSE);
	UpdateWindow(g_hWndMain);
	return FALSE;
}

VOID TestFull()
{
	int i		= 0;
	int x		= 0;
	int y		= 0;
	int ty		= 0;
	int count	= 0;
	static int arScoreInc[]	= {0, 1, 3, 8, 20};

	for (i = 0; i<4; i++)
	{
		board[g_iWidth+g_tShape[g_iBrick][g_iRotation][i].x][g_iHeigth+g_tShape[g_iBrick][g_iRotation][i].y]		= g_iBrick + 1;
	}

	int g_iBrick = -1;

	for (y = 1; y<BH+1; y++)
	{
		for (x = 1; x<BW+1; x++)
		{
			if (board[x][y] == EMPTY)
			{
				break;
			}
		}

		if (x == BW+1)
		{
			count++;
			for (ty = y; ty > 1; ty--)
			{
				for (x = 1; x<BW+1; x++)
				{
					board[x][ty]	= board[x][ty-1];
				}
			}
			InvalidateRect(g_hWndMain, NULL, FALSE);
			UpdateWindow(g_hWndMain);
			Sleep(150);
		}
	}
	g_iScore += arScoreInc[count];
	if (g_iBrickNum % 10 == 0 && iInterval > 200)
	{
		iInterval -= 50;
		SetTimer (g_hWndMain, TIMER_TYPE_DOWN, iInterval, NULL);
	}
}

VOID DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit)
{
	HDC			MemDC;
	HBITMAP		OldBitmap;
	int			bx		= 0;
	int			by		= 0;
	BITMAP		bit;

	MemDC		= CreateCompatibleDC(hdc);
	OldBitmap	= (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit, sizeof(BITMAP), &bit);
	bx			= bit.bmWidth;
	by			= bit.bmHeight;

	BitBlt(hdc, x, y, bx, by, MemDC, 0, 0, SRCCOPY);


	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}


VOID PrintTile(HDC hdc, int x, int y, int c)
{
	DrawBitmap(hdc, x*TS, y*TS, hBit[c]);
	return;
}



VOID QuitRoom()
{
	int iFlag = 0;
	int iTotSize = 0;
	
	iFlag = WSABUFFER_QUITROOM;
	iTotSize = sizeof(PACKET_HEADER);

	SendFunction(iFlag, iTotSize, NULL);
}

int SendingBMP()
{
	int iFlag = 0;
	int iTotSize = 0;
	PBYTE pBody = NULL;
	WCHAR wFileName[MAX_PATH] = {0,};
	HANDLE hFile = NULL;
	DWORD dwRead = 0;

	wcscpy(wFileName, _T("capture.bmp"));

	hFile = CreateFile(wFileName, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return ERR_TS_INVALID_HANDLE;
	}

	iFileSize = g_tBmpInfoHeader.biSize + g_tBmpInfoHeader.biSizeImage;

	pBody = new BYTE[iFileSize];

	ReadFile(hFile, pBody, iFileSize, &dwRead, NULL);
	CloseHandle(hFile);

	iFlag = WSABUFFER_IMAGE;
	iTotSize = sizeof(PACKET_HEADER) + iFileSize;

	SendFunction(iFlag, iTotSize, pBody);

	delete [] pBody;
	pBody = NULL;

	return TRUE;
}


int SettingBMPHeader()
{
	BOOL	bRet				= 0;
	BITMAP	bitmap				= {0,};
	int		iPalSize			= 0;
	int		iSize				= 0;
	HDC		hbitDC				= NULL;
	WCHAR	wFileName[MAX_PATH]	= {0,};
	HANDLE	hFile				= NULL;
	DWORD	dwWrite				= 0;


	wcscpy(wFileName, _T("capture.bmp"));
	hFile = CreateFileW(wFileName, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ,  NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return ERR_TS_INVALID_HANDLE;
	}

	g_hTempDC = GetDC(g_hWndMain);
	g_hMemDC = CreateCompatibleDC(g_hTempDC);
	g_hBitmap = CreateCompatibleBitmap(g_hTempDC, (BW+2)*TS, (BH+2)*TS);


	g_old_obj = SelectObject(g_hMemDC, g_hBitmap);

	bRet = BitBlt(g_hMemDC, 0, 0, (BW+2)*TS, (BH+2)*TS, g_hTempDC, 0, 0, SRCCOPY);

	GetObject(g_hBitmap, sizeof(bitmap), (LPSTR)&bitmap);

	// Bitmap Header Info Setting
	g_tBmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	g_tBmpInfoHeader.biWidth = bitmap.bmWidth;
	g_tBmpInfoHeader.biHeight = bitmap.bmHeight;
	g_tBmpInfoHeader.biPlanes = 1;
	g_tBmpInfoHeader.biBitCount = BIT_COUNT;
	g_tBmpInfoHeader.biCompression = BI_RGB;
	g_tBmpInfoHeader.biSizeImage = 0;
	g_tBmpInfoHeader.biXPelsPerMeter = 0;
	g_tBmpInfoHeader.biYPelsPerMeter = 0;
	g_tBmpInfoHeader.biClrUsed = 0;
	g_tBmpInfoHeader.biClrImportant = 0;
	// Color Size
	iPalSize = (g_tBmpInfoHeader.biBitCount == 24 ? 0 : 1 << g_tBmpInfoHeader.biBitCount) * sizeof(RGBQUAD);
	iSize	 = g_tBmpInfoHeader.biSize + iPalSize + g_tBmpInfoHeader.biSizeImage;


	g_tBmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + iPalSize;
	g_tBmpFileHeader.bfReserved1 = 0;
	g_tBmpFileHeader.bfReserved2 = 0;
	g_tBmpFileHeader.bfSize = iSize + sizeof(BITMAPFILEHEADER);
	g_tBmpFileHeader.bfType = 0x4d42;

	WriteFile(hFile, (LPSTR)&g_tBmpFileHeader, sizeof(BITMAPFILEHEADER), &dwWrite, NULL); 

	hbitDC = GetDC(NULL);

	g_lpBmpHeader = new BITMAPINFO;
	ZeroMemory(g_lpBmpHeader, sizeof(BITMAPINFO));
	g_lpBmpHeader->bmiHeader = g_tBmpInfoHeader;
	GetDIBits(hbitDC, g_hBitmap, 0, bitmap.bmHeight, NULL, g_lpBmpHeader, DIB_RGB_COLORS);
	g_tBmpInfoHeader = g_lpBmpHeader->bmiHeader;
	if (g_tBmpInfoHeader.biSizeImage == 0)
	{
		//Forced Setting File Image Size
		g_tBmpInfoHeader.biSizeImage = ((bitmap.bmWidth * g_tBmpInfoHeader.biBitCount + 31) & ~31) / 8 * bitmap.bmHeight;
	}
	iSize = g_tBmpInfoHeader.biSize + iPalSize + g_tBmpInfoHeader.biSizeImage;
	g_lpBody = malloc(g_lpBmpHeader->bmiHeader.biSizeImage);
	GetDIBits(hbitDC, g_hBitmap, 0, g_lpBmpHeader->bmiHeader.biHeight, g_lpBody, g_lpBmpHeader, DIB_RGB_COLORS);

	WriteFile(hFile, (LPSTR)&g_lpBmpHeader->bmiHeader, sizeof(BITMAPINFOHEADER), &dwWrite, NULL);
	WriteFile(hFile, (LPSTR)g_lpBody, iSize, &dwWrite, NULL);
	CloseHandle(hFile);

	delete g_lpBmpHeader;
	g_lpBmpHeader = NULL;
	free(g_lpBody);
	g_lpBody = NULL;

	return TRUE;
}

VOID CreateReadyButton(HWND hWnd)
{
	g_hReadyButton = CreateWindow(TEXT("Button"), TEXT("Ready"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		(BW+12)*TS + 100, (BH+2)*TS - 100, 100, 50, hWnd, (HMENU)MENU_READYBUTTON, g_hInst, NULL);

}


VOID ClickedReadyButton()
{
	WCHAR wBuf[NAMEBUF] = {0,};
	int iFlag = 0;
	int iTotSize = 0;

	iTotSize = sizeof(PACKET_HEADER);

	GetWindowText(g_hReadyButton, wBuf, NAMEBUF);
	if ( wcscmp(wBuf, TEXT("Ready")) == 0 )
	{
		SetWindowText(g_hReadyButton, TEXT("Cancle"));
		iFlag = WSABUFFER_READY;
	}
	else if ( wcscmp(wBuf, TEXT("Cancle")) == 0 )
	{
		SetWindowText(g_hReadyButton, TEXT("Ready"));
		iFlag = WSABUFFER_NOTREADY;
	}

	SendFunction(iFlag, iTotSize, NULL);
}

VOID AllReadyIsDone(HWND hWnd)
{
	int iWidth = 0;
	int iHeigth = 0;

	if (eGameStatus != GAMEOVER)
	{
		return;
	}
	for (iWidth = 0; iWidth<BW + 2; iWidth++)
	{
		for (iHeigth = 0; iHeigth<BH + 2; iHeigth++)
		{
			board[iWidth][iHeigth] = (iHeigth==0 || iHeigth==BH+1 || iWidth==0 || iWidth==BW+1)?WALL:EMPTY;
		}
	}
	g_iScore		= 0;
	g_iBrickNum	= 0;
	eGameStatus	= RUNNING;
	g_iNumBrick		= random(sizeof(g_tShape)/sizeof(g_tShape[0]));
	MakeNewBrick();
	iInterval	= TIMER_TYPE_DOWN_DELAY;
	SetTimer(hWnd, TIMER_TYPE_DOWN, iInterval, NULL);
	SetTimer(hWnd, TIMER_TYPE_CAPTURE, TIMER_TYPE_CAPTURE_DELAY, NULL);

	ShowWindow(g_hReadyButton, SW_HIDE);
}


VOID SendFunction(int iFlag, int iTotSize, LPVOID pBodyBuf)
{
	DWORD dwRespBufSize = 0;
	PBYTE pRespBuf = NULL;
	PBYTE pBody = NULL;
	LPPACKET_HEADER pHeader = NULL;
	int iSendLen = 0;
	int iSendTot = 0;

	dwRespBufSize = iTotSize + 1;
	pRespBuf = new BYTE[dwRespBufSize];

	pHeader = (LPPACKET_HEADER)pRespBuf;
	pBody = pRespBuf + sizeof(PACKET_HEADER);

	//Setting Header
	pHeader->iFlag = iFlag;
	pHeader->iSize = iTotSize;

	//Setting Body
	memcpy(pBody, pBodyBuf, iTotSize - sizeof(PACKET_HEADER));

	do 
	{
		iSendLen = send(g_hSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
		if ( iSendLen == SOCKET_ERROR )
		{
			break;
		}
		iSendTot += iSendLen;
	} while (pHeader->iSize != iSendTot);

	delete [] pRespBuf;
	pRespBuf = NULL;
}