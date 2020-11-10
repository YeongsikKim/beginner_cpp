// WSAAsyncSelectServer.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

int iSize = 0;
int iStatusREAD = NOTBMP;
LPVOID lpBody	= NULL;

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
	char cTempbuf[10]	= {0,};
	LPUSERINFO pUserInfo	= NULL;
	SOCKETINFO *pClientSocketInfo			= NULL;
	SOCKET hClientSock		= 0;
	SOCKET hOtherSock		= 0;
	SOCKADDR_IN addrClient	= {0,};
	int iAddrlen			= 0;
	int iRetval				= 0;
	int iNameLen			= 0;


	switch (WSAGETSELECTEVENT (lParam))
	{
	case FD_ACCEPT:
		SocketAcceptFunction(hWnd, wParam, lParam);
		break;

	case FD_READ:
		pClientSocketInfo = GetSocketInfo(wParam);
		//Receive Data



		if (iStatusREAD == YESBMP)
		{
			iRetval = recv(pClientSocketInfo->sock, (LPSTR)lpBody, iSize, 0);
			pClientSocketInfo->recvbytes = iRetval;

			if (iRetval == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					err_display("recv()");
					RemoveSocketInfo(wParam);
				}
			}

			iAddrlen			= sizeof(addrClient);
			getpeername(wParam, (SOCKADDR*)&addrClient, &iAddrlen);

			pUserInfo = GetUserInfo(&addrClient);

			send(pClientSocketInfo->sock, (LPSTR)lpBody, pClientSocketInfo->recvbytes, NULL);

			if (mSOCKET.begin() == mSOCKET.end()) {;}
			else
			{
				for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
				{
					if (addrClient.sin_addr.s_addr == iterUser->second->addr){;}

					else if (pUserInfo->iRoomNumber == iterUser->second->iRoomNumber && iterUser->second->iRoomNumber != 0) //NOT Waiting room
					{
						hOtherSock = GetSock(iterUser->second);

						if ( hOtherSock )
						{							
							iRetval		= send(hOtherSock, (LPSTR)lpBody, pClientSocketInfo->recvbytes, 0);
							if (iRetval == SOCKET_ERROR)
							{
								if (WSAGetLastError() != WSAEWOULDBLOCK)
								{
									err_display("send()");
								}
								return;
							}
							printf("Sending BMP Body\n");
						}
						else
						{
							printf("bmp sockanother()\n");
						}
					}
				}
			}


			pClientSocketInfo->sendbytes += iRetval;
			if (pClientSocketInfo->recvbytes == pClientSocketInfo->sendbytes)
			{
				pClientSocketInfo->recvbytes = pClientSocketInfo->sendbytes = 0;
				free(lpBody);
				iStatusREAD = NOTBMP;
			}
		}
		else if (iStatusREAD == NOTBMP)
		{
			ZeroMemory(pClientSocketInfo->buf, BUFSIZE);
			iRetval	= recv(pClientSocketInfo->sock, pClientSocketInfo->buf, BUFSIZE, 0);
			iNameLen = strlen(pClientSocketInfo->buf);

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
			if (pClientSocketInfo->buf[1]	== REQUESTROOM)
			{
				for (iterRoom = mROOM.begin(); iterRoom != mROOM.end(); iterRoom++)
				{
					sprintf(pClientSocketInfo->buf, iterRoom->second->cRoomName);
					iNameLen				= strlen(pClientSocketInfo->buf);
					pClientSocketInfo->buf[iNameLen + 1]	= iterRoom->second->iNum;
					pClientSocketInfo->buf[iNameLen + 2]	= iterRoom->second->iPeopleIN;

					send(pClientSocketInfo->sock, pClientSocketInfo->buf, strlen(pClientSocketInfo->buf) + 4, NULL);
				}
			}
			//Confirm whether it is title of room or not.
			else if (pClientSocketInfo->buf[iNameLen + 1] == ROOMTITLE)
			{
				ZeroMemory(&addrClient, sizeof(SOCKADDR_IN));
				iAddrlen = sizeof(addrClient);

				pClientSocketInfo->buf[iNameLen + 1] = '\0';
				getpeername(pClientSocketInfo->sock, (SOCKADDR*)&addrClient, &iAddrlen);
				CreateRoomInfo(pClientSocketInfo->buf);

				pUserInfo = GetUserInfo(&addrClient);
				pUserInfo->iRoomNumber = g_iTempRoomNumber;

				RenewWaitingRoom();
			}
			//Join the room
			else if (pClientSocketInfo->buf[2] == ENTRANCE)		
			{
				g_iTempRoomNumber = pClientSocketInfo->buf[0];

				iterRoom = mROOM.find(g_iTempRoomNumber);
				if (iterRoom != mROOM.end())
				{
					if (iterRoom->second->iPeopleIN < MAX_PEOPLE)
					{
						iterRoom->second->iPeopleIN++;
						send(pClientSocketInfo->sock, "/i", 3, NULL);
						RenewWaitingRoom();
					}							
					else
					{					
						printf("The number of people in Room was full\n");
						send(pClientSocketInfo->sock, "/f", 3, NULL);
						break;
					}
				}
				pUserInfo = GetUserInfo(pClientSocketInfo);
				pUserInfo->iRoomNumber = g_iTempRoomNumber;
			}
			//Quit the room
			else if (pClientSocketInfo->buf[0] == '/' && pClientSocketInfo->buf[1] == 'e')
			{
				pUserInfo = GetUserInfo(pClientSocketInfo);

				iterRoom = mROOM.find(pUserInfo->iRoomNumber);
				if (iterRoom != mROOM.end())
				{
					iterRoom->second->iPeopleIN--;
					if (iterRoom->second->iPeopleIN <= 0)
					{
						delete iterRoom->second;
						mROOM.erase(iterRoom);
					}
				}
			}
			//about Chatting
			else if (pClientSocketInfo->buf[strlen(pClientSocketInfo->buf) + 1] == '/' && pClientSocketInfo->buf[strlen(pClientSocketInfo->buf) + 2] == 'c')
			{
				if (pClientSocketInfo->recvbytes > 0)
				{
					pClientSocketInfo->recvdelayed = TRUE;
					return;
				}

				pClientSocketInfo->recvbytes		= iRetval;


				iAddrlen			= sizeof(addrClient);
				getpeername(wParam, (SOCKADDR*)&addrClient, &iAddrlen);

				pUserInfo	= GetUserInfo(&addrClient);

				if ( pUserInfo == NULL )
				{
					err_quit("GetUserInfo()");
				}

				printf("[TCP/%s:%d] %s\n", inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port), pClientSocketInfo->buf);


				if (pClientSocketInfo->recvbytes <= pClientSocketInfo->sendbytes) return;

				//Sending Data
				iRetval		= send(pClientSocketInfo->sock, pClientSocketInfo->buf + pClientSocketInfo->sendbytes, pClientSocketInfo->recvbytes - pClientSocketInfo->sendbytes, 0);
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
						if (addrClient.sin_addr.s_addr == iterUser->second->addr){;}

						else if (pUserInfo->iRoomNumber == iterUser->second->iRoomNumber && iterUser->second->iRoomNumber != 0) //NOT Waiting room
						{
							hOtherSock = GetSock(iterUser->second);

							if ( hOtherSock )
							{
								pClientSocketInfo->buf[strlen(pClientSocketInfo->buf) + 1] = '/';
								pClientSocketInfo->buf[strlen(pClientSocketInfo->buf) + 2] = 'c';
								iRetval		= send(hOtherSock, pClientSocketInfo->buf + pClientSocketInfo->sendbytes, pClientSocketInfo->recvbytes - pClientSocketInfo->sendbytes, 0);
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
								printf("chatting sockanother()\n");
							}
						}

					}
				}
				//Check Bytes
				pClientSocketInfo->sendbytes += iRetval;
				if (pClientSocketInfo->recvbytes == pClientSocketInfo->sendbytes)
				{
					pClientSocketInfo->recvbytes = pClientSocketInfo->sendbytes = 0;
					if (pClientSocketInfo->recvdelayed)
					{
						pClientSocketInfo->recvdelayed = FALSE;
						PostMessage(hWnd, WM_SOCKET, wParam, FD_READ);
					}
				}
			}
			else if (pClientSocketInfo->buf[strlen(pClientSocketInfo->buf) + 1] == '/' && pClientSocketInfo->buf[strlen(pClientSocketInfo->buf) + 2] == 's')
			{
				iAddrlen			= sizeof(addrClient);
				getpeername(wParam, (SOCKADDR*)&addrClient, &iAddrlen);
				pUserInfo = GetUserInfo(&addrClient);

				pClientSocketInfo->buf[strlen(pClientSocketInfo->buf) + 1] = '\0';
				pClientSocketInfo->buf[strlen(pClientSocketInfo->buf) + 2] = '\0';

				iSize = atoi(pClientSocketInfo->buf);
				lpBody = malloc(iSize);

				pClientSocketInfo->buf[strlen(pClientSocketInfo->buf) + 1] = '/';
				pClientSocketInfo->buf[strlen(pClientSocketInfo->buf) + 2] = 'p';
				send(pClientSocketInfo->sock, pClientSocketInfo->buf, strlen(pClientSocketInfo->buf) + 3, NULL);


				for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
				{
					if (pUserInfo->iRoomNumber == iterUser->second->iRoomNumber && iterUser->second->iRoomNumber != 0)
					{
						if (pUserInfo->addr != iterUser->second->addr)
						{					
							hOtherSock = GetSock(iterUser->second);

							if (hOtherSock)
							{
								pClientSocketInfo->buf[strlen(pClientSocketInfo->buf) + 1] = '/';
								pClientSocketInfo->buf[strlen(pClientSocketInfo->buf) + 2] = 's';
								iRetval = send(hOtherSock, pClientSocketInfo->buf, strlen(pClientSocketInfo->buf) + 3, NULL);
								if (iRetval == SOCKET_ERROR)
								{
									if (WSAGetLastError() != WSAEWOULDBLOCK)
									{
										err_display("send()");
									}
									return;
								}
								printf("Send bmp Size\n");
							}
						}
					}
				}
				iStatusREAD = YESBMP;
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
	SOCKETINFO *ptr		= new SOCKETINFO;
	ZeroMemory(ptr, sizeof(SOCKETINFO));

	ptr->sock		= sock;
	ptr->recvbytes	= 0;
	ptr->sendbytes	= 0;
	ptr->recvdelayed = FALSE;

	mSOCKET.insert(pair<int, SOCKETINFO*>(ptr->sock, ptr));

	return TRUE;
}


