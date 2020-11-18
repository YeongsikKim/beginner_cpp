// TCPClient_winapi.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
//int vsprintf(char *cbuf, const char *fmt, va_list arg);




BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int iRetval		= 0;
	char cBuf[CHATSIZE] = {0,};

	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitProc(hDlg);
		SetParent(hDlg, g_hWndMain);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EnableWindow(g_hOKbutton, FALSE);
			GetDlgItemTextA(hDlg, IDC_EDIT2, cBuf, CHATSIZE+1);

			SendChatting(cBuf);

			SetFocus(g_hChatting);
			SendMessage(g_hChatting, EM_SETSEL, 0, -1);
			SendMessage(g_hChatting, EM_REPLACESEL, NULL, (LPARAM)"");
			EnableWindow(g_hOKbutton, TRUE);
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		return FALSE;
	case WM_SOCKET:
		ProcessSocketMessage(hDlg, uMsg, wParam, lParam);
		return TRUE;
	}
	return FALSE;
}


VOID ProcessSocketMessage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{	
	int iRetval		= 0;
	HBITMAP hBitmap = NULL;
	HBITMAP hOld_Bmp = NULL;
	HDC hMemDC = NULL;
	HDC hdc = NULL;
	
	ofstream streamRecv;

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CLOSE:
		closesocket(g_hSock);
		break;

	case FD_READ:
		ChattingReadFunction(hDlg, wParam, lParam);
		break;

	case FD_CONNECT:

		//WSAAsyncSelect()
		iRetval		= WSAAsyncSelect(g_hSock, hDlg, WM_SOCKET, FD_READ | FD_CLOSE);		
		if (iRetval == SOCKET_ERROR) err_quit("WSAAsyncSelect()");

		break;

	}
}


VOID InitProc(HWND hDlg)
{
	int iRetval	= 0;

	g_hChatWindow		= GetDlgItem(hDlg, IDC_EDIT1);
	g_hChatting			= GetDlgItem(hDlg, IDC_EDIT2);
	g_hOKbutton			= GetDlgItem(hDlg, IDOK);
	SendMessage(g_hChatting, EM_SETLIMITTEXT, CHATSIZE, 0);

	//Socket()
	g_hSock		= socket(AF_INET, SOCK_STREAM, 0);
	if (g_hSock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr = {0,};
	serveraddr.sin_family		= AF_INET;
	serveraddr.sin_port			= htons(9000);
	serveraddr.sin_addr.s_addr	= inet_addr("192.168.100.162");

	//WSAAsyncSelect()
	iRetval		= WSAAsyncSelect(g_hSock, hDlg, WM_SOCKET, FD_CONNECT | FD_CLOSE);
	if (iRetval == SOCKET_ERROR) err_quit("WSAAsyncSelect()");

	//connect()
	iRetval		= connect(g_hSock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (iRetval == INVALID_SOCKET)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			err_display("connect()");
		}
	}


	
}



VOID ChattingReadFunction(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	SOCKET hSock = (SOCKET)wParam;
	int iRecvLen = 0;
	int iBodySize = 0;
	int iMaxLen = BUFSIZE;	
//	LPPACKET_HEADER pHeader = NULL;
	LPSTR pBody = NULL;
	LPPACKET_HEADER pHeader = NULL;


	LPPACKET_BODY pPacket = new PACKET_BODY;
	ZeroMemory(pPacket, sizeof(PACKET_BODY));

	while(pPacket->iCurRecv < iMaxLen)
	{
		iRecvLen = recv(hSock, (LPSTR)(pPacket->cData + pPacket->iCurRecv), (iMaxLen - pPacket->iCurRecv), NULL);

		if ( iRecvLen == SOCKET_ERROR )
		{
			if ( WSAGetLastError() == WSAEWOULDBLOCK )
			{
				Sleep(1000);
				break;
			}
		}

		pPacket->iCurRecv += ((iRecvLen > 0) ? iRecvLen : 0);

		if (pPacket->iCurRecv >= sizeof(PACKET_HEADER))
		{
			if (!pHeader)
			{
				pHeader = (LPPACKET_HEADER) pPacket->cData;
				iMaxLen = pHeader->iSize;
			}
		}
	}

	pBody = (LPSTR)(pPacket->cData + sizeof(PACKET_HEADER));
	iBodySize = pHeader->iSize - sizeof(PACKET_HEADER);

	switch ( pHeader->iFlag )
	{
	case WSABUFFER_CHATTING:
		DisplayText("[TCP Client DATA] %s\r\n", pBody);
		EnableWindow(g_hOKbutton, true);
		break;

	case WSABUFFER_IMAGE:
		ReadBinaryBMP(pBody, iBodySize);
		break;
		
	case WSABUFFER_START:
		AllReadyIsDone(g_hWndMain);
		break;

	case WSABUFFER_END:
		VictoryOnGame();
		break;

	default:
		break;
	}

	delete pPacket;
	pPacket = NULL;
}

