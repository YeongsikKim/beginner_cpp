#include "stdafx.h"


// 傈开 函荐 包府 秦林技夸~

LPNMITEMACTIVATE g_lpNIA;
int g_iSaveRoomNumber;
BOOL bCreateRoom;

BOOL CALLBACK DlgProc_Waiting(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int iRet		= TRUE;

	LPNMHDR hdr			= NULL;
	LPNMLISTVIEW nlv	= NULL;
	
	WCHAR wCaption[NAMEBUF+1]	= {0,};

	hdr		= (LPNMHDR)lParam;
	nlv		= (LPNMLISTVIEW)lParam;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitProc_Waiting(hDlg);
		return TRUE;

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDC_BTN_MAKE_ROOM:
				bCreateRoom = FALSE;
				DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_MAKING), NULL, DlgProc_MakingRoom);
				if (bCreateRoom == TRUE)
				{
					EndDialog(hDlg, 0);
				}
				break;

			case IDC_BUTTON2:
				ListView_DeleteAllItems(g_hList);
				SendingRenew();
				break;

			case IDOK:
				JoinInTheRoom();
				break;

			case IDCANCEL:
				exit(-1);		//do not make tetris window
				break;
			}

			iRet = FALSE;
		}
		break;

	case WM_SOCKET:
		ProcessSocketMessage_Room(hDlg, uMsg, wParam, lParam);
		return TRUE;

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case LVN_ITEMCHANGED:
			EnableWindow(g_hOKbutton2, TRUE);
			g_lpNIA	= (LPNMITEMACTIVATE)lParam;
			g_iSaveRoomNumber = GetRoomNumber();
			break;
		}
		break;

	case WM_DESTROY:
		EndDialog(hDlg, 0);

		closesocket(g_hSockRoom);
		return TRUE;
	}

	return FALSE;
}


VOID InitProc_Waiting(HWND hDlg)
{
	int retval	= 0;
	LVCOLUMN	COL;

	g_hList			= GetDlgItem(hDlg, IDC_LIST1);
	g_hRoomCreate	= GetDlgItem(hDlg, IDC_BTN_MAKE_ROOM);
	g_hOKbutton2	= GetDlgItem(hDlg, IDOK);
	g_hRoomEdit		= GetDlgItem(hDlg, IDC_EDIT_ROOMNAME);
	g_hResetKey		= GetDlgItem(hDlg, IDC_BUTTON2);

	COL.mask		= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	COL.fmt			= LVCFMT_LEFT;
	COL.cx			= 150;

	COL.pszText		= L"Room Name";
	COL.iSubItem	= 0;
	SendMessage(g_hList, LVM_INSERTCOLUMN, 0, (LPARAM)&COL);

	COL.pszText		= L"Room ID";
	COL.iSubItem	= 1;
	SendMessage(g_hList, LVM_INSERTCOLUMN, 1, (LPARAM)&COL);

	COL.pszText		= L"personnel";
	COL.iSubItem	= 2;
	SendMessage(g_hList, LVM_INSERTCOLUMN, 2, (LPARAM)&COL);

	EnableWindow(g_hOKbutton2, FALSE);



	//Socket Create
	WSADATA wsa= {0,};
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) exit(-1);
	
	
	//Socket()
	g_hSockRoom	= socket(AF_INET, SOCK_STREAM, 0);
	if (g_hSockRoom == INVALID_SOCKET) err_quit("socket()");

	SOCKADDR_IN serveraddr = {0,};
	serveraddr.sin_family		= AF_INET;
	serveraddr.sin_port			= htons(9000);
	//serveraddr.sin_addr.s_addr	= inet_addr("127.0.0.1");
	serveraddr.sin_addr.s_addr	= inet_addr("192.168.100.162");


	//WSAAsyncSelect()
	retval = WSAAsyncSelect(g_hSockRoom, hDlg, WM_SOCKET, FD_CONNECT | FD_CLOSE);

	if (retval == SOCKET_ERROR) 
		err_quit("WSAAsyncSelect()");

	//connect()
	retval = connect(g_hSockRoom, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

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
	int iRetval		= 0;

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CLOSE:
		closesocket(g_hSockRoom);
		break;

	case FD_READ:
		WaitingRoomReadFunction(hDlg, wParam, lParam);
		break;

	case FD_CONNECT:
		//WSAAsyncSelect()
		iRetval = WSAAsyncSelect(g_hSockRoom, hDlg, WM_SOCKET, FD_READ | FD_CLOSE);

		if (iRetval == SOCKET_ERROR)
		{
			err_quit("WSAAsyncSelect()");
		}

		ListView_DeleteAllItems(g_hList);
		SendingRenew();
	}
}

