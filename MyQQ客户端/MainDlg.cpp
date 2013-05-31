#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include < Mmsystem.h>
#include "resource.h"
#include "MainDlg.h"
#include "FindFriend.h"
#include "AllstructinThis.h"
#include "ChatDlg.h"
#include "SetIcon.h"
#include "TreeviewFunction.h"
#include <list>
#include <map>
using namespace std;

#define CLIENT_MESSAGE WM_USER+100
#define ADDFRIEND      WM_USER+101
#define CHATWITHFRIEND WM_USER+102


//对话框列表用户信息结构体
struct DialogInfor
{
	int Id;     //对话框用户的id
	char Name[255]; //对话框用户的name
	HWND hwnd;  //对话框的句柄
};

//消息队列的格式
struct MsgList
{
	int Id;
	char Msg[1024];
};
//好友列表
struct FriendInfor
{
	int Id;
	int IconNum;
	char Name[255];
};

//当前窗口的全局变量
UserInfor* uIformation=NULL;
//list<int> openWinList;

HWND hPage[3];    //页面句柄
HTREEITEM hRoot;  //好友列表句柄
HWND hTreeview;
map <int,HWND> OpenWinList;   //打开的对话框列表
map <int,FriendInfor> Friendlist;
list < MsgList > RecvList;         //收到的未读消息




BOOL WINAPI Main_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG, Main_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, Main_OnCommand);
		HANDLE_MSG(hWnd,WM_CLOSE, Main_OnClose);
	case CLIENT_MESSAGE:
		Main_OnServerMsg(hWnd,wParam,lParam);
		break;
	case  WM_NOTIFY:
		{
			Main_OnSeleChange(hWnd,lParam);//tab选项卡切换
		}
		break;
	case IDM_MINMSG:               //托盘图标消息处理
		{
			ToolIconMsg(hWnd,lParam);
		}
		break;
	case WM_SYSCOMMAND:           //处理最下化消息建立托盘图标
		if ( SC_MINIMIZE == wParam )
		{
			SetToolIcon(hWnd,uIformation->uName);
			return true;
		}
		break;
	case WM_RBUTTONDOWN:
		{
		}
		break;
	case CHATWITHFRIEND:
		{
			char UserName[255];
			strcpy( UserName,(char*)lParam);
			ChatWithFriend(hWnd,UserName);
		}
		break;
	case WM_COPYDATA://子窗口发送句柄信息
		{
			//接受信息
			COPYDATASTRUCT *copydata =(COPYDATASTRUCT*) lParam;
			//查找是否存在
			map <int,HWND>::iterator iter;
			iter = OpenWinList.find (atoi( (char*)copydata->lpData ) );
			if (iter == OpenWinList.end() )
			{
				//未找到添加到窗口列表
				OpenWinList.insert(pair<int,HWND>( atoi( (char*)copydata->lpData ),(HWND)wParam ) );
				//遍历消息队列查过该用户的消息并发送
				ReadMsgToDlg( hWnd,(HWND)wParam,atoi( (char*)copydata->lpData ) );
			}
			else
			{
				//已经存在就需要删除
				OpenWinList.erase( atoi( (char*)copydata->lpData ) );
			}
		}
		break;
    }

    return FALSE;
}

//遍历消息队列查找用户消息并发送
void ReadMsgToDlg(HWND hwnd,HWND hchild,int id)
{
	list<MsgList>::iterator Msgiter =RecvList.begin();
	while( Msgiter != RecvList.end() )
	{
		if (Msgiter->Id == id)
		{
			//找到给窗口发送消息
			COPYDATASTRUCT CopyData;
			CopyData.cbData = 0;
			CopyData.dwData = sizeof(Msgiter->Msg);
			CopyData.lpData = Msgiter->Msg;
			SendMessage(hchild,WM_COPYDATA,(WPARAM)hwnd,(LPARAM)&CopyData);
			RecvList.erase(Msgiter);
			Msgiter = RecvList.begin();
		}
		else
		{
			Msgiter++;
		}
	}
}

