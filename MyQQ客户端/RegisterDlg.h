

#ifndef _FindDlg_H
#define _FindDlg_H

#include <windows.h>


BOOL WINAPI RegisterDlg_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL RegisterDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

void RegisterDlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

void RegisterDlg_OnClose(HWND hwnd);

bool CheckUpInfor(HWND hwnd);

bool LinkServer(HWND hwnd,char* IpAddress);

DWORD WINAPI Register(LPVOID lpParam);

DWORD  CheckUpUserId(HWND hwnd);

#endif