#include "stdafx.h"

struct _RoomInfo
{
	int iNum;
	int iPeopleIN;
	char cRoomName[BUFSIZE];
};


VOID CreateRoomInfo(char *buf, SOCKETINFO *ptr)
{
	_RoomInfo *RoomInfo	= NULL;
	RoomInfo			= new _RoomInfo;
	ZeroMemory(RoomInfo, sizeof(_RoomInfo));
	map<int, _RoomInfo*> Room_map;

	int i		= 0;
	i++;

	RoomInfo->iNum	= i;
	strcpy(RoomInfo->cRoomName, buf);
	RoomInfo->iPeopleIN = 0;

	Room_map.insert(pair<int, _RoomInfo*>(i, RoomInfo));
	printf("Successfully Create Room!\n");

	ptr->iRoomNumber		= RoomInfo->iNum;
}

