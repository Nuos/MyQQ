#include "stdafx.h"
#include "AcceptAndLogin.h"

bool SendMsg(SOCKET socket,int MsgType,char* MsgStr);


//������������
bool AcceptMessage( HWND hwnd,SOCKET socket,SOCKET serverSocket )
{
	struct sockaddr_in clientAddress;//�����Ϳͻ���ͨ�ŵ��׽��ֵ�ַ 
	memset(&clientAddress,0,sizeof(clientAddress)); 
	int addrlen = sizeof(clientAddress);
	//sprintf(serverSta,"��������״̬");
	SOCKET ClientSocket;
	//�������� 
	if( (ClientSocket=accept(serverSocket,(sockaddr*)&clientAddress,&addrlen)) == INVALID_SOCKET )
	{ 
		SetServerLog(hwnd,IDC_EDIT1,"���ܿͻ�������ʧ��!");
	//	sprintf(Error,"���ܿͻ�������ʧ�ܣ�"); 
		return false; 
	}
	SetServerLog(hwnd,IDC_EDIT1,"���ܿͻ�������...");
	return true;
	//sprintf(serverSta,"��������: %s������\n",inet_ntoa(clientAddress.sin_addr));
}


//map <SOCKET,int >Map1;           //map1 ����socket��uid
//map <int , UserInfor>Map2;
//������Ϣ
bool ReadMessage(HWND hwnd,SOCKET socket,MySql& mysql,
				 map <SOCKET,int > &Map1 ,map <int , UserInfor>& Map2 ,map <int ,Cache>&LocalCache)
{
	const int MAX_SIZE = 1024;
	char RecvStr[MAX_SIZE*6];
	memset( RecvStr,0,sizeof(RecvStr) );
	int RecvLen = 0;
	
	//������Ϣ
	if( (RecvLen=recv(socket,RecvStr,MAX_SIZE,0)) == SOCKET_ERROR ) 
	{ 
		SetServerLog(hwnd,IDC_EDIT1,"������Ϣʧ��...");
		return 0; 
	}

	//��������ת��
	MsgInfor* RecvMsg = (MsgInfor*)RecvStr;
	
	//������Ϣ
	char ARecvStr[MAX_SIZE];
	CodeUrl(ARecvStr,RecvMsg->MsgStr);

	//����Э��
	switch( RecvMsg->MsgType )
	{
	case 0://ע��   (-uid-upasswd-uname...)Ĭ��ֵΪ0
		{
			MsgInfor Register_Msg;
			if (strlen(ARecvStr) == 0)
			{
				Register_Msg.MsgType = 0;
			}
			else
			{
				char uid[20];
				char upasswd[20];
				char uname[20];
				char uiconum[20];
				try
				{
					sscanf(ARecvStr,"-%[^-]-%[^-]-%[^-]%s",uid,upasswd,uname,uiconum);
				}
				catch(...)
				{
					//��Ϣʧ��
					//����ʧ�ܴ���
				}
				//����ע��
				char SqlStr[1024];
				sprintf(SqlStr,"insert into userinfor(uid,uname,upasswd,uiconum) values(%s,'%s','%s',%s)",
					uid,upasswd,uname,uiconum);

				if ( mysql.Insert(SqlStr) >0 )
				{
					SetServerLog(hwnd,IDC_EDIT1,"ע��ɹ�!");
					Register_Msg.MsgType = 1;
					SetCache(LocalCache,200);
				}
				else
				{
					Register_Msg.MsgType = 0;
					//����ʧ�ܴ���
				}
			}
			if( send(socket,(char*)&Register_Msg,sizeof(Register_Msg),0) == SOCKET_ERROR )
			{ 	
				//����ʧ��
				SetServerLog(hwnd,IDC_EDIT1,"����ע����Ϣʧ��");
			}
		}
	break;
	case 1://��¼ (-uid-upasswd)
		{
			int  Login_RecvId = 0;
			char Login_RecvStr[MAX_SIZE];
			char LoginUserStr[MAX_SIZE];//����֪ͨ����
			//��ȡ�û�������
			char uid[25];
			int  id = 0;
			char upasswd[25];
			sscanf(ARecvStr,"-%[^-]-%[^-]",uid,upasswd);
			id = atoi(uid);
			map <int ,Cache>::iterator iter;
			if ( (iter = LocalCache.find(id) ) != LocalCache.end() )
			{
			//�ҵ��û�
				//��֤����
				if ( !strcmp(upasswd,iter->second.UPasswd.c_str() ) )
				{
				//��֤�ɹ�
					sprintf(Login_RecvStr,"-%s-%d",iter->second.UName.c_str(),iter->second.IcoNum);
					sprintf(LoginUserStr,"-%d.%s(%d)",
						iter->second.IcoNum,iter->second.UName.c_str(),iter->second.UId);
					Login_RecvId = 1;
				}
				else
				{
				//��֤ʧ��
				}
			}
			else
			{
			//û������û�
			}
			//����������Ϣ
			MsgInfor Login_RrecvMsg;
			Login_RrecvMsg.MsgType = Login_RecvId;
			EncodeUrl(Login_RrecvMsg.MsgStr,Login_RecvStr);

			//������Ϣ
			if( send(socket,(char*)&Login_RrecvMsg,sizeof(Login_RrecvMsg),0) == SOCKET_ERROR )
			{ 
				//����ʧ��
				//SetDlgItemText(hwnd,IDC_LOGSTA,"��������ʧ��!");
				//return ; 
			}
			else
			{
				if( Login_RecvId == 1 )//��½�ɹ�
				{
					/*
					map <SOCKET,int >Map1;           //map1 ����socket��uid
					map <int , UserInfor>Map2;       //map2 ����uid���û���Ϣ
					*/
					//����map--����������Ϣ
					
					map<int , UserInfor>::iterator offlineiter;//�Ƿ��Ѿ���¼
					offlineiter = Map2.find( id );
					if ( offlineiter != Map2.end() )//�Ѿ���¼
					{
						//֪ͨ����
						if ( !SendMsg(offlineiter->second.socket,104,"") )
						{
							SetServerLog(hwnd,IDC_EDIT1,"��������֪ͨʧ��");
							break;
						}
						else
						{
							SetServerLog(hwnd,IDC_EDIT1,"��������֪ͨ");
						}	
						//ɾ����Ϣ
						//�Ͽ�����
						//closesocket( offlineiter->second.socket );
						Map1.erase( offlineiter->second.socket );
						Map2.erase( offlineiter->first);
					}

					//��������֪ͨ
					TellFriend(Map1,LoginUserStr);

					//����map--
					UserInfor AddUser;
					AddUser.socket = socket;
					AddUser.UId = id;
					AddUser.UName = iter->second.UName;
					AddUser.IcoNum = iter->second.IcoNum;
					Map1.insert(pair<SOCKET,int>(socket,id));
					Map2.insert( pair<int,UserInfor>(id,AddUser) );
					char buff[255];
					sprintf(buff,"�û�%d��¼",id);
					SetServerLog(hwnd,IDC_EDIT1,buff);
				}
				else
				{
					closesocket(socket);
				}
			}
		}
	break;
	case 2://�鿴ID�Ƿ����
		{
			//MsgInfor CheckID;
			int ReturnType = 0;
			char uid[20];
			
			try
			{
				sscanf(ARecvStr,"-%s",uid);
				//���в�ѯ
				int rows = 0;
				int cols = 0;
				char *** result = NULL;
				char SqlStr[1024];
				sprintf(SqlStr,"select count(*) from userinfor where uid = %s",uid);
				result = mysql.Select(SqlStr,rows,cols);
				if ( (result == NULL)||(atoi( result[0][0] ) != 0) )//��ѯ�����������id�Ѿ�ʹ��
				{
					ReturnType = 0;
				}
				else
				{
					ReturnType = 1;
				}
			}
			catch(...)
			{
				ReturnType = 0;
			}

			//����
			if ( !SendMsg(socket,ReturnType,"") )
			{
				SetServerLog(hwnd,IDC_EDIT1,"���Ͳ鿴id����ʧ��!");
				break;
			}
			else
			{
				SetServerLog(hwnd,IDC_EDIT1,"��ѯ�û�ID�Ƿ����");
			}		
		}
	break;
	case 3://�б���Ϣ--�������û����͹�ȥ
		{
			char FriendList[1024];//���˱����б���Ϣ
			FriendList[0] = '\0';

			//����map��ȡ�б���Ϣ
			map <int , UserInfor>::iterator iter;
			for (iter = Map2.begin(); iter != Map2.end(); iter++)
			{
				sprintf(FriendList,"%s-%d.%s(%d)",FriendList,
					iter->second.IcoNum,
					iter->second.UName.c_str(),
					iter->second.UId);
			}

			//����
			if ( !SendMsg(socket,100,FriendList) )
			{
				SetDlgItemText(hwnd,IDC_EDIT1,"��ȡ�����б�ʧ��!");
				break;
			}
			else
			{
				SetServerLog(hwnd,IDC_EDIT1,"���ͺ����б�ɹ�");
			}
		}
	break;
	case 4://������Ϣ(-id-msg#)
		{
			char RecvUserId[25];//������Ϣ�ߵ�id
			char MsgStr[1024];//���͵���Ϣ
			SOCKET recvsocket;//�����ߵ�socket

			//�õ�������id����Ϣ
			sscanf(ARecvStr,"-%[^-]-%[^#]",RecvUserId,MsgStr);

			//�õ�������socket
			map <int , UserInfor>::iterator inforiter;
			inforiter = Map2.find(atoi(RecvUserId));	
			if (inforiter != Map2.end())
			{
				recvsocket = inforiter->second.socket;
			}
			else
			{
				SetServerLog(hwnd,IDC_EDIT1,"���յ��û�������Ϣ����δ�ҵ������û�");
				break;
			}
			
			//�õ�������id
			int SendUserId = 0;
			map <SOCKET,int >::iterator socketiter;
			socketiter = Map1.find(socket);
			if (socketiter != Map1.end())
			{
				SendUserId = socketiter->second;
			}
			else
			{
				SetServerLog(hwnd,IDC_EDIT1,"���յ��û�������Ϣ����δ�ҵ������û�");
				break;
			}

			//���췢����Ϣ-id-msg
			char SendMsgStr[1024];
			sprintf(SendMsgStr,"-%d-%s#",SendUserId,MsgStr);
			MsgInfor Send_Msg;
			Send_Msg.MsgType = 103;
			EncodeUrl(Send_Msg.MsgStr,SendMsgStr);
			
			//������Ϣ
			//����
			if( send(recvsocket,(char*)&Send_Msg,sizeof(Send_Msg),0) == SOCKET_ERROR )
			{ 
				//����ʧ��
				SetDlgItemText(hwnd,IDC_EDIT1,"���յ��û�������Ϣ��ת��ʧ��!");
				break; 
			}
		}
	break;
	case 5://���ڶ���
		{
			char RecvUserId[25];//������Ϣ�ߵ�id
			char MsgStr[1024];//���͵���Ϣ
			SOCKET recvsocket;//�����ߵ�socket

			//�õ�������id����Ϣ
			sscanf(ARecvStr,"-%[^-]-%[^#]",RecvUserId,MsgStr);

			//�õ�������socket
			map <int , UserInfor>::iterator inforiter;
			inforiter = Map2.find(atoi(RecvUserId));	
			if (inforiter != Map2.end())
			{
				recvsocket = inforiter->second.socket;
			}
			else
			{
				SetServerLog(hwnd,IDC_EDIT1,"���յ��û�������Ϣ����δ�ҵ������û�");
				break;
			}
			
			//�õ�������id
			int SendUserId = 0;
			map <SOCKET,int >::iterator socketiter;
			socketiter = Map1.find(socket);
			if (socketiter != Map1.end())
			{
				SendUserId = socketiter->second;
			}
			else
			{
				SetServerLog(hwnd,IDC_EDIT1,"���յ��û�������Ϣ����δ�ҵ������û�");
				break;
			}

			//���췢����Ϣ-id-msg
			char SendMsgStr[1024];
			sprintf(SendMsgStr,"-%d-%s#",SendUserId,MsgStr);
			MsgInfor Send_Msg;
			Send_Msg.MsgType = 105;
			EncodeUrl(Send_Msg.MsgStr,SendMsgStr);
			
			//������Ϣ
			//����
			if( send(recvsocket,(char*)&Send_Msg,sizeof(Send_Msg),0) == SOCKET_ERROR )
			{ 
				//����ʧ��
				SetDlgItemText(hwnd,IDC_EDIT1,"���յ��û�������Ϣ��ת��ʧ��!");
				break; 
			}
		}

	break;
/*
	case '5'://�޸���Ϣ
	break;
	case '6'://���Һ���
	break;
	case '7'://��Ӻ���
	break;
	case '8'://ɾ������
	break;
	case '9'://�鿴��Ϣ
	break;
	case 'A'://����
	break;
	case 'B'://�ļ�
	break;*/
	default:
	break;
	}
	return true;
}

