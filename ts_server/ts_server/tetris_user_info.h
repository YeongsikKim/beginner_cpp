#pragma once


//USER INFO
VOID AddUserInfo(SOCKADDR_IN * pAddrClient, SOCKET sockClient);
LPUSERINFO GetUserInfo(SOCKADDR_IN * pAddrClient);
LPUSERINFO GetUserInfo(SOCKETINFO *pSoketInfo);
SOCKET GetSock(LPUSERINFO pUserInfo);
VOID RemoveUserInfo(SOCKET hSock);
