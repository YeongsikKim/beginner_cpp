#include "stdafx.h"

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
	PACKET_HEADER hHeader = {0,};


	pPacket = mPACKET[hClientSock];
	ZeroMemory(pPacket, sizeof(PACKET_BODY));
	pClientSocketInfo = GetSocketInfo(wParam);


	while (pPacket->iCurRecv < iMaxLen)
	{
		iRecvLen = recv(hClientSock, (LPSTR)(pPacket->cData + pPacket->iCurRecv), (iMaxLen - pPacket->iCurRecv), NULL);
		if (iRecvLen == SOCKET_ERROR)
		{
			DWORD gle = WSAGetLastError();
			if (gle != WSAEWOULDBLOCK)
			{
				RemoveSocketInfo(hClientSock);
			}
		}

		pPacket->iCurRecv += ((iRecvLen > 0) ? iRecvLen : 0);

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

	pBody = (LPSTR)(pPacket->cData + sizeof(PACKET_HEADER));
	iBodySize = hHeader.iSize - sizeof(PACKET_HEADER);

	switch (hHeader.iFlag)
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
		RemoveUserInfo(hClientSock);

		printf("[Server] Successfully Clean Room\n");
		return;

	case WSABUFFER_READY:
		ReadyStatus(hClientSock, pClientSocketInfo);
		break;

	case WSABUFFER_NOTREADY:
		NotReadyStatus(pClientSocketInfo);
		break;

	case WSABUFFER_END:
		GameIsOver(pClientSocketInfo);
		break;

	default:
		break;
	}
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
	printf("[Server] Successfully Create Room!\n");

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
		//Sending the message only who has same number
#if 0
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
		//Sending the message everyone
#if 1
		iSendTot = 0;
		hSock = GetSock(iterUser->second);
		do 
		{
			iSendLen = send(hSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
			if (iSendLen == SOCKET_ERROR)
			{
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
	memcpy(pBody, pBuf, iBufSize);


	//Sending image only who has same Room Number
#if 1
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
				break;
			}
			iSendTot += iSendLen;
			printf("Image Send(%lu), ToTal(%d), Cur(%d), SendLen(%d)(%d)\nSock(%X)\n", (iterUser->second->addr), pHeader->iSize, iSendTot, pHeader->iSize - iSendTot, iSendLen, hOtherSock);
		} while (pHeader->iSize != iSendTot);
	}
#endif
	//Sending Image to me Using DEBUG
#if 0
	iSendTot = 0;
	do 
	{
		iSendLen = send(hSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);

		printf("Image Send(%X), ToTal(%d), Cur(%d), SendLen(%d)(%d)\n", hSock, pHeader->iSize, iSendTot, pHeader->iSize - iSendTot, iSendLen);
		if (iSendLen == SOCKET_ERROR)
		{
			break;
		}
		iSendTot += iSendLen;
	} while (pHeader->iSize != iSendTot);
#endif
}


