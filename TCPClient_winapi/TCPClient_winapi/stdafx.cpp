// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// TCPClient_winapi.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.


SOCKET sock;
char buf[BUFSIZE+1];
HWND hEdit1, hEdit2, hOKbutton;
HANDLE hReadEvent, hWriteEvent;
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.