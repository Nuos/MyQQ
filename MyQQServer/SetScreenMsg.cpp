///////////////////////////////////////////////
///设置屏幕信息
//////////////////////////////////////////////
#include "stdafx.h"
#include "SetScreenMsg.h"


//设置窗口消息自动下移
void SetServerLog(HWND hwnd,int ControlId,char* LogStr)
{
	//日志文件的最大长度
	const int LogMsgSize = 1024; 

	//获取原控件文本信息
	char LogMsg[LogMsgSize];
	GetDlgItemText(hwnd,ControlId,LogMsg,LogMsgSize);

	//将信息添加到文件尾
	if( strlen(LogMsg) > (LogMsgSize-100) )
	{
		sprintf(LogMsg,"%s\r\n\r\n%s",&LogMsg[500],LogStr);
	}
	else
	{
		sprintf(LogMsg,"%s\r\n\r\n%s",LogMsg,LogStr);
	}

	//设置控件信息
	SetDlgItemText(hwnd,ControlId,LogMsg);
	
	//获取应该向下滚动多少
	int n=0;
	for(int i=0; LogMsg[i]; i++)
	{
		if ( LogMsg[i]=='\n' )
		{
			n++;
		}
	}

	//发送滚屏消息
	SendDlgItemMessage(hwnd,ControlId,EM_LINESCROLL,0,n);
}