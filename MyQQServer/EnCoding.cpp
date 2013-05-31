/////////////////////////////////////////////
//         ±àÂë½âÂëÎÄ¼þ                    //
/////////////////////////////////////////////


#include "stdafx.h"
#include "EnCoding.h"
using namespace std;


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