#include "stdafx.h"

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
				break;
			}
			Sleep(100);
			continue;
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


BOOL AddSocketInfo(SOCKET sock)
{
	SOCKETINFO * pSockInfo = NULL;
	
	pSockInfo = new SOCKETINFO;
	ZeroMemory(pSockInfo, sizeof(SOCKETINFO));

	pSockInfo->sock			= sock;
	pSockInfo->recvbytes	= 0;
	pSockInfo->sendbytes	= 0;
	pSockInfo->recvdelayed	= FALSE;

	mSOCKET.insert(pair<int, SOCKETINFO*>(pSockInfo->sock, pSockInfo));

	LPPACKET_BODY pPacket = new PACKET_BODY;
	ZeroMemory(pPacket, sizeof(PACKET_BODY));

	mPACKET.insert(pair<SOCKET, LPPACKET_BODY>(sock, pPacket));
	return TRUE;
}


SOCKETINFO * GetSocketInfo(SOCKET sock)
{
	SOCKETINFO *lpSocketinfo = NULL;

	iterSocket = mSOCKET.find(sock);

	if (iterSocket != mSOCKET.end())
	{
		lpSocketinfo = iterSocket->second;
	}

	return lpSocketinfo;
}


int RemoveSocketInfo(SOCKET hSock)
{
	LPUSERINFO lpUserinfo = NULL;
	SOCKADDR_IN addrClient = {0,};
	int iAddrlen	= sizeof(addrClient);
	int iRoomNumber = 0;

	getpeername(hSock, (SOCKADDR*)&addrClient, &iAddrlen);
	printf("[TCP Server] Client Quit: IP Address = %s, Port Number = %d\n", inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));


	iterSocket = mSOCKET.find(hSock);

	if (iterSocket == mSOCKET.end())
	{
		return ERR_TS_NOHAS_MAP;
	}

	delete iterSocket->second;
	mSOCKET.erase(iterSocket);

	iterPacket = mPACKET.find(hSock);
	
	if (iterPacket == mPACKET.end())
	{
		return ERR_TS_NOHAS_MAP;
	}
	
	delete iterPacket->second;
	mPACKET.erase(iterPacket);

	closesocket(hSock);
	hSock = NULL;

	return TRUE;
}

// 실패가 일어날 수 있는 함수는 무조건 리턴이 있어야 한다.
int SocketAcceptFunction(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int				iRetval				= 0;
	int				iAddrlen			= 0;
	SOCKETINFO *	pClientSokcetInfo	= NULL;
	SOCKADDR_IN		addrClient			= {0,};
	SOCKET			hClientSock				= 0;

	iAddrlen		= sizeof(addrClient);
	hClientSock		= accept(wParam, (SOCKADDR*)&addrClient, &iAddrlen);

	if (hClientSock == INVALID_SOCKET)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			err_display("accept()");
		}
		return ERR_TS_INVALID_SOCKET;
	}

	printf("[TCP Server] Client Accept: IP address = %s Port Number = %d\n", inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));
	AddSocketInfo(hClientSock);
	AddUserInfo(&addrClient, hClientSock);

	iRetval = WSAAsyncSelect(hClientSock, hWnd, WM_SOCKET, FD_READ|FD_WRITE|FD_CLOSE);
	if (iRetval == SOCKET_ERROR)
	{
		err_display("WSAAsyncSelect()");
		RemoveSocketInfo(hClientSock);
		return ERR_TS_INVALID_SOCKET;
	}
	
	return TRUE;
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
	SOCKET hSock = 0;
	LPUSERINFO pUserInfo = NULL;
	int iFlag = 0;
	int iTotSize = 0;

	iFlag = WSABUFFER_CHATTING;
	iTotSize = sizeof(PACKET_HEADER) + strlen(pBuf);

	pUserInfo = GetUserInfo(pSocketInfo);

	//Sending
	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		//Sending the message only who has same number
		if (pUserInfo->iRoomNumber == iterUser->second->iRoomNumber)
		{
			hSock = GetSock(iterUser->second);
			
			SendFunction(hSock, iFlag, iTotSize, pBuf);
		}
		//Sending the message everyone
	}
}

