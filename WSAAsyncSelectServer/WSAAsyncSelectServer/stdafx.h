// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <WinSock2.h>
#include <Windows.h>
#include <map>


#define BUFSIZE		512
#define WM_SOCKET	(WM_USER+1)

using namespace std;

struct SOCKETINFO
{
	SOCKET sock;
	char buf[BUFSIZE+1];
	int recvbytes;
	int sendbytes;
	int iRoomNumber;
	BOOL recvdelayed;
};


struct _RoomInfo
{
	int iNum;
	int iPeopleIN;
	char cRoomName[BUFSIZE];
};


//Map
extern map<INT, SOCKETINFO*> socket_map;
extern map<INT, SOCKETINFO*>::iterator it;
extern map<int, _RoomInfo*> Room_map;
extern map<int, _RoomInfo*>::iterator roomiter;


#include "aboutRoom.h"


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
