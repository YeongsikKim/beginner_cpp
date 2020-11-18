#include "stdafx.h"

BOOL AddSocketInfo(SOCKET hSock)
{
	SOCKETINFO *pSocketInfo = new SOCKETINFO;
	ZeroMemory(pSocketInfo, sizeof(SOCKETINFO));

	pSocketInfo->sock			= hSock;
	pSocketInfo->recvbytes		= 0;
	pSocketInfo->sendbytes		= 0;
	pSocketInfo->recvdelayed	= FALSE;

	mSOCKET.insert(pair<int, SOCKETINFO*>(pSocketInfo->sock, pSocketInfo));

	LPPACKET_BODY pPacket = new PACKET_BODY;
	ZeroMemory(pPacket, sizeof(PACKET_BODY));

	mPACKET.insert(pair<SOCKET, LPPACKET_BODY>(hSock, pPacket));
	return TRUE;
}


SOCKETINFO *GetSocketInfo(SOCKET hSock)
{
	SOCKETINFO *pSocketInfo = NULL;

	iterSocket = mSOCKET.find(hSock);

	if ( iterSocket != mSOCKET.end() )
	{
		pSocketInfo = iterSocket->second;
	}

	return pSocketInfo;
}


VOID RemoveSocketInfo(SOCKET hSock)
{
	LPUSERINFO	pUserInfo	= NULL;
	SOCKADDR_IN	tAddrClient = {0,};
	int			iAddrlen	= sizeof(tAddrClient);
	int			iRoomNumber = 0;

	getpeername(hSock, (SOCKADDR*)&tAddrClient, &iAddrlen);
	printf("[TCP Server] Client Quit: IP Address = %s, Port Number = %d\n", inet_ntoa(tAddrClient.sin_addr), ntohs(tAddrClient.sin_port));


	iterSocket = mSOCKET.find(hSock);

	if ( iterSocket != mSOCKET.end() )
	{
		delete iterSocket->second;
		mSOCKET.erase(iterSocket);
	}

	delete mPACKET[hSock];
	iterPacket = mPACKET.find(hSock);

	if ( iterPacket != mPACKET.end() )
	{
		mPACKET.erase(iterPacket);
	}

	closesocket(hSock);
}


VOID SocketAcceptFunction(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int			iRetval				= 0;
	int			iAddrlen			= 0;
	SOCKETINFO *pClientSokcetInfo	= NULL;
	SOCKADDR_IN addrClient			= {0,};
	SOCKET		hClientSock			= 0;

	iAddrlen		= sizeof(addrClient);
	hClientSock		= accept(wParam, (SOCKADDR*)&addrClient, &iAddrlen);

	if ( hClientSock == INVALID_SOCKET )
	{
		if ( WSAGetLastError() != WSAEWOULDBLOCK )
		{
			err_display("accept()");
		}
		return;
	}

	printf("[TCP Server] Client Accept: IP address = %s Port Number = %d\n", inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));
	AddSocketInfo(hClientSock);
	AddUserInfo(&addrClient, hClientSock);

	iRetval = WSAAsyncSelect(hClientSock, hWnd, WM_SOCKET, FD_READ|FD_WRITE|FD_CLOSE);

	if ( iRetval == SOCKET_ERROR )
	{
		err_display("WSAAsyncSelect()");
		RemoveSocketInfo(hClientSock);
	}
}


VOID SocketReadFunction(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	
	LPUSERINFO		pUserInfo			= NULL;
	SOCKET			hClientSock			= (SOCKET)wParam;
	SOCKETINFO		*pClientSocketInfo	= NULL;
	int				iMaxLen				= BUFSIZE;
	int				iRecvLen			= 0;
	int				iBodySize			= 0;
	int				iRetval				= 0;
	LPSTR			pBody				= NULL;
	LPPACKET_BODY	pPacket				= NULL;
	PACKET_HEADER	hHeader				= {0,};


	pPacket = mPACKET[hClientSock];
	ZeroMemory(pPacket, sizeof(PACKET_BODY));
	pClientSocketInfo = GetSocketInfo(wParam);

	while ( pPacket->iCurRecv < iMaxLen )
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

		if ( pPacket->iCurRecv >= sizeof(PACKET_HEADER) )
		{
			memcpy(&hHeader, pPacket->cData, sizeof(PACKET_HEADER));
			iMaxLen = hHeader.iSize;
		}


		if ( pPacket->iCurRecv >= hHeader.iSize )
		{
			break;
		}
	}

	pBody = (LPSTR)(pPacket->cData + sizeof(PACKET_HEADER));
	iBodySize = hHeader.iSize - sizeof(PACKET_HEADER);

	switch ( hHeader.iFlag )
	{
	case WSABUFFER_ROOMNAME:
		{
			iRetval = CreateRoomInfo(pBody);

			if ( iRetval == FALSE )
			{
				printf("[Error] CreateRoom Error()\n");
			}

			pUserInfo = GetUserInfo(pClientSocketInfo);
			pUserInfo->iRoomNumber = g_iTempRoomNumber;

			RenewWaitingRoom();
		}
		break;

	case WSABUFFER_CHATTING:
		{
			SendingChatting(pClientSocketInfo, pBody);
		}
		break;

	case WSABUFFER_IMAGE:
		{
			SendingImage(hClientSock, pBody, iBodySize);
		}
		break;

	case WSABUFFER_RENEW:
		{
			RenewWaitingRoom();
		}
		break;

	case WSABUFFER_JOIN:
		{
			JoinInTheRoom(pClientSocketInfo, pBody);
		}
		break;

	case WSABUFFER_QUITROOM:
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
			RemoveUserInfo(hClientSock);
			RenewWaitingRoom();
		}
		break;

	case WSABUFFER_READY:
		{
			ReadyStatus(hClientSock, pClientSocketInfo);
		}
		break;

	case WSABUFFER_NOTREADY:
		{
			NotReadyStatus(pClientSocketInfo);
		}
		break;

	case WSABUFFER_END:
		{
			GameIsOver(pClientSocketInfo);
		}
		break;
	}
}

