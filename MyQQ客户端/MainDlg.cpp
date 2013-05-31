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


//�Ի����б��û���Ϣ�ṹ��
struct DialogInfor
{
	int Id;     //�Ի����û���id
	char Name[255]; //�Ի����û���name
	HWND hwnd;  //�Ի���ľ��
};

//��Ϣ���еĸ�ʽ
struct MsgList
{
	int Id;
	char Msg[1024];
};
//�����б�
struct FriendInfor
{
	int Id;
	int IconNum;
	char Name[255];
};

//��ǰ���ڵ�ȫ�ֱ���
UserInfor* uIformation=NULL;
//list<int> openWinList;

HWND hPage[3];    //ҳ����
HTREEITEM hRoot;  //�����б���
HWND hTreeview;
map <int,HWND> OpenWinList;   //�򿪵ĶԻ����б�
map <int,FriendInfor> Friendlist;
list < MsgList > RecvList;         //�յ���δ����Ϣ




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
			Main_OnSeleChange(hWnd,lParam);//tabѡ��л�
		}
		break;
	case IDM_MINMSG:               //����ͼ����Ϣ����
		{
			ToolIconMsg(hWnd,lParam);
		}
		break;
	case WM_SYSCOMMAND:           //�������»���Ϣ��������ͼ��
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
	case WM_COPYDATA://�Ӵ��ڷ��;����Ϣ
		{
			//������Ϣ
			COPYDATASTRUCT *copydata =(COPYDATASTRUCT*) lParam;
			//�����Ƿ����
			map <int,HWND>::iterator iter;
			iter = OpenWinList.find (atoi( (char*)copydata->lpData ) );
			if (iter == OpenWinList.end() )
			{
				//δ�ҵ���ӵ������б�
				OpenWinList.insert(pair<int,HWND>( atoi( (char*)copydata->lpData ),(HWND)wParam ) );
				//������Ϣ���в�����û�����Ϣ������
				ReadMsgToDlg( hWnd,(HWND)wParam,atoi( (char*)copydata->lpData ) );
			}
			else
			{
				//�Ѿ����ھ���Ҫɾ��
				OpenWinList.erase( atoi( (char*)copydata->lpData ) );
			}
		}
		break;
    }

    return FALSE;
}

