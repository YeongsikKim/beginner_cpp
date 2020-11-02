// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// WSAAsyncSelectServer.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"


map<int, SOCKETINFO*> mSOCKET;
map<int, SOCKETINFO*>::iterator iterSocket;
map<int, ROOMINFO*> mROOM;
map<int, ROOMINFO*>::iterator iterRoom;
map<int, LPUSERINFO> mUSER;
map<int, LPUSERINFO>::iterator iterUser;

int g_iTempRoomNumber;
int g_iRoomIDX;