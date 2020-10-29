// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once
#pragma warning(disable:4996)

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
	BOOL recvdelayed;
};

typedef struct _USERINFO
{
	SOCKADDR_IN addr;
	int iRoomNumber;
	int iStatus;
}USERINFO, *LPUSERINFO;

struct ROOMINFO
{
	int iNum;
	int iPeopleIN;
	char cRoomName[BUFSIZE];
};


//Map
extern map<INT, SOCKETINFO*> socket_map;
extern map<INT, SOCKETINFO*>::iterator it;
extern map<int, ROOMINFO*> Room_map;
extern map<int, ROOMINFO*>::iterator iterRoom;
extern map<int, LPUSERINFO> mUSER;
extern map<int, LPUSERINFO>::iterator iterUser;

//global variable
extern int g_iTempRoomNumber;


#include "aboutRoom.h"
#include "WSAAsyncSelectServer.h"
