#include "stdafx.h"

LPNMITEMACTIVATE g_lpNIA;
int g_iSaveRoomNumber;

BOOL CALLBACK DlgProc_Waiting(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int retval		= 0;

	LPNMHDR hdr			= NULL;
	LPNMLISTVIEW nlv	= NULL;
	
	TCHAR Caption[ROOMNAME+1]	= {0,};

	hdr		= (LPNMHDR)lParam;
	nlv		= (LPNMLISTVIEW)lParam;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitProc_Waiting(hDlg);
		return TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BUTTON1:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_MAKING), NULL, DlgProc_MakingRoom);
			EndDialog(hDlg, 0);
			break;
		case IDOK:
			JoinInTheRoom();
			EndDialog(hDlg, 0);
			break;
		case IDCANCEL:
			exit(-1); 
			break;
		}
		return FALSE;

	case WM_SOCKET:
		ProcessSocketMessage_Room(hDlg, uMsg, wParam, lParam);
		return TRUE;

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case LVN_ITEMCHANGED:
			EnableWindow(hOKbutton2, TRUE);
			g_lpNIA	= (LPNMITEMACTIVATE)lParam;
			g_iSaveRoomNumber = GetRoomNumber();

			printf("Item : %d, %d, TEXT : %s\n", g_lpNIA->iItem, g_lpNIA->iSubItem, Caption);
			break;
		}
		break;

	case WM_DESTROY:
		EndDialog(hDlg, 0);
		closesocket(sock_room);
		return TRUE;
	}

	return FALSE;
}


VOID InitProc_Waiting(HWND hDlg)
{
	int retval	= 0;
	LVCOLUMN	COL;

	hList		= GetDlgItem(hDlg, IDC_LIST1);
	hRoomCreate	= GetDlgItem(hDlg, IDC_BUTTON1);
	hOKbutton2	= GetDlgItem(hDlg, IDOK);
	hEdit		= GetDlgItem(hDlg, IDC_EDIT_ROOMNAME);

	COL.mask		= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	COL.fmt			= LVCFMT_LEFT;
	COL.cx			= 150;

	COL.pszText		= L"Room Name";
	COL.iSubItem	= 0;
	SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&COL);

	COL.pszText		= L"Room ID";
	COL.iSubItem	= 1;
	SendMessage(hList, LVM_INSERTCOLUMN, 1, (LPARAM)&COL);

	COL.pszText		= L"personnel";
	COL.iSubItem	= 2;
	SendMessage(hList, LVM_INSERTCOLUMN, 2, (LPARAM)&COL);

	EnableWindow(hOKbutton2, FALSE);



	//Socket Create
	WSADATA wsa= {0,};
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) exit(-1);
	
	
	//Socket()
	sock_room	= socket(AF_INET, SOCK_STREAM, 0);
	if (sock_room == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr = {0,};
	serveraddr.sin_family		= AF_INET;
	serveraddr.sin_port			= htons(9000);
	//serveraddr.sin_addr.s_addr	= inet_addr("127.0.0.1");
	serveraddr.sin_addr.s_addr	= inet_addr("192.168.100.162");


	//WSAAsyncSelect()
	retval		= WSAAsyncSelect(sock_room, hDlg, WM_SOCKET, FD_CONNECT | FD_CLOSE);
	if (retval == SOCKET_ERROR) err_quit("WSAAsyncSelect()");

	//connect()
	retval		= connect(sock_room, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == INVALID_SOCKET)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			err_display("connect()");
		}
	}

}



VOID ProcessSocketMessage_Room(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int retval		= 0;

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CLOSE:
		closesocket(sock_room);
		break;

	case FD_READ:
		WaitingRoomReadFunction(hDlg, wParam, lParam);
		break;

	case FD_CONNECT:
		//WSAAsyncSelect()
		retval		= WSAAsyncSelect(sock_room, hDlg, WM_SOCKET, FD_READ | FD_CLOSE);
		if (retval == SOCKET_ERROR) err_quit("WSAAsyncSelect()");


		SendingRenew();
	}
}

