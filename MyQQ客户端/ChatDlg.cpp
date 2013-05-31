#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include "resource.h"
#include "ChatDlg.h"
#include "MainDlg.h"
#include "AllstructinThis.h"
#include <list>
using namespace std;


//当前窗口的全局变量
MsgSendToDlg * FriendInfor = NULL;


void LoadFormInfor(HWND hwnd,char* UserId);
void FreshMsgLog(HWND hwnd,char* str);
//窗口抖动
DWORD WINAPI  DlgShake(LPVOID lpParam);



BOOL WINAPI ChatDlg_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG, ChatDlg_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, ChatDlg_OnCommand);
		HANDLE_MSG(hWnd,WM_CLOSE, ChatDlg_OnClose);
		case WM_COPYDATA://子窗口发送句柄信息
		{
			//接收信息
			COPYDATASTRUCT *copydata =(COPYDATASTRUCT*) lParam;
			char MsgStr[1024];
			if (!strcmp((char*)copydata->lpData,"发送了窗口抖动"))
			{
				CreateThread(NULL,0,DlgShake,hWnd,0,NULL);
				sprintf(MsgStr,"%s:%s",FriendInfor->name,(char*)copydata->lpData);	
			}
			else
			{
				sprintf(MsgStr,"%s:%s",FriendInfor->name,(char*)copydata->lpData);
			}
			FreshMsgLog(hWnd,MsgStr);
		}
		break;
    }

    return FALSE;
}


//设置接收消息框内容
void FreshMsgLog(HWND hwnd,char* str)
{
	char MsgLog[1024*6];
	GetDlgItemText(hwnd,IDC_EDITRECV,MsgLog,sizeof(MsgLog));
	if (str == NULL)
	{
		return;
	}
	if (strlen(MsgLog) >= 1024*5)
	{
		sprintf(MsgLog,"%s\r\n%s",&MsgLog[1024],str);
	}
	else
	{
		sprintf(MsgLog,"%s\r\n%s",MsgLog,str);
	}
	//显示到窗体上
	SetDlgItemText(hwnd,IDC_EDITRECV,MsgLog);
	RefreshScreen(hwnd,MsgLog);
}



BOOL ChatDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	//接收主窗体信息
	FriendInfor = (MsgSendToDlg*)lParam;
	if ( FriendInfor == NULL )
	{
		EndDialog(hwnd, 0);
	}
	//给主窗体发送消息
	//char UserId [25];
	char UserId[25];//用来保存用户的id
	memset(UserId,0,sizeof(UserId));
	itoa(FriendInfor->id,UserId,10);
	COPYDATASTRUCT CopyData;//窗口打开和窗口关闭时给父窗口发送的消息
	CopyData.cbData = 0;
	CopyData.dwData = 25;
	CopyData.lpData = UserId;
	SendMessage(FriendInfor->hwnd,WM_COPYDATA,(WPARAM)hwnd,(LPARAM)&CopyData);

	//初始化窗体消息 日志
	//MsgLog[0] = '\0';

	//加载窗体信息
	LoadFormInfor(hwnd,UserId);

    return TRUE;
}

void LoadFormInfor(HWND hwnd,char* UserId)
{
	//加载窗体的信息
	SetDlgItemText(hwnd,IDC_UNAME,FriendInfor->name);
	SetDlgItemText(hwnd,IDC_UID,UserId);
	//设置标题
	char DlgText[255];
	sprintf(DlgText,"与%s聊天",FriendInfor->name);
	SetWindowText(hwnd,DlgText);
}

void ChatDlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	
    switch(id)
    {
        case IDOK:
		{
			//取得消息
			char sendmsg[1024];
			GetDlgItemText(hwnd,IDC_EDITSEND,sendmsg,sizeof(sendmsg));

			//检查是否为空
			if(!strcmp(sendmsg,""))
			{
				MessageBox(hwnd,"发送消息不能为空","",0);
				return;
			}

			//发送消息
			SendMsg(hwnd,4,sendmsg);
		}
        break;
		case IDC_BUTTON_SHAKE://窗口抖动
		{
			if (SendMsg(hwnd,5,"发送了窗口抖动"))
			{
				CreateThread(NULL,0,DlgShake,hwnd,0,NULL);
			}
		}
		break;
        default:
		break;
    }
}

void ChatDlg_OnClose(HWND hwnd)
{
	char UserId[255];
	GetDlgItemText(hwnd,IDC_UID,UserId,sizeof(UserId));
	COPYDATASTRUCT CopyData;//窗口打开和窗口关闭时给父窗口发送的消息
	CopyData.cbData = 0;
	CopyData.dwData = 25;
	CopyData.lpData = UserId;
	SendMessage(FriendInfor->hwnd,WM_COPYDATA,(WPARAM)hwnd,(LPARAM)&CopyData);
    EndDialog(hwnd, 0);
}

//窗口抖动
DWORD WINAPI  DlgShake(LPVOID lpParam)
{
	HWND hwnd = (HWND)lpParam;
	RECT rect;
	memset(&rect,0,sizeof(rect));
	GetWindowRect(hwnd,&rect);
	for(int i = 0; i<100; i++)
	{
		srand(time(NULL));
		int a = 1+rand()%6;
		int b = 1+rand()%6;
		if(a%2 == 0)
		{
			MoveWindow(hwnd,rect.left-a,rect.top,rect.right-rect.left,rect.bottom-rect.top,true);
		}
		else
		{
			MoveWindow(hwnd,rect.left+a,rect.top,rect.right-rect.left,rect.bottom-rect.top,true);
		}

		if(b%2 == 0)
		{
			MoveWindow(hwnd,rect.left,rect.top-b,rect.right-rect.left,rect.bottom-rect.top,true);
		}
		else
		{
			MoveWindow(hwnd,rect.left,rect.top+b,rect.right-rect.left,rect.bottom-rect.top,true);
		}
		//Sleep(200);

		MoveWindow(hwnd,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,true);
	}
	return 0;
}
//滚动屏幕
void RefreshScreen(HWND hwnd,char *show)
{
	int n=0;
	for(int i=0;show[i];i++)
	{
		if(show[i]=='\n')
		{
			n++;
		}
	}
	SendDlgItemMessage(hwnd,IDC_EDITRECV,EM_LINESCROLL,0,n);
}

//发送消息函数
bool SendMsg(HWND hwnd,int MsgType,char * str)
{
	char UserId[255];
	GetDlgItemText(hwnd,IDC_UID,UserId,sizeof(UserId));
	//设置发送消息(-id-msg)
	char SendStr[1024];
	switch(MsgType)
	{
	case 4:
		{
			sprintf(SendStr,"-%s-%s#",UserId,str);
		}
		break;
	case 5:
		{
			sprintf(SendStr,"-%s-%s#",UserId,str);
		}
		break;
	}
	 
	//设置发送消息格式
	MsgInfor Send_Msg;
	memset(&Send_Msg,0,sizeof(Send_Msg));
	Send_Msg.MsgType = MsgType;
	EncodeUrl( Send_Msg.Msg,SendStr );
	//发送消息
	if( send(FriendInfor->socket,(char*)&Send_Msg,sizeof(Send_Msg),0) == SOCKET_ERROR )
	{ 
		MessageBox(NULL,"发送消息失败","",0);
		return false; 
	}
	//设置聊天记录
	char SendMsgStr[1024];
	sprintf(SendMsgStr,"我:%s",str);
	FreshMsgLog(hwnd,SendMsgStr);
	//清空输入框
	if(MsgType == 4)
	{
		SetDlgItemText(hwnd,IDC_EDITSEND,"");
	}	
	return true;
}