SOCKETINFO *GetSocketInfo(SOCKET sock)
{
	SOCKETINFO *lpSocketinfo = NULL;
	
	iterSocket = mSOCKET.find(sock);
	if (iterSocket != mSOCKET.end())
	{
		lpSocketinfo = iterSocket->second;
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


	iterSocket = mSOCKET.find(sock);
	if (iterSocket != mSOCKET.end())
	{
		delete iterSocket->second;
		mSOCKET.erase(iterSocket);
	}
	
	closesocket(sock);
/*
	
	//Remove USER Info
	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		if (iterUser->second->iRoomNumber != 0)
		{
			if (iterUser->second->addr == addrClient.sin_addr.s_addr)
			{
				iRoomNumber = iterUser->second->iRoomNumber;
				delete iterUser->second;
				mUSER.erase(iterUser);
				DeletePeople(iRoomNumber);
				RenewWaitingRoom();
				break;
			}
		}
	}

	return;
	*/
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


VOID AddUserInfo(SOCKADDR_IN * pAddrClient, SOCKET sockClient)
{
	LPUSERINFO pUserInfo		= NULL;

	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		if (iterUser->second->addr == pAddrClient->sin_addr.s_addr)
		{
			iterUser->second->iRoomNumber = g_iTempRoomNumber;
			return;
		}
	}


	pUserInfo = new USERINFO;
	ZeroMemory(pUserInfo, sizeof(USERINFO));

	pUserInfo->addr = pAddrClient->sin_addr.s_addr;
	pUserInfo->iRoomNumber	= 0;
	pUserInfo->iStatus		= 0;

	mUSER.insert(pair<int, LPUSERINFO>(sockClient, pUserInfo));
}

LPUSERINFO GetUserInfo(SOCKADDR_IN * pAddrClient)
{
	LPUSERINFO pReturn = NULL;

	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		if (iterUser->second->addr == pAddrClient->sin_addr.s_addr)
		{
			pReturn = iterUser->second;
			break;
		}
	}
	return pReturn;
}



