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



//���ļ��ڵ�ȫ�ֱ���

MySql mysql("root","root","myqq","MySQL ODBC 5.1 Driver","127.0.0.1");  //���ݿ���
SOCKET serverSocket;             //������׽���
map <SOCKET,int >Map1;           //map1 ����socket��uid
map <int , UserInfor>Map2;       //map2 ����uid���û���Ϣ
bool ServerStart = false;        //��¼��������״̬
int UserCount = 0;               //��¼��¼�û�����
const int CacheMaxNum = 200;
//Cache LocalCache[CacheMaxNum];           //����cache�����û�����Ϣ,��¼ʱʹ��(���ٷ������ݿ����)
map <int ,Cache>LocalCache;



void TellFriendOffLine(SOCKET socket, char* UserStr );




BOOL WINAPI Main_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG, Main_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, Main_OnCommand);
		HANDLE_MSG(hWnd,WM_CLOSE, Main_OnClose);
		case SERVER_MESSAGE:	//������Ϣ(��������,������Ϣ,�Ͽ�����)	
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
        case IDC_OK:                 //����������
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
		case IDC_BUTTON_LOG:         //�鿴��־�ļ�
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
		case FD_ACCEPT:       //����������Ϣ
			{
				AcceptMessage( hwnd,(SOCKET)wParam,serverSocket);
			}
			break;

		case FD_READ:        //������Ϣ��������Ӧ����
			{
				ReadMessage(hwnd,(SOCKET)wParam,mysql,Map1,Map2 , LocalCache );
			}
			break;
			
		case FD_WRITE:       //socketΪ��д
			{
			}
			break;

		case FD_CLOSE:       //socket�ر�����--ɾ����Ӧ����Ϣ
			{
				CloseMessage( hwnd,(SOCKET)wParam );
			}
			break;
	}
}


void Main_OnClose(HWND hwnd)
{
	closesocket(serverSocket);
	SetServerLog(hwnd,IDC_EDIT1,"������ֹͣ");
    EndDialog(hwnd, 0);
}



//����������
void StartServer(HWND hwnd)
{
	SetCache(LocalCache,CacheMaxNum);
	ServerStart = true;
	SetDlgItemText(hwnd,IDC_OK,"ֹͣ");
	SetDlgItemText(hwnd,IDC_SERVER_STATIC,"������������");
	SYSTEMTIME Time;
	memset(&Time,0,sizeof(Time));
	GetSystemTime(&Time);
	char TimeStr[255];
	sprintf(TimeStr," ����: %d/%d\r\n ʱ��: %d:%d",
		Time.wMonth,Time.wDay,Time.wHour+8,Time.wMinute);
	SetDlgItemText(hwnd,IDC_LINKTIME,TimeStr);	
}



//ֹͣ������
void StopServer(HWND hwnd)
{
	closesocket(serverSocket);
	ServerStart = false;
	SetDlgItemText(hwnd,IDC_OK,"����");
	SetDlgItemText(hwnd,IDC_SERVER_STATIC,"��������ֹͣ");
	SetServerLog(hwnd,IDC_EDIT1,"��������ֹͣ");
	SetDlgItemText(hwnd,IDC_LINKTIME,"");	
}




//�鿴��־�ļ�
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



//���ݸ����ֵ�ı䴰�ڴ�С
bool SetWindowSize( HWND hwnd,long xPos_Change,long yPos_Change)
{
	RECT rect;
	//struct rect ˵����Ϣ
	//rect.left  ���Ͻ�x
	//rect.top   ���Ͻ�y
	//rect.right ���½�x
	//rect.bottom���½�y

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
		MessageBox(hwnd,TEXT("�Բ����ȡ��Ϣʧ��!"),TEXT("����"),MB_OK|MB_ICONERROR);
		return false;
	}
	return true;
}


//�������ֽڲ���������״̬

bool  Listen(HWND hwnd)
{	
	short port=8000; //���ö˿�
	
	WSADATA wsa; 
	//��ʼ���׽���DLL 
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0)
	{ 
		SetServerLog(hwnd,IDC_EDIT1,"�׽��ֳ�ʼ��ʧ��");
		return false; 
	} 
	//�����׽��� 
	
	if((serverSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
	{ 
		SetServerLog(hwnd,IDC_EDIT1,"�׽��ִ���ʧ��");
		return false; 
	} 
	//�����¼�
	WSAAsyncSelect(serverSocket,hwnd,SERVER_MESSAGE, FD_ACCEPT|FD_READ|FD_WRITE|FD_CLOSE);
	struct sockaddr_in serverAddress; 
	memset(&serverAddress,0,sizeof(sockaddr_in)); 
	serverAddress.sin_family=AF_INET; 
	serverAddress.sin_addr.S_un.S_addr = htonl(INADDR_ANY); 
	serverAddress.sin_port = htons(port); 
	//�� 
	if(bind(serverSocket,(sockaddr*)&serverAddress,sizeof(serverAddress))==SOCKET_ERROR)
	{ 
		SetServerLog(hwnd,IDC_EDIT1,"�׽��ְ󶨵��˿�ʧ�ܣ�");
		return false; 
	} 
	//��������״̬ 
	if(listen(serverSocket,SOMAXCONN)==SOCKET_ERROR)
	{ 
		SetServerLog(hwnd,IDC_EDIT1,"����ʧ�ܣ�");
		return false; 
	} 
	SetServerLog(hwnd,IDC_EDIT1,"�������˿����ڼ�������......");

	return true;	
}


void CloseMessage(HWND hwnd,SOCKET socket)
{
	//�ҵ��û�id
	map<SOCKET,int >::iterator iter;
	iter = Map1.find(socket);

	//���Ϊ��¼�û���������û���map�е���Ϣ
	if ( iter != Map1.end() )
	{
		//��������֪ͨ
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
			MessageBox(hwnd,"�û����ߵ�����ʧ��!","",0);
		}
		//���û�������Ϣд����־�ļ�
		char buff[255];
		sprintf(buff,"�û�%04d����",iter->second);
		SetServerLog(hwnd,IDC_EDIT1,buff);
		//ɾ���û���map�е���Ϣ
		Map2.erase( iter->second);
		Map1.erase( iter->first);
		UserCount--;
	}

	//�Ͽ�����
	closesocket(socket);
}


//֪ͨ��������
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
			MessageBox(NULL,"���ͺ����б�ʧ��","",0);
			//����ʧ��
			//SetDlgItemText(hwnd,IDC_LOGSTA,"��������ʧ��!");
			//return ; 
		}
	}
}

//����cache��Ϣ
/*
//cache�û���Ϣ
struct Cache
{
	int UId;
	string UName;
	string UPasswd; 
	int IcoNum;
};
map <int ,Cache>LocalCache;
*/
//��ʼ��cache
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





