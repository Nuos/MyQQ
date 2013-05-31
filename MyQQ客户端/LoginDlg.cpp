#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "LoginDlg.h"
#include "AllstructinThis.h"
#include "RegisterDlg.h"
#include "SetIcon.h"

//const int MaxSend = 1024;

/*
//������
struct MsgInfor
{
	int MsgType;
	char Msg[MaxSend*6];
};*/

//��ǰ���ڵ�ȫ�ֱ���
bool SetSerAdd = false;
UserInfor* MyInfor=NULL;
bool OnLogin = false;


void SendAndRecv(HWND hwnd);


//��ǰ���ڵ�����Ϣ����
BOOL WINAPI Login_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
    switch(uMsg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG, Login_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, Login_OnCommand);
		HANDLE_MSG(hWnd,WM_CLOSE, Login_OnClose);
	case IDM_MINMSG:
		ToolIconMsg(hWnd,lParam);
		break;
	case WM_SYSCOMMAND:
		if ( SC_MINIMIZE == wParam )
		{	
			SetToolIcon(hWnd,"MyQQ1.0��");
			//MessageBox(NULL,"dd","dd",0);
			return true;
		}
		break;
    }

    return FALSE;
}


//��ǰ���ڵļ��غ���
BOOL Login_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	
	//���ô����С
	SetWindowSize(hwnd,0,(-150));
	//��ʼ����������ַ���˿�
	SetDlgItemText(hwnd,IDC_IPADDRESS,"127.0.0.1");
	SetDlgItemText(hwnd,IDC_EDIT_PORT,"8000");
	//��ʼ��socket dll
	SetDLL(hwnd);

	MyInfor = (UserInfor*)lParam;
	
    return TRUE;
}


//��ǰ���ڰ�ť��Ϣ������
void Login_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch(id)
    {
	case IDC_BUTTON_SET:
		{
			if ( SetSerAdd )
			{
				SetWindowSize(hwnd,0,(-150));
				SetSerAdd = false;
			}
			else
			{
				SetWindowSize(hwnd,0,(150));
				SetSerAdd = true;
			}
		}
		break;
	case IDOK:
		{
			if ( !OnLogin )
			{
				if ( VisitUNP(hwnd) )
				{
					SetDlgItemText(hwnd,IDOK,"ȡ��");
					CreateThread(NULL,0,Login,hwnd,0,NULL);
					OnLogin = true;
				}
			}
			else
			{
				//�رս���
				SetDlgItemText(hwnd,IDOK,"��¼");
				OnLogin = false;
			}	
		}
		break;
	case IDC_BUTTON_REGISTER://ע��
		{
			ShowWindow(hwnd,SW_HIDE);
			char Ip[255];
			GetDlgItemText(hwnd,IDC_IPADDRESS,Ip,sizeof(Ip));
			DialogBoxParam(NULL, MAKEINTRESOURCE(IDD_DIALOG_REGISTER), hwnd, RegisterDlg_Proc,(LPARAM)Ip);
			ShowWindow(hwnd,SW_SHOW);
		}
		break;
	case ID_MENU_DISP:
		ShowWindow(hwnd,SW_SHOW);
		break;
	case ID_MENU_EXIT:
		EndDialog(hwnd, 0);
		break;
    }
}


//��ǰ���ڵĹرպ���
void Login_OnClose(HWND hwnd)
{
    EndDialog(hwnd, 0);
}


//�û��������Ƿ�Ϊ����֤
bool VisitUNP(HWND hwnd)
{
	char uname[25];
	char upasswd[25];
	GetDlgItemText(hwnd,IDC_EDIT_ID,uname,25);
	GetDlgItemText(hwnd,IDC_EDIT_PASSWD,upasswd,25);
	if((!strcmp(uname,""))||(!strcmp(uname,"")))
	{
		SetDlgItemText(hwnd,IDC_LOGSTA,"�û������벻����Ϊ��!");
	}
	else
	{
		strcpy(MyInfor->uId,uname);
		strcpy(MyInfor->upasswd,upasswd);
	}
	return true;

}



