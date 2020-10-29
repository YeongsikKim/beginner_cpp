#include "stdafx.h"

int g_iRoomIDX = 0;

BOOL CreateRoomInfo(char * buf, SOCKADDR_IN addrClient)
{
	ROOMINFO *RoomInfo	= NULL;
	RoomInfo			= new ROOMINFO;
	ZeroMemory(RoomInfo, sizeof(ROOMINFO));

	if ( buf == NULL)
	{
		return FALSE;
	}

	g_iRoomIDX++;

	RoomInfo->iNum	= g_iRoomIDX;
	strcpy_s(RoomInfo->cRoomName, strlen(buf), buf);
	RoomInfo->iPeopleIN = 1;

	Room_map.insert(pair<int, ROOMINFO*>(g_iRoomIDX, RoomInfo));
	printf("Successfully Create Room!\n");
 
	for (iterUser = mUSER.begin(); iterUser !=  mUSER.end(); iterUser++)
	{
		if (inet_ntoa(addrClient.sin_addr) == inet_ntoa(iterUser->second->addr.sin_addr))
		{
			iterUser->second->iRoomNumber = RoomInfo->iNum;
		}
	}
}