VOID SendChatting(char* cBuf)
{
	int iSendLen = 0;
	int iSendTot = 0;
	DWORD dwRespBufSize = 0;
	PBYTE pRespBuf = NULL;
	LPSTR pBody = NULL;

	dwRespBufSize = sizeof(PACKET_HEADER) + strlen(cBuf) + 1;
	pRespBuf = new BYTE[dwRespBufSize];
	ZeroMemory(pRespBuf, dwRespBufSize);

	LPPACKET_HEADER pHeader = (LPPACKET_HEADER)pRespBuf;
	pBody = (LPSTR)pRespBuf + sizeof(PACKET_HEADER);
	
	//Setting Header
	pHeader->iFlag = WSABUFFER_CHATTING;
	pHeader->iSize = sizeof(PACKET_HEADER) + strlen(cBuf);

	//Setting Body
	strcpy(pBody, cBuf);

	do 
	{
		iSendLen = send(g_hSock, (LPSTR)(pHeader + iSendTot), pHeader->iSize - iSendTot, NULL);
		if ( iSendLen == SOCKET_ERROR )
		{
			break;
		}
		iSendTot += iSendLen;
	} while (pHeader->iSize != iSendTot);	
	
	delete [] pRespBuf;
	pRespBuf = NULL;
}

VOID ReadBinaryBMP(LPSTR pBody, int iBodySize)
{
	HDC hdc = NULL;
	LPVOID lpBit = NULL;
	LPBITMAPINFO lpBmi = NULL;
	HBITMAP hImage = NULL;
	HBITMAP hOldBitmap = NULL;
	HANDLE hFile = 0;
	WCHAR wFileName[MAX_PATH] = {0,};
	DWORD dwWrite = 0;
	wcscpy(wFileName, L"ReceiveBMP.bmp");
	

	hFile = CreateFile(wFileName, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if ( hFile == INVALID_HANDLE_VALUE )
	{
		err_quit("FileHandleError()");
	}

	WriteFile(hFile, pBody, iBodySize, &dwWrite, NULL); 
	CloseHandle(hFile);


	hdc = GetDC(g_hWndMain);
	g_hRecvMemDC = CreateCompatibleDC(NULL);
	g_hBitmap = CreateCompatibleBitmap(hdc, (BW+2)*TS, (BH+2)*TS);

	lpBit = pBody + sizeof(BITMAPINFO) + sizeof(BITMAPFILEHEADER);
	lpBmi = new BITMAPINFO;
	ZeroMemory(lpBmi, sizeof(BITMAPINFO));
	memcpy(lpBmi, pBody + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFO));
	

	SetDIBits(g_hRecvMemDC, g_hBitmap, 0, g_tBmpInfoHeader.biHeight, lpBit, lpBmi, DIB_RGB_COLORS);
	SelectObject(g_hRecvMemDC, g_hBitmap);

	InvalidateRect(g_hWndMain, NULL, TRUE);
	UpdateWindow(g_hWndMain);

	BitBlt(hdc, (BW+12)*TS + 20, 0, (BW+2)*TS, (BH+2)*TS, g_hRecvMemDC, 0, 0, SRCCOPY);

	DeleteDC(g_hRecvMemDC);
	DeleteObject(g_hBitmap);
}



VOID VictoryOnGame()
{
	KillTimer(g_hWndMain, TIMER_TYPE_DOWN);
	KillTimer(g_hWndMain, TIMER_TYPE_CAPTURE);

	eGameStatus = GAMEOVER;

	MessageBox(g_hWndMain, TEXT("You Winner!!"), TEXT("NOTICE"), MB_OK);

	SetWindowText(g_hReadyButton, TEXT("Ready"));
	ShowWindow(g_hReadyButton, SW_SHOW);
}