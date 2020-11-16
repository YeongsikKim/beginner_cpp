#pragma once

//Management Socket function
BOOL AddSocketInfo(SOCKET sock);
SOCKETINFO *GetSocketInfo(SOCKET sock);
VOID RemoveSocketInfo(SOCKET sock);
VOID SocketAcceptFunction(HWND hWnd, WPARAM wParam, LPARAM lParam);
VOID SocketReadFunction(HWND hWnd, WPARAM wParam, LPARAM lParam);


//SocketReadFunction
BOOL CreateRoomInfo(char * buf);
VOID SendingChatting(SOCKETINFO *pSocketInfo, LPSTR pBuf);
VOID SendingImage(SOCKET hSock, LPSTR pBuf, int iBufSize);
VOID RenewWaitingRoom();
VOID JoinInTheRoom(SOCKETINFO* pClientSocketInfo, LPSTR pBody);
VOID ReadyStatus(SOCKET hClientSock, LPSOCKETINFO pClientSockInfo);
VOID NotReadyStatus(LPSOCKETINFO pClientSockInfo);
VOID GameIsOver(LPSOCKETINFO pClientSocketInfo);
VOID SendFunction(SOCKET hSock, int iFlag, int iTotSize, int iBodySize, LPVOID pBodyBuf);