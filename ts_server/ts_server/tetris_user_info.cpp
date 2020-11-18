#include "stdafx.h"


VOID AddUserInfo(SOCKADDR_IN * pAddrClient, SOCKET hClientSock)
{
	LPUSERINFO pUserInfo = NULL;

	for (iterUser = mUSER.begin(); iterUser != mUSER.end(); iterUser++)
	{
		if (iterUser->second->addr == pAddrClient->sin_addr.s_addr)
		{
			iterUser->second->iRoomNumber = g_iTempRoomNumber;
			iterUser->second->iStatus	= 0;
			return;
		}
	}


	pUserInfo = new USERINFO;
	ZeroMemory(pUserInfo, sizeof(USERINFO));

	pUserInfo->addr = pAddrClient->sin_addr.s_addr;
	pUserInfo->iRoomNumber	= 0;
	pUserInfo->iStatus		= 0;

	mUSER.insert(pair<int, LPUSERINFO>(hClientSock, pUserInfo));
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


VOID RemoveUserInfo(SOCKET hSock)
{
	iterUser = mUSER.find(hSock);

	if (iterUser != mUSER.end())
	{
		delete iterUser->second;
		mUSER.erase(iterUser);
	}
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

