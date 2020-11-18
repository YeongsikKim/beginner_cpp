// TETRIS.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"



LPCTSTR	lpszClass = TEXT("Tetris");
Point	Shape[][4][4]=
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
	if ( !InitInstance (g_hInst, nCmdShow) )
	{
		return FALSE;
	}

	//Set Accelerators
	hAccelTable = LoadAccelerators(g_hInst, MAKEINTRESOURCE(IDC_TETRIS));


	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	
	while ( GetMessage(&iMessage, NULL, 0, 0) )
	{
		if ( !TranslateAccelerator(iMessage.hwnd, hAccelTable, &iMessage) )
		{
			TranslateMessage(&iMessage);
			DispatchMessage(&iMessage);
		}
	}

	//Close Packet
	itPacket = mPACKET.find(g_hSock);
	delete itPacket->second;
	mPACKET.erase(itPacket);

	QuitRoom();
	//Close Socket
	if ( g_hSock == INVALID_SOCKET )
	{
		printf("It is Invalid Socket\n");
	}
	closesocket(g_hSock);
	WSACleanup();
	
	return (int) iMessage.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
//  설명:
//
//    Windows 95에서 추가된 'RegisterClassEx' 함수보다 먼저
//    해당 코드가 Win32 시스템과 호환되도록
//    하려는 경우에만 이 함수를 사용합니다. 이 함수를 호출해야
//    해당 응용 프로그램에 연결된
//    '올바른 형식의' 작은 아이콘을 가져올 수 있습니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX tWindowClassEx;

	tWindowClassEx.cbSize = sizeof(WNDCLASSEX);

	tWindowClassEx.style			= 0;
	tWindowClassEx.lpfnWndProc	= WndProc;
	tWindowClassEx.cbClsExtra		= 0;
	tWindowClassEx.cbWndExtra		= 0;
	tWindowClassEx.hInstance		= hInstance;
	tWindowClassEx.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TETRIS));
	tWindowClassEx.hCursor		= LoadCursor(NULL, IDC_ARROW);
	tWindowClassEx.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	tWindowClassEx.lpszMenuName	= MAKEINTRESOURCE(IDC_TETRIS);
	tWindowClassEx.lpszClassName	= lpszClass;
	tWindowClassEx.hIconSm		= LoadIcon(tWindowClassEx.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&tWindowClassEx);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   g_hInst = hInstance;

   hWnd = CreateWindow(lpszClass, lpszClass, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
	   CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, NULL, (HMENU)NULL, hInstance, NULL);

   if ( !hWnd )
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int			i		= 0;
	int			iTempRot= 0;
	int			iWidth	= 0;
	int			iHeigth	= 0;
	int			iRetval = 0;
	int			wmId	= 0;
	int			wmEvent = 0;
	PAINTSTRUCT ps		= {0,};
	HDC			hdc		= 0;
	RECT		crt		= {0,};
	

	switch ( uMsg )
	{
	case WM_CREATE:
		g_hWndMain = hWnd;
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
			{
				if (GameStatus != GAMEOVER)
				{
					break;
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
				GameStatus	= RUNNING;
				g_iNBrick		= random(sizeof(Shape)/sizeof(Shape[0]));
				MakeNewBrick();
				g_iInterval	= TIMER_TYPE_DOWN_DELAY;
				SetTimer(hWnd, TIMER_TYPE_DOWN, g_iInterval, NULL);
				SetTimer(hWnd, TIMER_TYPE_CAPTURE, TIMER_TYPE_CAPTURE_DELAY, NULL);
			}
			break;

		case IDM_GAME_PAUSE:
			{
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

			}
			break;

		case IDM_GAME_EXIT:
			{
				QuitRoom();
				DestroyWindow(hWnd);
			}
			break;

		case MENU_READYBUTTON:
			{
				ClickedReadyButton();
			}

		default:
			{
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}
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
		{
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
				iTempRot	= (g_iRot == 3? 0 : g_iRot + 1);
				if (GetAround(g_iWidth, g_iHeigth, g_iBrick, iTempRot) == EMPTY)
				{
					g_iRot	= iTempRot;
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
		}
		break;

	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			DrawScreen(hdc);
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_VSTETRIS:
		{
			SendingBMP();
		}
		break;

	case WM_DESTROY:
		{
			KillTimer(g_hWndMain, TIMER_TYPE_DOWN);
			KillTimer(g_hWndMain, TIMER_TYPE_CAPTURE);

			for ( i = 0; i < 11; i++ )
			{
				DeleteObject(g_tBit[i]);
			}

			PostQuitMessage(0);
		}
		break;

	default:
		{
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
	return 0;
}


VOID DrawScreen(HDC hdc)
{
	int		iWidth		= 0;
	int		iHeigth		= 0;
	int		i			= 0;
	WCHAR	wStr[128]	= {0,};

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
	if ( GameStatus != GAMEOVER && g_iBrick != -1 )
	{
		for (i = 0; i<4; i++)
		{
			PrintTile(hdc, g_iWidth+Shape[g_iBrick][g_iRot][i].x, g_iHeigth+Shape[g_iBrick][g_iRot][i].y, g_iBrick+1);
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
	if ( GameStatus != GAMEOVER )
	{
		for (i = 0; i<4; i++)
		{
			PrintTile(hdc, BW+7+Shape[g_iNBrick][0][i].x, BH-2+Shape[g_iNBrick][0][i].y, g_iNBrick+1);
		}
	}

	lstrcpy(wStr, TEXT("Tetris Ver 1.3"));
	TextOut(hdc, (BW+4)*TS, 30, wStr, lstrlen(wStr));
	wsprintf(wStr, TEXT("Score : %d   "), g_iScore);
	TextOut(hdc, (BW+4)*TS, 60, wStr, lstrlen(wStr));
	wsprintf(wStr, TEXT("Bricks: %d   "), g_iBrickNum);
	TextOut(hdc, (BW+4)*TS, 80, wStr, lstrlen(wStr));

	SettingBMPHeader();
}

VOID MakeNewBrick()
{
	g_iBrick		= g_iNBrick;
	g_iNBrick		= random(sizeof(Shape)/sizeof(Shape[0]));
	g_iWidth		= BW/2;
	g_iHeigth		= 3;
	g_iRot			= 0;

	//Game over Send
	int				iSendLen = 0;
	int				iSendTot = 0;
	DWORD			dwRespBufSize = 0;
	PBYTE			pRespBuf = NULL;
	LPPACKET_HEADER pHeader = NULL;

	g_iBrickNum++;

	InvalidateRect(g_hWndMain, NULL, FALSE);

	if (GetAround(g_iWidth, g_iHeigth, g_iBrick, g_iRot) != EMPTY)
	{
		KillTimer(g_hWndMain, TIMER_TYPE_DOWN);
		KillTimer(g_hWndMain, TIMER_TYPE_CAPTURE);
		GameStatus = GAMEOVER;

		dwRespBufSize = sizeof(PACKET_HEADER) + 1;
		pRespBuf = new BYTE[dwRespBufSize];
		ZeroMemory(pRespBuf, dwRespBufSize);
		pHeader = (LPPACKET_HEADER) pRespBuf;


		pHeader->iFlag = WSABUFFER_END;
		pHeader->iSize = sizeof(PACKET_HEADER);

		do 
		{
			iSendLen = send(g_hSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
			if ( iSendLen == SOCKET_ERROR )
			{
				err_display("End send()");
			}
			iSendTot += iSendLen;
		} while ( pHeader->iSize != iSendTot );

		delete [] pRespBuf;
		pRespBuf = NULL;

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
		k = max(k, board[x+Shape[b][r][i].x][y+Shape[b][r][i].y]);
	}
	return k;
}

BOOL MoveDown()
{
	if ( GetAround(g_iWidth, g_iHeigth+1, g_iBrick, g_iRot) != EMPTY )
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
	int			i			= 0;
	int			iWidth		= 0;
	int			iHeigth		= 0;
	int			iTempHeight	= 0;
	int			iCount		= 0;
	static int arScoreInc[]	= {0, 1, 3, 8, 20};

	for (i = 0; i<4; i++)
	{
		board[g_iWidth+Shape[g_iBrick][g_iRot][i].x][g_iHeigth+Shape[g_iBrick][g_iRot][i].y] = g_iBrick + 1;
	}

	g_iBrick = -1;

	for (iHeigth = 1; iHeigth<BH+1; iHeigth++)
	{
		for (iWidth = 1; iWidth<BW+1; iWidth++)
		{
			if (board[iWidth][iHeigth] == EMPTY)
			{
				break;
			}
		}

		if (iWidth == BW+1)
		{
			iCount++;
			for (iTempHeight = iHeigth; iTempHeight > 1; iTempHeight--)
			{
				for (iWidth = 1; iWidth<BW+1; iWidth++)
				{
					board[iWidth][iTempHeight]	= board[iWidth][iTempHeight-1];
				}
			}
			InvalidateRect(g_hWndMain, NULL, FALSE);
			UpdateWindow(g_hWndMain);
			Sleep(150);
		}
	}
	g_iScore += arScoreInc[iCount];

	if ( g_iBrickNum % 10 == 0 && g_iInterval > 200 )
	{
		g_iInterval -= 50;
		SetTimer (g_hWndMain, TIMER_TYPE_DOWN, g_iInterval, NULL);
	}
}

VOID DrawBitmap(HDC hdc, int iWidth, int iHeigth, HBITMAP hBit)
{
	HDC			MemDC		= {0,};
	HBITMAP		hOldBitmap	= {0,};
	int			iBitWidth	= 0;
	int			iBitHeigth	= 0;
	BITMAP		tBit		= {0,};

	MemDC		= CreateCompatibleDC(hdc);
	hOldBitmap	= (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit, sizeof(BITMAP), &tBit);
	iBitWidth			= tBit.bmWidth;
	iBitHeigth			= tBit.bmHeight;

	BitBlt(hdc, iWidth, iHeigth, iBitWidth, iBitHeigth, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, hOldBitmap);
	DeleteDC(MemDC);
}


VOID PrintTile(HDC hdc, int x, int y, int c)
{
	DrawBitmap(hdc, x*TS, y*TS, g_tBit[c]);
	return;
}



VOID QuitRoom()
{
	int				iSendLen		= 0;
	int				iSendTot		= 0;
	DWORD			dwRespBufSize	= 0;
	PBYTE			pRespBuf		= NULL;
	LPPACKET_HEADER	pHeader			= NULL;

	dwRespBufSize = sizeof(PACKET_HEADER) + 1;
	pRespBuf = new BYTE[dwRespBufSize];
	ZeroMemory(pRespBuf, dwRespBufSize);
	pHeader = (LPPACKET_HEADER)pRespBuf;

	//Setting Header
	pHeader->iFlag = WSABUFFER_QUITROOM;
	pHeader->iSize = sizeof(PACKET_HEADER);

	do
	{
		iSendLen = send(g_hSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
		if (iSendLen == SOCKET_ERROR)
		{
			DWORD gle = WSAGetLastError();
			if ( gle != WSAEWOULDBLOCK )
			{
				err_quit("send()");
			}
		}
		iSendTot += iSendLen;
	} while ( pHeader->iSize != iSendTot );

	printf("[Server] Successfully Quit this room\n");

	delete [] pRespBuf;
	pRespBuf = NULL;
}

VOID SendingBMP()
{
	int				iSendLen			= 0;
	int				iSendTot			= 0;
	DWORD			dwRead				= 0;
	DWORD			dwRespBufSize		= 0;
	PBYTE			pRespBuf			= NULL;
	LPSTR			pBody				= NULL;
	LPPACKET_HEADER pHeader				= NULL;
	HANDLE			hFile				= NULL;
	WCHAR			wFileName[SMALLBUF] = {0,};


	wcscpy(wFileName, _T("capture.bmp"));

	hFile = CreateFileW(wFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		DWORD dwGle = WSAGetLastError();
		printf("[Error] Createfile error : %d\n", dwGle);
	}

	g_iFileSize = GetFileSize(hFile, NULL);

	
	dwRespBufSize = sizeof(PACKET_HEADER) + g_iFileSize + 1;
	pRespBuf = new BYTE[dwRespBufSize];
	ZeroMemory(pRespBuf, dwRespBufSize);
	pHeader = (LPPACKET_HEADER)pRespBuf;
	pBody = (LPSTR)pRespBuf + sizeof(PACKET_HEADER);

	//Setting Header
	pHeader->iFlag = WSABUFFER_IMAGE;
	pHeader->iSize = sizeof(PACKET_HEADER) + g_iFileSize;

	//Setting Body
	ReadFile(hFile, pBody, g_iFileSize, &dwRead, NULL);
	CloseHandle(hFile);
	do 
	{
		iSendLen = send(g_hSock, (LPSTR)(pHeader + iSendTot), pHeader->iSize - iSendTot, NULL);
		if (iSendLen == SOCKET_ERROR)
		{
			break;
		}
		iSendTot += iSendLen;
	} while (pHeader->iSize != iSendTot);

	delete [] pRespBuf;
	pRespBuf = NULL;
}


VOID SettingBMPHeader()
{
	BOOL	bRet			= 0;
	BITMAP	bitmap			= {0,};
	int		iPalSize		= 0;
	HDC		hbitDC			= NULL;
	HANDLE	hFile			= NULL;
	DWORD	dwWrite			= 0;
	WCHAR	wBuf[SMALLBUF]	= {0,};

	wcscpy(wBuf, _T("capture.bmp"));

	hFile = CreateFileW(wBuf, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if ( hFile == INVALID_HANDLE_VALUE )
	{
		DWORD dwGle = WSAGetLastError();
		printf("[Error] Createfile error : %d\n", dwGle);
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

	WriteFile(hFile, (LPSTR)&g_tBitmap_FileHeader, sizeof(BITMAPFILEHEADER), &dwWrite, NULL);

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

	WriteFile(hFile, (LPSTR)&g_pBitmap_Info->bmiHeader, sizeof(BITMAPINFOHEADER), &dwWrite, NULL);
	WriteFile(hFile, (LPSTR)g_pBody, g_iSize, &dwWrite, NULL);
	CloseHandle(hFile);

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
	int				iSendLen		= 0;
	int				iSendTot		= 0;
	WCHAR			wBuf[SMALLBUF]	= {0,};
	DWORD			dwRespBufSize	= 0;
	PBYTE			pRespBuf		= NULL;
	LPPACKET_HEADER pHeader			= NULL;


	dwRespBufSize = sizeof(PACKET_HEADER) + 1;
	pRespBuf = new BYTE[dwRespBufSize];
	ZeroMemory(pRespBuf, dwRespBufSize);
	pHeader = (LPPACKET_HEADER) pRespBuf;

	GetWindowText(g_hReadyButton, wBuf, SMALLBUF);
	if ( wcscmp(wBuf, TEXT("Ready") ) == 0 )
	{
		SetWindowText(g_hReadyButton, TEXT("Cancle"));
		pHeader->iFlag = WSABUFFER_READY;
		pHeader->iSize = sizeof(PACKET_HEADER);
	}
	else if ( wcscmp(wBuf, TEXT("Cancle") ) == 0 )
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
	} while ( pHeader->iSize != iSendTot );

	delete [] pRespBuf;
	pRespBuf = NULL;
}