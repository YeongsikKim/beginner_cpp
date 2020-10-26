#include "stdafx.h"

BOOL CALLBACK DlgProc_Waiting(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int retval		= 0;

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
			
			return TRUE;
		case IDOK:

		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
			//case IDC_BUTTON1:

		}
		return FALSE;
	case WM_SOCKET:
		ProcessSocketMessage_Room(hDlg, uMsg, wParam, lParam);
		return TRUE;

	case WM_DESTROY:
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

	ViewRoomList();

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
	

	//Create room First
	EnableWindow(hOKbutton2, FALSE);

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
	LVITEM		LI;
	int i		= 0;
	int j		= 0;
	LI.mask			= LVIF_TEXT;


	/*
	LI.iSubItem		= 0;
	LI.iItem		= 0;
	LI.pszText		= L"Quickly Come in!";
	SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM)&LI);

	LI.iSubItem		= 1;
	LI.pszText		= L"1/2";
	SendMessage(hList, LVM_SETITEM, 0, (LPARAM)&LI);
	*/
}

VOID CreateRoom(HWND hDlg)
{
	EnableWindow(hRoomCreate, FALSE);
	GetDlgItemTextA(hDlg, IDC_EDIT_ROOMNAME, buf, BUFSIZE+1);
	//We have to coding input the some special character on buf. whether this is room name or not.///////////////////////////////////////////



	send(sock_room, buf, strlen(buf), 0);

	SetFocus(hRoomCreate);
	SendMessage(hRoomCreate, EM_SETSEL, 0, -1);
	SendMessage(hRoomCreate, EM_REPLACESEL, NULL, (LPARAM)"");
	EnableWindow(hOKbutton2, TRUE);

	return;

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
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		return TRUE;
	}
	return FALSE;
}