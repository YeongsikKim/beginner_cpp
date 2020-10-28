// TCPClient.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"


#define BUFSIZE 512

VOID err_quit(char *msg)
{
	LPVOID lpMsgBuf		= NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

VOID err_display(char *msg)
{
	LPVOID lpMsgBuf		= NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s]%s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
	
}


int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received		= 0;
	char *ptr			= buf;
	int left			= len;

	while (left > 0)
	{
		received		= recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		else if(received == 0) break;

		left -= received;
		ptr	+= received;
	}

	return (len - left);

}


int _tmain(int argc, _TCHAR* argv[])
{
	int retval		= 0;

	//winsock initialization
	WSADATA wsa		= {0,};
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return -1;

	//socket()
	SOCKET sock		= socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	//connect()
	SOCKADDR_IN serveraddr;
	serveraddr.sin_family	= AF_INET;
	serveraddr.sin_port		= htons(9000);
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	retval		= connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	if (retval == SOCKET_ERROR) err_quit("connect()");


	char buf[BUFSIZE + 1];
	int len;

	while (TRUE)
	{
		ZeroMemory(buf, sizeof(buf));
		printf("\nSending Data : ");
		if (fgets(buf, BUFSIZE+1, stdin) == NULL) break;

		len		= strlen(buf);
		if (buf[len-1] == '\n') buf[len-1] = '\0';
		if (strlen(buf) == 0) break;

		retval = send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}
		printf("[TCP Client] %d Bytes is sent.\n", retval);


		retval		= recvn(sock, buf, retval, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if(retval == 0) break;
		
		

		buf[retval] = '\0';
		printf("[TCP Client] %d Bytes received\n", retval);
		printf("[DATA] %s\n", buf);
	}

	closesocket(sock);

	WSACleanup();

	return 0;
}