//������Ϣ���в����û���Ϣ������
void ReadMsgToDlg(HWND hwnd,HWND hchild,int id)
{
	list<MsgList>::iterator Msgiter =RecvList.begin();
	while( Msgiter != RecvList.end() )
	{
		if (Msgiter->Id == id)
		{
			//�ҵ������ڷ�����Ϣ
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

//treeview��Ϣ����
BOOL WINAPI FriendTree_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
    {
	case WM_COMMAND:
		{
			switch(wParam)
			{
				case ID_MENU_SENDMSG://�Ҽ��˵� ������Ϣ
					{
						//�õ���ǰѡ������ı�ֵ
						char UserName[255];
						if (GetTreeSelectItemText(hWnd,IDC_FRTREE,UserName))
						{
							//���͸�������
							HWND hparent = GetParent(hWnd);  //tab���
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
				//�õ��������
				POINT pi;
				memset(&pi,0,sizeof(pi));
				GetCursorPos(&pi);
				//���ز˵�
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

//ѡ��л�
void Main_OnSeleChange(HWND hwnd, LPARAM lParam)
{
	if ( ((LPNMHDR)lParam)->code == TCN_SELCHANGE)
	{
		//�õ���ǰѡ����
		int Select = SendDlgItemMessage(hwnd,IDC_TAB1,TCM_GETCURSEL,0,0);
		switch (Select)
		{
		case 0:
			{
				ShowWindow(hPage[0],TRUE);//��ʾ����
				ShowWindow(hPage[1],FALSE);//��ʾ����
			}
			break;
		case 1:
			{
				ShowWindow(hPage[0],FALSE);//��ʾ����
				ShowWindow(hPage[1],TRUE);//��ʾ����
			}
			break;
		}

	}

}

//��ǰ���ڼ�����Ϣ����
BOOL Main_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	//MessageBox(hwnd,"���봰�ڳ�ʼ��...","",0);
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);

	//������Ϣ
	if ( (uIformation = (UserInfor*)lParam) == NULL )
	{
		//������Ϣʧ��,��Ҫ���µ�¼
		MessageBox(hwnd,"���ص�¼��Ϣʧ�������µ�¼","",0);
		return false;
	}
	//���ؽ���
	LoadForm( hInstance,hwnd );
	
	//�����첽��Ϣģʽ
	if((WSAAsyncSelect(uIformation->Socket,hwnd,CLIENT_MESSAGE,FD_READ|FD_CLOSE))==SOCKET_ERROR)
	{
		MessageBox(hwnd,"������Ϣ�첽ģʽʧ��","",0);
	}

	//��ȡ�����б�
	CreateThread(NULL,0,GetFriendList,hwnd,0,NULL);

	//������Ϣ���
	CreateThread(NULL,0,TellHaveMsg,hwnd,0,NULL);

	//��������ͼ��
	SetToolIcon(hwnd,uIformation->uName);

    return TRUE;
}

//��ȡ�����б�
DWORD WINAPI GetFriendList(LPVOID lpParam)
{
	Sleep(1000);
	//��ȡ�����б�
	MsgInfor GetFriendList;
	memset(&GetFriendList,0,sizeof(GetFriendList));
	GetFriendList.MsgType = 3;
	char GetList[255];
	strcpy(GetList,"-789");
	EncodeUrl( GetFriendList.Msg,GetList );
	if( send(uIformation->Socket,(char*)&GetFriendList,sizeof(GetFriendList),0) == SOCKET_ERROR )
	{ 
		MessageBox(NULL,"��ȡ�����б�ʧ��!","",0);
		return 0; 
	}
	else
	{
		return 1;
	}
}

//��������ʼ��
void LoadForm(HINSTANCE hInstance,HWND hwnd)
{
	char IconFile[25];
	sprintf(IconFile,"./icon/%s.ico",uIformation->uico);
	//MessageBox(hwnd,"ͼ����Ϣ������...","",0);
	//����ͼ��
	HICON hicon = LoadIconByText(IconFile);
	if (hicon != NULL)
	{
		SendDlgItemMessage(hwnd,IDC_STATIC_ICON,STM_SETIMAGE,(WPARAM)IMAGE_ICON ,(LPARAM)hicon);
	}
	else
	{
		MessageBox(hwnd,"ͼ����Ϣ����ʧ��,�������ͼ����Ϣ��ʧ","",0);
	}
	//MessageBox(hwnd,"ͼ����Ϣ���سɹ�,��ʼ���غ����б���...","",0);
	//���غ����б��
	LoadFriendBox(hwnd);
	//MessageBox(hwnd,"���غ����б���ɹ����ڴ���ͼ���б�","",0);
	CreateImageList(hPage[0]);
	//MessageBox(hwnd,"���غ����б���ɹ��ȴ�����״̬��Ϣ...","",0);
	//��������״̬
	SendDlgItemMessage(hwnd,IDC_COMBUSTA,CB_ADDSTRING,0,(LPARAM)"����");
	SendDlgItemMessage(hwnd,IDC_COMBUSTA,CB_INSERTSTRING,1,(LPARAM)"����");
	SendDlgItemMessage(hwnd,IDC_COMBUSTA,CB_SETCURSEL,0,0);
	//MessageBox(hwnd,"����������ͼ��","",0);
	//������ͼ�����
	hicon = LoadIcon( hInstance,MAKEINTRESOURCE(IDI_ICON1) );
	SendDlgItemMessage(hwnd,IDC_BUTTON1,BM_SETIMAGE,(WPARAM)IMAGE_ICON,(LPARAM)hicon);
	hicon = LoadIcon( hInstance,MAKEINTRESOURCE(IDI_ICON2) );
	SendDlgItemMessage(hwnd,IDC_OK,BM_SETIMAGE,(WPARAM)IMAGE_ICON,(LPARAM)hicon);
	SetDlgItemText(hwnd,IDC_USERNAME,uIformation->uName);
	SetDlgItemText(hwnd,IDC_USERID,uIformation->uId);
}

//����treeview�����б�
void LoadFriendBox(HWND hwnd)
{
//	int i;
	//MessageBox(hwnd,"������غ����б�ģ��...","",0);
	RECT rc ;
	TCITEM tie;
	GetClientRect(GetDlgItem(hwnd,IDC_TAB1), &rc);//��ȡTabControl�ؼ���������Ϣ
	rc.top+=25;//topΪ�ؼ����϶�����Դ����y����,Ϊ��ʹ���ؽ����Ĵ��岻�Ḳ��tab�ؼ�,����+25�����൱������25
	rc.left+=4;//leftΪ�ؼ��󶥵���Դ���x����,ͬ��+4
	rc.right-=1;//rightΪ�ؼ����¶�����Դ����x����
	rc.bottom-=2;//bottomΪ�ؼ����¶�����Դ����y����Ϊ�˼��ؽ����Ĵ��岻�Ḳ��tab�ؼ�����xyҪ��С
	HINSTANCE   hInstance   =   (HINSTANCE)GetWindowLong(hwnd,   GWL_HINSTANCE);
	hPage[0]=CreateDialog(hInstance,MAKEINTRESOURCE(IDD_FRTREE),GetDlgItem(hwnd,IDC_TAB1),FriendTree_Proc);//����ҳ
	MoveWindow(hPage[0],rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,TRUE); //�ƶ���������ʵ��Ĵ�С
	hPage[1]=CreateDialog(hInstance,MAKEINTRESOURCE(IDD_ACTIVEFR),GetDlgItem(hwnd,IDC_TAB1),NULL);//����ҳ
	MoveWindow(hPage[1],rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,TRUE); //�ƶ���������ʵ��Ĵ�С
	ShowWindow(hPage[0],TRUE);//��ʾ����
	ShowWindow(hPage[1],FALSE);//��ʾ����
	tie.mask = TCIF_TEXT; 
	tie.pszText = "�����б�";
	SendDlgItemMessage(hwnd,IDC_TAB1,TCM_INSERTITEM,0,(LPARAM)&tie);//��tab�ؼ�������Ϣ�����
	tie.pszText = "�����ϵ��";
	SendDlgItemMessage(hwnd,IDC_TAB1,TCM_INSERTITEM,1,(LPARAM)&tie);//��tab�ؼ�������Ϣ�����
	//MessageBox(hwnd,"��Ӻ����б�ѡ�...","",0);
	hRoot = AddItemToTree(hPage[0],IDC_FRTREE,"�����б�",0,NULL,true);
	//MessageBox(hwnd,"��ӳɹ�...","",0);
}


//����ͼ��
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
				MessageBox(hwnd,"�ù��ܻ�δ���ƾ����ڴ�","",0);
			}
			break;
		
        case IDC_OK://����
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
	//�����׽���ռ�õ���Դ 
	WSACleanup();
    EndDialog(hwnd, 0);
}

//�����Ϣ��ʾ��ť
bool ReadMsgList(HWND hwnd)
{
	if (RecvList.empty())//��Ϣ����Ϊ��û����Ӧ
	{
		return false;
	}
	else
	{
		//ȡ����Ϣ���еĵ�һ��Ԫ��
		list<MsgList>::iterator Msgiter =RecvList.begin();
		//MsgList msglist = (MsgList )RecvList.front();
		//�ҵ������Ϣ�����ߵ�name
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



//���������Ϣ����ʱ������Ӧ
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

				
				//������Ϣ
				if( (RecvLen=recv(uIformation->Socket,(char*)&RecvMsg,sizeof(RecvMsg),0)) == SOCKET_ERROR ) 
				{ 
					//SetServerLog(hwnd,IDC_EDIT1,"������Ϣʧ��...");
					//sprintf(Error,"��������ʧ�ܣ�"); 
					MessageBox(NULL,"������Ϣʧ��","",0);
					return 0; 
				}

				//��������ת��
				//MsgInfor* RecvMsg = (MsgInfor*)RecvStr;
				
				//������Ϣ
				char ARecvStr[MAX_SIZE];
				CodeUrl(ARecvStr,RecvMsg.Msg);

				//����Э��
				switch( RecvMsg.MsgType )
				{
				case 100://�����б�
					{
						
						LoadFriend_List(hPage[0],ARecvStr);
					}
					break;
				case 101://��������
					{
						LoadFriend_List(hPage[0],ARecvStr);
					}
					break;
				case 102://��������
					{
						DeleOff_Friend(hPage[0],ARecvStr);
					}
					break;
				case 103://������Ϣ
					{
						//��������
						PlaySound("msg.wav",NULL,SND_FILENAME|SND_ASYNC);
						//���н���
						Recv_Msg(hwnd,ARecvStr,false);
					}
					break;
				case 104://����֪ͨ
					closesocket(uIformation->Socket);
					MessageBox(hwnd,TEXT("�����˺�����ص�½,�����������½�����������"),TEXT("��������"),MB_OK);
					break;
				case 105://���ڶ���
					{
						Recv_Msg(hwnd,ARecvStr,true);
					}
					break;

				}				
			}
			break;
		case FD_CLOSE:
			MessageBox(hwnd,TEXT("��������Ͽ�����,���Ѵ�������״̬"),TEXT("��������"),MB_OK);
			//ɾ�����ݿ��е�socket
			closesocket(uIformation->Socket);
			break;
	}
	return 0;
}



//���غ����б�
void LoadFriend_List(HWND hwnd,char* FriendList)
{
	//MessageBox(NULL,"list","",0);
	char FriendStr[255];
	memset(FriendStr,0,sizeof(FriendStr));
	//���������б�
	for(int i = 0; FriendList[i]; )
	{
		if ( FriendList[i] == '-' )
		{
			i++;
			//��������Ϣ�ֿ�
			for(int j=0;(FriendList[i] != '-')&&(FriendList[i]);j++)
			{
				FriendStr[j] = FriendList[i++];
			}
			FriendStr[j] = '\0';
			//�ٴ���ȡ
			char NameStr[255];
			char IconId[25];
			char Name[255];
			char Id[25];
			sscanf(FriendStr,"%[^.].%s",IconId,NameStr);
			sscanf(NameStr,"%[^(](%[^)]",Name,Id);
			//����Ϣ���뵽�����б���
			FriendInfor frinfor;
			frinfor.Id = atoi(Id);
			frinfor.IconNum = atoi(IconId);
			strcpy(frinfor.Name,Name);
			Friendlist.insert( pair<int,FriendInfor>(frinfor.Id,frinfor) );
			
			//��������Ϣ��ӵ��б�
			AddItemToTree(hPage[0],IDC_FRTREE,NameStr,atoi(IconId),hRoot,false);
		}
	}
}


//��������
void DeleOff_Friend(HWND hwnd,char * FriendStr)
{
	char FriendName[255];
	sscanf(FriendStr,"-%s",FriendName);
	if ( !DeleteItemByText(hPage[0],IDC_FRTREE,FriendName) )
	{
		MessageBox(hwnd,"��������ɾ������ʧ��","",0);
	}
}

//�򿪴��ڷ�������Ϣ
void ChatWithFriend(HWND hwnd,char* NameStr)
{
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
	//��ʼ��������Ϣ
	MsgSendToDlg MsgSend;
	memset(&MsgSend,0,sizeof(MsgSend));
	MsgSend.hwnd = hwnd;
	char UserId[25];
	sscanf(NameStr,"%[^(](%[^)]",MsgSend.name,UserId);
	MsgSend.id = atoi(UserId);
	MsgSend.socket = uIformation->Socket;
	//������Ϣ
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_CHATDIALOG), NULL, ChatDlg_Proc,(LPARAM)&MsgSend);
}


//������Ϣ����
void Recv_Msg(HWND hwnd,char * MsgStr,bool IsOpen)
{
	int Uid = 0;
	//��ȡ�û�id
	char UidStr[25];
	char Msg[1024];
	sscanf(MsgStr,"-%[^-]-%[^#]",UidStr,Msg);
	Uid = atoi(UidStr);
	//���Ҵ����Ƿ��
	map <int,HWND>::iterator iter;
	iter = OpenWinList.find( Uid );
	if ( iter == OpenWinList.end() )
	{
		//δ�ҵ���ӵ������б�
		//������Ϣ�б�
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
		//�ҵ������ڷ�����Ϣ
		COPYDATASTRUCT CopyData;
		CopyData.cbData = 0;
		CopyData.dwData = sizeof(Msg);
		CopyData.lpData = Msg;
		SendMessage(iter->second,WM_COPYDATA,(WPARAM)hwnd,(LPARAM)&CopyData);
	}
	//��������sendmsg
	//��Ϊ�������list�������û�����Ϣ

	//recvList.push_back(buff);
}

//������ʾ��Ϣ
DWORD WINAPI TellHaveMsg(LPVOID lpParam)
{
	HWND hwnd = (HWND)lpParam;
	HINSTANCE hInstance=(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
	HICON hicon1 = LoadIcon( hInstance,MAKEINTRESOURCE(IDI_ICON_MSG1) );
	HICON hicon2 = LoadIcon( hInstance,MAKEINTRESOURCE(IDI_ICON_MSG2) );
	while(1)
	{
		//�ж���û����Ϣ
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

