#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>


#include <string>
#include "resource.h"
#include "MainDlg.h"
#include "EnCoding.h"


#include "AcceptAndLogin.h"
#define SERVER_MESSAGE 10000
using namespace std;



//本文件内的全局变量

MySql mysql("root","root","myqq","MySQL ODBC 5.1 Driver","127.0.0.1");  //数据库类
SOCKET serverSocket;             //服务端套接字
map <SOCKET,int >Map1;           //map1 保存socket与uid
map <int , UserInfor>Map2;       //map2 保存uid与用户信息
bool ServerStart = false;        //记录服务器的状态
int UserCount = 0;               //记录登录用户个数
const int CacheMaxNum = 200;
//Cache LocalCache[CacheMaxNum];           //本地cache保存用户的信息,登录时使用(减少访问数据库次数)
map <int ,Cache>LocalCache;



void TellFriendOffLine(SOCKET socket, char* UserStr );




BOOL WINAPI Main_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG, Main_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, Main_OnCommand);
		HANDLE_MSG(hWnd,WM_CLOSE, Main_OnClose);
		case SERVER_MESSAGE:	//服务消息(请求连接,接收消息,断开连接)	
			ServerMesssageCome(hWnd,wParam,lParam);
		break;
    }

    return FALSE;
}




BOOL Main_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	SetWindowSize( hwnd,(-250),0);
    return TRUE;
}



void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch(id)
    {
        case IDC_OK:                 //启动服务器
			{
				if ( !ServerStart )
				{
					if ( Listen(hwnd) )
					{
						StartServer(hwnd);
					}
				}
				else
				{
					StopServer(hwnd);
				}
			}
			break;
		case IDC_BUTTON_LOG:         //查看日志文件
			{
				SeeLogMsg(hwnd);
			}
			break;
        default:
		break;
    }
}



void ServerMesssageCome(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	switch(WSAGETSELECTEVENT(lParam))
	{
		case FD_ACCEPT:       //请求连接信息
			{
				AcceptMessage( hwnd,(SOCKET)wParam,serverSocket);
			}
			break;

		case FD_READ:        //接收消息并作出响应处理
			{
				ReadMessage(hwnd,(SOCKET)wParam,mysql,Map1,Map2 , LocalCache );
			}
			break;
			
		case FD_WRITE:       //socket为可写
			{
			}
			break;

		case FD_CLOSE:       //socket关闭连接--删除响应的信息
			{
				CloseMessage( hwnd,(SOCKET)wParam );
			}
			break;
	}
}


void Main_OnClose(HWND hwnd)
{
	closesocket(serverSocket);
	SetServerLog(hwnd,IDC_EDIT1,"服务已停止");
    EndDialog(hwnd, 0);
}



//启动服务器
void StartServer(HWND hwnd)
{
	SetCache(LocalCache,CacheMaxNum);
	ServerStart = true;
	SetDlgItemText(hwnd,IDC_OK,"停止");
	SetDlgItemText(hwnd,IDC_SERVER_STATIC,"服务器已启动");
	SYSTEMTIME Time;
	memset(&Time,0,sizeof(Time));
	GetSystemTime(&Time);
	char TimeStr[255];
	sprintf(TimeStr," 日期: %d/%d\r\n 时间: %d:%d",
		Time.wMonth,Time.wDay,Time.wHour+8,Time.wMinute);
	SetDlgItemText(hwnd,IDC_LINKTIME,TimeStr);	
}



//停止服务器
void StopServer(HWND hwnd)
{
	closesocket(serverSocket);
	ServerStart = false;
	SetDlgItemText(hwnd,IDC_OK,"启动");
	SetDlgItemText(hwnd,IDC_SERVER_STATIC,"服务器已停止");
	SetServerLog(hwnd,IDC_EDIT1,"服务器已停止");
	SetDlgItemText(hwnd,IDC_LINKTIME,"");	
}




//查看日志文件
void SeeLogMsg(HWND hwnd)
{
	long ChangeSize = 0;
	char MaxOrMin[3];
	memset(MaxOrMin,0,sizeof(MaxOrMin)/sizeof(char));
	GetDlgItemText(hwnd,IDC_BUTTON_LOG,MaxOrMin,sizeof(MaxOrMin)/sizeof(char));
	if ( !strcmp(MaxOrMin,"<") )
	{
		ChangeSize = -250;
		SetDlgItemText(hwnd,IDC_BUTTON_LOG,">");
	}
	else
	{
		ChangeSize = 250;
		SetDlgItemText(hwnd,IDC_BUTTON_LOG,"<");
	}
	SetWindowSize( hwnd,ChangeSize,0);
}



//根据给入的值改变窗口大小
bool SetWindowSize( HWND hwnd,long xPos_Change,long yPos_Change)
{
	RECT rect;
	//struct rect 说明信息
	//rect.left  左上角x
	//rect.top   左上角y
	//rect.right 右下角x
	//rect.bottom右下角y

	memset(&rect,0,sizeof(rect));
	if ( GetWindowRect(hwnd,&rect) )
	{
		wchar_t buff[255];
		memset(buff,0,sizeof(buff)/sizeof(wchar_t));
		long x = rect.right-rect.left;
		long y = rect.bottom-rect.top;
		SetWindowPos(hwnd,HWND_TOP,rect.left,rect.top,x + xPos_Change,y + yPos_Change,SWP_NOMOVE);
		//wsprintf(buff,L"%d %d",rect.right-rect.left,rect.bottom-rect.top);
		//MessageBox(hwnd,buff,TEXT(""),MB_OK);
	}
	else
	{
		MessageBox(hwnd,TEXT("对不起获取信息失败!"),TEXT("错误"),MB_OK|MB_ICONERROR);
		return false;
	}
	return true;
}


