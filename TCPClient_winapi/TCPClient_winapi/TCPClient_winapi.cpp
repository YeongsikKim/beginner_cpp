// TCPClient_winapi.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "TCPClient_winapi.h"



//int vsprintf(char *cbuf, const char *fmt, va_list arg);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: ���⿡ �ڵ带 �Է��մϴ�.

	
	wcscpy_s(szWindowClass, sizeof(_T("WinClass")), _T("WinClass"));
	hReadEvent		= CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) return -1;
	hWriteEvent		= CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return -1;
	
	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);

	closesocket(sock);

	WSACleanup();

	return 0;
}


BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		hEdit1		= GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2		= GetDlgItem(hDlg, IDC_EDIT2);
		hOKbutton	= GetDlgItem(hDlg, IDOK);
		SendMessage(hEdit2, EM_SETLIMITTEXT, BUFSIZE, 0);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EnableWindow(hOKbutton, FALSE);
			WaitForSingleObject(hReadEvent, INFINITE);
			GetDlgItemTextA(hDlg, IDC_EDIT2, buf, BUFSIZE+1);

			SetEvent(hWriteEvent);

			SetFocus(hEdit2);
			SendMessage(hEdit2, EM_SETSEL, 0, -1);
			SendMessage(hEdit2, EM_REPLACESEL, NULL, (LPARAM)"");
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		return FALSE;
	
	}
	return FALSE;
}



DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) return -1;

	sock		= socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr;
	serveraddr.sin_family		= AF_INET;
	serveraddr.sin_port			= htons(9000);
	serveraddr.sin_addr.s_addr	= inet_addr("127.0.0.1");

	retval		= connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");


	while (TRUE)
	{
		WaitForSingleObject(hWriteEvent, INFINITE);


		if (strlen(buf) == 0)
		{
			EnableWindow(hOKbutton, TRUE);

			SetEvent(hReadEvent);
			continue;
		}

		retval		= send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}

		DisplayText("[TCP Client] %d Bytes sending.\r\n", retval);
		
		

		retval		= recvn(sock, buf, retval, 0);
		if(retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (retval == 0) break;

		buf[retval]	= '\0';
		DisplayText("[TCP Client DATA] %s\r\n", buf);


		EnableWindow(hOKbutton, true);

		SetEvent(hReadEvent);
	}

	return 0;
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

/*
VOID DisplayText( char const * const format, ... )
{
	va_list args;
	int     len;
	char    *buffer;

	// retrieve the variable arguments
	va_start( args, format );

	len = _vscprintf( format, args ) // _vscprintf doesn't count
		+ 1; // terminating '\0'

	buffer = (char*)malloc( len * sizeof(char) );
	if ( 0 != buffer )
	{
		vsprintf( buffer, format, args ); // C4996
		// Note: vsprintf is deprecated; consider using vsprintf_s instead
		int nLength		=  GetWindowTextLength(hEdit2);
		SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
		SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)buffer);

		free( buffer );
	}
	va_end( args );
}
*/

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

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TCPCLIENT_WINAPI));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TCPCLIENT_WINAPI);
	wcex.lpszClassName	= szWindowClass;
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

   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����: �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND	- ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT	- �� â�� �׸��ϴ�.
//  WM_DESTROY	- ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �޴��� ���� ������ ���� �м��մϴ�.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


