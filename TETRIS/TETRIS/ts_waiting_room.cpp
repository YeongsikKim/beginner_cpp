#include "stdafx.h"


BOOL CALLBACK DlgProc_Waiting(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int iRetval = 0;
	LPNMHDR pHdr = NULL;
	LPNMLISTVIEW pNlv = NULL;

	pHdr		= (LPNMHDR)lParam;
	pNlv		= (LPNMLISTVIEW)lParam;

	switch ( uMsg )
	{
	case WM_INITDIALOG:
		{
			InitProc_Waiting(hDlg);
		}
		return TRUE;

	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
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
		}
		return TRUE;

	case WM_SOCKET:
		{
			ProcessSocketMessage_Room(hDlg, uMsg, wParam, lParam);
		}
		return TRUE;

	case WM_NOTIFY:
		{
			switch (((LPNMHDR)lParam)->code)
			{
			case LVN_ITEMCHANGED:
				EnableWindow(g_hOKbutton2, TRUE);
				g_lpNIA	= (LPNMITEMACTIVATE)lParam;
				g_iSaveRoomNumber = GetRoomNumber();

				break;
			}
		}
		break;

	case WM_DESTROY:
		{
			EndDialog(hDlg, 0);
			closesocket(g_hWRSock);
		}
		return TRUE;
	}

	return FALSE;
}


VOID InitProc_Waiting(HWND hDlg)
{
	int iRetval	= 0;
	LVCOLUMN	tLVcol;

	g_hList		= GetDlgItem(hDlg, IDC_LIST1);
	g_hRoomCreate	= GetDlgItem(hDlg, IDC_BUTTON1);
	g_hOKbutton2	= GetDlgItem(hDlg, IDOK);
	g_hEdit		= GetDlgItem(hDlg, IDC_EDIT_ROOMNAME);

	tLVcol.mask		= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	tLVcol.fmt			= LVCFMT_LEFT;
	tLVcol.cx			= 150;

	tLVcol.pszText		= L"Room Name";
	tLVcol.iSubItem	= 0;
	SendMessage(g_hList, LVM_INSERTCOLUMN, 0, (LPARAM)&tLVcol);

	tLVcol.pszText		= L"Room ID";
	tLVcol.iSubItem	= 1;
	SendMessage(g_hList, LVM_INSERTCOLUMN, 1, (LPARAM)&tLVcol);

	tLVcol.pszText		= L"personnel";
	tLVcol.iSubItem	= 2;
	SendMessage(g_hList, LVM_INSERTCOLUMN, 2, (LPARAM)&tLVcol);

	EnableWindow(g_hOKbutton2, FALSE);



	//Socket Create
	WSADATA tWSAdata= {0,};
	if ( WSAStartup(MAKEWORD(2,2), &tWSAdata) != 0 )
	{
		exit(-1);
	}
	
	
	//Socket()
	g_hWRSock	= socket(AF_INET, SOCK_STREAM, 0);
	if ( g_hWRSock == INVALID_SOCKET )
	{
		err_quit("socket()");
	}

	SOCKADDR_IN tAddrServer = {0,};
	tAddrServer.sin_family		= AF_INET;
	tAddrServer.sin_port			= htons(SIN_PORT);
	tAddrServer.sin_addr.s_addr	= inet_addr(SIN_ADDR);


	//WSAAsyncSelect()
	iRetval = WSAAsyncSelect(g_hWRSock, hDlg, WM_SOCKET, FD_CONNECT | FD_CLOSE);
	if ( iRetval == SOCKET_ERROR )
	{
		err_quit("WSAAsyncSelect()");
	}

	//connect()
	iRetval = connect(g_hWRSock, (SOCKADDR*)&tAddrServer, sizeof(tAddrServer));
	if ( iRetval == INVALID_SOCKET )
	{
		if ( WSAGetLastError() != WSAEWOULDBLOCK )
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
		{
			closesocket(g_hWRSock);
		}
		break;

	case FD_READ:
		{
			WaitingRoomReadFunction(hDlg, wParam, lParam);
		}
		break;

	case FD_CONNECT:
		{
			iRetval		= WSAAsyncSelect(g_hWRSock, hDlg, WM_SOCKET, FD_READ | FD_CLOSE);

			if ( iRetval == SOCKET_ERROR )
			{
				err_quit("WSAAsyncSelect()");
			}

			SendingRenew();
		}
	}
}

VOID ViewRoomList(char *pBody, int iBodySize)
{
	int		iSizeOfRoom		= 0;
	char	cNumOfRoom[10]	= {0,};

	g_tLVItem.mask		= LVIF_TEXT;
	g_tLVItem.iItem		= 0;

	while (iSizeOfRoom < iBodySize)
	{
		g_tLVItem.iSubItem = 0;
		g_tLVItem.pszText = pBody + (sizeof(int) * 2) + iSizeOfRoom;
		SendMessageA(g_hList, LVM_INSERTITEMA, 0, (LPARAM)&g_tLVItem);

		g_tLVItem.iSubItem = 1;
		g_tLVItem.pszText = pBody + iSizeOfRoom;
		sprintf(g_tLVItem.pszText, "%d", g_tLVItem.pszText[0]);
		SendMessageA(g_hList, LVM_SETITEMA, 1, (LPARAM)&g_tLVItem);

		g_tLVItem.iSubItem = 2;
		g_tLVItem.pszText = pBody + sizeof(int) + iSizeOfRoom;
		sprintf(g_tLVItem.pszText, "%d/%d", g_tLVItem.pszText[0], MAX_PEOPLE);
		SendMessageA(g_hList, LVM_SETITEMA, 2, (LPARAM)&g_tLVItem);

		iSizeOfRoom += sizeof(ROOMINFO);
		g_tLVItem.iItem++;
	}
}