//创建套字节并进入侦听状态

bool  Listen(HWND hwnd)
{	
	short port=8000; //设置端口
	
	WSADATA wsa; 
	//初始化套接字DLL 
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{ 
		SetServerLog(hwnd,IDC_EDIT1,"套接字初始化失败");
		return false; 
	} 
	//创建套接字 
	
	if((serverSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
	{ 
		SetServerLog(hwnd,IDC_EDIT1,"套接字创建失败");
		return false; 
	} 
	//创建事件
	WSAAsyncSelect(serverSocket,hwnd,SERVER_MESSAGE, FD_ACCEPT|FD_READ|FD_WRITE|FD_CLOSE);
	struct sockaddr_in serverAddress; 
	memset(&serverAddress,0,sizeof(sockaddr_in)); 
	serverAddress.sin_family=AF_INET; 
	serverAddress.sin_addr.S_un.S_addr = htonl(INADDR_ANY); 
	serverAddress.sin_port = htons(port); 
	//绑定 
	if(bind(serverSocket,(sockaddr*)&serverAddress,sizeof(serverAddress))==SOCKET_ERROR)
	{ 
		SetServerLog(hwnd,IDC_EDIT1,"套接字绑定到端口失败！");
		return false; 
	} 
	//进入侦听状态 
	if(listen(serverSocket,SOMAXCONN)==SOCKET_ERROR)
	{ 
		SetServerLog(hwnd,IDC_EDIT1,"侦听失败！");
		return false; 
	} 
	SetServerLog(hwnd,IDC_EDIT1,"服务器端口正在监听连接......");

	return true;	
}


void CloseMessage(HWND hwnd,SOCKET socket)
{
	//找到用户id
	map<SOCKET,int >::iterator iter;
	iter = Map1.find(socket);

	//如果为登录用户则清除该用户在map中的信息
	if ( iter != Map1.end() )
	{
		//发送下线通知
		map <int , UserInfor>::iterator iterInfor;
		iterInfor = Map2.find( iter->second );
		if ( iterInfor != Map2.end() )
		{
			char UserName[255];
			sprintf(UserName,"-%s(%d)",iterInfor->second.UName.c_str(),iterInfor->second.UId);
			TellFriendOffLine(socket,UserName);
		}
		else
		{
			MessageBox(hwnd,"用户下线但查找失败!","",0);
		}
		//将用户下线信息写入日志文件
		char buff[255];
		sprintf(buff,"用户%04d下线",iter->second);
		SetServerLog(hwnd,IDC_EDIT1,buff);
		//删除用户在map中的信息
		Map2.erase( iter->second);
		Map1.erase( iter->first);
		UserCount--;
	}

	//断开连接
	closesocket(socket);
}


//通知好友上线
void TellFriendOffLine(SOCKET socket, char* UserStr )
{
	MsgInfor AskFriend;
	AskFriend.MsgType = 102;
	//EncodeUrl(AskFriend.MsgStr,LoginUserStr);
	EncodeUrl(AskFriend.MsgStr,UserStr);

	map<SOCKET,int>::iterator iter;

	for (iter = Map1.begin(); iter != Map1.end(); iter++)
	{
		if (iter->first == socket)
			continue;

		if( send(iter->first,(char*)&AskFriend,sizeof(AskFriend),0) == SOCKET_ERROR )
		{ 
			MessageBox(NULL,"发送好友列表失败","",0);
			//发送失败
			//SetDlgItemText(hwnd,IDC_LOGSTA,"发送数据失败!");
			//return ; 
		}
	}
}

//更新cache信息
/*
//cache用户信息
struct Cache
{
	int UId;
	string UName;
	string UPasswd; 
	int IcoNum;
};
map <int ,Cache>LocalCache;
*/
//初始化cache
void SetCache( map <int ,Cache> &Local_Cache,int n )
{
	Local_Cache.clear();

	//memset( localcache,0,sizeof(Cache)*n );
	int rows=0;
	int cols=0;
	char*** results=mysql.Select("select uid,uname,upasswd,uiconum from userinfor",rows,cols);
	if ( (cols !=0) && (rows != 0) )
	{
		for(int i=0;i<rows;i++)
		{
			int count = atoi(results[i][0]);
			Cache cache;
			cache.UId = count;
			cache.UName = results[i][1];
			cache.UPasswd = results[i][2];
			cache.IcoNum = atoi(results[i][3]);
			Local_Cache.insert( pair<int,Cache>(count,cache) );
			/*
			LocalCache[count].UId = count;
			LocalCache[count].UName = results[i][1];
			LocalCache[count].UPasswd = results[i][2];
			LocalCache[count].IcoNum = atoi(results[i][3]);
			*/
			delete [] results[i][0];
			results[i][0] = NULL;
			delete [] results[i][1];
			results[i][1] = NULL;
			delete [] results[i][2];
			results[i][2] = NULL;
			delete [] results[i][3];
			results[i][3] = NULL;
			delete [] results[i];
			results[i] = NULL;
		}
	}
	//delete [] results;
	//results = NULL;
}