//treeview消息函数
BOOL WINAPI FriendTree_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
    {
	case WM_COMMAND:
		{
			switch(wParam)
			{
				case ID_MENU_SENDMSG://右键菜单 发送消息
					{
						//得到当前选中项的文本值
						char UserName[255];
						if (GetTreeSelectItemText(hWnd,IDC_FRTREE,UserName))
						{
							//发送给主窗体
							HWND hparent = GetParent(hWnd);  //tab句柄
							SendMessage( GetParent(hparent),CHATWITHFRIEND,0,(LPARAM)UserName );
						}			
					}
				break;
				case ID_MENU_LOOKMSG:
					{

					}
					break;
				default:
					break;

			}
		}
		break;
	case  WM_NOTIFY:
		{
			HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
			if(((LPNMHDR)lParam)->code == NM_RCLICK )
			{
				//得到鼠标坐标
				POINT pi;
				memset(&pi,0,sizeof(pi));
				GetCursorPos(&pi);
				//加载菜单
				HMENU hmenu = LoadMenu(hInstance,MAKEINTRESOURCE(IDR_MENU_FRIENDLIST));
				HMENU hmenucut = GetSubMenu(hmenu,0);
				SetForegroundWindow(hWnd);
				TrackPopupMenu(hmenucut,TPM_RIGHTALIGN,pi.x,pi.y,0,hWnd,NULL);
			}
		}
		break;
    }

    return FALSE;
}

//选项卡切换
void Main_OnSeleChange(HWND hwnd, LPARAM lParam)
{
	if ( ((LPNMHDR)lParam)->code == TCN_SELCHANGE)
	{
		//得到当前选中项
		int Select = SendDlgItemMessage(hwnd,IDC_TAB1,TCM_GETCURSEL,0,0);
		switch (Select)
		{
		case 0:
			{
				ShowWindow(hPage[0],TRUE);//显示窗体
				ShowWindow(hPage[1],FALSE);//显示窗体
			}
			break;
		case 1:
			{
				ShowWindow(hPage[0],FALSE);//显示窗体
				ShowWindow(hPage[1],TRUE);//显示窗体
			}
			break;
		}

	}

}

//当前窗口加载信息函数
BOOL Main_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	//MessageBox(hwnd,"进入窗口初始化...","",0);
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

	//接收信息
	if ( (uIformation = (UserInfor*)lParam) == NULL )
	{
		//加载信息失败,需要重新登录
		MessageBox(hwnd,"加载登录信息失败请重新登录","",0);
		return false;
	}
	//加载界面
	LoadForm( hInstance,hwnd );
	
	//建立异步消息模式
	if((WSAAsyncSelect(uIformation->Socket,hwnd,CLIENT_MESSAGE,FD_READ|FD_CLOSE))==SOCKET_ERROR)
	{
		MessageBox(hwnd,"创建信息异步模式失败","",0);
	}

	//获取好友列表
	CreateThread(NULL,0,GetFriendList,hwnd,0,NULL);

	//启动消息监控
	CreateThread(NULL,0,TellHaveMsg,hwnd,0,NULL);

	//建立托盘图标
	SetToolIcon(hwnd,uIformation->uName);

    return TRUE;
}

//获取好友列表
DWORD WINAPI GetFriendList(LPVOID lpParam)
{
	Sleep(1000);
	//获取好友列表
	MsgInfor GetFriendList;
	memset(&GetFriendList,0,sizeof(GetFriendList));
	GetFriendList.MsgType = 3;
	char GetList[255];
	strcpy(GetList,"-789");
	EncodeUrl( GetFriendList.Msg,GetList );
	if( send(uIformation->Socket,(char*)&GetFriendList,sizeof(GetFriendList),0) == SOCKET_ERROR )
	{ 
		MessageBox(NULL,"获取好友列表失败!","",0);
		return 0; 
	}
	else
	{
		return 1;
	}
}

