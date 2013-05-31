///////////////////////////////////////////////
///������Ļ��Ϣ
//////////////////////////////////////////////
#include "stdafx.h"
#include "SetScreenMsg.h"


//���ô�����Ϣ�Զ�����
void SetServerLog(HWND hwnd,int ControlId,char* LogStr)
{
	//��־�ļ�����󳤶�
	const int LogMsgSize = 1024; 

	//��ȡԭ�ؼ��ı���Ϣ
	char LogMsg[LogMsgSize];
	GetDlgItemText(hwnd,ControlId,LogMsg,LogMsgSize);

	//����Ϣ��ӵ��ļ�β
	if( strlen(LogMsg) > (LogMsgSize-100) )
	{
		sprintf(LogMsg,"%s\r\n\r\n%s",&LogMsg[500],LogStr);
	}
	else
	{
		sprintf(LogMsg,"%s\r\n\r\n%s",LogMsg,LogStr);
	}

	//���ÿؼ���Ϣ
	SetDlgItemText(hwnd,ControlId,LogMsg);
	
	//��ȡӦ�����¹�������
	int n=0;
	for(int i=0; LogMsg[i]; i++)
	{
		if ( LogMsg[i]=='\n' )
		{
			n++;
		}
	}

	//���͹�����Ϣ
	SendDlgItemMessage(hwnd,ControlId,EM_LINESCROLL,0,n);
}