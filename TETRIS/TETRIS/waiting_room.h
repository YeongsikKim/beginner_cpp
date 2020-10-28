#pragma once

BOOL CALLBACK DlgProc_Waiting(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID InitProc_Waiting(HWND hDlg);
VOID ProcessSocketMessage_Room(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
VOID ViewRoomList(char *buf);
VOID CreateRoom(HWND hDlg);
BOOL CALLBACK DlgProc_MakingRoom(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID JoinInTheRoom(int iItem);