//软件界面初始化
void LoadForm(HINSTANCE hInstance,HWND hwnd)
{
	char IconFile[25];
	sprintf(IconFile,"./icon/%s.ico",uIformation->uico);
	//MessageBox(hwnd,"图标信息加载中...","",0);
	//加载图标
	HICON hicon = LoadIconByText(IconFile);
	if (hicon != NULL)
	{
		SendDlgItemMessage(hwnd,IDC_STATIC_ICON,STM_SETIMAGE,(WPARAM)IMAGE_ICON ,(LPARAM)hicon);
	}
	else
	{
		MessageBox(hwnd,"图标信息加载失败,这可能是图标信息丢失","",0);
	}
	//MessageBox(hwnd,"图标信息加载成功,开始加载好友列表窗体...","",0);
	//加载好友列表框
	LoadFriendBox(hwnd);
	//MessageBox(hwnd,"加载好友列表窗体成功正在创建图标列表","",0);
	CreateImageList(hPage[0]);
	//MessageBox(hwnd,"加载好友列表窗体成功等待设置状态信息...","",0);
	//设置在线状态
	SendDlgItemMessage(hwnd,IDC_COMBUSTA,CB_ADDSTRING,0,(LPARAM)"在线");
	SendDlgItemMessage(hwnd,IDC_COMBUSTA,CB_INSERTSTRING,1,(LPARAM)"离线");
	SendDlgItemMessage(hwnd,IDC_COMBUSTA,CB_SETCURSEL,0,0);
	//MessageBox(hwnd,"加载主窗体图标","",0);
	//主窗体图标加载
	hicon = LoadIcon( hInstance,MAKEINTRESOURCE(IDI_ICON1) );
	SendDlgItemMessage(hwnd,IDC_BUTTON1,BM_SETIMAGE,(WPARAM)IMAGE_ICON,(LPARAM)hicon);
	hicon = LoadIcon( hInstance,MAKEINTRESOURCE(IDI_ICON2) );
	SendDlgItemMessage(hwnd,IDC_OK,BM_SETIMAGE,(WPARAM)IMAGE_ICON,(LPARAM)hicon);
	SetDlgItemText(hwnd,IDC_USERNAME,uIformation->uName);
	SetDlgItemText(hwnd,IDC_USERID,uIformation->uId);
}

//加载treeview好友列表
void LoadFriendBox(HWND hwnd)
{
//	int i;
	//MessageBox(hwnd,"进入加载好友列表模块...","",0);
	RECT rc ;
	TCITEM tie;
	GetClientRect(GetDlgItem(hwnd,IDC_TAB1), &rc);//获取TabControl控件的坐标信息
	rc.top+=25;//top为控件左上顶点相对窗体的y坐标,为了使加载进来的窗体不会覆盖tab控件,所以+25就是相当于下移25
	rc.left+=4;//left为控件左顶点相对窗体x坐标,同样+4
	rc.right-=1;//right为控件右下顶点相对窗体的x坐标
	rc.bottom-=2;//bottom为控件右下顶点相对窗体的y坐标为了加载进来的窗体不会覆盖tab控件所以xy要减小
	HINSTANCE   hInstance   =   (HINSTANCE)GetWindowLong(hwnd,   GWL_HINSTANCE);
	hPage[0]=CreateDialog(hInstance,MAKEINTRESOURCE(IDD_FRTREE),GetDlgItem(hwnd,IDC_TAB1),FriendTree_Proc);//创建页
	MoveWindow(hPage[0],rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,TRUE); //移动窗体调整适当的大小
	hPage[1]=CreateDialog(hInstance,MAKEINTRESOURCE(IDD_ACTIVEFR),GetDlgItem(hwnd,IDC_TAB1),NULL);//创建页
	MoveWindow(hPage[1],rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,TRUE); //移动窗体调整适当的大小
	ShowWindow(hPage[0],TRUE);//显示窗体
	ShowWindow(hPage[1],FALSE);//显示窗体
	tie.mask = TCIF_TEXT; 
	tie.pszText = "好友列表";
	SendDlgItemMessage(hwnd,IDC_TAB1,TCM_INSERTITEM,0,(LPARAM)&tie);//向tab控件发送消息添加项
	tie.pszText = "最近联系人";
	SendDlgItemMessage(hwnd,IDC_TAB1,TCM_INSERTITEM,1,(LPARAM)&tie);//向tab控件发送消息添加项
	//MessageBox(hwnd,"添加好友列表选项卡...","",0);
	hRoot = AddItemToTree(hPage[0],IDC_FRTREE,"好友列表",0,NULL,true);
	//MessageBox(hwnd,"添加成功...","",0);
}


//加载图标
HICON LoadIconByText(char* FileName)
{
	return (HICON) LoadImage(NULL,FileName,IMAGE_ICON,0,0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
}

void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
    switch(id)
    {
		case IDC_SET:
			{
				MessageBox(hwnd,"该功能还未完善敬请期待","",0);
			}
			break;
		
        case IDC_OK://设置
			{
				
			}
			break;

		case IDC_STATIC_ICON:
			{
				MessageBox(hwnd,"","",0);
			}
			break;

	case ID_MENU_DISP:
			{
				ShowWindow(hwnd,SW_SHOW);
			}
			break;

	case ID_MENU_EXIT:
			{
				EndDialog(hwnd, 0);
			}
			break;

	case IDC_HAVEMSG:
			{
				ReadMsgList(hwnd);
			}	
			break;
    }
}

