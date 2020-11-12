// WSAAsyncSelectServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
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
		SocketReadFunction(hWnd, wParam, lParam);
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

	LPPACKET_BODY pPacket = new PACKET_BODY;
	ZeroMemory(pPacket, sizeof(PACKET_BODY));
	
	mPACKET.insert(pair<SOCKET, LPPACKET_BODY>(sock, pPacket));
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

	delete mPACKET[sock];
	iterPacket = mPACKET.find(sock);
	if (iterPacket != mPACKET.end())
	{
		mPACKET.erase(iterPacket);
	}
	
	
	closesocket(sock);
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
	SOCKET sock = 0;
	PBYTE pRespBuf = NULL;
	LPPACKET_HEADER pHeader = NULL;
	PBYTE pBody = NULL;
	ROOMINFO* pRoomInfo = NULL;
	int iSizeOfRoom = 0;
	int iSendByte = 0;
	int iSendTot = 0;

#if 1
	DWORD dwRespBufSize = sizeof(PACKET_HEADER) + (sizeof(ROOMINFO) * mROOM.size()) + 1;

	pRespBuf = new BYTE[dwRespBufSize];
	pHeader = (LPPACKET_HEADER) pRespBuf;

	pHeader->iFlag = WSABUFFER_RENEW;
	pHeader->iSize = mROOM.size() * sizeof(ROOMINFO);

	for (iterRoom = mROOM.begin(); iterRoom != mROOM.end(); iterRoom++)
	{
		pRoomInfo = (ROOMINFO*)(pRespBuf + sizeof(PACKET_HEADER) + iSizeOfRoom);
		memcpy(pRoomInfo, iterRoom->second, sizeof(ROOMINFO));
		iSizeOfRoom += sizeof(ROOMINFO);
	}

	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		sock = GetSock(iterUser->second);
		iSendTot = 0;
		do 
		{
			iSendByte = send(sock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
			if (iSendByte == SOCKET_ERROR)
			{
				printf("send()\n");
				break;
			}
			iSendTot += iSendByte;
		} while (pHeader->iSize != iSendTot);		
	}
	delete [] pRespBuf;
	pRespBuf = NULL;
#endif
#if 0
/////////////////////////////////////////////////////////////////////////////////////////////////////////
	DWORD dwRespBufSize = sizeof(PACKET_HEADER) + sizeof(ROOMINFO) + 1;
	for (iterRoom = mROOM.begin(); iterRoom != mROOM.end(); iterRoom++)
	{
		pRespBuf = new BYTE[dwRespBufSize];
		pHeader = (LPPACKET_HEADER) pRespBuf;
		pBody = pRespBuf + sizeof(PACKET_HEADER);

		pHeader->iFlag = WSABUFFER_RENEW;
		pHeader->iSize = sizeof(ROOMINFO) + sizeof(PACKET_HEADER);

		pRoomInfo = (ROOMINFO*)pBody;
		memcpy(pRoomInfo, iterRoom->second, sizeof(ROOMINFO));
		
		for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
		{
			sock = GetSock(iterUser->second);
			send(sock, (LPSTR)pHeader, pHeader->iSize, NULL);
		}

		delete [] pRespBuf;
		pRespBuf = NULL;
	}
#endif
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
	SOCKET hOtherSock = 0;
	SOCKETINFO *pClientSocketInfo = NULL;
	int iMaxLen = BUFSIZE;
	int iRecvLen = 0;
	int iBodySize = 0;
	int iSendLen = 0;
	int iSendTot = 0;
	LPSTR pBody = NULL;

	LPPACKET_BODY pPacket = NULL;
	LPPACKET_HEADER pHeader = NULL;
	
	pPacket = mPACKET[hClientSock];
	ZeroMemory(pPacket, sizeof(PACKET_BODY));
	

	LPPACKET_BODY pSendPacket = NULL;
	LPPACKET_HEADER pSendHeader = NULL;
	
	pSendPacket = mPACKET[hClientSock];

	pClientSocketInfo = GetSocketInfo(wParam);

	while (pPacket->iCurRecv < iMaxLen)
	{
		iRecvLen = recv(hClientSock, (LPSTR)(pPacket->cData + pPacket->iCurRecv), (iMaxLen - pPacket->iCurRecv), NULL);
		if (iRecvLen == SOCKET_ERROR)
		{
			DWORD gle = WSAGetLastError();
			if (gle == WSAEWOULDBLOCK)
			{
				printf("Can't Complete Immediately\n");
				//RemoveSocketInfo(hClientSock);
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
				pHeader = (LPPACKET_HEADER) pPacket->cData;
				iMaxLen = pHeader->iSize;
			}
		}
	}

	pBody = (LPSTR)(pPacket->cData + sizeof(PACKET_HEADER));
	iBodySize = pHeader->iSize - sizeof(PACKET_HEADER);

	switch (pHeader->iFlag)
	{
	case WSABUFFER_ROOMNAME:
		CreateRoomInfo(pBody);
		pUserInfo = GetUserInfo(pClientSocketInfo);
		pUserInfo->iRoomNumber = g_iTempRoomNumber;

		RenewWaitingRoom();
		return;

	case WSABUFFER_CHATTING:
		SendingChatting(pClientSocketInfo, pBody);
		break;

	case WSABUFFER_IMAGE:
		SendingImage(hClientSock, pBody, iBodySize);
		break;

	case WSABUFFER_RENEW:
		RenewWaitingRoom();
		return;

	case WSABUFFER_JOIN:
		JoinInTheRoom(pClientSocketInfo, pBody);
		return;

	case WSABUFFER_QUITROOM:
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
		return;

	default:
		break;
	}
}


