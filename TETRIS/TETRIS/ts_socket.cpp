
#include "stdafx.h"

BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int iRetval = 0;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitProc(hDlg);
			SetParent(hDlg, g_hWndMain);
		}
		return TRUE;

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDOK:
				EnableWindow(g_hOkbutton, FALSE);
				GetDlgItemTextA(hDlg, IDC_EDIT2, g_cBuf, SMALLBUF+1);

				SendChatting();

				SetFocus(g_hChatInputEdit);
				SendMessage(g_hChatInputEdit, EM_SETSEL, 0, -1);
				SendMessage(g_hChatInputEdit, EM_REPLACESEL, NULL, (LPARAM)"");
				EnableWindow(g_hOkbutton, TRUE);
		}
			return TRUE;

		case IDCANCEL:
			{
				EndDialog(hDlg, 0);
			}
			return TRUE;
		}
		return FALSE;

	case WM_SOCKET:
		{
			ProcessSocketMessage(hDlg, uMsg, wParam, lParam);
		}
		return TRUE;
	}
	return FALSE;
}



VOID ProcessSocketMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{	
	int iRetval = 0;

	switch ( WSAGETSELECTEVENT(lParam) )
	{
	case FD_CLOSE:
		{
			closesocket(g_hSock);
		}
		break;

	case FD_READ:
		{
			ChattingReadFunction(hDlg, wParam, lParam);
		}
		break;

	case FD_CONNECT:
		{
			//WSAAsyncSelect()
			iRetval		= WSAAsyncSelect(g_hSock, hDlg, WM_SOCKET, FD_READ | FD_CLOSE);		
			if (iRetval == SOCKET_ERROR) err_quit("WSAAsyncSelect()");
		}
		break;
	}
}


VOID ChattingReadFunction(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	SOCKET			hSock		= (SOCKET)wParam;
	int				iRecvLen	= 0;
	int				iBodySize	= 0;
	int				iMaxLen		= BUFSIZE;
	LPSTR			pBody		= NULL;
	PACKET_HEADER	hHeader		= {0,};

	LPPACKET_BODY pPacket = mPACKET[hSock];
	ZeroMemory(pPacket, sizeof(PACKET_BODY));

	while(pPacket->iCurRecv < iMaxLen)
	{
		iRecvLen = recv(hSock, (LPSTR)(pPacket->cData + pPacket->iCurRecv), (iMaxLen - pPacket->iCurRecv), NULL);

		if (iRecvLen == SOCKET_ERROR )
		{
			DWORD gle = WSAGetLastError();
		}
		pPacket->iCurRecv += ( (iRecvLen > 0) ? iRecvLen : 0 );

		if (pPacket->iCurRecv >= sizeof(PACKET_HEADER))
		{
			memcpy(&hHeader, pPacket->cData, sizeof(PACKET_HEADER));
			iMaxLen = hHeader.iSize;
		}

		if ( pPacket->iCurRecv >= hHeader.iSize)
		{
			break;
		}
	}

	pBody = (LPSTR) (pPacket->cData + sizeof(PACKET_HEADER));
	iBodySize = hHeader.iSize - sizeof(PACKET_HEADER);
	switch (hHeader.iFlag)
	{
	case WSABUFFER_CHATTING:
		{
			DisplayText("[TCP Client DATA] %s\r\n", pBody);
			EnableWindow(g_hOkbutton, true);
		}
		break;

	case WSABUFFER_IMAGE:
		{
			ReadBinaryBMP(pBody, iBodySize);
		}
		break;

	case WSABUFFER_START:
		{
			AllReadyIsDone(g_hWndMain);
		}
		break;

	case WSABUFFER_END:
		{
			VictoryOnGame();
		}
		break;

	case WSABUFFER_WATCHDOG:
		{
			Watchdog_Kill(g_hWndMain);
		}
		break;
	}
}


VOID InitProc(HWND hDlg)
{
	int			iRetval		= 0;
	SOCKADDR_IN tAddrServer = {0,};

	g_hChatViewEdit	= GetDlgItem(hDlg, IDC_EDIT1);
	g_hChatInputEdit = GetDlgItem(hDlg, IDC_EDIT2);
	g_hOkbutton	= GetDlgItem(hDlg, IDOK);
	SendMessage(g_hChatInputEdit, EM_SETLIMITTEXT, BUFSIZE, 0);

	//Socket()
	g_hSock	= socket(AF_INET, SOCK_STREAM, 0);

	if ( g_hSock == INVALID_SOCKET )
	{
		err_quit("socket()");
	}

	tAddrServer.sin_family		= AF_INET;
	tAddrServer.sin_port		= htons(SIN_PORT);
	tAddrServer.sin_addr.s_addr	= inet_addr(SIN_ADDR);

	//WSAAsyncSelect()
	iRetval = WSAAsyncSelect(g_hSock, hDlg, WM_SOCKET, FD_CONNECT | FD_CLOSE);

	if ( iRetval == SOCKET_ERROR )
	{
		err_quit("WSAAsyncSelect()");
	}

	//connect()
	iRetval = connect(g_hSock, (SOCKADDR*)&tAddrServer, sizeof(tAddrServer));

	if ( iRetval == INVALID_SOCKET )
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			err_display("connect()");
		}
	}

	LPPACKET_BODY pPacket = new PACKET_BODY;
	ZeroMemory(pPacket, sizeof(PACKET_BODY));
	mPACKET.insert(pair<SOCKET, LPPACKET_BODY>(g_hSock, pPacket));
}



