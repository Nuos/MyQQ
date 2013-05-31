#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "FindFriend.h"




BOOL WINAPI FindDlg_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG, FindDlg_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, FindDlg_OnCommand);
		HANDLE_MSG(hWnd,WM_CLOSE, FindDlg_OnClose);
    }

    return FALSE;
}

BOOL FindDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    return TRUE;
}

void FindDlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch(id)
    {
        case IDOK:
		{
			MessageBox(hwnd,TEXT("��ӭ���������� www.RuPeng.com ��ѧ�������ѧϰ����"),TEXT("�ʺ�"),MB_OK);
		}
        break;
        default:
		break;
    }
}

void FindDlg_OnClose(HWND hwnd)
{
    EndDialog(hwnd, 0);
}