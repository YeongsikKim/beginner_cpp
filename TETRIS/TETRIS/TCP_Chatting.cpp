// TCPClient_winapi.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
//int vsprintf(char *cbuf, const char *fmt, va_list arg);




BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int retval		= 0;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitProc(hDlg);
		SetParent(hDlg, hWndMain);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EnableWindow(hOKbutton, FALSE);
			GetDlgItemTextA(hDlg, IDC_EDIT2, cBuf, CHATSIZE+1);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			SendChatting();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			SetFocus(hEdit2);
			SendMessage(hEdit2, EM_SETSEL, 0, -1);
			SendMessage(hEdit2, EM_REPLACESEL, NULL, (LPARAM)"");
			EnableWindow(hOKbutton, TRUE);

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


VOID DisplayText(char *fmt, ...)
{
	char cbuf[BUFSIZE + 256]	= {0,};
	int ret		= 0;

	va_list arg;
	va_start(arg, fmt);

	vsprintf(cbuf, fmt, arg);


	int nLength		=  GetWindowTextLength(hEdit1);
	//SendMessageA(hEdit1, EM_SETSEL, nLength, nLength);
	SendMessageA(hEdit1, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}


VOID err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBoxA(NULL, (LPCSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}


VOID err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	DisplayText("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);

}

int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr		= buf;
	int left		= len;

	while (left > 0)
	{
		received		= recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		else if (received == 0)
		{
			break;
		}
		left -= received;
		ptr  += received;
	}

	return (len - left);
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
		closesocket(hSock);
		break;

	case FD_READ:
		ChattingReadFunction(hDlg, wParam, lParam);
		break;

	case FD_CONNECT:

		//WSAAsyncSelect()
		iRetval		= WSAAsyncSelect(hSock, hDlg, WM_SOCKET, FD_READ | FD_CLOSE);		
		if (iRetval == SOCKET_ERROR) err_quit("WSAAsyncSelect()");

		break;

	}
}


VOID InitProc(HWND hDlg)
{
	int retval	= 0;

	hEdit1		= GetDlgItem(hDlg, IDC_EDIT1);
	hEdit2		= GetDlgItem(hDlg, IDC_EDIT2);
	hOKbutton	= GetDlgItem(hDlg, IDOK);
	SendMessage(hEdit2, EM_SETLIMITTEXT, BUFSIZE, 0);

	//Socket()
	hSock		= socket(AF_INET, SOCK_STREAM, 0);
	if (hSock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr = {0,};
	serveraddr.sin_family		= AF_INET;
	serveraddr.sin_port			= htons(9000);
	serveraddr.sin_addr.s_addr	= inet_addr("192.168.100.162");

	//WSAAsyncSelect()
	retval		= WSAAsyncSelect(hSock, hDlg, WM_SOCKET, FD_CONNECT | FD_CLOSE);
	if (retval == SOCKET_ERROR) err_quit("WSAAsyncSelect()");

	//connect()
	retval		= connect(hSock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == INVALID_SOCKET)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			err_display("connect()");
		}
	}

	LPPACKET_BODY pPacket = new PACKET_BODY;
	ZeroMemory(pPacket, sizeof(PACKET_BODY));
	mPACKET.insert(pair<SOCKET, LPPACKET_BODY>(hSock, pPacket));
}



VOID ChattingReadFunction(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	SOCKET hSock = (SOCKET)wParam;
	int iRecvLen = 0;
	int iBodySize = 0;
	int iMaxLen = BUFSIZE;	
//	LPPACKET_HEADER pHeader = NULL;
	LPSTR pBody = NULL;
	PACKET_HEADER hHeader = {0,};

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
		DisplayText("[TCP Client DATA] %s\r\n", pBody);
		EnableWindow(hOKbutton, true);
		break;

	case WSABUFFER_IMAGE:
		ReadBinaryBMP(pBody, iBodySize);
		break;
		
	case WSABUFFER_START:
		AllReadyIsDone(hWndMain);
		break;

	default:
		break;
	}
}

