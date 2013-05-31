#ifndef _Login_H
#define _Login_H

#include <windows.h>

BOOL WINAPI Login_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL Login_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void Login_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void Login_OnClose(HWND hwnd);
DWORD WINAPI Login(LPVOID lpParam);
bool SetDLL(HWND hwnd);
bool SetSocket(HWND hwnd);
bool ClientSocket(HWND hwnd);
bool VisitUNP(HWND hwnd);

#endif