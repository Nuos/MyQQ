#include <list>
using namespace std;

const int MaxSend = 1024;
//��ǰ�û�����Ϣ
struct UserInfor
{
	SOCKET Socket;     //�û���socket
	char  uName[25];   //�û�������
	char  uId[25];     //�û���id��
	char  upasswd[25]; //�û�����
	char  uico[25];
};

//������Ϣ��ʽ�ṹ��
struct MsgInfor
{
	int MsgType;
	char Msg[MaxSend*6];
};

//char* UNICODEtoASCII(char * str);
//char* ASCIItoUNICODE(char * str);

//�����������촰�ڴ�����Ϣ
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