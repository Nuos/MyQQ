#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "RegisterDlg.h"
#include "AllstructinThis.h"
/*
//������
struct MsgInfor
{
	int MsgType;
	char Msg[1024*6];
};*/
//仲

struct Infor
{
	char Id[255];
	char Name[255];
	char Passwd[255];
};

Infor Myinfor;
SOCKET Socket;
bool IdCanUse = false;
bool CanConnect = false;
bool CanClickR = true;
bool CanClick = true;


BOOL WINAPI RegisterDlg_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG, RegisterDlg_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, RegisterDlg_OnCommand);
	HANDLE_MSG(hWnd,WM_CLOSE, RegisterDlg_OnClose);
    }

    return FALSE;
}

BOOL RegisterDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	char IpAddress[255];
	strcpy(IpAddress,(char*)lParam);
	//MessageBox(hwnd,IpAddress,"",0);
	if ( LinkServer(hwnd,IpAddress) )
	{
		CanConnect = true;
	}
    return TRUE;
}

void RegisterDlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch(id)
    {
        case IDOK://ע�ᰴť
		{
			//����ע����Ϣ�Ƿ�ϸ�
			if ( !CheckUpInfor(hwnd) )
			{
				return;
			}
			CreateThread(NULL,0,Register,hwnd,0,NULL);
		}
        break;
		case IDC_BT_CHEEKUP://�����û����Ƿ�ϸ�
			{
				CheckUpUserId(hwnd);
			}
			break;
        default:
		break;
    }
}

void RegisterDlg_OnClose(HWND hwnd)
{
    EndDialog(hwnd, 0);
}


//ע��
bool LinkServer(HWND hwnd,char* IpAddress)
{
	
	//ת��ip
	unsigned long ip;
	int  i_Port = 8000;
	if( (ip = inet_addr(IpAddress)) == INADDR_NONE )
	{ 
		//char Error[255];
		//sprintf(Error,"���Ϸ���IP��ַ"); 
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"������ip��ַ����");
		return false; 
	} 
	//��ʼ��server��Ϣ
	struct sockaddr_in serverAddress; 
	serverAddress.sin_family=AF_INET; 
	serverAddress.sin_addr.S_un.S_addr = ip; 
	serverAddress.sin_port = htons(i_Port);
	
	//�����׽��� 
	if( (Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == INVALID_SOCKET )
	{ 
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"�����׽���ʧ�ܣ�");
		return false; 
	} 

	//���ӷ�������¼
	if( connect(Socket,(sockaddr*)&serverAddress,sizeof(serverAddress)) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"��������ʧ�ܣ�");
		return false; 
	}

	return true;
		
}


//����ע��
DWORD WINAPI Register(LPVOID lpParam)
{
	HWND hwnd = (HWND)lpParam;

	//�ж�������������Ƿ�ɹ�
	if ( !CanConnect )
	{
		return 0;
	}

	//�ж��û����ɲ�����
	if ( !IdCanUse )
	{
		CheckUpUserId(hwnd);
	}

	if ( !IdCanUse )
	{
		return 0;
	}

	
	
	SetDlgItemText(hwnd,IDC_STATIC_ERROR,"���ӷ������ɹ�����ע��..");

	MsgInfor msg;

	char SendStr[1024];
	sprintf(SendStr,"-%s-%s-%s-0",Myinfor.Id,Myinfor.Name,Myinfor.Passwd);
	char Url_SendStr[1024*6];
	EncodeUrl( Url_SendStr,SendStr );

	msg.MsgType = 0;
	strcpy(msg.Msg,Url_SendStr);
	//���͵�¼��Ϣ
	if( send(Socket,(char*)&msg,sizeof(msg),0) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"��������ʧ��!");
		return 0; 
	}
	

	//���յ�¼��Ϣ
	memset(&msg,0,sizeof(msg));	
	if( recv(Socket,(char*)&msg,sizeof(msg),0) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"��������ʧ��!");
		return 0; 
	}
	
	switch( msg.MsgType )
	{
		
	case 0://ע��ʧ��
		{
			SetDlgItemText(hwnd,IDC_STATIC_ERROR,"ע��ʧ��,�����ǵ�ǰע����û�̫��!");
		}
		break;
	case 1://ע��ɹ�
		{
			MessageBox(hwnd, "ע��ɹ�!","ע��ɹ�",0);
			SetDlgItemText(hwnd,IDC_STATIC_ERROR,"ע��ɹ�!");
			EndDialog(hwnd, 0);
		}
		break;	
	}
	return 0;
}


//��֤�û����Ƿ����
DWORD  CheckUpUserId(HWND hwnd)
{
//	HWND hwnd = (HWND)lpParam;
	if ( !CanConnect )
	{
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"�޷���֤ID�Ƿ����");
		return 0;
	}

	GetDlgItemText(hwnd,IDC_EDIT_ID,Myinfor.Id,sizeof(Myinfor.Id));
	if ( !strcmp(Myinfor.Id,"") )
	{
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"�û�������Ϊ��");
		return 0;
	}

	MsgInfor msg;

	char SendStr[1024];
	sprintf(SendStr,"-%s",Myinfor.Id);
	char Url_SendStr[1024*6];
	EncodeUrl( Url_SendStr,SendStr );

	msg.MsgType = 2;
	strcpy(msg.Msg,Url_SendStr);
	//���͵�¼��Ϣ
	if( send(Socket,(char*)&msg,sizeof(msg),0) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"��������ʧ��!");
		return 0; 
	}
	

	//���յ�¼��Ϣ
	memset(&msg,0,sizeof(msg));	
	if( recv(Socket,(char*)&msg,sizeof(msg),0) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"��������ʧ��!");
		return 0; 
	}

	if ( msg.MsgType == 0 )
	{
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"ID������");
		IdCanUse = false;
	}
	else
	{
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"ID����");
		IdCanUse = true;
	}
	return 0;
	
}



//�˲���Ϣ���޴���
bool CheckUpInfor(HWND hwnd)
{
	//�û���
	//char Id[255];
	GetDlgItemText(hwnd,IDC_EDIT_ID,Myinfor.Id,sizeof(Myinfor.Id));
	if ( !strcmp(Myinfor.Id,"") )
	{
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"�û�ID����Ϊ��");
		return false;
	}
	GetDlgItemText(hwnd,IDC_EDIT_NAME,Myinfor.Name,sizeof(Myinfor.Name));
	if ( !strcmp(Myinfor.Name,"") )
	{
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"�û�������Ϊ��");
		return false;
	}
	//����
	char Passwd2[255];
	GetDlgItemText(hwnd,IDC_EDIT_PASSWD1,Myinfor.Passwd,sizeof(Myinfor.Passwd));
	GetDlgItemText(hwnd,IDC_EDIT_PASSWD2,Passwd2,sizeof(Passwd2));
	if ( strcmp(Myinfor.Passwd,Passwd2) )
	{
		//��ʾ���벻�ϸ�
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"�������벻��ͬ!");
		return false;	
	}

	if ( !strcmp(Myinfor.Passwd,"") )
	{
		//��ʾ���벻�ϸ�
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"������Ϊ��!");
		return false;
	}

	SetDlgItemText(hwnd,IDC_STATIC_ERROR,"��Ϣ�ϸ�");
	//����ϸ�
	return true;
}
