// WSAAsyncSelectServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	int iRet = 0;

	//Window class Enrolls
	WNDCLASS tWindowClass;
	tWindowClass.cbClsExtra		= 0;
	tWindowClass.cbWndExtra		= 0;
	tWindowClass.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	tWindowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
	tWindowClass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	tWindowClass.hInstance		= NULL;
	tWindowClass.lpfnWndProc		= (WNDPROC)WndProc;
	tWindowClass.lpszClassName	= L"TCP Server";
	tWindowClass.lpszMenuName	= NULL;
	tWindowClass.style			= CS_HREDRAW | CS_VREDRAW;

	if (RegisterClass(&tWindowClass) == 0)
	{
		return -1;
	}

	//Create Window
	HWND hWnd = CreateWindow(L"TCP Server", L"TCP Server", WS_DISABLED, 0, 0, 600, 300, NULL, (HMENU)NULL, NULL, NULL);

	if ( hWnd == NULL )
	{
		return -1;
	}

	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);

	//WinSock initialization
	WSADATA	tWsa = {0,};

	if ( WSAStartup(MAKEWORD(2,2), &tWsa) != 0 )
	{
		return -1;
	}

	//socket()
	SOCKET listen_sock	= socket(AF_INET, SOCK_STREAM, 0);

	if ( listen_sock == INVALID_SOCKET )
	{
		err_quit("socket()");
	}

	//WSAAsyncSelect()
	iRet = WSAAsyncSelect(listen_sock, hWnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);

	if ( iRet == SOCKET_ERROR )
	{
		err_quit("WSAAsyncSelect()");
	}

	//bind()
	SOCKADDR_IN serveraddr = {0,};
	
	serveraddr.sin_family		= AF_INET;
	serveraddr.sin_port			= htons(9000);
	serveraddr.sin_addr.s_addr	= htonl(INADDR_ANY);

	iRet = bind(listen_sock, (const sockaddr*)&serveraddr, sizeof(serveraddr));

	if ( iRet == SOCKET_ERROR )
	{
		err_quit("bind()");
	}

	//listen()
	iRet = listen(listen_sock, SOMAXCONN);

	if ( iRet == SOCKET_ERROR )
	{
		err_quit("listen()");
	}

	//Message Loop
	MSG msg = {0,};

	while( GetMessage(&msg, 0, 0, 0) > 0 )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		{
			SetTimer(hWnd, TIMER_TYPE_WATCHDOG, TIMER_TYPE_WATCHDOG_DELAY, NULL);
		}
		break;

	case WM_SOCKET:
		{
			ProcessSocketMessage(hWnd, uMsg, wParam, lParam);
		}
		break;

	case WM_TIMER:
		{
			switch(	wParam )
			{
			case TIMER_TYPE_WATCHDOG :
				{
					Watchdog_Kill();
				}
				break;
			}
		}
		break;
		
	case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;
	}
	
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}



VOID ProcessSocketMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SOCKETINFO *pClientSocketInfo = NULL;

	switch (WSAGETSELECTEVENT (lParam))
	{
	case FD_ACCEPT:
		{
			SocketAcceptFunction(hWnd, wParam, lParam);
		}
		break;

	case FD_READ:
		{
			pClientSocketInfo = GetSocketInfo(wParam);
			SocketReadFunction(hWnd, wParam, lParam);
		}
		break;

	case FD_CLOSE:
		{
			RemoveSocketInfo(wParam);
		}
		break;
	}
}