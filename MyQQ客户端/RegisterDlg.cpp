#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "RegisterDlg.h"
#include "AllstructinThis.h"
/*
//机构体
struct MsgInfor
{
	int MsgType;
	char Msg[1024*6];
};*/

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
        case IDOK://注册按钮
		{
			//检验注册信息是否合格
			if ( !CheckUpInfor(hwnd) )
			{
				return;
			}
			CreateThread(NULL,0,Register,hwnd,0,NULL);
		}
        break;
		case IDC_BT_CHEEKUP://检验用户名是否合格
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


//注册
bool LinkServer(HWND hwnd,char* IpAddress)
{
	
	//转换ip
	unsigned long ip;
	int  i_Port = 8000;
	if( (ip = inet_addr(IpAddress)) == INADDR_NONE )
	{ 
		//char Error[255];
		//sprintf(Error,"不合法的IP地址"); 
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"服务器ip地址错误");
		return false; 
	} 
	//初始化server信息
	struct sockaddr_in serverAddress; 
	serverAddress.sin_family=AF_INET; 
	serverAddress.sin_addr.S_un.S_addr = ip; 
	serverAddress.sin_port = htons(i_Port);
	
	//创建套接字 
	if( (Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == INVALID_SOCKET )
	{ 
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"创建套接字失败！");
		return false; 
	} 

	//连接服务器登录
	if( connect(Socket,(sockaddr*)&serverAddress,sizeof(serverAddress)) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"建立连接失败！");
		return false; 
	}

	return true;
		
}


//进行注册
DWORD WINAPI Register(LPVOID lpParam)
{
	HWND hwnd = (HWND)lpParam;

	//判断与服务器连接是否成功
	if ( !CanConnect )
	{
		return 0;
	}

	//判断用户名可不可用
	if ( !IdCanUse )
	{
		CheckUpUserId(hwnd);
	}

	if ( !IdCanUse )
	{
		return 0;
	}

	
	
	SetDlgItemText(hwnd,IDC_STATIC_ERROR,"连接服务器成功正在注册..");

	MsgInfor msg;

	char SendStr[1024];
	sprintf(SendStr,"-%s-%s-%s-0",Myinfor.Id,Myinfor.Name,Myinfor.Passwd);
	char Url_SendStr[1024*6];
	EncodeUrl( Url_SendStr,SendStr );

	msg.MsgType = 0;
	strcpy(msg.Msg,Url_SendStr);
	//发送登录信息
	if( send(Socket,(char*)&msg,sizeof(msg),0) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"发送数据失败!");
		return 0; 
	}
	

	//接收登录信息
	memset(&msg,0,sizeof(msg));	
	if( recv(Socket,(char*)&msg,sizeof(msg),0) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"接收数据失败!");
		return 0; 
	}
	
	switch( msg.MsgType )
	{
		
	case 0://注册失败
		{
			SetDlgItemText(hwnd,IDC_STATIC_ERROR,"注册失败,可能是当前注册的用户太多!");
		}
		break;
	case 1://注册成功
		{
			MessageBox(hwnd, "注册成功!","注册成功",0);
			SetDlgItemText(hwnd,IDC_STATIC_ERROR,"注册成功!");
			EndDialog(hwnd, 0);
		}
		break;	
	}
	return 0;
}


//验证用户名是否可用
DWORD  CheckUpUserId(HWND hwnd)
{
//	HWND hwnd = (HWND)lpParam;
	if ( !CanConnect )
	{
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"无法验证ID是否可用");
		return 0;
	}

	GetDlgItemText(hwnd,IDC_EDIT_ID,Myinfor.Id,sizeof(Myinfor.Id));
	if ( !strcmp(Myinfor.Id,"") )
	{
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"用户名不能为空");
		return 0;
	}

	MsgInfor msg;

	char SendStr[1024];
	sprintf(SendStr,"-%s",Myinfor.Id);
	char Url_SendStr[1024*6];
	EncodeUrl( Url_SendStr,SendStr );

	msg.MsgType = 2;
	strcpy(msg.Msg,Url_SendStr);
	//发送登录信息
	if( send(Socket,(char*)&msg,sizeof(msg),0) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"发送数据失败!");
		return 0; 
	}
	

	//接收登录信息
	memset(&msg,0,sizeof(msg));	
	if( recv(Socket,(char*)&msg,sizeof(msg),0) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"接收数据失败!");
		return 0; 
	}

	if ( msg.MsgType == 0 )
	{
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"ID不可用");
		IdCanUse = false;
	}
	else
	{
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"ID可用");
		IdCanUse = true;
	}
	return 0;
	
}



//核查信息有无错误
bool CheckUpInfor(HWND hwnd)
{
	//用户名
	//char Id[255];
	GetDlgItemText(hwnd,IDC_EDIT_ID,Myinfor.Id,sizeof(Myinfor.Id));
	if ( !strcmp(Myinfor.Id,"") )
	{
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"用户ID不能为空");
		return false;
	}
	GetDlgItemText(hwnd,IDC_EDIT_NAME,Myinfor.Name,sizeof(Myinfor.Name));
	if ( !strcmp(Myinfor.Name,"") )
	{
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"用户名不能为空");
		return false;
	}
	//密码
	char Passwd2[255];
	GetDlgItemText(hwnd,IDC_EDIT_PASSWD1,Myinfor.Passwd,sizeof(Myinfor.Passwd));
	GetDlgItemText(hwnd,IDC_EDIT_PASSWD2,Passwd2,sizeof(Passwd2));
	if ( strcmp(Myinfor.Passwd,Passwd2) )
	{
		//提示密码不合格
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"两次密码不相同!");
		return false;	
	}

	if ( !strcmp(Myinfor.Passwd,"") )
	{
		//提示密码不合格
		SetDlgItemText(hwnd,IDC_STATIC_ERROR,"有密码为空!");
		return false;
	}

	SetDlgItemText(hwnd,IDC_STATIC_ERROR,"信息合格");
	//密码合格
	return true;
}