VOID ViewRoomList(char *pBody, int iBodySize)
{
	int iSizeOfRoom = 0;
	char cNumOfRoom[10] = {0,};

	g_tLI.mask			= LVIF_TEXT;
	g_tLI.iItem		= 0;

	while (iSizeOfRoom < iBodySize)
	{
		g_tLI.iSubItem = 0;
		g_tLI.pszText = pBody + (sizeof(int) * 2) + iSizeOfRoom;
		SendMessageA(g_hList, LVM_INSERTITEMA, 0, (LPARAM)&g_tLI);

		g_tLI.iSubItem = 1;
		g_tLI.pszText = pBody + iSizeOfRoom;
		sprintf(g_tLI.pszText, "%d", g_tLI.pszText[0]);
		SendMessageA(g_hList, LVM_SETITEMA, 1, (LPARAM)&g_tLI);

		g_tLI.iSubItem = 2;
		g_tLI.pszText = pBody + sizeof(int) + iSizeOfRoom;
		sprintf(g_tLI.pszText, "%d/%d", g_tLI.pszText[0], MAX_PEOPLE);
		SendMessageA(g_hList, LVM_SETITEMA, 2, (LPARAM)&g_tLI);

		iSizeOfRoom += sizeof(ROOMINFO);
		g_tLI.iItem++;
	}
}

BOOL CreateRoom(HWND hDlg)
{
	int iSendTot = 0;
	int iSendSize = 0;

	LPPACKET_BODY pPacket = NULL;
	pPacket = new PACKET_BODY;
	ZeroMemory(pPacket, sizeof(PACKET_BODY));

	GetDlgItemTextA(hDlg, IDC_EDIT_ROOMNAME, (LPSTR)pPacket->cData, NAMEBUF+1);

	if ( pPacket->cData[0] == '\0' )
	{
		MessageBox(hDlg, _T("Can't make the room which name is blank"), _T("Error"), NULL);
		return FALSE;
	}
	
 	DWORD dwRespBufSize = strlen(pPacket->cData) + sizeof(PACKET_HEADER) + 1;

	PBYTE pRespBuf = new BYTE[dwRespBufSize];
	ZeroMemory(pRespBuf, dwRespBufSize);


	LPPACKET_HEADER pHeader = (LPPACKET_HEADER)pRespBuf;
	PBYTE pBody = pRespBuf + sizeof(PACKET_HEADER);

	//header setting
	pHeader->iFlag = WSABUFFER_ROOMNAME;
	pHeader->iSize = strlen(pPacket->cData) + sizeof(PACKET_HEADER);

	//body setting
	memcpy(pBody, pPacket->cData, strlen(pPacket->cData));


	do 
	{
		iSendSize = send(g_hSockRoom, (char*)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
		if (iSendSize == SOCKET_ERROR)
		{
			printf("send()\n");
			break;
		}
		iSendTot += iSendSize;

	} while (pHeader->iSize != iSendTot);

	SetFocus(g_hRoomEdit);
	SendMessage(g_hRoomEdit, EM_SETSEL, 0, -1);
	SendMessage(g_hRoomEdit, EM_REPLACESEL, NULL, (LPARAM)"");

	delete [] pRespBuf;
	pRespBuf = NULL;

	delete pPacket;
	return TRUE;
}

BOOL CALLBACK DlgProc_MakingRoom(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			bCreateRoom = CreateRoom(hDlg);
			EndDialog(hDlg, 0);
			break;

		case IDCANCEL:
			EndDialog(hDlg, 0);
			break;
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
	LPPACKET_HEADER pHeader = NULL;
	DWORD dwRespBufSize = sizeof(PACKET_HEADER) + sizeof(int) + 1;
	PBYTE pRespBuf = NULL;
	PBYTE pBody = NULL;
	

	pRespBuf = new BYTE[dwRespBufSize];
	ZeroMemory(pRespBuf, dwRespBufSize);
	
	pHeader = (LPPACKET_HEADER)pRespBuf;
	pBody = pRespBuf + sizeof(PACKET_HEADER);

	pHeader->iFlag = WSABUFFER_JOIN;
	pHeader->iSize = sizeof(PACKET_HEADER) + sizeof(int);


	iSaveRoomNum = g_iSaveRoomNumber;
	memcpy(pBody, &iSaveRoomNum, sizeof(int));
	
	do 
	{
		iSendLen = send(g_hSockRoom, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
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
	TCHAR cBuf[NAMEBUF]	= {0,};

	iBuflength = wcslen(cBuf);

	ListView_GetItemText(g_hList, g_lpNIA->iItem, 1, cBuf, 255);

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
	SOCKET hSock	= (SOCKET)wParam;
	int iRecvLen	= 0;
	int iBodySize	= 0;
	int iMaxLen		= 0;

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
		ListView_DeleteAllItems(g_hList);
		ViewRoomList(pBody, iBodySize);
		break;

	case WSABUFFER_JOIN:
		break;

	case WSABUFFER_FULL:
		MessageBox(hDlg, _T("This Room has too many People"), NULL, MB_OK);
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
	int iSendLen = 0;
	int iSendTot = 0;
	PACKET_HEADER hHeader = {0,};

	hHeader.iFlag = WSABUFFER_RENEW;
	hHeader.iSize = sizeof(PACKET_HEADER);

	do 
	{
		iSendLen = send(g_hSockRoom, (LPSTR)&hHeader + iSendTot, hHeader.iSize - iSendTot, NULL);

		if ( iSendLen == SOCKET_ERROR )
		{
			err_quit("send()");
		}

		iSendTot += iSendLen;
	} while (hHeader.iSize != iSendTot);
}