VOID SendingImage(SOCKET hSock, LPSTR pBuf, int iBufSize)
{
	SOCKET hOtherSock = 0;
	LPUSERINFO pUserInfo = NULL;
	LPSOCKETINFO pSocketInfo = NULL;
	int iTotSize = 0;
	int iFlag = 0;
	

	pSocketInfo = GetSocketInfo(hSock);
	pUserInfo = GetUserInfo(pSocketInfo);

	iTotSize = sizeof(PACKET_HEADER) + iBufSize;
	iFlag = WSABUFFER_IMAGE;

	//Sending image only who has same Room Number
	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		hOtherSock = GetSock(iterUser->second);
		if (hSock == hOtherSock)
		{
			continue;
		}

		if ( iterUser->second->iRoomNumber == pUserInfo->iRoomNumber )
		{
			break;
		}
	}

	if ( iterUser == mUSER.end() )
	{
		return;
	}
	SendFunction(hOtherSock, iFlag, iTotSize, pBuf);

	//Sending Image to me Using DEBUG
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
}



VOID JoinInTheRoom(SOCKETINFO* pClientSocketInfo, LPSTR pBody)
{
	DWORD dwRespBufSize = 0;
	PBYTE pRespBuf = NULL;
	LPPACKET_HEADER pHeader = NULL;
	LPUSERINFO pUserInfo = NULL;
	int iSendLen = 0;
	int iSendTot = 0;

	memcpy(&g_iTempRoomNumber, pBody, sizeof(int));

	dwRespBufSize = sizeof(PACKET_HEADER) +	1;
	pRespBuf = new BYTE[dwRespBufSize];
	
	pHeader = (LPPACKET_HEADER)pRespBuf;

	pHeader->iSize = sizeof(PACKET_HEADER);

	iterRoom = mROOM.find(g_iTempRoomNumber);
	if (iterRoom != mROOM.end())
	{
		if (iterRoom->second->iPeopleIN < MAX_PEOPLE)
		{
			iterRoom->second->iPeopleIN++;
			pHeader->iFlag = WSABUFFER_NOTFULL;
			do 
			{
				iSendLen = send(pClientSocketInfo->sock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
				if ( iSendLen == SOCKET_ERROR )
				{
					break;
				}
				iSendTot += iSendLen;
			} while (pHeader-> iSize != iSendTot);			
			RenewWaitingRoom();
		}
		else
		{					
			printf("[Server] The number of people in Room was full\n");
			pHeader->iFlag = WSABUFFER_FULL;
			do 
			{
				iSendLen = send(pClientSocketInfo->sock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
				if ( iSendLen == SOCKET_ERROR )
				{
					break;
				}
				iSendTot += iSendLen;
			} while (pHeader-> iSize != iSendTot);
			return;
		}
	}
	pUserInfo = GetUserInfo(pClientSocketInfo);
	pUserInfo->iRoomNumber = g_iTempRoomNumber;
	
	delete [] pRespBuf;
	pRespBuf = NULL;
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
				break;
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
				break;
			}

			iSendTot += iSendLen;
		} while (pHeader->iSize != iSendTot);
	}
	delete [] pRespBuf;
	pRespBuf = NULL;
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
			break;
		}
		iSendTot += iSendLen;
	} while (pHeader->iSize != iSendTot);
	
	delete [] pRespBuf;
	pRespBuf = NULL;
}



VOID SendFunction(SOCKET hSock, int iFlag, int iTotSize, LPVOID pBodyBuf)
{
	DWORD dwRespBufSize = 0;
	PBYTE pRespBuf = NULL;
	PBYTE pBody = NULL;
	LPPACKET_HEADER pHeader = NULL;
	int iSendLen = 0;
	int iSendTot = 0;
	
	dwRespBufSize = iTotSize + 1;
	pRespBuf = new BYTE[dwRespBufSize];

	pHeader = (LPPACKET_HEADER)pRespBuf;
	pBody = pRespBuf + sizeof(PACKET_HEADER);

	//Setting Header
	pHeader->iFlag = iFlag;
	pHeader->iSize = iTotSize;

	//Setting Body
	memcpy(pBody, pBodyBuf, iTotSize - sizeof(PACKET_HEADER));

	do 
	{
		iSendLen = send(hSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
		if ( iSendLen == SOCKET_ERROR )
		{
			break;
		}
		iSendTot += iSendLen;
	} while (pHeader->iSize != iSendTot);

	delete [] pRespBuf;
	pRespBuf = NULL;
}