
#include "stdafx.h"
#include "SetIcon.h"
#include "resource.h"
#include "MainDlg.h"

//隐藏到托盘
bool SetToolIcon(HWND hwnd ,char* MsgStr)
{
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
	NOTIFYICONDATA icodata;
	icodata.cbSize = sizeof(icodata);
	icodata.hWnd = hwnd;
	icodata.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE (IDI_ICON3));
	icodata.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	icodata.uID = 1;
	icodata.uCallbackMessage = IDM_MINMSG;
	strcpy(icodata.szTip ,MsgStr);
	Shell_NotifyIcon(NIM_ADD,&icodata);
	ShowWindow(hwnd,SW_HIDE);
	return true;
}

//响应消息
void ToolIconMsg(HWND hWnd, LPARAM lParam)
{
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
	switch(lParam)
	{
	case WM_LBUTTONDBLCLK:
		{
			if ( !ReadMsgList(hWnd) )
			{
				ShowWindow(hWnd,SW_SHOW);
			}
		}
		break;
	
	case WM_RBUTTONDOWN:
		{
			POINT pi;
			memset(&pi,0,sizeof(pi));
			HMENU hmenu = LoadMenu(hInstance,MAKEINTRESOURCE(IDR_MENU_MIN));
			HMENU hmenuTrackPopup = GetSubMenu(hmenu, 0); 
			GetCursorPos(&pi);
			SetForegroundWindow(hWnd);
            TrackPopupMenu(hmenuTrackPopup,TPM_RIGHTALIGN,pi.x,pi.y,0,hWnd,NULL);
		}
		break;
	}
}

//修改托盘图标
void SetIcon(HWND hwnd,int IconId,char* MsgStr)
{
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
	NOTIFYICONDATA icodata;
	icodata.cbSize = sizeof(icodata);
	icodata.hWnd = hwnd;
	icodata.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE (IconId));
	icodata.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	icodata.uID = 1;
	icodata.uCallbackMessage = IDM_MINMSG;
	strcpy(icodata.szTip ,MsgStr);
	Shell_NotifyIcon(NIM_MODIFY,&icodata);
}
/*
//设置动态托盘图标
void SetDIcon(HWND hwnd,int IconNum)
{
	Sleep(500);
	SetIcon(hwnd,IDI_ICON_DEFINE);
	Sleep(500);
	SetIcon(hwnd,IDI_ICON3);
}*/

//右键菜单响应