LPUSERINFO GetUserInfo(SOCKETINFO *pSoketInfo)
{
	LPUSERINFO lpUserinfo	= NULL;
	SOCKADDR_IN addrClient	= {0,};
	int iAddrlen			= 0;

	iAddrlen = sizeof(addrClient);
	getpeername(pSoketInfo->sock, (sockaddr*)&addrClient, &iAddrlen);

	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		if (iterUser->second->addr == addrClient.sin_addr.s_addr)
		{
			lpUserinfo = iterUser->second;
			break;
		}
	}

	
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

		if (pTempAddr.sin_addr.s_addr == pUserInfo->addr)
		{
			hSock = iterSocket->second->sock;
			break;
		}
	}


	return hSock;
}


VOID RenewWaitingRoom()
{
	int iNameLen			= 0;
	char buf[BUFSIZE]		= {0,};
	LPUSERINFO lpUserInfo	= NULL;
	SOCKET sock = 0;

	LPPACKET_HEADER pHeader = NULL;
	LPPACKET_BODY pPacket	= NULL;

	LPSTR pBody = NULL;

	pHeader = new PACKET_HEADER;
	pPacket = new PACKET_BODY;

	pHeader->iFlag = WSABUFFER_RENEW;
	pHeader->iSize = (mROOM.size() * sizeof(ROOMINFO));

	for (iterRoom = mROOM.begin(); iterRoom != mROOM.end(); iterRoom++)
	{
		sprintf(pBody, (LPSTR)iterRoom->second);
	}
	///////////////////////////////////////////////////////////////////////////////////�� ������ ��� Sending �ؾ���

	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		if (iterUser->second->iRoomNumber == 0)
		{
			sock = GetSock(iterUser->second);
			send (sock, (LPSTR)pBody, pHeader->iSize, NULL);
		}
	}

	delete pHeader;
	delete pPacket;
}



