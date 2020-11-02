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
		//Receive Data
		iRetval	= recv(ptr->sock, ptr->buf, BUFSIZE, 0);
		iNameLen = strlen(ptr->buf);
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
		if (ptr->buf[1]	== REQUESTROOM)
		{
			for (iterRoom = mROOM.begin(); iterRoom != mROOM.end(); iterRoom++)
			{
				sprintf(ptr->buf, iterRoom->second->cRoomName);
				iNameLen				= strlen(ptr->buf);
				ptr->buf[iNameLen + 1]	= iterRoom->second->iNum;
				ptr->buf[iNameLen + 2]	= iterRoom->second->iPeopleIN;

				send(ptr->sock, ptr->buf, strlen(ptr->buf) + 4, NULL);
			}
		}
		//Confirm whether it is title of room or not.
		else if (ptr->buf[iNameLen + 1] == ROOMTITLE)
		{
			ptr->buf[iNameLen + 1] = '\0';
			getpeername(ptr->sock, (SOCKADDR*)&addrClient, &iAddrlen);
			CreateRoomInfo(ptr->buf);

	
			RenewWaitingRoom();
			
			break;
		}
		else if (ptr->buf[2] == ENTRANCE)		//Join the room
		{
			g_iTempRoomNumber = ptr->buf[0];

			for (iterRoom = mROOM.begin(); iterRoom != mROOM.end(); iterRoom++)
			{
				if (iterRoom->second->iNum == (int)(ptr->buf[0] - '0'))
				{
					break;
				}
			}
			if (iterRoom == mROOM.end())
			{
				break;
			}

			if (iterRoom->second->iPeopleIN >= MAX_PEOPLE)
			{
				;
			}
			else
			{
				iterRoom->second->iPeopleIN++;
				RenewWaitingRoom();
			}
		}
		else
		{
			if (ptr->recvbytes > 0)
			{
				ptr->recvdelayed = TRUE;
				return;
			}

			ptr->recvbytes		= iRetval;

			ptr->buf[iRetval]	= '\0';
			iAddrlen			= sizeof(addrClient);
			getpeername(wParam, (SOCKADDR*)&addrClient, &iAddrlen);

			pUserInfo	= GetUserInfo(&addrClient);

			if ( pUserInfo == NULL )
			{
				err_quit("GetUserInfo()");
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

			if (mSOCKET.begin() == mSOCKET.end()) {;}
			else
			{
				for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
				{
					if (addrClient.sin_addr.s_addr == iterUser->second->addr.sin_addr.s_addr){;}

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
				if (iterUser == mUSER.end())
				{
					break;
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

	mSOCKET.insert(pair<int, SOCKETINFO*>(i, ptr));

	return TRUE;
}


SOCKETINFO *GetSocketInfo(SOCKET sock)
{
	SOCKETINFO *lpSocketinfo = NULL;
	for (iterSocket = mSOCKET.begin(); iterSocket != mSOCKET.end(); iterSocket++)
	{
		if (iterSocket->second->sock == sock)
		{
			lpSocketinfo = iterSocket->second;
		}
		if (iterSocket == mSOCKET.end())
		{
			err_display("GetSocketInfo()");
			return NULL;
		}
	}
	
	return lpSocketinfo;
}

VOID RemoveSocketInfo(SOCKET sock)
{
	LPUSERINFO lpUserinfo = NULL;
	SOCKADDR_IN addrClient = {0,};
	int iAddrlen	= sizeof(addrClient);
	int iRoomNumber = 0;

	getpeername(sock, (SOCKADDR*)&addrClient, &iAddrlen);
	printf("[TCP Server] Client Quit: IP Address = %s, Port Number = %d\n", inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));

	for (iterSocket = mSOCKET.begin(); iterSocket != mSOCKET.end(); iterSocket++)
	{
		if (iterSocket->second->sock == sock)
		{
			delete iterSocket->second;
			mSOCKET.erase(iterSocket);
			break;
		}
	}
	
	//Remove USER Info
	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		if (iterUser->second->addr.sin_addr.s_addr == addrClient.sin_addr.s_addr)
		{
			break;
		}
		if (iterUser == mUSER.end())
		{
			return;
		}
	}

	if (iterUser->second->iRoomNumber != 0)
	{
		iRoomNumber = iterUser->second->iRoomNumber;
		delete iterUser->second;
		mUSER.erase(iterUser);
	}

	//Subtraction the number of people
	if (iRoomNumber != 0)
	{
		DeletePeople(iRoomNumber);
		RenewWaitingRoom();
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
		if (pAddrClient->sin_addr.s_addr == iterUser->second->addr.sin_addr.s_addr)
		{
			iterUser->second->iRoomNumber = g_iTempRoomNumber;
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
		if (iterUser->second->addr.sin_addr.s_addr == pAddrClient->sin_addr.s_addr)
		{
			pReturn = iterUser->second;
			break;
		}
	}

	return pReturn;
}



LPUSERINFO GetUserInfo(SOCKETINFO *pSoketInfo)
{
	LPUSERINFO lpUserinfo	= {0,};
	SOCKADDR_IN addrClient	= {0,};
	int iAddrSize			= 0;
	
	iAddrSize = sizeof(addrClient);

	getpeername(pSoketInfo->sock, (SOCKADDR*)&addrClient, &iAddrSize);

	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		if ((addrClient.sin_addr.s_addr) == (iterUser->second->addr.sin_addr.s_addr))
		{
			break;
		}
	}

	if (iterUser == mUSER.end())
	{
		return NULL;
	}
	
	lpUserinfo = iterUser->second;
	
	return lpUserinfo;
}


SOCKET GetSock(LPUSERINFO pUserInfo)
{
	SOCKET		hSock		= NULL;

	int			iAddrlen	= 0;
	SOCKADDR_IN pTempAddr	= {0,};

	iAddrlen = sizeof(SOCKADDR_IN);

	for (iterSocket = mSOCKET.begin(); iterSocket != mSOCKET.end(); iterSocket++)
	{
		getpeername(iterSocket->second->sock, (SOCKADDR*)&pTempAddr, &iAddrlen);

		if (pTempAddr.sin_addr.s_addr == pUserInfo->addr.sin_addr.s_addr)
		{
			hSock = iterSocket->second->sock;
			break;
		}
	}


	return hSock;
}

VOID DeletePeople(int iRoomNumber)
{
	for (iterRoom = mROOM.begin(); iterRoom != mROOM.end(); iterRoom++)
	{
		if (iterRoom->second->iNum == iRoomNumber)
		{
			break;
		}

		if (iterRoom == mROOM.end())
		{
			return;
		}	
	}
	iterRoom->second->iPeopleIN--;

	if (iterRoom->second->iPeopleIN <= 0)
	{
		delete iterRoom->second;
		mROOM.erase(iterRoom);
	}
}

VOID RenewWaitingRoom()
{
	int iNameLen			= 0;
	char buf[BUFSIZE]		= {0,};
	LPUSERINFO lpUserInfo	= NULL;
	SOCKET sock = 0;


	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		if (iterUser->second->iRoomNumber == 0)
		{
			sock = GetSock(iterUser->second);
			send (sock, "~", sizeof(char) * 2, NULL);

			for (iterRoom = mROOM.begin(); iterRoom != mROOM.end(); iterRoom++)
			{
				sprintf(buf, iterRoom->second->cRoomName);
				iNameLen				= strlen(buf);
				buf[iNameLen + 1]	= iterRoom->second->iNum;
				buf[iNameLen + 2]	= iterRoom->second->iPeopleIN;


				send(iterSocket->second->sock, buf, strlen(buf) + 4, NULL);
			}		
		}
	}

	/*
	for (iterSocket = mSOCKET.begin(); iterSocket != mSOCKET.end(); iterSocket++)
	{
	lpUserInfo = GetUserInfo(iterSocket->second);

	if (lpUserInfo->iRoomNumber == 0)
	{
	send(iterSocket->second->sock, "~", sizeof(char) * 2, NULL);
	for (iterRoom = mROOM.begin(); iterRoom != mROOM.end(); iterRoom++)
	{
	sprintf(buf, iterRoom->second->cRoomName);
	iNameLen				= strlen(buf);
	buf[iNameLen + 1]	= iterRoom->second->iNum;
	buf[iNameLen + 2]	= iterRoom->second->iPeopleIN;


	send(iterSocket->second->sock, buf, strlen(buf) + 4, NULL);
	}
	}
	}
	*/
}