VOID JoinInTheRoom(SOCKETINFO* pClientSocketInfo, LPSTR pBody)
{
	LPPACKET_HEADER pHeader = NULL;
	LPUSERINFO pUserInfo = NULL;

	pHeader = new PACKET_HEADER;
	ZeroMemory(pHeader, sizeof(PACKET_HEADER));

	GetUserInfo(pClientSocketInfo);
	g_iTempRoomNumber = atoi(pBody);

	iterRoom = mROOM.find(g_iTempRoomNumber);
	if (iterRoom != mROOM.end())
	{
		if (iterRoom->second->iPeopleIN < MAX_PEOPLE)
		{
			iterRoom->second->iPeopleIN++;
			pHeader->iFlag = WSABUFFER_NOTFULL;
			send(pClientSocketInfo->sock, (LPSTR)pHeader, sizeof(PACKET_HEADER), NULL);
			RenewWaitingRoom();
		}
		else
		{					
			printf("The number of people in Room was full\n");
			pHeader->iFlag = WSABUFFER_FULL;
			send(pClientSocketInfo->sock, (LPSTR)pHeader, sizeof(PACKET_HEADER), NULL);
			RenewWaitingRoom();
		}
	}
	pUserInfo = GetUserInfo(pClientSocketInfo);
	pUserInfo->iRoomNumber = g_iTempRoomNumber;
	
	delete pHeader;
	pHeader = NULL;
}


BOOL CreateRoomInfo(char * buf)
{

	ROOMINFO *RoomInfo	= NULL;
	RoomInfo			= new ROOMINFO;
	ZeroMemory(RoomInfo, sizeof(ROOMINFO));

	if (buf == NULL)
	{
		return FALSE;
	}

	g_iRoomIDX++;

	RoomInfo->iNum	= g_iRoomIDX;
	strcpy_s(RoomInfo->cRoomName, strlen(buf) + 1, buf);
	RoomInfo->iPeopleIN = 1;

	mROOM.insert(pair<int, ROOMINFO*>(g_iRoomIDX, RoomInfo));
	printf("Successfully Create Room!\n");

	g_iTempRoomNumber = g_iRoomIDX;
	return TRUE;
}


VOID SendingChatting(SOCKETINFO *pSocketInfo, LPSTR pBuf)
{
	int iSendLen = 0;
	int iSendTot = 0;
	SOCKET hSock = 0;
	DWORD dwRespBufSize = sizeof(PACKET_HEADER) + strlen(pBuf) + 1;
	PBYTE pRespBuf = new BYTE[dwRespBufSize];
	LPSTR pBody = NULL;
	LPUSERINFO pUserInfo = NULL;

	pUserInfo = GetUserInfo(pSocketInfo);

	LPPACKET_HEADER pHeader = (LPPACKET_HEADER)pRespBuf;
	pBody = (LPSTR)pRespBuf + sizeof(PACKET_HEADER);
	
	//Setting header
	pHeader->iFlag = WSABUFFER_CHATTING;
	pHeader->iSize = sizeof(PACKET_HEADER) + strlen(pBuf);

	//Setting Body
	strcpy(pBody, pBuf);

	//Sending
	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
#if 1
		if (pUserInfo->iRoomNumber == iterUser->second->iRoomNumber)
		{
			iSendTot = 0;
			hSock = GetSock(iterUser->second);
			do 
			{
				iSendLen = send(hSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
				if (iSendLen == SOCKET_ERROR)
				{
					printf("send()\n");
					break;
				}
				iSendTot += iSendLen;
			} while (pHeader->iSize != iSendTot);
		}
#endif
#if 0
		iSendTot = 0;
		hSock = GetSock(iterUser->second);
		do 
		{
			iSendLen = send(hSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
			if (iSendLen == SOCKET_ERROR)
			{
				printf("send()\n");
				break;
			}
			iSendTot += iSendLen;
		} while (pHeader->iSize != iSendTot);
#endif
	}


	delete [] pRespBuf;
	pRespBuf = NULL;
}

VOID SendingImage(SOCKET hSock, LPSTR pBuf, int iBufSize)
{
	int iSendLen = 0;
	int iSendTot = 0;
	SOCKET hOtherSock = 0;
	DWORD dwRespBufSize = sizeof(PACKET_HEADER) + iBufSize + 1;
	PBYTE pRespBuf = new BYTE[dwRespBufSize];
	LPSTR pBody = NULL;


	LPPACKET_HEADER pHeader = (LPPACKET_HEADER)pRespBuf;
	pBody = (LPSTR)pRespBuf + sizeof(PACKET_HEADER);

	//Setting Header
	pHeader->iFlag = WSABUFFER_IMAGE;
	pHeader->iSize = sizeof(PACKET_HEADER) + iBufSize;

	//Setting body
	strcpy(pBody, pBuf);

	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		hOtherSock = GetSock(iterUser->second);
		if (hSock == hOtherSock)
		{
			continue;
		}
		iSendTot = 0;
		do 
		{
			iSendLen = send(hOtherSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
			if (iSendLen == SOCKET_ERROR)
			{
				err_display("send()");
			}
			iSendTot += iSendLen;
		} while (pHeader->iSize != iSendTot);
	}
	
}