//��ʼ��DLL
bool SetDLL(HWND hwnd)
{	
	//��ʼ���׽���DLL 
	WSADATA wsa;	
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{ 
		char Error[255];
		sprintf(Error,"��ʼ��DLLʧ��!"); 
		SetDlgItemText(hwnd,IDC_LOGSTA,Error);
		return false; 
	}
	return true;	
}


//��¼
DWORD WINAPI Login(LPVOID lpParam)
{
	HWND hwnd = (HWND)lpParam;
	char ServerAdd[255];
	char c_Port[255];
	int  i_Port;
	GetDlgItemText(hwnd,IDC_IPADDRESS,ServerAdd,sizeof(ServerAdd));
	GetDlgItemText(hwnd,IDC_EDIT_PORT,c_Port,sizeof(c_Port));
	i_Port = atoi(c_Port);
	
	//ת��ip
	unsigned long ip;
	if( (ip = inet_addr(ServerAdd)) == INADDR_NONE )
	{ 
		char Error[255];
		sprintf(Error,"���Ϸ���IP��ַ"); 
		SetDlgItemText(hwnd,IDC_LOGSTA,Error);
		return false; 
	} 
	//��ʼ��server��Ϣ
	struct sockaddr_in serverAddress; 
	serverAddress.sin_family=AF_INET; 
	serverAddress.sin_addr.S_un.S_addr = ip; 
	serverAddress.sin_port = htons(i_Port);

	//�����׽��� 
	if( (MyInfor->Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == INVALID_SOCKET )
	{ 
		SetDlgItemText(hwnd,IDC_LOGSTA,"�����׽���ʧ�ܣ�");
		return false; 
	} 

	//���ӷ�������¼
	if( connect(MyInfor->Socket,(sockaddr*)&serverAddress,sizeof(serverAddress)) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_LOGSTA,"��������ʧ�ܣ�");
		return false; 
	}
	SetDlgItemText(hwnd,IDC_LOGSTA,"���ӷ������ɹ�������֤����..");
	SendAndRecv(hwnd);

	return 0;
}



//���е�¼
void SendAndRecv(HWND hwnd)
{
	MsgInfor msg;

	char SendStr[MaxSend];
	sprintf(SendStr,"-%s-%s",MyInfor->uId,MyInfor->upasswd);
	char Url_SendStr[MaxSend*6];
	EncodeUrl( Url_SendStr,SendStr );

	msg.MsgType = 1;
	strcpy(msg.Msg,Url_SendStr);
	//���͵�¼��Ϣ
	if( send(MyInfor->Socket,(char*)&msg,sizeof(msg),0) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_LOGSTA,"��������ʧ��!");
		return ; 
	}
	
	SetDlgItemText(hwnd,IDC_LOGSTA,"��֤����..");

	//���յ�¼��Ϣ
	memset(&msg,0,sizeof(msg));	
	if( recv(MyInfor->Socket,(char*)&msg,sizeof(msg),0) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_LOGSTA,"��������ʧ��!");
		return ; 
	}
	
	switch( msg.MsgType )
	{
	case 0://��¼ʧ��
		{
			SetDlgItemText(hwnd,IDOK,"��¼");
			SetDlgItemText(hwnd,IDC_LOGSTA,"�û������������");
			closesocket(MyInfor->Socket);
		}
		break;
	case 1://��¼�ɹ�
		{
			//������Ϣ
			char MyMsg[MaxSend];
			CodeUrl(MyMsg,msg.Msg);	
			sscanf(MyMsg,"-%[^-]-%[^-]",MyInfor->uName,MyInfor->uico);
			//MessageBox(hwnd,MyMsg,"",0);
			EndDialog(hwnd, 1);
		}
		break;
	}
	
}
