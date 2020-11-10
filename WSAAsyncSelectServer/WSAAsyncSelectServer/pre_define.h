#pragma once


#define BUFSIZE		1024*1024
#define BIT_COUNT	24


//Personnel
#define MAX_PEOPLE		2


//Sign in buf
#define REQUESTROOM	'*'
#define ROOMTITLE	'@'
#define ENTRANCE	'^'


#define NOTBMP		0
#define YESBMP		1

//Receive Buffer Flag
#define WSABUFFER_ROOMNAME 0x1
#define WSABUFFER_CHATTING 0x2
#define WSABUFFER_IMAGE 0x4

//WM_USER
#define WM_SOCKET	(WM_USER+1)
#define WM_RENEW	(WM_USER+2)