VOID ViewRoomList(char *pBody, int iBodySize)
{
#if 1
	int iSizeOfRoom = 0;
	char cNumOfRoom[10] = {0,};

	LI.mask			= LVIF_TEXT;
	LI.iItem		= 0;

	while (iSizeOfRoom < iBodySize)
	{
		LI.iSubItem = 0;
		LI.pszText = pBody + (sizeof(int) * 2) + iSizeOfRoom;
		SendMessageA(hList, LVM_INSERTITEMA, 0, (LPARAM)&LI);

		LI.iSubItem = 1;
		LI.pszText = pBody + iSizeOfRoom;
		sprintf(LI.pszText, "%d", LI.pszText[0]);
		SendMessageA(hList, LVM_SETITEMA, 1, (LPARAM)&LI);

		LI.iSubItem = 2;
		LI.pszText = pBody + sizeof(int) + iSizeOfRoom;
		sprintf(LI.pszText, "%d/%d", LI.pszText[0], MAX_PEOPLE);
		SendMessageA(hList, LVM_SETITEMA, 2, (LPARAM)&LI);

		iSizeOfRoom += sizeof(ROOMINFO);
		LI.iItem++;
	}
#endif

#if 0
	LPSTR cNum = NULL;
	LPSTR cCurrentPeople = NULL;
	char cMaxPeople[ROOMNAME] = {0,};
	
	LI.mask			= LVIF_TEXT;

	memcpy(cNum, pBody + 0, sizeof(int));
	memcpy(cCurrentPeople, pBody + sizeof(int) , sizeof(int));


	LI.iSubItem		= 0;
	strcpy(LI.pszText, pBody + (sizeof(int) * 2));
	SendMessageA(hList, LVM_INSERTITEMA, 0, (LPARAM)&LI);

	LI.iSubItem		= 1;
	strcpy(LI.pszText, cNum);
	SendMessageA(hList, LVM_SETITEMA, 0, (LPARAM)&LI);

	LI.iSubItem		= 2;
	sprintf(cMaxPeople, "%s/%d", cCurrentPeople, MAX_PEOPLE);
	strcpy_s(LI.pszText, sizeof(cMaxPeople), cMaxPeople);
	SendMessageA(hList, LVM_SETITEMA, 0, (LPARAM)&LI);


	LI.iItem++;
#endif
}

