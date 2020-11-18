#pragma once

//Management Socket function
BOOL AddSocketInfo(SOCKET sock);
SOCKETINFO *GetSocketInfo(SOCKET sock);
int RemoveSocketInfo(SOCKET hSock);
int SocketAcceptFunction(HWND hWnd, WPARAM wParam, LPARAM lParam);
VOID SocketReadFunction(HWND hWnd, WPARAM wParam, LPARAM lParam);


//SocketReadFunction
BOOL CreateRoomInfo(char * buf);
VOID SendingChatting(SOCKETINFO *pSocketInfo, LPSTR pBuf);
VOID SendingImage(SOCKET hSock, PBYTE pBuf, int iBufSize);
VOID RenewWaitingRoom();
VOID JoinInTheRoom(SOCKETINFO* pClientSocketInfo, LPSTR pBody);
VOID ReadyStatus(SOCKET hClientSock, LPSOCKETINFO pClientSockInfo);
VOID NotReadyStatus(LPSOCKETINFO pClientSockInfo);
VOID GameIsOver(LPSOCKETINFO pClientSocketInfo);
VOID SendFunction(SOCKET hSock, int iFlag, int iTotSize, LPVOID pBodyBuf);