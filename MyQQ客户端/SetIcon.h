#ifndef _SetIcon_H
#define _SetIcon_H

#include <windows.h>
#include <Shellapi.h>


bool SetToolIcon(HWND hwnd ,char* MsgStr);
void ToolIconMsg(HWND hWnd, LPARAM lParam);
void SetIcon(HWND hwnd,int IconId,char* MsgStr);


#endif