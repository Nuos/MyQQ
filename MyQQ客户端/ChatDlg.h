#ifndef _ChatDlg_H
#define _ChatDlg_H

#include <windows.h>

BOOL WINAPI ChatDlg_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL ChatDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void ChatDlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void ChatDlg_OnClose(HWND hwnd);
bool SendMsg(HWND hwnd,int MsgType,char * str);
DWORD WINAPI LookrecvList(LPVOID lpParam);
void RefreshScreen(HWND hwnd,char *show);
char * getLocalTime();

#endif