void Main_OnClose(HWND hwnd)
{
	
	closesocket(uIformation->Socket);
	//清理套接字占用的资源 
	WSACleanup();
    EndDialog(hwnd, 0);
}

//点击消息提示按钮
bool ReadMsgList(HWND hwnd)
{
	if (RecvList.empty())//消息队列为空没有响应
	{
		return false;
	}
	else
	{
		//取出消息队列的第一个元素
		list<MsgList>::iterator Msgiter =RecvList.begin();
		//MsgList msglist = (MsgList )RecvList.front();
		//找到这个消息接收者的name
		map <int,FriendInfor>::iterator iter;
		iter = Friendlist.find(Msgiter->Id);
		if ( iter != Friendlist.end() )
		{
			char NameStr[255];
			sprintf(NameStr,"%s(%d)",iter->second.Name,iter->second.Id);
			ChatWithFriend(hwnd,NameStr);
		}
		return true;
	}
}



//服务端有消息过来时进行响应
int Main_OnServerMsg(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	switch(WSAGETSELECTEVENT(lParam))
	{	
		case FD_READ:
			{
				//MessageBox(hwnd,"","",0);
				const int MAX_SIZE = 1024;
				//char RecvStr[MAX_SIZE*6];
				//memset( RecvStr,0,sizeof(RecvStr) );
				int RecvLen = 0;
				MsgInfor RecvMsg;
				memset(&RecvMsg,0,sizeof(RecvMsg));

				
				//接收消息
				if( (RecvLen=recv(uIformation->Socket,(char*)&RecvMsg,sizeof(RecvMsg),0)) == SOCKET_ERROR ) 
				{ 
					//SetServerLog(hwnd,IDC_EDIT1,"接收消息失败...");
					//sprintf(Error,"接收数据失败！"); 
					MessageBox(NULL,"接收消息失败","",0);
					return 0; 
				}

				//进行类型转换
				//MsgInfor* RecvMsg = (MsgInfor*)RecvStr;
				
				//解码信息
				char ARecvStr[MAX_SIZE];
				CodeUrl(ARecvStr,RecvMsg.Msg);

				//分析协议
				switch( RecvMsg.MsgType )
				{
				case 100://好友列表
					{
						
						LoadFriend_List(hPage[0],ARecvStr);
					}
					break;
				case 101://好友上线
					{
						LoadFriend_List(hPage[0],ARecvStr);
					}
					break;
				case 102://好友下线
					{
						DeleOff_Friend(hPage[0],ARecvStr);
					}
					break;
				case 103://接收消息
					{
						//声音提醒
						PlaySound("msg.wav",NULL,SND_FILENAME|SND_ASYNC);
						//进行接收
						Recv_Msg(hwnd,ARecvStr,false);
					}
					break;
				case 104://下线通知
					closesocket(uIformation->Socket);
					MessageBox(hwnd,TEXT("您的账号在异地登陆,如果不是您登陆的请更改密码"),TEXT("下线提醒"),MB_OK);
					break;
				case 105://窗口抖动
					{
						Recv_Msg(hwnd,ARecvStr,true);
					}
					break;

				}				
			}
			break;
		case FD_CLOSE:
			MessageBox(hwnd,TEXT("与服务器断开连接,您已处于离线状态"),TEXT("下线提醒"),MB_OK);
			//删除数据库中得socket
			closesocket(uIformation->Socket);
			break;
	}
	return 0;
}



