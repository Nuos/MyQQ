#include "stdafx.h"
#include "MySql.h"

void MySql::setAll()
{
	//为错误信息申请空间
//	Error=new char[255];
	ZeroMemory(Error,255);

	//设置环境句柄
	setHandle();
	//数据库信息空间申请
	//db.Drive=new char[30];
	//db.Server=new char[30];
//	db.UID=new char[30];
	//db.Passwd=new char[30];
	//db.dbname=new char[255];
	//sql语句空间申请
	//sqlStr=new char[255];
	ZeroMemory(sqlStr,sizeof(sqlStr));
	//结果集初始化
	results=NULL;
	//初始化结果集大小及影响行数
	resultRows=resultCols=resultCount=0;
	
}

bool MySql::setHandle()
{
	//分配环境句柄
	codbc.result = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE,
		&codbc.henv);
	//设置管理环境属性
	codbc.result = SQLSetEnvAttr(codbc.henv, SQL_ATTR_ODBC_VERSION,
		(void*)SQL_OV_ODBC3, 0);
	//分配连接句柄
	codbc.result = SQLAllocHandle(SQL_HANDLE_DBC, codbc.henv, &codbc.hdbc);
	
	//设置连接属性
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
	//初始化句柄,申请空间
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
	//连接mssql	
	CHAR * ConnStrIn=new CHAR[255];
	sprintf(ConnStrIn,"DRIVER={%s};SERVER=%s;UID=%s;PWD=%s;DATABASE=%s;CharSet=gbk;",
		db.Drive,db.Server,db.UID,db.Passwd,db.dbname);
	SQLCHAR  * ConnStrOut = new SQLCHAR [255];
	//连接数据库--创建连接--result返回连接结果
	codbc.result = SQLDriverConnect(codbc.hdbc,NULL,
		(SQLCHAR*)ConnStrIn,SQL_NTS,
		ConnStrOut,MAXBUFLEN,
		(SQLSMALLINT*)0,SQL_DRIVER_NOPROMPT);
	//检查是否成功
	if(SQL_ERROR==codbc.result)
	{
		//显示连接错误
		ShowDBError(SQL_HANDLE_DBC,codbc.hdbc);
		return false;
	}
	//设置语句句柄
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
	MessageBox(NULL,Error,TEXT("数据库执行错误"),MB_ICONERROR|MB_OK);
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
		MessageBox(NULL,Error,TEXT("数据库执行错误"),MB_ICONERROR|MB_OK);
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
	//执行语句
	if(!NoResult())
	{
		return false;
	}
	//得到列数
	SQLNumResultCols(codbc.hstmt,(SQLSMALLINT*)&resultCols);
	if(resultCols==0)
	{
		return false;
	}
	//分析结果集
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
	//释放sql语句
	SQLFreeStmt(codbc.hstmt,SQL_CLOSE);
	//断开连接
	SQLDisconnect(codbc.hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC,codbc.hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV,codbc.henv);
	//delete[]  sqlStr;    //执行语句
	if ( results != NULL)
	{
		delete[] results;   //结果集
		results = NULL;
	}
//	delete[] Error;     //错误信息
	//delete[] db.Drive;
	//delete[] db.Server;
	//delete[] db.UID;
	//delete[] db.Passwd;
	//delete[] db.dbname;

}





