// TCPClient_winapi.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"


BOOL bStatusREAD = NOTBMP;
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
		closesocket(sock);
		break;

	case FD_READ:
		ChattingReadFunction(hDlg, wParam, lParam);
		break;

	case FD_CONNECT:

		//WSAAsyncSelect()
		iRetval		= WSAAsyncSelect(sock, hDlg, WM_SOCKET, FD_READ | FD_CLOSE);		
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
	sock		= socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr = {0,};
	serveraddr.sin_family		= AF_INET;
	serveraddr.sin_port			= htons(9000);
	serveraddr.sin_addr.s_addr	= inet_addr("192.168.100.162");

	//WSAAsyncSelect()
	retval		= WSAAsyncSelect(sock, hDlg, WM_SOCKET, FD_CONNECT | FD_CLOSE);
	if (retval == SOCKET_ERROR) err_quit("WSAAsyncSelect()");

	//connect()
	retval		= connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == INVALID_SOCKET)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			err_display("connect()");
		}
	}

	//Create Packet
	pPacket = new PACKET_BODY;
	ZeroMemory(pPacket, sizeof(PACKET_BODY));
	return;
}


VOID ChattingReadFunction(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	SOCKET hSock = (SOCKET)wParam;
	int iRecvLen = 0;
	int iBodySize = 0;
	int iMaxLen = 0;
	int iCurRecv = 0;
	char cData[BUFSIZE] = {0,};

	ZeroMemory(pPacket, sizeof(PACKET_BODY));
	LPPACKET_HEADER pHeader = NULL;
	LPSTR pBody = NULL;
	
	iMaxLen = BUFSIZE;

	while(pPacket->iCurRecv < iMaxLen)
	{
		iRecvLen = recv(hSock, (LPSTR)(pPacket->cData + pPacket->iCurRecv), (iMaxLen - pPacket->iCurRecv), NULL);
		if (iRecvLen == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				err_display("recv()");
				continue;
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

	pBody = (LPSTR) (pPacket->cData + sizeof(PACKET_HEADER));
	iBodySize = pHeader->iSize - sizeof(PACKET_HEADER);
	

	switch (pHeader->iFlag)
	{
	case WSABUFFER_CHATTING:
		DisplayText("[TCP Client DATA] %s\r\n", pBody);
		EnableWindow(hOKbutton, true);
		break;

	case WSABUFFER_IMAGE:
		ReadBinaryBMP(pBody, iBodySize);
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
		iSendLen = send(sock, (LPSTR)(pHeader + iSendTot), pHeader->iSize - iSendTot, NULL);
		if (iSendLen == SOCKET_ERROR)
		{
			printf("send()\n");
			Sleep(1000);
			exit(-1);
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
	streamRecv.write((LPSTR)pBody, iFileSize);
	streamRecv.close();

	hdc = GetDC(hWndMain); 

	hMemDC = CreateCompatibleDC(NULL);
	hBitmap = CreateCompatibleBitmap(hdc, (BW+2)*TS, (BH+2)*TS);

	SetDIBits(hMemDC, hBitmap, 0, bi.biHeight, pBmpBody, lpHeader, DIB_RGB_COLORS);
	SelectObject(hMemDC, hBitmap);

	InvalidateRect(hWndMain, NULL, FALSE);
	UpdateWindow(hWndMain);

	BitBlt(hdc, (BW+12)*TS + 10, 0, (BW+12)*TS, (BH+2)*TS, hMemDC, 0, 0, SRCCOPY);

	DeleteDC(hMemDC);
	DeleteObject(hBitmap);
	bStatusREAD = NOTBMP;
}