//��ָ����socket����ָ������Ϣ
bool SendMsg(SOCKET socket,int MsgType,char* MsgStr)
{
	if (MsgStr == NULL)
	{
		return false;
	}
	MsgInfor Send_Msg;
	Send_Msg.MsgType = MsgType;
	EncodeUrl(Send_Msg.MsgStr,MsgStr);
	if( send(socket,(char*)&Send_Msg,sizeof(Send_Msg),0) == SOCKET_ERROR )
	{ 
		return false;
	}
	else 
	{
		return true;
	}
}

//֪ͨ��������
void TellFriend(map <SOCKET,int > &Map1,char* LoginUserStr )
{
	MsgInfor AskFriend;
	AskFriend.MsgType = 100;
	//EncodeUrl(AskFriend.MsgStr,LoginUserStr);
	EncodeUrl(AskFriend.MsgStr,LoginUserStr);

	map<SOCKET,int>::iterator iter;

	for (iter = Map1.begin(); iter != Map1.end(); iter++)
	{
		if( send(iter->first,(char*)&AskFriend,sizeof(AskFriend),0) == SOCKET_ERROR )
		{ 
			MessageBox(NULL,"���͵�¼��Ϣʧ��","",0);
			//����ʧ��
			//SetDlgItemText(hwnd,IDC_LOGSTA,"��������ʧ��!");
			//return ; 
		}
	}
}