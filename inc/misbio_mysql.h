/*=============================================================
 *文件名称: misbio_mysql.h
 *创建时间: 2017.9.13
 *作    者: 郭世江
 *修改记录:
 *2017.9.13 创建文件
 *功能描述: mysql基本操作
 * ===========================================================*/
#ifndef __MISBIOMYSQL_H
#define __MISBIOMYSQL_H
#include <stdio.h>
#include <mysql.h>

#define SQLINITERR        0x00                    //初始化数据库失败
#define SQLCONNECTERR     0x00                    //连接数据库失败
#define NOSELECTSQLERR    2000                    //执行非查询SQL语句失败
#define SET_TRAN	        "SET AUTOCOMMIT=0"  		//手动提交事务
#define UNSET_TRAN	      "SET AUTOCOMMIT=1"      //自动提交事务

typedef MYSQL*             MISBIOSQL;
typedef const char*        MISBIOSTR;
typedef int                MISBIOINT;
typedef unsigned int       MISBIOUNSIGNINT;
typedef unsigned long      MISBIOLONG;
typedef MYSQL_RES*         MISBIOSQLRES;
typedef MYSQL_ROW          MISBIOSQLROW;
typedef MYSQL_FIELD*       MISBIOFIELD;
typedef short              MISBIOSHORT;
typedef char               MISBIOCHA;

MISBIOUNSIGNINT MISBIODB_Erron(MISBIOSQL uSql);

MISBIOSQL MISBIODB_init(MISBIOSQL uSql);

MISBIOSQL MISBIODB_Connect(MISBIOSQL uSql, MISBIOSTR iHost, MISBIOSTR iUser, MISBIOSTR iPasswd, MISBIOSTR iDbName, MISBIOUNSIGNINT iPort, MISBIOSTR iUnixSock, MISBIOLONG iClientFlag);

int MISBIODB_ExecSql(MISBIOSQL uSql, MISBIOSTR iSql);

MISBIOSQLRES MISBIODB_ExecQuerySqlRes(MISBIOSQL uSql, MISBIOSTR iSql);

MISBIOUNSIGNINT MISBIODB_GetSqlCount(MISBIOSQL uSql);

MISBIOFIELD MISBIODB_iFields(MISBIOSQLRES iResult);

MISBIOFIELD MISBIODB_iField(MISBIOSQLRES iResult);

MISBIOSQLROW MISBIODB_FetchRow(MISBIOSQLRES iResult);

void MISBIOFree_DBRes(MISBIOSQLRES iResult);
 
void MISBIODB_Close(MISBIOSQL uSql);

MISBIOINT MISBIODB_StartTrans(MISBIOSQL uSql);  			

MISBIOINT MISBIODB_OperationTran(MISBIOSQL uSql);

MISBIOINT MISBIODB_AutoTran(MISBIOSQL uSql);	
	
MISBIOINT MISBIODB_Commit(MISBIOSQL uSql);

MISBIOINT MISBIODB_Rollback(MISBIOSQL uSql);	
	
	
#endif