VOID SendChatting()
{
	int iSendLen = 0;
	int iSendTot = 0;
	DWORD dwRespBufSize = sizeof(PACKET_HEADER) + strlen(cBuf) + 1;
	PBYTE pRespBuf = new BYTE[dwRespBufSize];
	LPSTR pBody = NULL;

	LPPACKET_HEADER pHeader = (LPPACKET_HEADER)pRespBuf;
	pBody = (LPSTR)pRespBuf + sizeof(PACKET_HEADER);
	
	//Setting Header
	pHeader->iFlag = WSABUFFER_CHATTING;
	pHeader->iSize = sizeof(PACKET_HEADER) + strlen(cBuf);

	//Setting Body
	strcpy(pBody, cBuf);

	do 
	{
		iSendLen = send(hSock, (LPSTR)(pHeader + iSendTot), pHeader->iSize - iSendTot, NULL);
		if (iSendLen == SOCKET_ERROR)
		{
			break;
		}
		iSendTot += iSendLen;
	} while (pHeader->iSize != iSendTot);	
}

VOID ReadBinaryBMP(LPSTR pBody, int iBodySize)
{
	HDC hdc = NULL;
	ofstream streamRecv;
	LPBITMAPINFO pBmpHeader = NULL;
	LPSTR pBmpBody = NULL;
	

	streamRecv.open("c:\\beginnerC\\RecievedBMP.bmp", ios::binary);
	if (!streamRecv.is_open())
	{
		cout << "File open error!!" << endl;
		exit(-1);
	}
	streamRecv.write((LPSTR)pBody, iBodySize);
	streamRecv.close();

#if 0
	hdc = GetDC(hWndMain); 
	hMemDC = CreateCompatibleDC(NULL);
	hBitmap = CreateCompatibleBitmap(hdc, (BW+2)*TS, (BH+2)*TS);

	SetDIBits(hMemDC, hBitmap, 0, bi.biHeight, pBody + sizeof(BITMAPINFO) + 1, lpHeader, DIB_RGB_COLORS);
	SelectObject(hMemDC, hBitmap);

	InvalidateRect(hWndMain, NULL, FALSE);
	UpdateWindow(hWndMain);

	BitBlt(hdc, (BW+12)*TS + 20, 0, (BW+2)*TS, (BH+2)*TS, hMemDC, 0, 0, SRCCOPY);

	DeleteDC(hMemDC);
	DeleteObject(hBitmap);
#endif

#if 1
	BITMAP bit = {0,};
	int bx = 0;
	int by = 0;
	hdc = GetDC(hWndMain);
	hMemDC = CreateCompatibleDC(NULL);
	HBITMAP hImage = (HBITMAP)LoadImageA(g_hInst, "c:\\beginnerC\\ReceivedBMP.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	HBITMAP hOldBitmap = (HBITMAP) SelectObject(hMemDC, hImage);

	GetObject(hImage, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;

	BitBlt(hdc, (BW+12)*TS + 20, 0, (BW+2)*TS, (BH+2)*TS, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hImage);
	DeleteDC(hMemDC);

#endif 
}


VOID AllReadyIsDone(HWND hWnd)
{
	int x = 0;
	int y = 0;
	Point Shape[36][4][4];

	if (GameStatus != GAMEOVER)
	{
		return;
	}
	for (x = 0; x<BW + 2; x++)
	{
		for (y = 0; y<BH + 2; y++)
		{
			board[x][y] = (y==0 || y==BH+1 || x==0 || x==BW+1)?WALL:EMPTY;
		}
	}
	score		= 0;
	bricknum	= 0;
	GameStatus	= RUNNING;
	nbrick		= random(sizeof(Shape)/sizeof(Shape[0]));
	MakeNewBrick();
	iInterval	= TIMER_TYPE_DOWN_DELAY;
	SetTimer(hWnd, TIMER_TYPE_DOWN, iInterval, NULL);
	SetTimer(hWnd, TIMER_TYPE_CAPTURE, TIMER_TYPE_CAPTURE_DELAY, NULL);
}