VOID RenewWaitingRoom()
{
	SOCKET hSock = 0;
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
		hSock = GetSock(iterUser->second);
		iSendTot = 0;

		do 
		{
			iSendByte = send(hSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);

			if (iSendByte == SOCKET_ERROR)
			{
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



VOID JoinInTheRoom(SOCKETINFO* pClientSocketInfo, LPSTR pBody)
{
	LPPACKET_HEADER pHeader = NULL;
	LPUSERINFO pUserInfo = NULL;

	pHeader = new PACKET_HEADER;
	ZeroMemory(pHeader, sizeof(PACKET_HEADER));

	memcpy(&g_iTempRoomNumber, pBody, sizeof(int));

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


VOID ReadyStatus(SOCKET hClientSock, LPSOCKETINFO pClientSockInfo)
{
	SOCKET hOthetSocket = 0;
	LPUSERINFO pClientInfo = NULL;
	DWORD dwRespBufSize = 0;
	PBYTE pRespBuf = NULL;
	LPPACKET_HEADER pHeader = NULL;
	int iSendLen = 0;
	int iSendTot = 0;
	SOCKADDR_IN addrClient = {0,};
	int iAddrLen = 0;

	dwRespBufSize = sizeof(PACKET_HEADER) + 1;
	pRespBuf = new BYTE[dwRespBufSize];
	
	pHeader = (LPPACKET_HEADER)pRespBuf;

	pClientInfo = GetUserInfo(pClientSockInfo);
	
	pClientInfo->iStatus = READY;

	iAddrLen = sizeof(addrClient);
	getpeername(hClientSock, (SOCKADDR*)&addrClient, &iAddrLen);
	printf("%s is Ready\n", inet_ntoa(addrClient.sin_addr));

	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		if (iterUser->second == pClientInfo)
		{
			continue;
		}

		if (iterUser->second->iRoomNumber == pClientInfo->iRoomNumber)
		{
			break;
		}
	}
	if (iterUser == mUSER.end()) //There is not player
	{
		return;
	}

	if ( iterUser->second->iStatus == READY )
	{
		pHeader->iFlag = WSABUFFER_START;
		pHeader->iSize = sizeof(PACKET_HEADER);

		do
		{
			iSendLen = send(hClientSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
			if (iSendLen == SOCKET_ERROR)
			{
				err_quit("Start Send()");
			}
			
			iSendTot += iSendLen;
		} while (pHeader->iSize != iSendTot);
		
		iSendTot = 0;
		hOthetSocket = GetSock(iterUser->second);
		do
		{
			iSendLen = send(hOthetSocket, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
			if (iSendLen == SOCKET_ERROR)
			{
				err_quit("Start Send()");
			}

			iSendTot += iSendLen;
		} while (pHeader->iSize != iSendTot);
	}
}

VOID NotReadyStatus(LPSOCKETINFO pClientSockInfo)
{
	SOCKET hClientSock = 0;
	LPUSERINFO pUserinfo = NULL;
	SOCKADDR_IN addrClient = {0,};
	int iAddrLen = 0;

	pUserinfo = GetUserInfo(pClientSockInfo);
	pUserinfo->iStatus = NOTREADY;

	hClientSock = pClientSockInfo->sock;
	iAddrLen = sizeof(SOCKADDR_IN);
	getpeername(hClientSock, (SOCKADDR*)&addrClient, &iAddrLen);

	printf("%s is Not Ready\n", inet_ntoa(addrClient.sin_addr));
}

VOID GameIsOver(LPSOCKETINFO pClientSocketInfo)
{
	SOCKET hClientSocket = 0;
	SOCKET hOtherSock = 0;
	LPUSERINFO pClientUserInfo = NULL;
	LPUSERINFO pOtherUserInfo = NULL;
	
	int iSendLen = 0;
	int iSendTot = 0;
	LPPACKET_HEADER pHeader = NULL;
	DWORD dwRespBufSize = 0;
	PBYTE pRespBuf = NULL;


	hClientSocket = pClientSocketInfo->sock;
	pClientUserInfo = GetUserInfo(pClientSocketInfo);

	for ( iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++ )
	{
		if ( pClientUserInfo == iterUser->second )
		{
			continue;
		}
		
		if ( iterUser->second->iRoomNumber == pClientUserInfo->iRoomNumber )
		{
			break;
		}
	}

	if ( iterUser == mUSER.end() )
	{
		return;
	}
	
	pClientUserInfo->iStatus = NOTREADY;
	iterUser->second->iStatus = NOTREADY;

	hOtherSock = GetSock(iterUser->second);
	dwRespBufSize = sizeof(PACKET_HEADER) + 1;
	pRespBuf = new BYTE[dwRespBufSize];
	pHeader = (LPPACKET_HEADER)pRespBuf;

	pHeader->iFlag = WSABUFFER_END;
	pHeader->iSize = sizeof(PACKET_HEADER);
	do 
	{
		iSendLen = send(hOtherSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
		if (iSendLen == SOCKET_ERROR)
		{
			err_quit("End send()");
		}
		iSendTot += iSendLen;
	} while (pHeader->iSize != iSendTot);
}