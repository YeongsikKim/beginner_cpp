#include "stdafx.h"

BOOL CALLBACK DlgProc_Waiting(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int retval		= 0;

	LPNMHDR hdr			= NULL;
	LPNMLISTVIEW nlv	= NULL;
	LPNMITEMACTIVATE nia = NULL;
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
			return TRUE;
		case IDOK:
			JoinInTheRoom();
			EndDialog(hDlg, 0);
			return TRUE;
		case IDCANCEL:
			exit(-1);
			return TRUE;
		}
		return FALSE;
	case WM_SOCKET:
		ProcessSocketMessage_Room(hDlg, uMsg, wParam, lParam);
		return TRUE;

	case WM_NOTIFY:
		switch (hdr->code)
		{
		case NM_CLICK:
			EnableWindow(hOKbutton2, TRUE);
			ListView_GetItemText(hList, nlv->iItem, 0, Caption, 255);
			InvalidateRect(hDlg, NULL, TRUE);
			break;
		}
		
		

		return TRUE;
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

	COL.pszText		= L"Room ID";
	COL.iSubItem	= 0;
	SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&COL);

	COL.pszText		= L"Room Name";
	COL.iSubItem	= 1;
	SendMessage(hList, LVM_INSERTCOLUMN, 1, (LPARAM)&COL);

	COL.pszText		= L"personnel";
	COL.iSubItem	= 2;
	SendMessage(hList, LVM_INSERTCOLUMN, 2, (LPARAM)&COL);

	EnableWindow(hOKbutton2, FALSE);

	//Socket Create
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) exit(-1);
	
	
	//Socket()
	sock_room	= socket(AF_INET, SOCK_STREAM, 0);
	if (sock_room == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr = {0,};
	serveraddr.sin_family		= AF_INET;
	serveraddr.sin_port			= htons(9000);
	serveraddr.sin_addr.s_addr	= inet_addr("127.0.0.1");


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
	

	ViewRoomList();
}



VOID ProcessSocketMessage_Room(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int retval		= 0;

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CLOSE:
		closesocket(sock);
		break;

	case FD_READ:
		retval		= recv(sock, buf, BUFSIZE, 0);
		if(retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (retval == 0) break;

		buf[retval]	= '\0';
		DisplayText("[TCP Client DATA] %s\r\n", buf);


		EnableWindow(hOKbutton, true);
		break;

	case FD_CONNECT:

		//WSAAsyncSelect()
		retval		= WSAAsyncSelect(sock_room, hDlg, WM_SOCKET, FD_READ | FD_CLOSE);
		if (retval == SOCKET_ERROR) err_quit("WSAAsyncSelect()");

	}
}

VOID ViewRoomList()
{
	LVITEM		LI = {0,};
	int i		= 0;
	int j		= 0;
	LI.mask			= LVIF_TEXT;



	LI.iSubItem		= 0;
	LI.iItem		= 0;
	LI.pszText		= L"1";
	SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM)&LI);

	LI.iSubItem		= 1;
	LI.pszText		= L"Hello World";
	SendMessage(hList, LVM_SETITEM, 0, (LPARAM)&LI);

	LI.iSubItem		= 2;
	LI.pszText		= L"1/2";
	SendMessage(hList, LVM_SETITEM, 0, (LPARAM)&LI);
	
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
		buf[iCharlen]		= '@';

		send(sock_room, buf, strlen(buf), 0);

		SetFocus(hEdit);
		SendMessage(hEdit, EM_SETSEL, 0, -1);
		SendMessage(hEdit, EM_REPLACESEL, NULL, (LPARAM)"");
		return;
	}

}

BOOL CALLBACK DlgProc_MakingRoom(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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
			return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}

VOID JoinInTheRoom()
{
	
}