#pragma once

//Personnel
#define MAX_PEOPLE		2


//ChattingWnd.cpp define
#define MAX_LOADSTRING	100
#define BUFSIZE			1024*512
#define NAMEBUF			24
#define CHATSIZE		512
#define WM_SOCKET		(WM_USER+1)
#define WM_VSTETRIS		(WM_USER+2)
#define WM_RENEW		(WM_USER+3)


//TESTRIS.cpp define
#define BW				10
#define BH				20
#define TS				24
#define random(n) (rand()%n)

#define BIT_COUNT		24
#define NOTBMP			0
#define YESBMP			1


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

//Timer Delay
#define TIMER_TYPE_DOWN_DELAY		1000
#define TIMER_TYPE_CAPTURE_DELAY	1000 * 1

//Timer Type
#define TIMER_TYPE_DOWN		1
#define TIMER_TYPE_CAPTURE	2

//Menu Clicked
#define MENU_READYBUTTON	0x00000001


//Block Size
#define SIZEOFSHAPE			1152
#define SIZEOFBLCOK			128