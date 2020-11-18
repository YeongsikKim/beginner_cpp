// TETRIS.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"



LPCTSTR			lpszClass = TEXT("Tetris3");
Point			Shape[][4][4]=
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


	g_hInst		= hInstance;


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



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
//  ����:
//
//    Windows 95���� �߰��� 'RegisterClassEx' �Լ����� ����
//    �ش� �ڵ尡 Win32 �ý��۰� ȣȯ�ǵ���
//    �Ϸ��� ��쿡�� �� �Լ��� ����մϴ�. �� �Լ��� ȣ���ؾ�
//    �ش� ���� ���α׷��� �����
//    '�ùٸ� ������' ���� �������� ������ �� �ֽ��ϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= 0;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TETRIS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TETRIS);
	wcex.lpszClassName	= lpszClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   g_hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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
	int trot	= 0;
	int x		= 0;
	int y		= 0;
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

		GameStatus	= GAMEOVER;
		srand(GetTickCount());
		for (i = 0; i<11; i++)
		{
			g_tBit[i]	= LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1+i));
		}
		return 0;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		
		switch (wmId)
		{
		case IDM_GAME_START:
			if (GameStatus != GAMEOVER)
			{
				break;
			}
			for (x = 0; x<BW + 2; x++)
			{
				for (y = 0; y<BH + 2; y++)
				{
					board[x][y] = (y==0 || y==BH+1 || x==0 || x==BW+1)?WALL:EMPTY;
				}
			}
			g_iScore		= 0;
			g_iBrickNum	= 0;
			GameStatus	= RUNNING;
			g_iNBrick		= random(sizeof(Shape)/sizeof(Shape[0]));
			MakeNewBrick();
			g_iInterval	= TIMER_TYPE_DOWN_DELAY;
			SetTimer(hWnd, TIMER_TYPE_DOWN, g_iInterval, NULL);
			SetTimer(hWnd, TIMER_TYPE_CAPTURE, TIMER_TYPE_CAPTURE_DELAY, NULL);
			break;

		case IDM_GAME_PAUSE:
			if (GameStatus == RUNNING)
			{
				GameStatus	= PAUSE;
				KillTimer(hWnd, TIMER_TYPE_DOWN);
				KillTimer(hWnd, TIMER_TYPE_CAPTURE);
			}
			else if (GameStatus == PAUSE)
			{
				GameStatus	= RUNNING;
				SetTimer(hWnd, TIMER_TYPE_DOWN, g_iInterval, NULL);
				SetTimer(hWnd, TIMER_TYPE_CAPTURE, TIMER_TYPE_CAPTURE_DELAY, NULL);
			}
			break;

		case IDM_GAME_EXIT:
			QuitRoom();
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
		if (GameStatus != RUNNING || g_iBrick == -1)
		{
			return 0;
		}
		switch (wParam)
		{
		case VK_LEFT:
			if (GetAround(g_iWidth - 1, g_iHeigth, g_iBrick, g_iRot) == EMPTY)
			{
				g_iWidth--;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case VK_RIGHT:
			if (GetAround(g_iWidth + 1, g_iHeigth, g_iBrick, g_iRot) == EMPTY)
			{
				g_iWidth++;
				InvalidateRect(hWnd, NULL, FALSE);			
			}
			break;
		case VK_UP:
			trot	= (g_iRot == 3? 0 : g_iRot + 1);
			if (GetAround(g_iWidth, g_iHeigth, g_iBrick, trot) == EMPTY)
			{
				g_iRot	= trot;
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
		SendingBMP();
		break;

	case WM_DESTROY:
		KillTimer(g_hWndMain, TIMER_TYPE_DOWN);
		KillTimer(g_hWndMain, TIMER_TYPE_CAPTURE);

		for ( i = 0; i < 11; i++ )
		{
			DeleteObject(g_tBit[i]);
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
	int x		= 0;
	int y		= 0;
	int i		= 0;
	TCHAR str[128] = {0,};

	//Draw the WALL
	for (x = 0; x<BW+1; x++)
	{
		PrintTile(hdc, x, 0, WALL);
		PrintTile(hdc, x, BH+1, WALL);
	}
	for (y = 0; y<BH+2; y++)
	{
		PrintTile(hdc, 0, y, WALL);
		PrintTile(hdc, BW+1, y, WALL);
	}

	for (x = 1; x<BW+1; x++)
	{
		for (y = 1; y<BH+1; y++)
		{
			PrintTile(hdc, x, y, board[x][y]);
		}
	}

	//draw the moving brick
	if (GameStatus != GAMEOVER && g_iBrick != -1)
	{
		for (i = 0; i<4; i++)
		{
			PrintTile(hdc, g_iWidth+Shape[g_iBrick][g_iRot][i].x, g_iHeigth+Shape[g_iBrick][g_iRot][i].y, g_iBrick+1);
		}
	}
	
	//draw the next brick
	for (x=BW+3; x<=BW+11; x++)
	{
		for (y=BH-5; y<=BH+1; y++)
		{
			if (x==BW+3 || x==BW+11 || y==BH-5 || y==BH+1)
			{
				PrintTile(hdc, x, y, WALL);
			}
			else
			{
				PrintTile(hdc, x, y, 0);
			}
		}
	}
	if (GameStatus != GAMEOVER)
	{
		for (i = 0; i<4; i++)
		{
			PrintTile(hdc, BW+7+Shape[g_iNBrick][0][i].x, BH-2+Shape[g_iNBrick][0][i].y, g_iNBrick+1);
		}
	}

	lstrcpy(str, TEXT("Tetris Ver 1.2"));
	TextOut(hdc, (BW+4)*TS, 30, str, lstrlen(str));
	wsprintf(str, TEXT("Score : %d   "), g_iScore);
	TextOut(hdc, (BW+4)*TS, 60, str, lstrlen(str));
	wsprintf(str, TEXT("Bricks: %d   "), g_iBrickNum);
	TextOut(hdc, (BW+4)*TS, 80, str, lstrlen(str));

	SettingBMPHeader();
}

VOID MakeNewBrick()
{
	g_iBrick		= g_iNBrick;
	g_iNBrick		= random(sizeof(Shape)/sizeof(Shape[0]));
	g_iWidth			= BW/2;
	g_iHeigth			= 3;
	g_iRot			= 0;
	//Game over Send
	DWORD dwRespBufSize = 0;
	PBYTE pRespBuf = NULL;
	LPPACKET_HEADER pHeader = NULL;
	int iSendLen = 0;
	int iSendTot = 0;


	g_iBrickNum++;

	InvalidateRect(g_hWndMain, NULL, FALSE);

	if (GetAround(g_iWidth, g_iHeigth, g_iBrick, g_iRot) != EMPTY)
	{
		KillTimer(g_hWndMain, TIMER_TYPE_DOWN);
		KillTimer(g_hWndMain, TIMER_TYPE_CAPTURE);
		GameStatus = GAMEOVER;

		MessageBox(g_hWndMain, TEXT("GameOver... Do you want to play, again?"), TEXT("NOTICE"), MB_OK);
	}

	dwRespBufSize = sizeof(PACKET_HEADER) + 1;
	pRespBuf = new BYTE[dwRespBufSize];

	pHeader = (LPPACKET_HEADER) pRespBuf;
	
	pHeader->iFlag = WSABUFFER_END;
	pHeader->iSize = sizeof(PACKET_HEADER);

	do 
	{
		iSendLen = send(g_hSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
		if (iSendLen == SOCKET_ERROR)
		{
			err_quit("End send()");
		}
		iSendTot += iSendLen;
	} while (pHeader->iSize != iSendTot);
}


int GetAround(int x, int y, int b, int r)
{
	int i, k = EMPTY;
	
	for (i = 0; i<4; i++)
	{
		k	= max(k, board[x+Shape[b][r][i].x][y+Shape[b][r][i].y]);
	}
	return k;
}

BOOL MoveDown()
{
	if (GetAround(g_iWidth, g_iHeigth+1, g_iBrick, g_iRot) != EMPTY)
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
		board[g_iWidth+Shape[g_iBrick][g_iRot][i].x][g_iHeigth+Shape[g_iBrick][g_iRot][i].y]		= g_iBrick + 1;
	}

	g_iBrick = -1;

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
	if (g_iBrickNum % 10 == 0 && g_iInterval > 200)
	{
		g_iInterval -= 50;
		SetTimer (g_hWndMain, TIMER_TYPE_DOWN, g_iInterval, NULL);
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
	DrawBitmap(hdc, x*TS, y*TS, g_tBit[c]);
	return;
}



VOID QuitRoom()
{
	int iSendLen = 0;
	int iSendTot = 0;
	DWORD dwRespBufSize = sizeof(PACKET_HEADER) + 1;
	PBYTE pRespBuf = new BYTE[dwRespBufSize];
	LPPACKET_HEADER pHeader = (LPPACKET_HEADER)pRespBuf;

	//Setting Header
	pHeader->iFlag = WSABUFFER_QUITROOM;
	pHeader->iSize = sizeof(PACKET_HEADER);

	do
	{
		iSendLen = send(g_hSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
		if (iSendLen == SOCKET_ERROR)
		{
			DWORD gle = WSAGetLastError();
			if (gle != WSAEWOULDBLOCK)
			{
				err_quit("send()");
			}
		}
		iSendTot += iSendLen;
	} while (pHeader->iSize != iSendTot);
	
}

VOID SendingBMP()
{
	int iSendLen = 0;
	int iSendTot = 0;

	streamSending.open("c:\\beginnerC\\capture.bmp", ios::binary);
	streamSending.seekg(0, std::ios::end);

	g_iFileSize = streamSending.tellg();

	streamSending.seekg(0, std::ios::beg);

	
	DWORD dwRespBufSize = sizeof(PACKET_HEADER) + g_iFileSize + 1;
	PBYTE pRespBuf = new BYTE[dwRespBufSize];
	LPSTR pBody = NULL;

	LPPACKET_HEADER pHeader = (LPPACKET_HEADER)pRespBuf;
	pBody = (LPSTR)pRespBuf + sizeof(PACKET_HEADER);

	//Setting Header
	pHeader->iFlag = WSABUFFER_IMAGE;
	pHeader->iSize = sizeof(PACKET_HEADER) + g_iFileSize;

	//Setting Body
	streamSending.read((LPSTR)pBody, g_iFileSize);
	streamSending.close();

	do 
	{
		iSendLen = send(g_hSock, (LPSTR)(pHeader + iSendTot), pHeader->iSize - iSendTot, NULL);
		if (iSendLen == SOCKET_ERROR)
		{
			break;
		}
		iSendTot += iSendLen;
	} while (pHeader->iSize != iSendTot);
}


VOID SettingBMPHeader()
{
	BOOL bRet = 0;
	BITMAP bitmap = {0,};
	int iPalSize = 0;
	HDC hbitDC = NULL;


	stream.open("c:\\beginnerC\\capture.bmp", ios::binary);
	if (!stream.is_open())
	{
		cout << "File open error!!" << endl;
		return;
	}

	g_hTempDC = GetDC(g_hWndMain);
	g_hMemDC = CreateCompatibleDC(g_hTempDC);
	g_hBitmap = CreateCompatibleBitmap(g_hTempDC, (BW+2)*TS, (BH+2)*TS);


	g_old_obj = SelectObject(g_hMemDC, g_hBitmap);

	bRet = BitBlt(g_hMemDC, 0, 0, (BW+2)*TS, (BH+2)*TS, g_hTempDC, 0, 0, SRCCOPY);

	GetObject(g_hBitmap, sizeof(bitmap), (LPSTR)&bitmap);

	// Bitmap Header Info Setting
	g_tBitmap_InfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	g_tBitmap_InfoHeader.biWidth = bitmap.bmWidth;
	g_tBitmap_InfoHeader.biHeight = bitmap.bmHeight;
	g_tBitmap_InfoHeader.biPlanes = 1;
	g_tBitmap_InfoHeader.biBitCount = BIT_COUNT;
	g_tBitmap_InfoHeader.biCompression = BI_RGB;
	g_tBitmap_InfoHeader.biSizeImage = 0;
	g_tBitmap_InfoHeader.biXPelsPerMeter = 0;
	g_tBitmap_InfoHeader.biYPelsPerMeter = 0;
	g_tBitmap_InfoHeader.biClrUsed = 0;
	g_tBitmap_InfoHeader.biClrImportant = 0;
	// Color Size
	iPalSize = (g_tBitmap_InfoHeader.biBitCount == 24 ? 0 : 1 << g_tBitmap_InfoHeader.biBitCount) * sizeof(RGBQUAD);


	g_tBitmap_FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + iPalSize;
	g_tBitmap_FileHeader.bfReserved1 = 0;
	g_tBitmap_FileHeader.bfReserved2 = 0;
	g_tBitmap_FileHeader.bfSize = g_iSize + sizeof(BITMAPFILEHEADER);
	g_tBitmap_FileHeader.bfType = 0x4d42;

	stream.write((LPSTR)&g_tBitmap_FileHeader, sizeof(BITMAPFILEHEADER));

	hbitDC = GetDC(NULL);

	g_pBitmap_Info = new BITMAPINFO;
	ZeroMemory(g_pBitmap_Info, sizeof(BITMAPINFO));
	g_pBitmap_Info->bmiHeader = g_tBitmap_InfoHeader;
	GetDIBits(hbitDC, g_hBitmap, 0, bitmap.bmHeight, NULL, g_pBitmap_Info, DIB_RGB_COLORS);
	g_tBitmap_InfoHeader = g_pBitmap_Info->bmiHeader;
	if (g_tBitmap_InfoHeader.biSizeImage == 0)
	{
		//Forced Setting File Image Size
		g_tBitmap_InfoHeader.biSizeImage = ((bitmap.bmWidth * g_tBitmap_InfoHeader.biBitCount + 31) & ~31) / 8 * bitmap.bmHeight;
	}
	g_iSize = g_tBitmap_InfoHeader.biSize + iPalSize + g_tBitmap_InfoHeader.biSizeImage;
	g_pBody = malloc(g_pBitmap_Info->bmiHeader.biSizeImage);
	GetDIBits(hbitDC, g_hBitmap, 0, g_pBitmap_Info->bmiHeader.biHeight, g_pBody, g_pBitmap_Info, DIB_RGB_COLORS);

	stream.write((LPSTR)&g_pBitmap_Info->bmiHeader, sizeof(BITMAPINFOHEADER));
	stream.write((LPSTR)g_pBody, g_iSize);
	stream.close();

	free(g_pBody);
	g_pBody = NULL;

//	DeleteDC(hMemDC);
//	DeleteObject(hBitmap);
}

VOID CreateReadyButton(HWND hWnd)
{
	g_hReadyButton = CreateWindow(TEXT("Button"), TEXT("Ready"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		(BW+12)*TS + 100, (BH+2)*TS - 100, 100, 50, hWnd, (HMENU)MENU_READYBUTTON, g_hInst, NULL);

}


VOID ClickedReadyButton()
{
	WCHAR wBuf[NAMEBUF] = {0,};
	DWORD dwRespBufSize = 0;
	PBYTE pRespBuf = NULL;
	LPPACKET_HEADER pHeader = NULL;
	int iSendLen = 0;
	int iSendTot = 0;

	dwRespBufSize = sizeof(PACKET_HEADER) + 1;
	pRespBuf = new BYTE[dwRespBufSize];

	pHeader = (LPPACKET_HEADER) pRespBuf;

	GetWindowText(g_hReadyButton, wBuf, NAMEBUF);
	if ( wcscmp(wBuf, TEXT("Ready")) == 0 )
	{
		SetWindowText(g_hReadyButton, TEXT("Cancle"));
		pHeader->iFlag = WSABUFFER_READY;
		pHeader->iSize = sizeof(PACKET_HEADER);
	}
	else if ( wcscmp(wBuf, TEXT("Cancle")) == 0 )
	{
		SetWindowText(g_hReadyButton, TEXT("Ready"));
		pHeader->iFlag = WSABUFFER_NOTREADY;
		pHeader->iSize = sizeof(PACKET_HEADER);
	}

	do 
	{
		iSendLen = send(g_hSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
		if ( iSendLen == SOCKET_ERROR )
		{
			err_quit("Ready Send()");
		}
		iSendTot += iSendLen;
	} while (pHeader->iSize != iSendTot);
}