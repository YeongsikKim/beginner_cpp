#pragma once

//Personnel
#define MAX_PEOPLE		2


//ChattingWnd.cpp define
#define MAX_LOADSTRING	100
#define BUFSIZE			1024*512
#define SMALLBUF		512
#define WM_SOCKET		(WM_USER+1)
#define WM_VSTETRIS		(WM_USER+2)
#define WM_RENEW		(WM_USER+3)


//TESTRIS.cpp define
#define BW				10
#define BH				20
#define TS				24
#define random(n) (rand()%n)

#define BIT_COUNT		24


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

//Timer Delay
#define TIMER_TYPE_DOWN_DELAY		1000
#define TIMER_TYPE_CAPTURE_DELAY	1000 * 1
#define TIMER_TYPE_WATCHDOG_DELAY	1000 * 5

//Timer Type
#define TIMER_TYPE_DOWN		1
#define TIMER_TYPE_CAPTURE	2
#define TIMER_TYPE_WATCHDOG	5

//Menu Clicked
#define MENU_READYBUTTON	0x00000001


//Block Size
#define SIZEOFSHAPE			1152
#define SIZEOFBLCOK			128


//Network define
#define SIN_PORT			9000
#define SIN_ADDR			"192.168.100.162"



//Structure define
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

//Socket info
typedef struct _SOCKETINFO
{
	SOCKET sock;
	char buf[BUFSIZE+1];
	int recvbytes;
	int sendbytes;
	BOOL recvdelayed;
}SOCKETINFO, *LPSOCKETINFO;

//Room info
typedef struct _ROOMINFO
{
	int iNum;
	int iPeopleIN;
	char cRoomName[SMALLBUF];
}ROOMINFO, *LPROOMINFO;

struct Point
{
	int x, y;
};


//Enum define
enum {EMPTY, BRICK, WALL = 10};
enum tag_Status {GAMEOVER, RUNNING, PAUSE};