//加载好友列表
void LoadFriend_List(HWND hwnd,char* FriendList)
{
	//MessageBox(NULL,"list","",0);
	char FriendStr[255];
	memset(FriendStr,0,sizeof(FriendStr));
	//分析好友列表
	for(int i = 0; FriendList[i]; )
	{
		if ( FriendList[i] == '-' )
		{
			i++;
			//将好友信息分开
			for(int j=0;(FriendList[i] != '-')&&(FriendList[i]);j++)
			{
				FriendStr[j] = FriendList[i++];
			}
			FriendStr[j] = '\0';
			//再次提取
			char NameStr[255];
			char IconId[25];
			char Name[255];
			char Id[25];
			sscanf(FriendStr,"%[^.].%s",IconId,NameStr);
			sscanf(NameStr,"%[^(](%[^)]",Name,Id);
			//将信息导入到好友列表中
			FriendInfor frinfor;
			frinfor.Id = atoi(Id);
			frinfor.IconNum = atoi(IconId);
			strcpy(frinfor.Name,Name);
			Friendlist.insert( pair<int,FriendInfor>(frinfor.Id,frinfor) );
			
			//将好友信息添加到列表
			AddItemToTree(hPage[0],IDC_FRTREE,NameStr,atoi(IconId),hRoot,false);
		}
	}
}


//好友下线
void DeleOff_Friend(HWND hwnd,char * FriendStr)
{
	char FriendName[255];
	sscanf(FriendStr,"-%s",FriendName);
	if ( !DeleteItemByText(hPage[0],IDC_FRTREE,FriendName) )
	{
		MessageBox(hwnd,"好友下线删除好友失败","",0);
	}
}

//打开窗口发生送消息
void ChatWithFriend(HWND hwnd,char* NameStr)
{
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
	//初始化发送信息
	MsgSendToDlg MsgSend;
	memset(&MsgSend,0,sizeof(MsgSend));
	MsgSend.hwnd = hwnd;
	char UserId[25];
	sscanf(NameStr,"%[^(](%[^)]",MsgSend.name,UserId);
	MsgSend.id = atoi(UserId);
	MsgSend.socket = uIformation->Socket;
	//发送消息
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_CHATDIALOG), NULL, ChatDlg_Proc,(LPARAM)&MsgSend);
}


//接收消息函数
void Recv_Msg(HWND hwnd,char * MsgStr,bool IsOpen)
{
	int Uid = 0;
	//提取用户id
	char UidStr[25];
	char Msg[1024];
	sscanf(MsgStr,"-%[^-]-%[^#]",UidStr,Msg);
	Uid = atoi(UidStr);
	//查找窗口是否打开
	map <int,HWND>::iterator iter;
	iter = OpenWinList.find( Uid );
	if ( iter == OpenWinList.end() )
	{
		//未找到添加到窗口列表
		//放入消息列表
		MsgList msglist;
		msglist.Id = Uid;
		strcpy(msglist.Msg, Msg);
		RecvList.push_back(msglist);
		if( IsOpen )
		{
			map <int,FriendInfor>::iterator iterFlist;
			iterFlist = Friendlist.find(Uid);
			char NameStr[255];
			sprintf(NameStr,"%s(%d)",iterFlist->second.Name,iterFlist->second.Id);
			ChatWithFriend(hwnd,NameStr);
		}

	}
	else
	{
		//找到给窗口发送消息
		COPYDATASTRUCT CopyData;
		CopyData.cbData = 0;
		CopyData.dwData = sizeof(Msg);
		CopyData.lpData = Msg;
		SendMessage(iter->second,WM_COPYDATA,(WPARAM)hwnd,(LPARAM)&CopyData);
	}
	//若打开了则sendmsg
	//若为打开则放入list并提醒用户有消息

	//recvList.push_back(buff);
}

//设置提示消息
DWORD WINAPI TellHaveMsg(LPVOID lpParam)
{
	HWND hwnd = (HWND)lpParam;
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
	HICON hicon1 = LoadIcon( hInstance,MAKEINTRESOURCE(IDI_ICON_MSG1) );
	HICON hicon2 = LoadIcon( hInstance,MAKEINTRESOURCE(IDI_ICON_MSG2) );
	while(1)
	{
		//判断有没有消息
		if (RecvList.size() != 0)
		{
			SendDlgItemMessage(hwnd,IDC_HAVEMSG,STM_SETIMAGE,(WPARAM)IMAGE_ICON ,(LPARAM)hicon1);
			SetIcon(hwnd,IDI_ICON_DEFINE,uIformation->uName);
			Sleep(500);
			SendDlgItemMessage(hwnd,IDC_HAVEMSG,STM_SETIMAGE,(WPARAM)IMAGE_ICON ,(LPARAM)hicon2);
			SetIcon(hwnd,IDI_ICON3,uIformation->uName);
			Sleep(500);
		}
		else
		{
			Sleep(1000);
		}
	}

}