VOID SendChatting()
{
	int		iSendLen		= 0;
	int		iSendTot		= 0;
	DWORD	dwRespBufSize	= 0;
	PBYTE	pRespBuf		= NULL;
	LPSTR	pBody			= NULL;

	dwRespBufSize = sizeof(PACKET_HEADER) + strlen(g_cBuf) + 1;
	pRespBuf = new BYTE[dwRespBufSize];
	ZeroMemory(pRespBuf, dwRespBufSize);
	LPPACKET_HEADER pHeader = (LPPACKET_HEADER)pRespBuf;
	pBody = (LPSTR)pRespBuf + sizeof(PACKET_HEADER);
	
	//Setting Header
	pHeader->iFlag = WSABUFFER_CHATTING;
	pHeader->iSize = sizeof(PACKET_HEADER) + strlen(g_cBuf);

	//Setting Body
	memcpy(pBody, g_cBuf, strlen(g_cBuf));

	do 
	{
		iSendLen = send(g_hSock, (LPSTR)(pHeader + iSendTot), pHeader->iSize - iSendTot, NULL);
		if (iSendLen == SOCKET_ERROR)
		{
			break;
		}
		iSendTot += iSendLen;
	} while ( pHeader->iSize != iSendTot );

	delete [] pRespBuf;
	pRespBuf = NULL;
}

VOID ReadBinaryBMP(LPSTR pBody, int iBodySize)
{
	HDC				hdc					= NULL;
	LPVOID			lpBit				= NULL;
	LPBITMAPINFO	lpBmi				= NULL;
	HANDLE			hFile				= 0;
	WCHAR			wFileName[MAX_PATH] = {0,};
	DWORD			dwWrite				= 0;

	wcscpy(wFileName, L"C:\\beginnerC\\ReceiveBMP.bmp");

	hFile = CreateFileW(wFileName, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hFile, pBody, iBodySize, &dwWrite, NULL); 
	hdc = GetDC(g_hWndMain);
	g_hRecvMemDC = CreateCompatibleDC(NULL);
	g_hBitmap = CreateCompatibleBitmap(hdc, (BW+2)*TS, (BH+2)*TS);

	lpBit = pBody + sizeof(BITMAPINFO) + sizeof(BITMAPFILEHEADER);
	lpBmi = new BITMAPINFO;
	ZeroMemory(lpBmi, sizeof(BITMAPINFO));
	memcpy(lpBmi, pBody + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFO));
	
	SetDIBits(g_hRecvMemDC, g_hBitmap, 0, g_tBitmap_InfoHeader.biHeight, lpBit, lpBmi, DIB_RGB_COLORS);
	SelectObject(g_hRecvMemDC, g_hBitmap);

	InvalidateRect(g_hWndMain, &g_tRecvRec, FALSE);
	//UpdateWindow(g_hWndMain);

	BitBlt(hdc, (BW+12)*TS + 20, 0, (BW+2)*TS, (BH+2)*TS, g_hRecvMemDC, 0, 0, SRCCOPY);

	DeleteDC(g_hRecvMemDC);
	DeleteObject(g_hBitmap);
}


VOID AllReadyIsDone(HWND hWnd)
{
	int iWidth = 0;
	int iHeigth = 0;

	if (GameStatus != GAMEOVER)
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
	g_iScore = 0;
	g_iBrickNum	= 0;
	GameStatus = RUNNING;
	g_iNBrick = random(SIZEOFSHAPE/SIZEOFBLCOK);
	MakeNewBrick();
	g_iInterval	= TIMER_TYPE_DOWN_DELAY;
	SetTimer(hWnd, TIMER_TYPE_DOWN, g_iInterval, NULL);
	SetTimer(hWnd, TIMER_TYPE_CAPTURE, TIMER_TYPE_CAPTURE_DELAY, NULL);

	ShowWindow(g_hReadyButton, SW_HIDE);
}

VOID VictoryOnGame()
{
	KillTimer(g_hWndMain, TIMER_TYPE_DOWN);
	KillTimer(g_hWndMain, TIMER_TYPE_CAPTURE);
	GameStatus = GAMEOVER;

	MessageBox(g_hWndMain, TEXT("You Winner!!"), TEXT("NOTICE"), MB_OK);

	InvalidateRect(g_hWndMain, &g_tRecvRec, TRUE);

	SetWindowText(g_hReadyButton, TEXT("Ready"));
	ShowWindow(g_hReadyButton, SW_SHOW);
}

VOID Watchdog_Kill(HWND hWnd)
{
	KillTimer(hWnd, TIMER_TYPE_WATCHDOG);
	SetTimer(hWnd, TIMER_TYPE_WATCHDOG, TIMER_TYPE_WATCHDOG_DELAY, NULL);
}