VOID CreateRoom(HWND hDlg)
{
	int iSendTot = 0;
	int iSendSize = 0;

	LPPACKET_BODY pPacket = NULL;
	pPacket = new PACKET_BODY;
	ZeroMemory(pPacket, sizeof(PACKET_BODY));

	GetDlgItemTextA(hDlg, IDC_EDIT_ROOMNAME, (LPSTR)pPacket->cData, ROOMNAME+1);
	DWORD dwRespBufSize = strlen(pPacket->cData) + sizeof(PACKET_HEADER) + 1;
	PBYTE pRespBuf = new BYTE[dwRespBufSize];

	LPPACKET_HEADER pHeader = (LPPACKET_HEADER)pRespBuf;
	PBYTE pBody = pRespBuf + sizeof(PACKET_HEADER);

	//header setting
	pHeader->iFlag = WSABUFFER_ROOMNAME;
	pHeader->iSize = strlen(pPacket->cData) + sizeof(PACKET_HEADER);

	//body setting
	strcpy((LPSTR)pBody, pPacket->cData);

	do 
	{
		iSendSize = send(sock_room, (char*)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
		if (iSendSize == SOCKET_ERROR)
		{
			printf("send()\n");
			break;
		}
		iSendTot += iSendSize;
	} while (pHeader->iSize != iSendTot);

	SetFocus(hEdit);
	SendMessage(hEdit, EM_SETSEL, 0, -1);
	SendMessage(hEdit, EM_REPLACESEL, NULL, (LPARAM)"");

	delete pRespBuf;
	delete pPacket;
	return;	
}

BOOL CALLBACK DlgProc_MakingRoom(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WCHAR Caption[100]		= {0,};


	switch (uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			CreateRoom(hDlg);
			EndDialog(hDlg, 0);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

VOID JoinInTheRoom()
{
	int iSendTot = 0;
	int iSendLen = 0;
	int iSaveRoomNum = 0;
	int *pSaveRoomNum = &iSaveRoomNum;
	LPPACKET_HEADER pHeader = NULL;
	DWORD dwRespBufSize = sizeof(PACKET_HEADER) + sizeof(int) + 1;
	PBYTE pRespBuf = NULL;
	PBYTE pBody = NULL;
	
	pRespBuf = new BYTE[dwRespBufSize];
	
	pHeader = (LPPACKET_HEADER)pRespBuf;
	pBody = pRespBuf + sizeof(PACKET_HEADER);

	pHeader->iFlag = WSABUFFER_JOIN;
	pHeader->iSize = sizeof(PACKET_HEADER) + sizeof(int);

//	iSaveRoomNum = g_iSaveRoomNumber;

	pSaveRoomNum = (int*)pBody;
	*pSaveRoomNum = g_iSaveRoomNumber;
	
	do 
	{
		iSendLen = send(sock_room, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
		if (iSendLen == SOCKET_ERROR)
		{
			printf("send()\n");
			continue;
		}
		iSendTot += iSendLen;
	} while (pHeader->iSize != iSendTot);
	
}



int GetRoomNumber()
{
	int iBuflength		= 0;
	TCHAR cBuf[ROOMNAME]	= {0,};

	iBuflength = wcslen(cBuf);

	ListView_GetItemText(hList, g_lpNIA->iItem, 1, cBuf, 255);

	if (iBuflength <= 1)
	{
		g_iSaveRoomNumber = (int) (cBuf[0] - '0');
	}
	else
	{
		g_iSaveRoomNumber--;
		g_iSaveRoomNumber += (int)(cBuf[iBuflength - 0] - '0');
		g_iSaveRoomNumber += (int)((cBuf[iBuflength - 1] - '0') * 10);
		g_iSaveRoomNumber += (int)((cBuf[iBuflength - 2] - '0') * 100);
	}

	return g_iSaveRoomNumber;
}


VOID WaitingRoomReadFunction(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	SOCKET hSock = (SOCKET)wParam;
	int iRecvLen = 0;
	int iBodySize = 0;
	int iMaxLen = 0;

	LPPACKET_BODY pPacket = NULL;
	pPacket = new PACKET_BODY;
	ZeroMemory(pPacket, sizeof(PACKET_BODY));


	LPPACKET_HEADER pHeader = NULL;
	LPSTR pBody = NULL;


	iMaxLen = BUFSIZE;

	while(pPacket->iCurRecv < iMaxLen)
	{
		iRecvLen = recv(hSock, (LPSTR)(pPacket->cData + pPacket->iCurRecv), (iMaxLen - pPacket->iCurRecv), NULL);
		if (iRecvLen == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}
		}

		pPacket->iCurRecv += ((iRecvLen > 0) ? iRecvLen : 0);

		if (pPacket->iCurRecv >= sizeof(PACKET_HEADER))
		{
			if (!pHeader)
			{
				pHeader = (LPPACKET_HEADER) pPacket->cData;
				iMaxLen = pHeader->iSize;
			}
		}
	}

	pBody = (LPSTR)(pPacket->cData + sizeof(PACKET_HEADER));
	iBodySize = pHeader->iSize - sizeof(PACKET_HEADER);


	switch (pHeader->iFlag)
	{
	case WSABUFFER_RENEW:
		ViewRoomList(pBody, iBodySize);
		break;

	case WSABUFFER_JOIN:
		
		break;

	case WSABUFFER_FULL:
		MessageBox(hDlg, _T("Can't Join this room"), NULL, MB_OK);
		break;

	case WSABUFFER_NOTFULL:
		EndDialog(hDlg, 0);
		break;

	default:
		break;
	}
}

VOID SendingRenew()
{
	int iSendByte = 0;
	LPPACKET_HEADER pHeader = {0,};

	pHeader = new PACKET_HEADER;
	ZeroMemory(pHeader, sizeof(PACKET_HEADER));
	
	pHeader->iFlag = WSABUFFER_RENEW;

	iSendByte = send(sock_room, (LPSTR)pHeader, sizeof(PACKET_HEADER), NULL);
}