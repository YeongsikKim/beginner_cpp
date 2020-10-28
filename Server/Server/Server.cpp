// Server.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Server.h"


#define BUFSIZE			512
#define WM_SOCKET		(WM_USER+1)

using namespace std;

struct SOCKETINFO
{
	SOCKET sock;
	char buf[BUFSIZE+1];
	int recvbytes;
	int sendbytes;
	BOOL recvdelayed;
};

map<int, SOCKETINFO*> m;

HWND hWnd;
HINSTANCE g_hInst;
LPCTSTR	lpszClass = TEXT("TCP Server");
HDC	 hdc;
TCHAR cBuf[BUFSIZE + 256];

ATOM MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
VOID ProcessSocketMessage(HWND, UINT, WPARAM, LPARAM);

BOOL AddSocketInfo(SOCKET sock);
SOCKETINFO *GetSocketInfo(SOCKET sock);
VOID RemoveSocketInfo(SOCKET sock);



VOID err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
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

	printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
	
}

WCHAR wClass[50] = {0,};

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	int retval			= 0;
	MSG iMessage		= {0,};
	HACCEL hAccelTable	= NULL;

	//Window Initialization
	MyRegisterClass(hInstance);
	g_hInst		= hInstance;

	hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"ClassName", L"TCP Server", WS_OVERLAPPEDWINDOW, 50, 50, 600, 300, NULL, NULL, hInstance, NULL);

	if (hWnd == NULL)
	{
		return -1;
	}


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	

	//Winsock Initialization
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return -1;
	}

	//Socket()
	SOCKET listen_sock		= socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	//WSAAsyncSelect()
	retval		= WSAAsyncSelect(listen_sock, hWnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);
	if (retval == SOCKET_ERROR) err_quit("WSAAsyncSelect()");

	
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family		= AF_INET;
	serveraddr.sin_port			= htons(9000);
	serveraddr.sin_addr.s_addr	= htonl(INADDR_ANY);

	//bind()
	retval		= bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	//listen()
	retval		= listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	//Loop Message
	while (GetMessage(&iMessage, 0, 0, 0) > 0)
	{
		TranslateMessage(&iMessage);
		DispatchMessage(&iMessage);
	}

	return iMessage.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex	= {0,};

	wcex.cbSize				= sizeof(WNDCLASSEX);
	wcex.cbClsExtra			= 0;
	wcex.cbWndExtra			= 0;
	wcex.hbrBackground		= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon				= LoadIcon(NULL, IDI_APPLICATION);
	wcex.hInstance			= hInstance;
	wcex.lpfnWndProc		= WndProc;
	wcex.lpszClassName		= L"ClassName";
	wcex.lpszMenuName		= NULL;
	wcex.style				= CS_HREDRAW | CS_VREDRAW;

	return RegisterClassEx(&wcex);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SOCKET:
		ProcessSocketMessage(hWnd, message, wParam, lParam);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


VOID ProcessSocketMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SOCKETINFO socket_info	= {0,};
	SOCKETINFO *ptr			= &socket_info;
	SOCKET client_sock		= NULL;
	SOCKADDR_IN clientaddr	= {0,};
	hdc						= GetDC(hWnd);

	int addrlen				= 0;
	int retval				= 0;

	map<int, SOCKETINFO*> :: iterator i;

	

	if(WSAGETSELECTERROR(lParam))
	{
		err_display((char*)(WSAGETSELECTERROR(lParam)));
		RemoveSocketInfo(wParam);
		return;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
		addrlen		= sizeof(clientaddr);
		client_sock	= accept(wParam, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				err_display("accept()");
			}
			return;
		}

		wsprintf(cBuf, _T("[TCP Server] Client accepted : IP address = %s, Port Number = %d\n"),
			inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port));

		printf("[TCP Server] Client accepted : IP address = %s, Port Number = %d\n",
			inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port));
		AddSocketInfo(client_sock);
		retval		= WSAAsyncSelect(client_sock, hWnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);
		if (retval == SOCKET_ERROR)
		{
			err_display("WSAAsyncSelect()");
			RemoveSocketInfo(client_sock);
		}
		break;
	case FD_READ:
		ptr		= GetSocketInfo(wParam);
		if (ptr->recvbytes > 0)
		{
			ptr->recvdelayed	= TRUE;
			return;
		}

		retval = recv(ptr->sock, ptr->buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				err_display("recv()");
				RemoveSocketInfo(wParam);
			}
			return;
		}

		ptr->recvbytes		= retval;
		ptr->buf[retval]	= '\0';
		addrlen				= sizeof(clientaddr);
		getpeername(wParam, (SOCKADDR*)&clientaddr, &addrlen);
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), ptr->buf);
		
	case FD_WRITE:
		ptr			= GetSocketInfo(wParam);
		if (ptr->recvbytes <= ptr->sendbytes)
		{
			return;
		}

		retval		= send(ptr->sock, ptr->buf + ptr->sendbytes, ptr->recvbytes - ptr->sendbytes, 0);
/*
		for (i = m.begin(); i != m.end(); i++)
		{
			if (i->second->sock == ptr->sock) {;}
			else 
			{
				retval		= send(i->second->sock, i->second->buf + i->second->sendbytes, i->second->recvbytes - i->second->sendbytes, 0);
				if (retval == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						err_display("send()");
						RemoveSocketInfo(wParam);
					}
					return;
				}
			}			
		}
*/
		
		ptr->sendbytes += retval;

		if (ptr->recvbytes == ptr->sendbytes)
		{
			ptr->recvbytes = ptr->sendbytes	= 0;
			if (ptr->recvdelayed)
			{
				ptr->recvdelayed	= FALSE;
				PostMessage(hWnd, WM_SOCKET, wParam, FD_READ);
			}
		}
		break;
	case FD_CLOSE:
		RemoveSocketInfo(wParam);
		break;
	}
}


BOOL AddSocketInfo(SOCKET sock)
{
	static int i		= 0;

	SOCKETINFO *ptr		= new SOCKETINFO;
	if (ptr == NULL)
	{
		printf("[ERROR]\n");
		return FALSE;
	}

	i++;
	m.insert(map<int , SOCKETINFO*>::value_type(i, ptr));

	ptr->sock			= sock;
	ptr->recvbytes		= 0;
	ptr->sendbytes		= 0;
	ptr->recvdelayed	= FALSE;

	return TRUE;
}


SOCKETINFO *GetSocketInfo (SOCKET sock)
{
	map<int, SOCKETINFO*>::iterator i;


	for (i = m.begin(); i != m.end(); i++)
	{
		if (i->second->sock == sock)
		{
			return i->second;
		}
	}

	return NULL;
}

VOID RemoveSocketInfo(SOCKET sock)
{
	SOCKADDR_IN		clientaddr;
	int addrlen		= sizeof(clientaddr);
	getpeername(sock, (SOCKADDR*)&clientaddr, &addrlen);
	printf("[TCP Server] Client Exit: IP address = %s, Port Number = %d\n", 
		inet_ntoa(clientaddr.sin_addr), 
		ntohs(clientaddr.sin_port));

	map<int, SOCKETINFO*>::iterator it;
	it		= m.find(sock);

	if (it != m.end()) m.erase(it);
/*
	map<int, SOCKETINFO*>::iterator i;
	
	for (i = m.begin(); i != m.end(); i++)
	{
		if (i->second->sock == sock)
		{
			break;
		}
	}
*/
}
