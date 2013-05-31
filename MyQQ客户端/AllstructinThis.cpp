#include "stdafx.h"
#include "AllstructinThis.h"

//asc->url
int EncodeUrl(char* UrlStr,char* AscStr )//,int Size)
{
	if( (UrlStr == NULL)||(AscStr == NULL) )//||(Size <= 0) )
	{
		return 0;
	}

	strcpy(UrlStr,"");
	//byte a;

	for (int i=0; AscStr[i]; i++)
	{
		if ( (AscStr[i]>='0'&&AscStr[i]<='9') || (AscStr[i]>='A'&&AscStr[i]<='z') )
		{
			sprintf(UrlStr,"%s%c",UrlStr,AscStr[i]);
		}
		else if (AscStr[i]==' ')
		{
			sprintf(UrlStr,"%s+",UrlStr);
		}
		else
		{
			sprintf(UrlStr,"%s%%%x",UrlStr,(BYTE)AscStr[i]);
		}
	}
	return strlen(AscStr);

}


//url->asc
int CodeUrl(char* AscStr,char* UrlStr)//,int Size)
{
	if( (UrlStr == NULL)||(AscStr == NULL) )//||(Size <= 0) )
	{
		return 0;
	}

	strcpy(AscStr,"");

	for(int i=0; UrlStr[i] ;i++)
	{
		switch (UrlStr[i])
		{
		case '+':
			sprintf(AscStr,"%s ",AscStr);
			break;
		case '%':
			{
				char buff[3];
				sprintf(buff,"%c%c",UrlStr[i+1],UrlStr[i+2]);
				int	Lvalue = strtol(buff, 0, 16);
				
				sprintf(AscStr,"%s%c",AscStr,Lvalue);
				i+=2;
			}	
			break;
		default:
			sprintf(AscStr,"%s%c",AscStr,UrlStr[i]);
			break;
		}
	}
	return strlen(AscStr);
}

//���ݸ����ֵ�ı䴰�ڴ�С
bool SetWindowSize( HWND hwnd,long xPos_Change,long yPos_Change)
{
	RECT rect;
	//struct rect ˵����Ϣ
	//rect.left  ���Ͻ�x
	//rect.top   ���Ͻ�y
	//rect.right ���½�x
	//rect.bottom���½�y

	memset(&rect,0,sizeof(rect));
	if ( GetWindowRect(hwnd,&rect) )
	{
		wchar_t buff[255];
		memset(buff,0,sizeof(buff)/sizeof(wchar_t));
		long x = rect.right-rect.left;
		long y = rect.bottom-rect.top;
		SetWindowPos(hwnd,HWND_TOP,rect.left,rect.top,x + xPos_Change,y + yPos_Change,SWP_NOMOVE);
		//wsprintf(buff,L"%d %d",rect.right-rect.left,rect.bottom-rect.top);
		//MessageBox(hwnd,buff,TEXT(""),MB_OK);
	}
	else
	{
		MessageBox(hwnd,TEXT("�Բ����ȡ��Ϣʧ��!"),TEXT("����"),MB_OK|MB_ICONERROR);
		return false;
	}
	return true;
}
