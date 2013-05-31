#ifndef _MAIN_H
#define _MAIN_H

#include <windows.h>


//���͸��ӶԻ������Ϣ��ʽ
struct MsgSendToDlg
{
	int id;          //�û���id
	char name[255];       //�û���name
	int iconNum;     //�û���ͼ��
	SOCKET socket;   //���̼乲���socket
	HWND hwnd;       //��ǰ���ھ�� Ϊ�������巢����Ϣ��׼��
};

BOOL WINAPI Main_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL Main_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void Main_OnClose(HWND hwnd);
int  Main_OnServerMsg(HWND hwnd,WPARAM wParam,LPARAM lParam);
void LoadFriend_List(HWND hwnd,char* FriendList);
void DeleOff_Friend(HWND hwnd,char * FriendStr);
void RecvMsg(char * buff);
HICON LoadIconByText(char* FileName);
void LoadFriendBox(HWND hwnd);
void Main_OnSeleChange(HWND hwnd, LPARAM lParam);
void LoadForm(HINSTANCE hInstance,HWND hwnd);
DWORD WINAPI GetFriendList(LPVOID lpParam);
void ChatWithFriend(HWND hwnd,char* NameStr);
DWORD WINAPI TellHaveMsg(LPVOID lpParam);
//void Recv_Msg(HWND hwnd,char * MsgStr);
void Recv_Msg(HWND hwnd,char * MsgStr,bool IsOpen);
bool ReadMsgList(HWND hwnd);
void ReadMsgToDlg(HWND hwnd,HWND hchild,int id);

#endif