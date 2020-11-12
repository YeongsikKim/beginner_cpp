#pragma once


#define BUFSIZE		456295
#define ROOMNAME	24
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
#define WSABUFFER_ROOMNAME	0x00000001
#define WSABUFFER_CHATTING	0x00000002
#define WSABUFFER_IMAGE		0x00000004
#define WSABUFFER_QUITROOM	0x00000008
#define WSABUFFER_RENEW		0x00000010
#define WSABUFFER_JOIN		0x00000020
#define WSABUFFER_FULL		0x00000040
#define WSABUFFER_NOTFULL	0x00000080


//WM_USER
#define WM_SOCKET	(WM_USER+1)
#define WM_RENEW	(WM_USER+2)