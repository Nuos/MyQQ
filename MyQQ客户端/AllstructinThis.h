#include <list>
using namespace std;

const int MaxSend = 1024;
//当前用户的信息
struct UserInfor
{
	SOCKET Socket;     //用户的socket
	char  uName[25];   //用户的名称
	char  uId[25];     //用户的id号
	char  upasswd[25]; //用户密码
	char  uico[25];
};

//发送消息格式结构体
struct MsgInfor
{
	int MsgType;
	char Msg[MaxSend*6];
};

//char* UNICODEtoASCII(char * str);
//char* ASCIItoUNICODE(char * str);

//主窗口与聊天窗口传递信息
struct ChatFriend
{
	UserInfor* userinfor;
	char*  fname;
	int  fId;
	list<char*> *recvList;
	
};

int EncodeUrl(char* UrlStr,char* AscStr );
int CodeUrl(char* AscStr,char* UrlStr);
bool SetWindowSize( HWND hwnd,long xPos_Change,long yPos_Change);