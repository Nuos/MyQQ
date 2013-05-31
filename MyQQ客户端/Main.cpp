// MMÁÄÌì¿Í»§¶Ë.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include "LoginDlg.h"
#include <COMMCTRL.H>
#include "AllstructinThis.h"


UserInfor userInfor;


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	//Enable IPAddress¡¢Calendar.etc
	InitCommonControls();
	if(DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_LOGIINDIALOG), NULL, Login_Proc,(LPARAM)&userInfor)==1)
	{
		//MessageBox(NULL,userInfor.uName,"",0);
		DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, Main_Proc,(LPARAM)&userInfor);
	}
	return 0;
}





