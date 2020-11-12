#pragma once
#pragma warning(disable:4996)

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <WinSock2.h>
#include <Windows.h>
#include <map>
#include <iostream>
#include <fstream>

#include "pre_define.h"


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
	ULONG addr;
	int iRoomNumber;
	int iStatus;
}USERINFO, *LPUSERINFO;


struct ROOMINFO
{
	int iNum;
	int iPeopleIN;
	char cRoomName[ROOMNAME];
};


typedef struct _Packet_Header
{
	int iFlag;
	int iSize;
}PACKET_HEADER, *LPPACKET_HEADER;

typedef struct _Packet_Body
{
	char cData[BUFSIZE];
	int iCurRecv;
}PACKET_BODY, *LPPACKET_BODY;


//Map
extern map<int, SOCKETINFO*> mSOCKET;
extern map<int, SOCKETINFO*>::iterator iterSocket;
extern map<int, ROOMINFO*> mROOM;
extern map<int, ROOMINFO*>::iterator iterRoom;
extern map<int, LPUSERINFO> mUSER;
extern map<int, LPUSERINFO>::iterator iterUser;
extern map<SOCKET, LPPACKET_BODY> mPACKET;
extern map<SOCKET, LPPACKET_BODY>::iterator iterPacket;

//global variable
extern int g_iTempRoomNumber;
extern int g_iRoomIDX;


#include "SERVER.h"
