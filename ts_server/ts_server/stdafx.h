#pragma once
#pragma warning(disable:4996)

#include "targetver.h"

//----------C lib
#include <stdio.h>
#include <tchar.h>
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <fstream>

//----------C++ lib
#include <map>


//----------User lib
#include "pre_define.h"
#include "ts_server.h"
#include "ts_socket.h"
#include "ts_user_info.h"
#include "err_print.h"


using namespace std;



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


