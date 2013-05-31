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


//��ǰ���ڵ�ȫ�ֱ���
MsgSendToDlg * FriendInfor = NULL;


void LoadFormInfor(HWND hwnd,char* UserId);
void FreshMsgLog(HWND hwnd,char* str);
//���ڶ���
DWORD WINAPI  DlgShake(LPVOID lpParam);



BOOL WINAPI ChatDlg_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG, ChatDlg_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, ChatDlg_OnCommand);
		HANDLE_MSG(hWnd,WM_CLOSE, ChatDlg_OnClose);
		case WM_COPYDATA://�Ӵ��ڷ��;����Ϣ
		{
			//������Ϣ
			COPYDATASTRUCT *copydata =(COPYDATASTRUCT*) lParam;
			char MsgStr[1024];
			if (!strcmp((char*)copydata->lpData,"�����˴��ڶ���"))
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


//���ý�����Ϣ������
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
	//��ʾ��������
	SetDlgItemText(hwnd,IDC_EDITRECV,MsgLog);
	RefreshScreen(hwnd,MsgLog);
}



BOOL ChatDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	//������������Ϣ
	FriendInfor = (MsgSendToDlg*)lParam;
	if ( FriendInfor == NULL )
	{
		EndDialog(hwnd, 0);
	}
	//�������巢����Ϣ
	//char UserId [25];
	char UserId[25];//���������û���id
	memset(UserId,0,sizeof(UserId));
	itoa(FriendInfor->id,UserId,10);
	COPYDATASTRUCT CopyData;//���ڴ򿪺ʹ��ڹر�ʱ�������ڷ��͵���Ϣ
	CopyData.cbData = 0;
	CopyData.dwData = 25;
	CopyData.lpData = UserId;
	SendMessage(FriendInfor->hwnd,WM_COPYDATA,(WPARAM)hwnd,(LPARAM)&CopyData);

	//��ʼ��������Ϣ ��־
	//MsgLog[0] = '\0';

	//���ش�����Ϣ
	LoadFormInfor(hwnd,UserId);

    return TRUE;
}

void LoadFormInfor(HWND hwnd,char* UserId)
{
	//���ش������Ϣ
	SetDlgItemText(hwnd,IDC_UNAME,FriendInfor->name);
	SetDlgItemText(hwnd,IDC_UID,UserId);
	//���ñ���
	char DlgText[255];
	sprintf(DlgText,"��%s����",FriendInfor->name);
	SetWindowText(hwnd,DlgText);
}

void ChatDlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	
    switch(id)
    {
        case IDOK:
		{
			//ȡ����Ϣ
			char sendmsg[1024];
			GetDlgItemText(hwnd,IDC_EDITSEND,sendmsg,sizeof(sendmsg));

			//����Ƿ�Ϊ��
			if(!strcmp(sendmsg,""))
			{
				MessageBox(hwnd,"������Ϣ����Ϊ��","",0);
				return;
			}

			//������Ϣ
			SendMsg(hwnd,4,sendmsg);
		}
        break;
		case IDC_BUTTON_SHAKE://���ڶ���
		{
			if (SendMsg(hwnd,5,"�����˴��ڶ���"))
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
	COPYDATASTRUCT CopyData;//���ڴ򿪺ʹ��ڹر�ʱ�������ڷ��͵���Ϣ
	CopyData.cbData = 0;
	CopyData.dwData = 25;
	CopyData.lpData = UserId;
	SendMessage(FriendInfor->hwnd,WM_COPYDATA,(WPARAM)hwnd,(LPARAM)&CopyData);
    EndDialog(hwnd, 0);
}

//���ڶ���
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
//������Ļ
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

//������Ϣ����
bool SendMsg(HWND hwnd,int MsgType,char * str)
{
	char UserId[255];
	GetDlgItemText(hwnd,IDC_UID,UserId,sizeof(UserId));
	//���÷�����Ϣ(-id-msg)
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
	 
	//���÷�����Ϣ��ʽ
	MsgInfor Send_Msg;
	memset(&Send_Msg,0,sizeof(Send_Msg));
	Send_Msg.MsgType = MsgType;
	EncodeUrl( Send_Msg.Msg,SendStr );
	//������Ϣ
	if( send(FriendInfor->socket,(char*)&Send_Msg,sizeof(Send_Msg),0) == SOCKET_ERROR )
	{ 
		MessageBox(NULL,"������Ϣʧ��","",0);
		return false; 
	}
	//���������¼
	char SendMsgStr[1024];
	sprintf(SendMsgStr,"��:%s",str);
	FreshMsgLog(hwnd,SendMsgStr);
	//��������
	if(MsgType == 4)
	{
		SetDlgItemText(hwnd,IDC_EDITSEND,"");
	}	
	return true;
}