VOID CreateRoom(HWND hDlg)
{
	int iSendTot	= 0;
	int iSendSize	= 0;
	LPPACKET_BODY pPacket = NULL;
	LPPACKET_HEADER pHeader = NULL;
	DWORD dwRespBufSize = 0;
	PBYTE pRespBuf = NULL;
	PBYTE pBody = NULL;

	dwRespBufSize = strlen(pPacket->cData) + sizeof(PACKET_HEADER) + 1;
	pRespBuf = new BYTE[dwRespBufSize];
	ZeroMemory(pRespBuf, dwRespBufSize);
	pHeader = (LPPACKET_HEADER)pRespBuf;
	pBody = pRespBuf + sizeof(PACKET_HEADER);

	pPacket = new PACKET_BODY;
	ZeroMemory(pPacket, sizeof(PACKET_BODY));

	GetDlgItemTextA(hDlg, IDC_EDIT_ROOMNAME, (LPSTR)pPacket->cData, SMALLBUF+1);

	//header setting
	pHeader->iFlag = WSABUFFER_ROOMNAME;
	pHeader->iSize = strlen(pPacket->cData) + sizeof(PACKET_HEADER);

	//body setting
	memcpy((LPSTR)pBody, pPacket->cData, strlen(pPacket->cData));

	do 
	{
		iSendSize = send(g_hWRSock, (char*)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
		if (iSendSize == SOCKET_ERROR)
		{
			printf("send()\n");
			break;
		}
		iSendTot += iSendSize;
	} while ( pHeader->iSize != iSendTot );

	SetFocus(g_hEdit);
	SendMessage(g_hEdit, EM_SETSEL, 0, -1);
	SendMessage(g_hEdit, EM_REPLACESEL, NULL, (LPARAM)"");

	delete [] pRespBuf;
	pRespBuf = NULL;
	
	delete pPacket;
	return;	
}

BOOL CALLBACK DlgProc_MakingRoom(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch ( uMsg )
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				CreateRoom(hDlg);
				EndDialog(hDlg, 0);
			}
			return TRUE;

		case IDCANCEL:
			{
				EndDialog(hDlg, 0);
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}

VOID JoinInTheRoom()
{
	int				iSendTot		= 0;
	int				iSendLen		= 0;
	int				iSaveRoomNum	= 0;
	LPPACKET_HEADER pHeader			= NULL;
	DWORD			dwRespBufSize	= 0;
	PBYTE			pRespBuf		= NULL;
	PBYTE			pBody			= NULL;
	
	dwRespBufSize = sizeof(PACKET_HEADER) + sizeof(int) + 1;
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
		iSendLen = send(g_hWRSock, (LPSTR)pHeader + iSendTot, pHeader->iSize - iSendTot, NULL);
		if ( iSendLen == SOCKET_ERROR )
		{
			printf("send()\n");
			continue;
		}
		iSendTot += iSendLen;
	} while ( pHeader->iSize != iSendTot );
	
	delete [] pRespBuf;
	pRespBuf = NULL;
}



int GetRoomNumber()
{
	int		iBuflength		= 0;
	WCHAR	wBuf[SMALLBUF]	= {0,};

	iBuflength = wcslen(wBuf);

	ListView_GetItemText(g_hList, g_lpNIA->iItem, 1, wBuf, 255);

	if (iBuflength <= 1)
	{
		g_iSaveRoomNumber = (int) (wBuf[0] - '0');
	}
	else
	{
		g_iSaveRoomNumber--;
		g_iSaveRoomNumber += (int)(wBuf[iBuflength - 0] - '0');
		g_iSaveRoomNumber += (int)((wBuf[iBuflength - 1] - '0') * 10);
		g_iSaveRoomNumber += (int)((wBuf[iBuflength - 2] - '0') * 100);
	}

	return g_iSaveRoomNumber;
}


VOID WaitingRoomReadFunction(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	SOCKET			hSock		= (SOCKET)wParam;
	int				iRecvLen	= 0;
	int				iBodySize	= 0;
	int				iMaxLen		= 0;
	LPPACKET_BODY	pPacket		= NULL;
	LPPACKET_HEADER pHeader		= NULL;
	LPSTR			pBody		= NULL;
	
	
	pPacket = new PACKET_BODY;
	ZeroMemory(pPacket, sizeof(PACKET_BODY));

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
			if ( !pHeader )
			{
				pHeader = (LPPACKET_HEADER) pPacket->cData;
				iMaxLen = pHeader->iSize;
			}
		}
	}

	pBody = (LPSTR)(pPacket->cData + sizeof(PACKET_HEADER));
	iBodySize = pHeader->iSize - sizeof(PACKET_HEADER);


	switch ( pHeader->iFlag )
	{
	case WSABUFFER_RENEW:
		{
			ViewRoomList(pBody, iBodySize);
		}
		break;

	case WSABUFFER_JOIN:
		{
		}
		break;

	case WSABUFFER_FULL:
		{
			MessageBox(hDlg, _T("Can't Join this room"), NULL, MB_OK);
		}
		break;

	case WSABUFFER_NOTFULL:
		{
			EndDialog(hDlg, 0);
		}
		break;
	}
}

VOID SendingRenew()
{
	int				iSendByte = 0;
	LPPACKET_HEADER pHeader = {0,};

	pHeader = new PACKET_HEADER;
	ZeroMemory(pHeader, sizeof(PACKET_HEADER));
	
	pHeader->iFlag = WSABUFFER_RENEW;

	iSendByte = send(g_hWRSock, (LPSTR)pHeader, sizeof(PACKET_HEADER), NULL);
}