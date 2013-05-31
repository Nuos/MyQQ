/***************************/
/*FileName: MySql.h        */
/*Author:   zechunzhou     */
/*Date:     2012/1/3       */
/*LastModified: 2012/1/3   */
/*Note:这个文件为类mysql类 */
/* 的头文件,mysql是写来用于 */
/* 完成连接数据库和完成一些 */
/* 基本的操作的类           */
/***************************/



#include <windows.h>
#include <windowsx.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <stdlib.h>
#include <stdio.h>


#define LOGIN_TIMEOUT 30
#define MAXBUFLEN 255

//odbc句柄及结果结构体
struct CODBC
{
	SQLHENV henv ;    //环境句柄
	SQLHDBC hdbc;    //代表一个数据库连接句柄
	SQLHSTMT hstmt;  //代表一个sql语句
	SQLRETURN result;//数据库执行结果
};
//数据库信息结构体
struct Database
{
	char Drive[255];
	char Server[255];
	char UID[255];
	char Passwd[255];
	char dbname[255];
};

/*
classname:mysql
author:zechunzhou
note:这个类用了完成连接数据库和进行一些基本的操作的类
intfacefunction:
1.构造函数:
	MySql(char * uid,char * passwd,char * dbase,char * drive="SQL Server",char * server=".");
	参数:uid,passwd,dbase,drive,server分别是登录数据库用户名,密码,数据库名,驱动,服务器地址
	     其中drive默认是ms sql的驱动,server的默认值是本机
	返回值:无

2.设置sql语句:
	void setSqlStr(char * sqlstr);
	参数:sqlstr是要执行的sql语句
	     当对数据库执行之前或者对数据库操作之后想更改sql语句可以调用该函数
	返回值:无

3.插入语句:
	int Insert(char * sqlstr);
	参数:sqlstr为插入操作要执行的语句
	     当sql语句已经设置可以调用无参
	返回值:执行结果影响行数
	int Insert();	

4.删除语句:
	int Delete(char * sqlstr);
	参数:sqlstr为删除操作要执行的语句
	     当sql语句已经设置可以调用无参
	返回值:执行结果影响行数
	int Delete();

5.更新语句:
	int Update(char * sqlstr);
	参数:sqlstr为更新操作要执行的语句
	     当sql语句已经设置可以调用无参
	返回值:执行结果影响行数
	int Update();

6.查询语句:
	char*** Select(char * sqlstr,int & rows,int & cols);
	参数:sqlstr为更新操作要执行的语句
	     rows(output)为结果集的行数
		 cols(output)为结果集的列数
	返回值:查询结果集,执行失败返回NULL
	char*** Select(int & rows,int & cols);
	参数:rows(output)为结果集的行数
		 cols(output)为结果集的列数
	返回值:查询结果集,执行失败返回NULL
*/

class MySql
{
private:

	CODBC  codbc;     //句柄及执行结果
	Database db;      //要连接的数据库信息
	char  sqlStr[1024];    //执行语句
	int    resultRows;//结果集行数
	int    resultCols;//结果集列数
	int    resultCount;//影响行数
	char*** results;   //结果集
	char  Error[255];//错误信息

protected:
	bool Result();//有返回结果执行结果
	bool NoResult();//无返回结果执行
	bool setHandle();//设置句柄
	void setAll();//初始化数据
	bool setConnect();//建立连接
	void ShowDBError(SQLSMALLINT type,SQLHANDLE sqlHandle);//数据库错误信息
	
	
public:
	
	MySql(char * uid,char * passwd,char * dbase,char * drive="SQL Server",char * server=".");
	//MySql();
	~MySql();
	
	void setSqlStr(char * sqlstr);//设置sql语句
	int Insert(char * sqlstr);
	int Insert();//增
	int Delete(char * sqlstr);//删
	int Delete();//删
	int Update(char * sqlstr);//改
	int Update();//改
	char*** Select(char * sqlstr,int & rows,int & cols);//查
	char*** Select(int & rows,int & cols);//查
	
};