BOOL CreateRoomInfo(char *pBuf)
{
	LPROOMINFO pRoomInfo	= NULL;


	pRoomInfo = new ROOMINFO;
	ZeroMemory(pRoomInfo, sizeof(ROOMINFO));

	if ( pBuf == NULL )
	{
		return FALSE;
	}

	g_iRoomIDX++;

	pRoomInfo->iNum	= g_iRoomIDX;
	strcpy_s(pRoomInfo->cRoomName, strlen(pBuf) + 1, pBuf);
	pRoomInfo->iPeopleIN = 1;

	mROOM.insert(pair<int, ROOMINFO*>(g_iRoomIDX, pRoomInfo));
	printf("[Server] Successfully Create Room!\n");

	g_iTempRoomNumber = g_iRoomIDX;
	return TRUE;
}

VOID SendingChatting(SOCKETINFO *pSocketInfo, LPSTR pBuf)
{
	int			iSendLen		= 0;
	int			iSendTot		= 0;
	SOCKET		hSock			= 0;
	DWORD		dwRespBufSize	= 0;
	PBYTE		pRespBuf		= NULL;
	LPSTR		pBody			= NULL;
	LPUSERINFO	pUserInfo		= NULL;

	dwRespBufSize = sizeof(PACKET_HEADER) + strlen(pBuf) + 1;
	pRespBuf = new BYTE[dwRespBufSize];
	ZeroMemory(pRespBuf, dwRespBufSize);
	LPPACKET_HEADER pHeader = (LPPACKET_HEADER)pRespBuf;
	pBody = (LPSTR)pRespBuf + sizeof(PACKET_HEADER);

	pUserInfo = GetUserInfo(pSocketInfo);

	//Setting header
	pHeader->iFlag = WSABUFFER_CHATTING;
	pHeader->iSize = sizeof(PACKET_HEADER) + strlen(pBuf);

	//Setting Body
	memcpy(pBody, pBuf, strlen(pBuf));

	//Sending
	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		//Sending the message only who has same number
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
	}

	delete [] pRespBuf;
	pRespBuf = NULL;
}

VOID SendingImage(SOCKET hSock, LPSTR pBuf, int iBufSize)
{
	int		iSendLen		= 0;
	int		iSendTot		= 0;
	SOCKET	hOtherSock		= 0;
	DWORD	dwRespBufSize	= 0;
	PBYTE	pRespBuf		= NULL;
	LPSTR	pBody			= NULL;

	dwRespBufSize = sizeof(PACKET_HEADER) + iBufSize + 1;
	pRespBuf = new BYTE[dwRespBufSize];
	ZeroMemory(pRespBuf, dwRespBufSize);
	LPPACKET_HEADER pHeader = (LPPACKET_HEADER)pRespBuf;
	pBody = (LPSTR)pRespBuf + sizeof(PACKET_HEADER);

	//Setting Header
	pHeader->iFlag = WSABUFFER_IMAGE;
	pHeader->iSize = sizeof(PACKET_HEADER) + iBufSize;

	//Setting body
	memcpy(pBody, pBuf, iBufSize);


	//Sending image only who has same Room Number
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
			printf("Image Send(%lu), ToTal(%d), Cur(%d), SendLen(%d)(%d)\nSock(%X)\n", (iterUser->second->addr), pHeader->iSize, iSendTot, pHeader->iSize - iSendTot, iSendLen, hOtherSock);
			if (iSendLen == SOCKET_ERROR)
			{
				break;
			}
			iSendTot += iSendLen;
		} while ( pHeader->iSize != iSendTot );
	}
}


