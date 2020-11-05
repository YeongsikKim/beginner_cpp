// TCPClient_winapi.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"


LPVOID lpBody = NULL;
BOOL bStatusREAD = NOTBMP;
int iSize = 0;
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
			GetDlgItemTextA(hDlg, IDC_EDIT2, buf, BUFSIZE+1);
			buf[strlen(buf) + 1] = '/';
			buf[strlen(buf) + 2] = 'c';
			send(sock, buf, strlen(buf) + 3, 0);

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
	HBITMAP hOld_Bmp = NULL;

	int iRetval		= 0;

	HBITMAP hBitmap = NULL;
	HDC hMemDC = NULL;
	HDC hdc = NULL;
	

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CLOSE:
		//closesocket(sock);
		break;

	case FD_READ:
		ZeroMemory(buf, sizeof(buf));
		
		if (bStatusREAD == NOTBMP)
		{
			iRetval = recv(sock, buf, BUFSIZE, 0);
		}
		else if (bStatusREAD == YESBMP)
		{
			lpBody = malloc(iSize);
			iRetval = recv(sock, (LPSTR)lpBody, iSize, 0);
		}
		if(iRetval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (iRetval == 0) break;

		if (buf[strlen(buf) + 1] == '/' && buf[strlen(buf) + 2] == 'c')
		{
			buf[iRetval]	= '\0';
			DisplayText("[TCP Client DATA] %s\r\n", buf);


			EnableWindow(hOKbutton, true);
		}
		else if (buf[strlen(buf) + 1] == '/' && buf[strlen(buf) + 2] == 's')
		{
			buf[strlen(buf) + 1] = '\0';
			buf[strlen(buf) + 2] = '\0';
			iSize = atoi(buf);

			bStatusREAD = YESBMP;
		}
		else if (bStatusREAD == YESBMP)
		{
			hdc = GetDC(hWndMain);

			hMemDC = CreateCompatibleDC(NULL);
			hBitmap = CreateCompatibleBitmap(hdc, (BW+12)*TS, (BH+2)*TS);

			SetDIBits(hMemDC, hBitmap, 0, bi.biHeight, lpBody, lpHeader, DIB_RGB_COLORS);
			SelectObject(hMemDC, hBitmap);

			BitBlt(hdc, (BW+12)*TS + 4, 0, (BW+2)*TS, (BH+2)*TS, hMemDC, 0, 0, SRCCOPY);
			
			free(lpBody);
			DeleteDC(hMemDC);
			DeleteObject(hBitmap);
			

			
			bStatusREAD = NOTBMP;
		}
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
		return;
	}



}
