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
//机构体
struct MsgInfor
{
	int MsgType;
	char Msg[MaxSend*6];
};*/

//当前窗口的全局变量
bool SetSerAdd = false;
UserInfor* MyInfor=NULL;
bool OnLogin = false;


void SendAndRecv(HWND hwnd);


//当前窗口的主消息函数
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
			SetToolIcon(hWnd,"MyQQ1.0版");
			//MessageBox(NULL,"dd","dd",0);
			return true;
		}
		break;
    }

    return FALSE;
}


//当前窗口的加载函数
BOOL Login_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	
	//设置窗体大小
	SetWindowSize(hwnd,0,(-150));
	//初始化服务器地址及端口
	SetDlgItemText(hwnd,IDC_IPADDRESS,"127.0.0.1");
	SetDlgItemText(hwnd,IDC_EDIT_PORT,"8000");
	//初始化socket dll
	SetDLL(hwnd);

	MyInfor = (UserInfor*)lParam;
	
    return TRUE;
}


//当前窗口按钮信息处理函数
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
					SetDlgItemText(hwnd,IDOK,"取消");
					CreateThread(NULL,0,Login,hwnd,0,NULL);
					OnLogin = true;
				}
			}
			else
			{
				//关闭进程
				SetDlgItemText(hwnd,IDOK,"登录");
				OnLogin = false;
			}	
		}
		break;
	case IDC_BUTTON_REGISTER://注册
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


//当前窗口的关闭函数
void Login_OnClose(HWND hwnd)
{
    EndDialog(hwnd, 0);
}


//用户名密码是否为空验证
bool VisitUNP(HWND hwnd)
{
	char uname[25];
	char upasswd[25];
	GetDlgItemText(hwnd,IDC_EDIT_ID,uname,25);
	GetDlgItemText(hwnd,IDC_EDIT_PASSWD,upasswd,25);
	if((!strcmp(uname,""))||(!strcmp(uname,"")))
	{
		SetDlgItemText(hwnd,IDC_LOGSTA,"用户名密码不允许为空!");
	}
	else
	{
		strcpy(MyInfor->uId,uname);
		strcpy(MyInfor->upasswd,upasswd);
	}
	return true;

}



//初始化DLL
bool SetDLL(HWND hwnd)
{	
	//初始化套接字DLL 
	WSADATA wsa;	
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{ 
		char Error[255];
		sprintf(Error,"初始化DLL失败!"); 
		SetDlgItemText(hwnd,IDC_LOGSTA,Error);
		return false; 
	}
	return true;	
}


//登录
DWORD WINAPI Login(LPVOID lpParam)
{
	HWND hwnd = (HWND)lpParam;
	char ServerAdd[255];
	char c_Port[255];
	int  i_Port;
	GetDlgItemText(hwnd,IDC_IPADDRESS,ServerAdd,sizeof(ServerAdd));
	GetDlgItemText(hwnd,IDC_EDIT_PORT,c_Port,sizeof(c_Port));
	i_Port = atoi(c_Port);
	
	//转换ip
	unsigned long ip;
	if( (ip = inet_addr(ServerAdd)) == INADDR_NONE )
	{ 
		char Error[255];
		sprintf(Error,"不合法的IP地址"); 
		SetDlgItemText(hwnd,IDC_LOGSTA,Error);
		return false; 
	} 
	//初始化server信息
	struct sockaddr_in serverAddress; 
	serverAddress.sin_family=AF_INET; 
	serverAddress.sin_addr.S_un.S_addr = ip; 
	serverAddress.sin_port = htons(i_Port);

	//创建套接字 
	if( (MyInfor->Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == INVALID_SOCKET )
	{ 
		SetDlgItemText(hwnd,IDC_LOGSTA,"创建套接字失败！");
		return false; 
	} 

	//连接服务器登录
	if( connect(MyInfor->Socket,(sockaddr*)&serverAddress,sizeof(serverAddress)) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_LOGSTA,"建立连接失败！");
		return false; 
	}
	SetDlgItemText(hwnd,IDC_LOGSTA,"连接服务器成功正在验证密码..");
	SendAndRecv(hwnd);

	return 0;
}



//进行登录
void SendAndRecv(HWND hwnd)
{
	MsgInfor msg;

	char SendStr[MaxSend];
	sprintf(SendStr,"-%s-%s",MyInfor->uId,MyInfor->upasswd);
	char Url_SendStr[MaxSend*6];
	EncodeUrl( Url_SendStr,SendStr );

	msg.MsgType = 1;
	strcpy(msg.Msg,Url_SendStr);
	//发送登录信息
	if( send(MyInfor->Socket,(char*)&msg,sizeof(msg),0) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_LOGSTA,"发送数据失败!");
		return ; 
	}
	
	SetDlgItemText(hwnd,IDC_LOGSTA,"验证密码..");

	//接收登录信息
	memset(&msg,0,sizeof(msg));	
	if( recv(MyInfor->Socket,(char*)&msg,sizeof(msg),0) == SOCKET_ERROR )
	{ 
		SetDlgItemText(hwnd,IDC_LOGSTA,"接收数据失败!");
		return ; 
	}
	
	switch( msg.MsgType )
	{
	case 0://登录失败
		{
			SetDlgItemText(hwnd,IDOK,"登录");
			SetDlgItemText(hwnd,IDC_LOGSTA,"用户名或密码错误");
			closesocket(MyInfor->Socket);
		}
		break;
	case 1://登录成功
		{
			//解码消息
			char MyMsg[MaxSend];
			CodeUrl(MyMsg,msg.Msg);	
			sscanf(MyMsg,"-%[^-]-%[^-]",MyInfor->uName,MyInfor->uico);
			//MessageBox(hwnd,MyMsg,"",0);
			EndDialog(hwnd, 1);
		}
		break;
	}
	
}
