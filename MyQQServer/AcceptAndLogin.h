#include "resource.h"
#include "MainDlg.h"

struct MsgInfor
{
	int MsgType;
	char MsgStr[1024];
};

bool AcceptMessage( HWND hwnd,SOCKET socket,SOCKET serverSocket );
//bool ReadMessage(HWND hwnd,SOCKET socket,MySql& mysql,
//				 map <SOCKET,int > &Map1 ,map <int , UserInfor>& Map2);
bool ReadMessage(HWND hwnd,SOCKET socket,MySql& mysql,
				 map <SOCKET,int > &Map1 ,map <int , UserInfor>& Map2 ,map <int ,Cache>&LocalCache);
//bool ReadMessage(HWND hwnd,SOCKET socket,MySql& mysql);
void TellFriend(map <SOCKET,int > &Map1,char* LoginUserStr );
