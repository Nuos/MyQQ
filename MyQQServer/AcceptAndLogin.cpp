#include "stdafx.h"
#include "AcceptAndLogin.h"

bool SendMsg(SOCKET socket,int MsgType,char* MsgStr);


//接收连接请求
bool AcceptMessage( HWND hwnd,SOCKET socket,SOCKET serverSocket )
{
	struct sockaddr_in clientAddress;//用来和客户端通信的套接字地址 
	memset(&clientAddress,0,sizeof(clientAddress)); 
	int addrlen = sizeof(clientAddress);
	//sprintf(serverSta,"正常监听状态");
	SOCKET ClientSocket;
	//接受连接 
	if( (ClientSocket=accept(serverSocket,(sockaddr*)&clientAddress,&addrlen)) == INVALID_SOCKET )
	{ 
		SetServerLog(hwnd,IDC_EDIT1,"接受客户端连接失败!");
	//	sprintf(Error,"接受客户端连接失败！"); 
		return false; 
	}
	SetServerLog(hwnd,IDC_EDIT1,"接受客户端连接...");
	return true;
	//sprintf(serverSta,"接受来自: %s的连接\n",inet_ntoa(clientAddress.sin_addr));
}


//map <SOCKET,int >Map1;           //map1 保存socket与uid
//map <int , UserInfor>Map2;
//接收消息
bool ReadMessage(HWND hwnd,SOCKET socket,MySql& mysql,
				 map <SOCKET,int > &Map1 ,map <int , UserInfor>& Map2 ,map <int ,Cache>&LocalCache)
{
	const int MAX_SIZE = 1024;
	char RecvStr[MAX_SIZE*6];
	memset( RecvStr,0,sizeof(RecvStr) );
	int RecvLen = 0;
	
	//接收消息
	if( (RecvLen=recv(socket,RecvStr,MAX_SIZE,0)) == SOCKET_ERROR ) 
	{ 
		SetServerLog(hwnd,IDC_EDIT1,"接收消息失败...");
		return 0; 
	}

	//进行类型转换
	MsgInfor* RecvMsg = (MsgInfor*)RecvStr;
	
	//解码信息
	char ARecvStr[MAX_SIZE];
	CodeUrl(ARecvStr,RecvMsg->MsgStr);

	//分析协议
	switch( RecvMsg->MsgType )
	{
	case 0://注册   (-uid-upasswd-uname...)默认值为0
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
					//信息失败
					//返回失败代码
				}
				//进行注册
				char SqlStr[1024];
				sprintf(SqlStr,"insert into userinfor(uid,uname,upasswd,uiconum) values(%s,'%s','%s',%s)",
					uid,upasswd,uname,uiconum);

				if ( mysql.Insert(SqlStr) >0 )
				{
					SetServerLog(hwnd,IDC_EDIT1,"注册成功!");
					Register_Msg.MsgType = 1;
					SetCache(LocalCache,200);
				}
				else
				{
					Register_Msg.MsgType = 0;
					//返回失败代码
				}
			}
			if( send(socket,(char*)&Register_Msg,sizeof(Register_Msg),0) == SOCKET_ERROR )
			{ 	
				//发送失败
				SetServerLog(hwnd,IDC_EDIT1,"返回注册信息失败");
			}
		}
	break;
	case 1://登录 (-uid-upasswd)
		{
			int  Login_RecvId = 0;
			char Login_RecvStr[MAX_SIZE];
			char LoginUserStr[MAX_SIZE];//用于通知好友
			//提取用户名密码
			char uid[25];
			int  id = 0;
			char upasswd[25];
			sscanf(ARecvStr,"-%[^-]-%[^-]",uid,upasswd);
			id = atoi(uid);
			map <int ,Cache>::iterator iter;
			if ( (iter = LocalCache.find(id) ) != LocalCache.end() )
			{
			//找到用户
				//验证密码
				if ( !strcmp(upasswd,iter->second.UPasswd.c_str() ) )
				{
				//验证成功
					sprintf(Login_RecvStr,"-%s-%d",iter->second.UName.c_str(),iter->second.IcoNum);
					sprintf(LoginUserStr,"-%d.%s(%d)",
						iter->second.IcoNum,iter->second.UName.c_str(),iter->second.UId);
					Login_RecvId = 1;
				}
				else
				{
				//验证失败
				}
			}
			else
			{
			//没有这个用户
			}
			//构建返回信息
			MsgInfor Login_RrecvMsg;
			Login_RrecvMsg.MsgType = Login_RecvId;
			EncodeUrl(Login_RrecvMsg.MsgStr,Login_RecvStr);

			//发送信息
			if( send(socket,(char*)&Login_RrecvMsg,sizeof(Login_RrecvMsg),0) == SOCKET_ERROR )
			{ 
				//发送失败
				//SetDlgItemText(hwnd,IDC_LOGSTA,"发送数据失败!");
				//return ; 
			}
			else
			{
				if( Login_RecvId == 1 )//登陆成功
				{
					/*
					map <SOCKET,int >Map1;           //map1 保存socket与uid
					map <int , UserInfor>Map2;       //map2 保存uid与用户信息
					*/
					//查找map--发送下线消息
					
					map<int , UserInfor>::iterator offlineiter;//是否已经登录
					offlineiter = Map2.find( id );
					if ( offlineiter != Map2.end() )//已经登录
					{
						//通知下线
						if ( !SendMsg(offlineiter->second.socket,104,"") )
						{
							SetServerLog(hwnd,IDC_EDIT1,"发送下线通知失败");
							break;
						}
						else
						{
							SetServerLog(hwnd,IDC_EDIT1,"发送下线通知");
						}	
						//删除信息
						//断开连接
						//closesocket( offlineiter->second.socket );
						Map1.erase( offlineiter->second.socket );
						Map2.erase( offlineiter->first);
					}

					//发送上线通知
					TellFriend(Map1,LoginUserStr);

					//更新map--
					UserInfor AddUser;
					AddUser.socket = socket;
					AddUser.UId = id;
					AddUser.UName = iter->second.UName;
					AddUser.IcoNum = iter->second.IcoNum;
					Map1.insert(pair<SOCKET,int>(socket,id));
					Map2.insert( pair<int,UserInfor>(id,AddUser) );
					char buff[255];
					sprintf(buff,"用户%d登录",id);
					SetServerLog(hwnd,IDC_EDIT1,buff);
				}
				else
				{
					closesocket(socket);
				}
			}
		}
	break;
	case 2://查看ID是否可用
		{
			//MsgInfor CheckID;
			int ReturnType = 0;
			char uid[20];
			
			try
			{
				sscanf(ARecvStr,"-%s",uid);
				//进行查询
				int rows = 0;
				int cols = 0;
				char *** result = NULL;
				char SqlStr[1024];
				sprintf(SqlStr,"select count(*) from userinfor where uid = %s",uid);
				result = mysql.Select(SqlStr,rows,cols);
				if ( (result == NULL)||(atoi( result[0][0] ) != 0) )//查询不到结果或者id已经使用
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

			//发送
			if ( !SendMsg(socket,ReturnType,"") )
			{
				SetServerLog(hwnd,IDC_EDIT1,"发送查看id数据失败!");
				break;
			}
			else
			{
				SetServerLog(hwnd,IDC_EDIT1,"查询用户ID是否可用");
			}		
		}
	break;
	case 3://列表信息--将在线用户发送过去
		{
			char FriendList[1024];//用了保存列表信息
			FriendList[0] = '\0';

			//遍历map提取列表信息
			map <int , UserInfor>::iterator iter;
			for (iter = Map2.begin(); iter != Map2.end(); iter++)
			{
				sprintf(FriendList,"%s-%d.%s(%d)",FriendList,
					iter->second.IcoNum,
					iter->second.UName.c_str(),
					iter->second.UId);
			}

			//发送
			if ( !SendMsg(socket,100,FriendList) )
			{
				SetDlgItemText(hwnd,IDC_EDIT1,"获取好友列表失败!");
				break;
			}
			else
			{
				SetServerLog(hwnd,IDC_EDIT1,"发送好友列表成功");
			}
		}
	break;
	case 4://发送消息(-id-msg#)
		{
			char RecvUserId[25];//接收消息者的id
			char MsgStr[1024];//发送的消息
			SOCKET recvsocket;//接收者的socket

			//得到接收者id与消息
			sscanf(ARecvStr,"-%[^-]-%[^#]",RecvUserId,MsgStr);

			//得到接收者socket
			map <int , UserInfor>::iterator inforiter;
			inforiter = Map2.find(atoi(RecvUserId));	
			if (inforiter != Map2.end())
			{
				recvsocket = inforiter->second.socket;
			}
			else
			{
				SetServerLog(hwnd,IDC_EDIT1,"接收到用户发送消息但是未找到接收用户");
				break;
			}
			
			//得到发送者id
			int SendUserId = 0;
			map <SOCKET,int >::iterator socketiter;
			socketiter = Map1.find(socket);
			if (socketiter != Map1.end())
			{
				SendUserId = socketiter->second;
			}
			else
			{
				SetServerLog(hwnd,IDC_EDIT1,"接收到用户发送消息但是未找到发送用户");
				break;
			}

			//构造发送信息-id-msg
			char SendMsgStr[1024];
			sprintf(SendMsgStr,"-%d-%s#",SendUserId,MsgStr);
			MsgInfor Send_Msg;
			Send_Msg.MsgType = 103;
			EncodeUrl(Send_Msg.MsgStr,SendMsgStr);
			
			//发送消息
			//发送
			if( send(recvsocket,(char*)&Send_Msg,sizeof(Send_Msg),0) == SOCKET_ERROR )
			{ 
				//发送失败
				SetDlgItemText(hwnd,IDC_EDIT1,"接收到用户发送消息但转发失败!");
				break; 
			}
		}
	break;
	case 5://窗口抖动
		{
			char RecvUserId[25];//接收消息者的id
			char MsgStr[1024];//发送的消息
			SOCKET recvsocket;//接收者的socket

			//得到接收者id与消息
			sscanf(ARecvStr,"-%[^-]-%[^#]",RecvUserId,MsgStr);

			//得到接收者socket
			map <int , UserInfor>::iterator inforiter;
			inforiter = Map2.find(atoi(RecvUserId));	
			if (inforiter != Map2.end())
			{
				recvsocket = inforiter->second.socket;
			}
			else
			{
				SetServerLog(hwnd,IDC_EDIT1,"接收到用户发送消息但是未找到接收用户");
				break;
			}
			
			//得到发送者id
			int SendUserId = 0;
			map <SOCKET,int >::iterator socketiter;
			socketiter = Map1.find(socket);
			if (socketiter != Map1.end())
			{
				SendUserId = socketiter->second;
			}
			else
			{
				SetServerLog(hwnd,IDC_EDIT1,"接收到用户发送消息但是未找到发送用户");
				break;
			}

			//构造发送信息-id-msg
			char SendMsgStr[1024];
			sprintf(SendMsgStr,"-%d-%s#",SendUserId,MsgStr);
			MsgInfor Send_Msg;
			Send_Msg.MsgType = 105;
			EncodeUrl(Send_Msg.MsgStr,SendMsgStr);
			
			//发送消息
			//发送
			if( send(recvsocket,(char*)&Send_Msg,sizeof(Send_Msg),0) == SOCKET_ERROR )
			{ 
				//发送失败
				SetDlgItemText(hwnd,IDC_EDIT1,"接收到用户发送消息但转发失败!");
				break; 
			}
		}

	break;
/*
	case '5'://修改信息
	break;
	case '6'://查找好友
	break;
	case '7'://添加好友
	break;
	case '8'://删除好友
	break;
	case '9'://查看信息
	break;
	case 'A'://抖动
	break;
	case 'B'://文件
	break;*/
	default:
	break;
	}
	return true;
}

//给指定的socket发送指定的消息
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

//通知好友上线
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
			MessageBox(NULL,"发送登录信息失败","",0);
			//发送失败
			//SetDlgItemText(hwnd,IDC_LOGSTA,"发送数据失败!");
			//return ; 
		}
	}
}