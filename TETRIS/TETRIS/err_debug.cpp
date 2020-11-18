#include "stdafx.h"

VOID DisplayText(char *fmt, ...)
{
	char	cbuf[SMALLBUF + 256]	= {0,};
	int		iLength					= 0;

	va_list arg;
	va_start(arg, fmt);

	vsprintf(cbuf, fmt, arg);


	iLength = GetWindowTextLength(g_hChatViewEdit);
	SendMessageA(g_hChatViewEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}


VOID err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBoxA(NULL, (LPCSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}


VOID err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	DisplayText("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);

}
