#include "stdafx.h"
#include "MySql.h"

void MySql::setAll()
{
	//Ϊ������Ϣ����ռ�
//	Error=new char[255];
	ZeroMemory(Error,255);

	//���û������
	setHandle();
	//���ݿ���Ϣ�ռ�����
	//db.Drive=new char[30];
	//db.Server=new char[30];
//	db.UID=new char[30];
	//db.Passwd=new char[30];
	//db.dbname=new char[255];
	//sql���ռ�����
	//sqlStr=new char[255];
	ZeroMemory(sqlStr,sizeof(sqlStr));
	//�������ʼ��
	results=NULL;
	//��ʼ���������С��Ӱ������
	resultRows=resultCols=resultCount=0;
	
}

bool MySql::setHandle()
{
	//���价�����
	codbc.result = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE,
		&codbc.henv);
	//���ù���������
	codbc.result = SQLSetEnvAttr(codbc.henv, SQL_ATTR_ODBC_VERSION,
		(void*)SQL_OV_ODBC3, 0);
	//�������Ӿ��
	codbc.result = SQLAllocHandle(SQL_HANDLE_DBC, codbc.henv, &codbc.hdbc);
	
	//������������
	codbc.result = SQLSetConnectAttr(codbc.hdbc, SQL_LOGIN_TIMEOUT,
		(void*)LOGIN_TIMEOUT, 0);
	
	if (codbc.hdbc==NULL||codbc.henv==NULL||codbc.hstmt==NULL)
	{
		sprintf(Error,"setHandle error");
		return false;
	}
	return true;
}

MySql::MySql(char * uid,char * passwd,char * dbase,char * drive,char * server)
{
	//��ʼ�����,����ռ�
	setAll();
	strcpy(db.Drive,drive);
	strcpy(db.Server,server);
	strcpy(db.UID,uid);
	strcpy(db.Passwd,passwd);
	strcpy(db.dbname,dbase);
	setConnect();
}

bool MySql::setConnect()
{	
	//����mssql	
	CHAR * ConnStrIn=new CHAR[255];
	sprintf(ConnStrIn,"DRIVER={%s};SERVER=%s;UID=%s;PWD=%s;DATABASE=%s;CharSet=gbk;",
		db.Drive,db.Server,db.UID,db.Passwd,db.dbname);
	SQLCHAR  * ConnStrOut = new SQLCHAR [255];
	//�������ݿ�--��������--result�������ӽ��
	codbc.result = SQLDriverConnect(codbc.hdbc,NULL,
		(SQLCHAR*)ConnStrIn,SQL_NTS,
		ConnStrOut,MAXBUFLEN,
		(SQLSMALLINT*)0,SQL_DRIVER_NOPROMPT);
	//����Ƿ�ɹ�
	if(SQL_ERROR==codbc.result)
	{
		//��ʾ���Ӵ���
		ShowDBError(SQL_HANDLE_DBC,codbc.hdbc);
		return false;
	}
	//���������
	codbc.result = SQLAllocHandle(SQL_HANDLE_STMT, codbc.hdbc, &codbc.hstmt);
	delete [] ConnStrIn;
	delete [] ConnStrOut;
	return true;
}

void MySql::ShowDBError(SQLSMALLINT type,SQLHANDLE sqlHandle)
{
	char pStatus[10], pMsg[101];
	SQLSMALLINT SQLmsglen;
	SQLINTEGER SQLerr;
	long erg2 = SQLGetDiagRec(type, sqlHandle,1,
		(SQLCHAR *)pStatus,&SQLerr,(SQLCHAR*)pMsg,100,&SQLmsglen);
	wsprintf(Error,"%s (%d)\n",pMsg,(int)SQLerr);
	MessageBox(NULL,Error,TEXT("���ݿ�ִ�д���"),MB_ICONERROR|MB_OK);
}

void MySql::setSqlStr(char * sqlstr)
{
	if(sqlstr==NULL||sqlStr==NULL)
		return;
	ZeroMemory(sqlStr,255);
	strcpy(sqlStr,sqlstr);
}

bool MySql::NoResult()
{
	if(!strcmp(sqlStr,""))
	{
		sprintf(Error,"no sqlStr");
		MessageBox(NULL,Error,TEXT("���ݿ�ִ�д���"),MB_ICONERROR|MB_OK);
		return false;
	}
	codbc.result=SQLExecDirect(codbc.hstmt,(SQLCHAR*)sqlStr,255);
	if(codbc.result==SQL_ERROR)
	{
		ShowDBError(SQL_HANDLE_STMT,codbc.hstmt);
		return false;
	}

	SQLRowCount(codbc.hstmt,(SQLINTEGER*)&resultCount);
	return true;
}

bool MySql::Result()
{
	//ִ�����
	if(!NoResult())
	{
		return false;
	}
	//�õ�����
	SQLNumResultCols(codbc.hstmt,(SQLSMALLINT*)&resultCols);
	if(resultCols==0)
	{
		return false;
	}
	//���������
	if ( results != NULL)
	{
		delete [] results;
		results = NULL;
	}
	results=new char**[255];
	SQLINTEGER cbsatid=SQL_NTS;
	//TCHAR name[20];
	//printf("\n");
	for (int i=0;SQLFetch(codbc.hstmt)!=SQL_NO_DATA_FOUND;i++)
	{
		results[i]=new char*[resultCols+1];
		for (int j=0;j<resultCols;j++)
		{
			results[i][j]=new char[255];
			SQLGetData(codbc.hstmt,j+1,SQL_C_CHAR,results[i][j],255,&cbsatid);
			//printf("%s ",name);
		}	
	}
	resultRows=i;
	return true;
}

int MySql::Insert()
{
	if(!NoResult())
	{
		return 0;
	}	
	return resultCount;
}

int MySql::Insert(char * sqlstr)
{
	setSqlStr(sqlstr);

	if(!NoResult())
	{
		return 0;
	}	
	return resultCount;
}


int MySql::Delete()
{
	return Insert();
}


int MySql::Delete(char * sqlstr)
{
	return Insert(sqlstr);
}

int MySql::Update()
{
	return Insert();
}

int MySql::Update(char * sqlstr)
{
	return Insert(sqlstr);
}

char*** MySql::Select(int & rows,int & cols)
{
	if(!Result())
	{
		rows=0;
		cols=0;
		return NULL;
	}	
	rows=resultRows;
	cols=resultCols;
	return results;
}

char*** MySql::Select(char * sqlstr,int & rows,int & cols)
{
	setSqlStr(sqlstr);
	if(!Result())
	{
		rows=0;
		cols=0;
		return NULL;
	}	
	rows=resultRows;
	cols=resultCols;
	return results;
}

MySql::~MySql()
{
	//�ͷ�sql���
	SQLFreeStmt(codbc.hstmt,SQL_CLOSE);
	//�Ͽ�����
	SQLDisconnect(codbc.hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC,codbc.hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV,codbc.henv);
	//delete[]  sqlStr;    //ִ�����
	if ( results != NULL)
	{
		delete[] results;   //�����
		results = NULL;
	}
//	delete[] Error;     //������Ϣ
	//delete[] db.Drive;
	//delete[] db.Server;
	//delete[] db.UID;
	//delete[] db.Passwd;
	//delete[] db.dbname;

}





