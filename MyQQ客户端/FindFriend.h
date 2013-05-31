#ifndef _FindDlg_H
#define _FindDlg_H

#include <windows.h>

BOOL WINAPI FindDlg_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL FindDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void FindDlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void FindDlg_OnClose(HWND hwnd);

#endif