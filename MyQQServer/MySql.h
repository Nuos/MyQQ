/***************************/
/*FileName: MySql.h        */
/*Author:   zechunzhou     */
/*Date:     2012/1/3       */
/*LastModified: 2012/1/3   */
/*Note:����ļ�Ϊ��mysql�� */
/* ��ͷ�ļ�,mysql��д������ */
/* ����������ݿ�����һЩ */
/* �����Ĳ�������           */
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

//odbc���������ṹ��
struct CODBC
{
	SQLHENV henv ;    //�������
	SQLHDBC hdbc;    //����һ�����ݿ����Ӿ��
	SQLHSTMT hstmt;  //����һ��sql���
	SQLRETURN result;//���ݿ�ִ�н��
};
//���ݿ���Ϣ�ṹ��
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
note:�������������������ݿ�ͽ���һЩ�����Ĳ�������
intfacefunction:
1.���캯��:
	MySql(char * uid,char * passwd,char * dbase,char * drive="SQL Server",char * server=".");
	����:uid,passwd,dbase,drive,server�ֱ��ǵ�¼���ݿ��û���,����,���ݿ���,����,��������ַ
	     ����driveĬ����ms sql������,server��Ĭ��ֵ�Ǳ���
	����ֵ:��

2.����sql���:
	void setSqlStr(char * sqlstr);
	����:sqlstr��Ҫִ�е�sql���
	     �������ݿ�ִ��֮ǰ���߶����ݿ����֮�������sql�����Ե��øú���
	����ֵ:��

3.�������:
	int Insert(char * sqlstr);
	����:sqlstrΪ�������Ҫִ�е����
	     ��sql����Ѿ����ÿ��Ե����޲�
	����ֵ:ִ�н��Ӱ������
	int Insert();	

4.ɾ�����:
	int Delete(char * sqlstr);
	����:sqlstrΪɾ������Ҫִ�е����
	     ��sql����Ѿ����ÿ��Ե����޲�
	����ֵ:ִ�н��Ӱ������
	int Delete();

5.�������:
	int Update(char * sqlstr);
	����:sqlstrΪ���²���Ҫִ�е����
	     ��sql����Ѿ����ÿ��Ե����޲�
	����ֵ:ִ�н��Ӱ������
	int Update();

6.��ѯ���:
	char*** Select(char * sqlstr,int & rows,int & cols);
	����:sqlstrΪ���²���Ҫִ�е����
	     rows(output)Ϊ�����������
		 cols(output)Ϊ�����������
	����ֵ:��ѯ�����,ִ��ʧ�ܷ���NULL
	char*** Select(int & rows,int & cols);
	����:rows(output)Ϊ�����������
		 cols(output)Ϊ�����������
	����ֵ:��ѯ�����,ִ��ʧ�ܷ���NULL
*/

class MySql
{
private:

	CODBC  codbc;     //�����ִ�н��
	Database db;      //Ҫ���ӵ����ݿ���Ϣ
	char  sqlStr[1024];    //ִ�����
	int    resultRows;//���������
	int    resultCols;//���������
	int    resultCount;//Ӱ������
	char*** results;   //�����
	char  Error[255];//������Ϣ

protected:
	bool Result();//�з��ؽ��ִ�н��
	bool NoResult();//�޷��ؽ��ִ��
	bool setHandle();//���þ��
	void setAll();//��ʼ������
	bool setConnect();//��������
	void ShowDBError(SQLSMALLINT type,SQLHANDLE sqlHandle);//���ݿ������Ϣ
	
	
public:
	
	MySql(char * uid,char * passwd,char * dbase,char * drive="SQL Server",char * server=".");
	//MySql();
	~MySql();
	
	void setSqlStr(char * sqlstr);//����sql���
	int Insert(char * sqlstr);
	int Insert();//��
	int Delete(char * sqlstr);//ɾ
	int Delete();//ɾ
	int Update(char * sqlstr);//��
	int Update();//��
	char*** Select(char * sqlstr,int & rows,int & cols);//��
	char*** Select(int & rows,int & cols);//��
	
};