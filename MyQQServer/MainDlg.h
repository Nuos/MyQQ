#ifndef _MAIN_H
#define _MAIN_H

#include <windows.h>
#include <winsock2.h> 
#include <string>
#include <map>
#include "SetScreenMsg.h"
#include "EnCoding.h"
#include "MySql.h"
using namespace std;
#pragma comment(lib,"ws2_32.lib") 

//�û���Ϣ�ṹ��
struct UserInfor
{
	SOCKET  socket;
	int UId;     //�û�id
	int IcoNum;  //ͼ����Ϣ
	string UName; //�û���
};

//cache�û���Ϣ
struct Cache
{
	int UId;
	string UName;
	string UPasswd;
	int IcoNum;
};

BOOL WINAPI Main_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL Main_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void Main_OnClose(HWND hwnd);
bool SetWindowSize( HWND hwnd,long xPos_Change,long yPos_Change);
void SeeLogMsg(HWND hwnd);
bool  Listen(HWND hwnd);
void StartServer(HWND hwnd);
void StopServer(HWND hwnd);
void ServerMesssageCome(HWND hwnd,WPARAM wParam,LPARAM lParam);
void CloseMessage(HWND hwnd,SOCKET socket);
void SetCache( map <int ,Cache> &Local_Cache,int n );
//void SetCache( Cache* localcache,int n );


#endif