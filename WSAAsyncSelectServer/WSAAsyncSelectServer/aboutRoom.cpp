#include "stdafx.h"

BOOL CreateRoomInfo(char * buf)
{
	
	ROOMINFO *RoomInfo	= NULL;
	RoomInfo			= new ROOMINFO;
	ZeroMemory(RoomInfo, sizeof(ROOMINFO));

	if (buf == NULL)
	{
		return FALSE;
	}

	g_iRoomIDX++;

	RoomInfo->iNum	= g_iRoomIDX;
	strcpy_s(RoomInfo->cRoomName, strlen(buf) + 1, buf);
	RoomInfo->iPeopleIN = 1;

	mROOM.insert(pair<int, ROOMINFO*>(g_iRoomIDX, RoomInfo));
	printf("Successfully Create Room!\n");
 
	g_iTempRoomNumber = g_iRoomIDX;
	return TRUE;
}