VOID SocketAcceptFunction(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int iRetval					= 0;
	int iAddrlen					= 0;
	SOCKETINFO *pClientSokcetInfo	= NULL;
	SOCKADDR_IN addrClient			= {0,};
	SOCKET hClientSock				= 0;

	iAddrlen		= sizeof(addrClient);
	hClientSock		= accept(wParam, (SOCKADDR*)&addrClient, &iAddrlen);
	if (hClientSock == INVALID_SOCKET)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			err_display("accept()");
		}
		return;
	}
	printf("[TCP Server] Client Accept: IP address = %s Port Number = %d\n", inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));
	AddSocketInfo(hClientSock);
	AddUserInfo(&addrClient, hClientSock);

	iRetval = WSAAsyncSelect(hClientSock, hWnd, WM_SOCKET, FD_READ|FD_WRITE|FD_CLOSE);
	if (iRetval == SOCKET_ERROR)
	{
		err_display("WSAAsyncSelect()");
		RemoveSocketInfo(hClientSock);
	}
}


VOID SocketReadFunction(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPUSERINFO pUserInfo = NULL;
	SOCKET hClientSock = (SOCKET)wParam;
	SOCKETINFO *pClientSocketInfo = NULL;
	int iMaxLen = BUFSIZE;
	int iRecvLen = 0;
	int iBodySize = 0;
	int iSendLen = 0;
	int iSendTot = 0;
	LPSTR pBody = NULL;

	LPPACKET_BODY pPacket = NULL;
	LPPACKET_HEADER pHeader = NULL;

	LPPACKET_BODY pSendPacket = NULL;
	LPPACKET_HEADER pSendHeader = NULL;

	pClientSocketInfo = GetSocketInfo(wParam);

	while (pPacket->iCurRecv < iMaxLen)
	{
		iRecvLen = recv(hClientSock, (LPSTR)(pPacket->ucData + pPacket->iCurRecv), (iMaxLen - pPacket->iCurRecv), NULL);
		if (iRecvLen == SOCKET_ERROR)
		{
			DWORD gle = WSAGetLastError();
			if (gle == WSAEWOULDBLOCK)
			{
				printf("Can't Complete Immediately\n");
				RemoveSocketInfo(hClientSock);
				continue;
			}
		}
		else if(iRecvLen == 0)
		{
			DWORD gle = WSAGetLastError();
			printf("Recv()\n");
			RemoveSocketInfo(hClientSock);
		}

		pPacket->iCurRecv += ((iRecvLen > 0) ? iRecvLen : 0);

		if (pPacket->iCurRecv >= sizeof(PACKET_HEADER))
		{
			if (!pHeader)
			{
				pHeader = (LPPACKET_HEADER) pPacket->ucData;
				iMaxLen = pHeader->iSize;
			}
		}
	}

	pBody = (LPSTR)(pPacket->ucData + sizeof(PACKET_HEADER));
	iBodySize = pHeader->iSize - sizeof(PACKET_HEADER);

	PBYTE pRespBuf = NULL;
	DWORD dwRespBufSize = 0;

	dwRespBufSize = iBodySize + sizeof(PACKET_HEADER) + 1;
	pRespBuf = new BYTE[dwRespBufSize];
	if (!pRespBuf)
	{
		printf("Not responded\n");
	}
	//Setting Header
	pSendHeader = (LPPACKET_HEADER)pRespBuf;
	PBYTE pBdy = pRespBuf + sizeof(PACKET_HEADER);
	pSendHeader->iSize = sizeof(PACKET_HEADER) + iBodySize;

	switch (pHeader->iFlag)
	{
	case WSABUFFER_ROOMNAME:
		CreateRoomInfo(pBody);
		pUserInfo = GetUserInfo(pClientSocketInfo);
		pUserInfo->iRoomNumber = g_iTempRoomNumber;

		RenewWaitingRoom();
		return;

	case WSABUFFER_CHATTING:
		pSendHeader->iFlag = WSABUFFER_CHATTING;
		break;

	case WSABUFFER_IMAGE:
		pSendHeader->iFlag = WSABUFFER_IMAGE;
		break;

	case WSABUFFER_RENEW:
		RenewWaitingRoom();
		return;

	default:
		break;
	}

	//Sending
	do 
	{
		iSendLen = send(hClientSock, (LPSTR)pHeader + iSendTot, pSendHeader->iSize - iSendTot, NULL);
	} while (pSendHeader->iSize != iSendTot);

	delete [] pRespBuf;
	pRespBuf = NULL;
}