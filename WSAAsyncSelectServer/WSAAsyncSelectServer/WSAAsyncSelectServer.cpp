// WSAAsyncSelectServer.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"



int _tmain(int argc, _TCHAR* argv[])
{
	int iRet = 0;

	//Window class Enrolls
	WNDCLASS wcs;
	wcs.cbClsExtra		= 0;
	wcs.cbWndExtra		= 0;
	wcs.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcs.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wcs.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcs.hInstance		= NULL;
	wcs.lpfnWndProc		= (WNDPROC)WndProc;
	wcs.lpszClassName	= L"TCP Server";
	wcs.lpszMenuName	= NULL;
	wcs.style			= CS_HREDRAW | CS_VREDRAW;

	if (RegisterClass(&wcs) == 0)
	{
		return -1;
	}


	//Create Window
	HWND hWnd = CreateWindow(L"TCP Server", L"TCP Server", WS_DISABLED, 0, 0, 600, 300, NULL, (HMENU)NULL, NULL, NULL);

	if (hWnd == NULL) 
		return -1;

	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);

	//WinSock initialization
	WSADATA	wsa	= {0,};

	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return -1;

	//socket()
	SOCKET listen_sock	= socket(AF_INET, SOCK_STREAM, 0);

	if (listen_sock == INVALID_SOCKET) 
		err_quit("socket()");

	//WSAAsyncSelect()
	iRet = WSAAsyncSelect(listen_sock, hWnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);

	if (iRet == SOCKET_ERROR) 
		err_quit("WSAAsyncSelect()");

	//bind()
	SOCKADDR_IN serveraddr = {0,};
	
	serveraddr.sin_family		= AF_INET;
	serveraddr.sin_port			= htons(9000);
	serveraddr.sin_addr.s_addr	= htonl(INADDR_ANY);

	iRet = bind(listen_sock, (const sockaddr*)&serveraddr, sizeof(serveraddr));

	if (iRet == SOCKET_ERROR) 
		err_quit("bind()");

	//listen()
	iRet = listen(listen_sock, SOMAXCONN);

	if (iRet == SOCKET_ERROR) 
		err_quit("listen()");

	//Message Loop
	MSG msg = {0,};

	while(GetMessage(&msg, 0, 0, 0) > 0)
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
	case WM_CREATE : 
		break;

	case WM_SOCKET:
		ProcessSocketMessage(hWnd, uMsg, wParam, lParam);
		break;
		
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


VOID ProcessSocketMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPUSERINFO pUserInfo	= NULL;
	SOCKETINFO *ptr			= NULL;
	SOCKET sockClient		= {0,};
	SOCKET sockAnother		= {0,};
	SOCKADDR_IN addrClient	= {0,};
	int iAddrlen			= 0;
	int iRetval				= 0;
	int iNameLen			= 0;


	if (ptr != NULL)
	{
		iNameLen				= strlen(ptr->buf);
	}

	switch (WSAGETSELECTEVENT (lParam))
	{
	case FD_ACCEPT:
		iAddrlen		= sizeof(addrClient);
		sockClient		= accept(wParam, (SOCKADDR*)&addrClient, &iAddrlen);
		if (sockClient == INVALID_SOCKET)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				err_display("accept()");
			}
			return;
		}
		printf("[TCP Server] Client Accept: IP address = %s Port Number = %d\n", inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));
		AddSocketInfo(sockClient);
		AddUserInfo(&addrClient);

		iRetval = WSAAsyncSelect(sockClient, hWnd, WM_SOCKET, FD_READ|FD_WRITE|FD_CLOSE);
		if (iRetval == SOCKET_ERROR)
		{
			err_display("WSAAsyncSelect()");
			RemoveSocketInfo(sockClient);
		}
		

		break;
	case FD_READ:
		ptr		= GetSocketInfo(wParam);
		if (ptr->recvbytes > 0)
		{
			ptr->recvdelayed		= TRUE;
			return;
		}

		//Receive Data
		iRetval			= recv(ptr->sock, ptr->buf, BUFSIZE, 0);
		if (iRetval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				err_display("recv()");
				RemoveSocketInfo(wParam);
			}
			return;
		}
		

		//Confirm request about room info
		if (ptr->buf[1]	== '*')
		{
			for (iterRoom = Room_map.begin(); iterRoom != Room_map.end(); iterRoom++)
			{
				sprintf(ptr->buf, iterRoom->second->cRoomName);
				iNameLen				= strlen(ptr->buf);
				ptr->buf[iNameLen + 1]	= iterRoom->second->iNum;
				ptr->buf[iNameLen + 2]	= iterRoom->second->iPeopleIN;

				send(ptr->sock, ptr->buf, strlen(ptr->buf) + 4, NULL);
			}
		}
		//Confirm whether it is title of room or not.
		else if (ptr->buf[iNameLen + 1] == '@')
		{
			ptr->buf[iNameLen + 1] = '\0';
			getpeername(ptr->sock, (SOCKADDR*)&addrClient, &iAddrlen);
			CreateRoomInfo(ptr->buf, addrClient);
			
			break;
		}		//Confirm Join
		else
		{
			ptr->recvbytes		= iRetval;

			ptr->buf[iRetval]	= '\0';
			iAddrlen			= sizeof(addrClient);
			getpeername(wParam, (SOCKADDR*)&addrClient, &iAddrlen);

			pUserInfo	= GetUserInfo(&addrClient);

			if ( pUserInfo == NULL )
			{

				// ó�� �ʿ�....
			}

			printf("[TCP/%s:%d] %s\n", inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port), ptr->buf);


			if (ptr->recvbytes <= ptr->sendbytes) return;

			//Sending Data
			iRetval		= send(ptr->sock, ptr->buf + ptr->sendbytes, ptr->recvbytes - ptr->sendbytes, 0);
			if (iRetval == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					err_display("send()");
					RemoveSocketInfo(wParam);
				}
				return;
			}

			if (socket_map.begin() == socket_map.end()) {;}
			else
			{
				for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
				{
					if (inet_ntoa(addrClient.sin_addr) == inet_ntoa(iterUser->second->addr.sin_addr)){;}

					else if (pUserInfo->iRoomNumber == iterUser->second->iRoomNumber && iterUser->second->iRoomNumber != 0) //NOT Waiting room
					{
						sockAnother = GetSock(iterUser->second);

						if ( sockAnother )
						{
							iRetval		= send(sockAnother, ptr->buf + ptr->sendbytes, ptr->recvbytes - ptr->sendbytes, 0);
							if (iRetval == SOCKET_ERROR)
							{
								if (WSAGetLastError() != WSAEWOULDBLOCK)
								{
									err_display("send()");
								}
								return;
							}
						}
						else
						{
							// ����ó��
						}
					}

				}
			}
			//Check Bytes
			ptr->sendbytes += iRetval;
			if (ptr->recvbytes == ptr->sendbytes)
			{
				ptr->recvbytes = ptr->sendbytes = 0;
				if (ptr->recvdelayed)
				{
					ptr->recvdelayed = FALSE;
					PostMessage(hWnd, WM_SOCKET, wParam, FD_READ);
				}
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
	i++;

	SOCKETINFO *ptr		= new SOCKETINFO;
	ZeroMemory(ptr, sizeof(SOCKETINFO));

	ptr->sock		= sock;
	ptr->recvbytes	= 0;
	ptr->sendbytes	= 0;
	ptr->recvdelayed = FALSE;

	socket_map.insert(pair<int, SOCKETINFO*>(i, ptr));

	return TRUE;
}


