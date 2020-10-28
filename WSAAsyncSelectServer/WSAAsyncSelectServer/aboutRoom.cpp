#include "stdafx.h"



VOID CreateRoomInfo(char *buf, SOCKETINFO *ptr)
{
	_RoomInfo *RoomInfo	= NULL;
	RoomInfo			= new _RoomInfo;
	ZeroMemory(RoomInfo, sizeof(_RoomInfo));

	static int i		= 0;
	
	i++;

	RoomInfo->iNum	= i;
	strcpy(RoomInfo->cRoomName, buf);
	RoomInfo->iPeopleIN = 1;

	Room_map.insert(pair<int, _RoomInfo*>(i, RoomInfo));
	printf("Successfully Create Room!\n");
 
	ptr->iRoomNumber		= RoomInfo->iNum;
}

