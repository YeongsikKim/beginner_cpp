#include "stdafx.h"

LPNMITEMACTIVATE g_lpNIA;
int iSaveRoomNumber;

BOOL CALLBACK DlgProc_Waiting(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int retval		= 0;

	LPNMHDR hdr			= NULL;
	LPNMLISTVIEW nlv	= NULL;
	
	TCHAR Caption[BUFSIZE+1]	= {0,};

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
			iSaveRoomNumber = GetRoomNumber();

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
		retval		= recv(sock_room, buf, BUFSIZE, 0);
		//Exception
		if(retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (retval == 0) break;

		//listview initialization
		if (buf[0] == '~')
		{ 
			ListView_DeleteAllItems(hList);
			LI.iItem = 0;
		}
		else if (buf[0] == '/' && buf[1] == 'f')
		{
			MessageBox(hDlg, _T("Can't Join this room"), NULL, MB_OK);
		}
		else if (buf[0] == '/' && buf[1] == 'i')
		{
			EndDialog(hDlg, 0);
		}
		else
		{
			ViewRoomList(buf);
		}
		
		break;

	case FD_CONNECT:

		//WSAAsyncSelect()
		retval		= WSAAsyncSelect(sock_room, hDlg, WM_SOCKET, FD_READ | FD_CLOSE);
		if (retval == SOCKET_ERROR) err_quit("WSAAsyncSelect()");

		
		buf[1]		= '*';
		send(sock_room, buf, sizeof(char)*2, NULL);
		

	}
}

VOID ViewRoomList(char *buf)
{
	int iBufLen		= 0;
	int iNum		= 0;
	int iPeople		= 0;
	char temp[10]	= {0,};

	

	LI.mask			= LVIF_TEXT;



	iBufLen			= strlen(buf);
	iNum			= buf[iBufLen + 1];

	iPeople			= buf[iBufLen + 2];
	buf[iBufLen + 1] = buf[iBufLen + 2] = '\0';


	LI.iSubItem		= 0;
	LI.pszText		= buf;
	SendMessageA(hList, LVM_INSERTITEMA, 0, (LPARAM)&LI);

	LI.iSubItem		= 1;
	sprintf(LI.pszText, "%d", iNum);
	SendMessageA(hList, LVM_SETITEMA, 0, (LPARAM)&LI);

	LI.iSubItem		= 2;
	sprintf(temp, "%d/%d", iPeople, MAX_PEOPLE);
	strcpy_s(LI.pszText, sizeof(temp), temp);
	SendMessageA(hList, LVM_SETITEMA, 0, (LPARAM)&LI);


	LI.iItem++;
}

VOID CreateRoom(HWND hDlg)
{
	int iCharlen		= 0;
	GetDlgItemTextA(hDlg, IDC_EDIT_ROOMNAME, buf, BUFSIZE+1);
	if (buf[0] == '\0'){;}
	else
	{
		//We have to coding input the some special character on buf. whether this is room name or not.
		iCharlen			= strlen(buf);
		buf[iCharlen + 1]	= '@';

		send(sock_room, buf, iCharlen + 2, 0);

		SetFocus(hEdit);
		SendMessage(hEdit, EM_SETSEL, 0, -1);
		SendMessage(hEdit, EM_REPLACESEL, NULL, (LPARAM)"");
		return;
	}

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
	ZeroMemory(buf, BUFSIZE);
	
	buf[0]		= iSaveRoomNumber;
	buf[1]		= NULL;
	buf[2]		= '^';
	send(sock_room, buf, BUFSIZE, NULL);
}



int GetRoomNumber()
{
	int iBuflength		= 0;
	TCHAR cBuf[BUFSIZE]	= {0,};

	iBuflength = wcslen(cBuf);

	ListView_GetItemText(hList, g_lpNIA->iItem, 1, cBuf, 255);

	if (iBuflength <= 1)
	{
		iSaveRoomNumber = (int) (cBuf[0] - '0');
	}
	else
	{
		iSaveRoomNumber--;
		iSaveRoomNumber += (int)(cBuf[iBuflength - 0] - '0');
		iSaveRoomNumber += (int)((cBuf[iBuflength - 1] - '0') * 10);
		iSaveRoomNumber += (int)((cBuf[iBuflength - 2] - '0') * 100);
	}

	return iSaveRoomNumber;
}


VOID WaitingRoomReadFunction(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	SOCKET hSock = (SOCKET)wParam;
	int iRecvLen = 0;
	int iBodySize = 0;
	int iMaxLen = 0;

	LPPACKET_BODY pPacket = NULL;
	LPPACKET_HEADER pHeader = NULL;
	LPSTR pBody = NULL;


	iMaxLen = BUFSIZE;

	while(pPacket->iCurRecv < iMaxLen)
	{
		iRecvLen = recv(hSock, (LPSTR)(pPacket->ucData + pPacket->iCurRecv), (iMaxLen - pPacket->iCurRecv), NULL);
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
				pHeader = (LPPACKET_HEADER) pPacket->ucData;
				iMaxLen = pHeader->iSize;
			}
		}
	}

	pBody = (LPSTR) (pPacket->ucData + sizeof(PACKET_HEADER));
	iBodySize = pHeader->iSize - sizeof(PACKET_HEADER);


	switch (pHeader->iFlag)
	{
	case WSABUFFER_RENEW:
		RenewList(pHeader->iSize, pBody);
		break;

	case WSABUFFER_JOIN:
		break;

	default:
		break;
	}
}


VOID RenewList(int iSize, LPSTR pBody)
{
	int iNumRoom = 0;
	int i = 0;

	ListView_DeleteAllItems(hList);
	LI.iItem = 0;

	iNumRoom = iSize/(sizeof(ROOMINFO));

	for (i = 1; i <= iNumRoom; i++)
	{
		
	}
}