SOCKETINFO *GetSocketInfo(SOCKET sock)
{
	for (it = socket_map.begin(); it != socket_map.end(); it++)
	{
		if (it->second->sock == sock)
			return it->second;
	}
	err_display("GetSocketInfo()");
	return NULL;
}

VOID RemoveSocketInfo(SOCKET sock)
{
	SOCKADDR_IN addrClient;
	int iAddrlen	= sizeof(addrClient);
	getpeername(sock, (SOCKADDR*)&addrClient, &iAddrlen);
	printf("[TCP Server] Client Quit: IP Address = %s, Port Number = %d\n", inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));

	for (it = socket_map.begin(); it != socket_map.end(); it++)
	{
		if (it->second->sock == sock)
		{
			delete it->second;
			socket_map.erase(it);
			return;
		}
	}
	return;
}

VOID err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

VOID err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

VOID err_display(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf, 0, NULL);
	printf("[Error] %s", (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}


VOID AddUserInfo(SOCKADDR_IN * pAddrClient)
{
	static int i		= 0;
	
	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		if (inet_ntoa(pAddrClient->sin_addr) == inet_ntoa(iterUser->second->addr.sin_addr))
		{
			return;
		}
	}

	i++;

	LPUSERINFO pUserInfo		= NULL;
	pUserInfo		= new USERINFO;
	ZeroMemory(pUserInfo, sizeof(USERINFO));

	memcpy(&(pUserInfo->addr), pAddrClient, sizeof(SOCKADDR_IN));
	pUserInfo->iRoomNumber	= 0;
	pUserInfo->iStatus		= 0;

	mUSER.insert(pair<int, LPUSERINFO>(i, pUserInfo));
}

LPUSERINFO GetUserInfo(SOCKADDR_IN * pAddrClient)
{
	LPUSERINFO pReturn = NULL;

	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		if (inet_ntoa(iterUser->second->addr.sin_addr) == inet_ntoa(pAddrClient->sin_addr))
		{
			pReturn = iterUser->second;
			break;
		}
	}

	return pReturn;
}


SOCKET GetSock(LPUSERINFO pUserInfo)
{
	SOCKET		hSock		= NULL;

	int			iAddrlen	= 0;
	SOCKADDR_IN pTempAddr	= {0,};

	iAddrlen = sizeof(SOCKADDR_IN);

	for (it = socket_map.begin(); it != socket_map.end(); it++)
	{
		getpeername(it->second->sock, (SOCKADDR*)&pTempAddr, &iAddrlen);

		if (inet_ntoa(pTempAddr.sin_addr) == inet_ntoa(pUserInfo->addr.sin_addr))
		{
			hSock = it->second->sock;
			break;
		}
	}


	return hSock;
}