VOID RenewWaitingRoom()
{
	int				iSizeOfRoom		= 0;
	int				iSendByte		= 0;
	int				iSendTot		= 0;
	SOCKET			hSock			= 0;
	LPPACKET_HEADER pHeader			= NULL;
	DWORD			dwRespBufSize	= 0;
	PBYTE			pRespBuf		= NULL;
	PBYTE			pBody			= NULL;
	ROOMINFO*		pRoomInfo		= NULL;

	dwRespBufSize = sizeof(PACKET_HEADER) + (sizeof(ROOMINFO) * mROOM.size()) + 1;
	pRespBuf = new BYTE[dwRespBufSize];
	ZeroMemory(pRespBuf, dwRespBufSize);
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
		} while ( pHeader->iSize != iSendTot );		
	}
	delete [] pRespBuf;
	pRespBuf = NULL;
}



VOID JoinInTheRoom(SOCKETINFO* pClientSocketInfo, LPSTR pBody)
{
	LPPACKET_HEADER		pHeader		= NULL;
	LPUSERINFO			pUserInfo	= NULL;

	pHeader = new PACKET_HEADER;
	ZeroMemory(pHeader, sizeof(PACKET_HEADER));
	memcpy(&g_iTempRoomNumber, pBody, sizeof(int));

	iterRoom = mROOM.find(g_iTempRoomNumber);
	if ( iterRoom != mROOM.end() )
	{
		if ( iterRoom->second->iPeopleIN < MAX_PEOPLE || iterRoom->second->iPeopleIN > 0)
		{
			iterRoom->second->iPeopleIN++;
			pHeader->iFlag = WSABUFFER_NOTFULL;
			send(pClientSocketInfo->sock, (LPSTR)pHeader, sizeof(PACKET_HEADER), NULL);
			RenewWaitingRoom();
		}
		else if ( iterRoom->second->iPeopleIN >= MAX_PEOPLE )
		{					
			printf("[Server] The number of people in Room was full\n");
			pHeader->iFlag = WSABUFFER_FULL;
			send(pClientSocketInfo->sock, (LPSTR)pHeader, sizeof(PACKET_HEADER), NULL);
			RenewWaitingRoom();
		}
		else if ( iterRoom->second->iPeopleIN <= 0 )
		{
			printf("[Server] Empty Room, Wrong access\n");
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
	int				iSendLen		= 0;
	int				iSendTot		= 0;
	int				iAddrLen		= 0;
	SOCKET			hOthetSocket	= 0;
	LPUSERINFO		pClientInfo		= NULL;
	DWORD			dwRespBufSize	= 0;
	PBYTE			pRespBuf		= NULL;
	LPPACKET_HEADER	pHeader			= NULL;
	SOCKADDR_IN		tAddrClient		= {0,};
	

	dwRespBufSize = sizeof(PACKET_HEADER) + 1;
	pRespBuf = new BYTE[dwRespBufSize];
	ZeroMemory(pRespBuf, dwRespBufSize);
	pHeader = (LPPACKET_HEADER)pRespBuf;

	pClientInfo = GetUserInfo(pClientSockInfo);
	
	pClientInfo->iStatus = READY;

	iAddrLen = sizeof(tAddrClient);
	getpeername(hClientSock, (SOCKADDR*)&tAddrClient, &iAddrLen);
	printf("[Server] %s is Ready\n", inet_ntoa(tAddrClient.sin_addr));

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
		} while ( pHeader->iSize != iSendTot );
		
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
		} while ( pHeader->iSize != iSendTot );
	}
}

VOID NotReadyStatus(LPSOCKETINFO pClientSockInfo)
{
	int			iAddrLen	= 0;
	SOCKET		hClientSock = 0;
	LPUSERINFO	pUserinfo	= NULL;
	SOCKADDR_IN tAddrClient = {0,};

	pUserinfo = GetUserInfo(pClientSockInfo);
	pUserinfo->iStatus = NOTREADY;

	hClientSock = pClientSockInfo->sock;
	iAddrLen = sizeof(SOCKADDR_IN);
	getpeername(hClientSock, (SOCKADDR*)&tAddrClient, &iAddrLen);

	printf("[Server] %s is Not Ready\n", inet_ntoa(tAddrClient.sin_addr));
}

VOID GameIsOver(LPSOCKETINFO pClientSocketInfo)
{
	int				iSendLen		= 0;
	int				iSendTot		= 0;
	SOCKET			hClientSocket	= 0;
	SOCKET			hOtherSock		= 0;
	LPUSERINFO		pClientUserInfo = NULL;
	LPPACKET_HEADER pHeader			= NULL;
	DWORD			dwRespBufSize	= 0;
	PBYTE			pRespBuf		= NULL;

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
	ZeroMemory(pRespBuf, dwRespBufSize);
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
	} while ( pHeader->iSize != iSendTot );
}