// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// WSAAsyncSelectServer.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"


map<INT, SOCKETINFO*> socket_map;
map<INT, SOCKETINFO*>::iterator it;
map<int, ROOMINFO*> Room_map;
map<int, ROOMINFO*>::iterator iterRoom;
map<int, LPUSERINFO> mUSER;
map<int, LPUSERINFO>::iterator iterUser;