#pragma once
#pragma warning(disable:4996)

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <WinSock2.h>
#include <Windows.h>

// c++
#include <map>
using namespace std;

#include "pre_define.h"

#include "tetris_server.h"
#include "tetris_socket.h"
#include "user_info.h"

#include "err_print.h"

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


