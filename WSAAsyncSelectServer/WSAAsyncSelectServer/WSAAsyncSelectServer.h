#pragma once


	//About Windows Message
LRESULT CALLBACK WndProc(HWND, UINT,WPARAM, LPARAM);
VOID ProcessSocketMessage(HWND, UINT, WPARAM, LPARAM);


	//Management Socket function
BOOL AddSocketInfo(SOCKET sock);
SOCKETINFO *GetSocketInfo(SOCKET sock);
VOID RemoveSocketInfo(SOCKET sock);


	//Print Error
VOID err_quit(char *msg);
VOID err_display(char *msg);
VOID err_display(int errcode);


	//USER INFO
VOID AddUserInfo(SOCKADDR_IN * pAddrClient, SOCKET sockClient);
LPUSERINFO GetUserInfo(SOCKADDR_IN * pAddrClient);
LPUSERINFO GetUserInfo(SOCKETINFO *pSoketInfo);
SOCKET GetSock(LPUSERINFO pUserInfo);
VOID DeletePeople(int iRoomNumber);
VOID RenewWaitingRoom();