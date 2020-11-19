#pragma once


#define BUFSIZE		1024*512
#define SMALLBUF	512
#define BIT_COUNT	24


//Personnel
#define MAX_PEOPLE		2


//Sign in buf
#define READY		1
#define NOTREADY	0

//Receive Buffer Flag
#define WSABUFFER_ROOMNAME	0x00000001
#define WSABUFFER_CHATTING	0x00000002
#define WSABUFFER_IMAGE		0x00000004
#define WSABUFFER_QUITROOM	0x00000008
#define WSABUFFER_RENEW		0x00000010
#define WSABUFFER_JOIN		0x00000020
#define WSABUFFER_FULL		0x00000040
#define WSABUFFER_NOTFULL	0x00000080
#define WSABUFFER_READY		0x00000100
#define WSABUFFER_NOTREADY	0x00000200
#define WSABUFFER_START		0x00000400
#define WSABUFFER_END		0x00000800
#define WSABUFFER_PAUSE		0x00001000
#define WSABUFFER_WATCHDOG	0x00002000


//WM_USER
#define WM_SOCKET	(WM_USER+1)
#define WM_RENEW	(WM_USER+2)

//WM_TIMER
#define TIMER_TYPE_WATCHDOG			1
#define TIMER_TYPE_WATCHDOG_DELAY	1000 * 3



//Structure define
typedef struct _SOCKETINFO
{
	SOCKET sock;
	char buf[BUFSIZE+1];
	int recvbytes;
	int sendbytes;
	BOOL recvdelayed;
}SOCKETINFO, *LPSOCKETINFO;

typedef struct _USERINFO
{
	ULONG addr;
	int iRoomNumber;
	int iStatus;
}USERINFO, *LPUSERINFO;


typedef struct _ROOMINFO
{
	int iNum;
	int iPeopleIN;
	char cRoomName[SMALLBUF];
}ROOMINFO, *LPROOMINFO;


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
