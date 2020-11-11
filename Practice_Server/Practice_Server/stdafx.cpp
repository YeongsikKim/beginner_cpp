
#include "stdafx.h"


map<int, SOCKETINFO*> mSOCKET;
map<int, SOCKETINFO*>::iterator iterSocket;
map<int, ROOMINFO*> mROOM;
map<int, ROOMINFO*>::iterator iterRoom;
map<int, LPUSERINFO> mUSER;
map<int, LPUSERINFO>::iterator iterUser;


int g_iTempRoomNumber;
int g_iRoomIDX;