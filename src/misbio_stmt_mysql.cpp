/*=============================================================
 *文件名称: MISBIOStmtMysql.c
 *创建时间: 2017.6.22
 *作    者: 郭世江
 *修改记录:
 *2017.6.22 创建文件
 *功能描述：mysql预处理封装 
 * ===========================================================*/
#include <stdlib.h>
#include <unistd.h>
#include "misbio_mysql.h"
#include "misbio_stmt_mysql.h"

//数据类型定义
typedef   MYSQL_STMT*     MISBIOSTMT;
typedef   my_bool         MISBIOBOOL;
typedef   MYSQL_BIND      *MISBIOBIND;

//返回最近调用的语句API函数的错误代码
MISBIOUNSIGNINT MISBIO_StmtErrno(MISBIOSTMT iStmt)
{
	return mysql_stmt_errno(iStmt); 
}

//描述了错误的字符串。如果未出现错误，返回空字符串
MISBIOSTR MISBIO_StmtError(MISBIOSTMT iStmt)
{
	return mysql_stmt_error(iStmt); 
}

//创建MYSQL_STMT句柄
MISBIOSTMT MISBIO_StmtInit(MISBIOSQL uSql)
{
	MISBIOSTMT  iStmtRet;
	if(uSql == NULL)
	{
		printf("uSql is NULL%s\n");
		exit(0);	
	}
	iStmtRet = mysql_stmt_init(uSql);
	if(uSql == NULL)
	{
		printf("CR_OUT_OF_MEMORY\n");	
		exit(0);	
	}
	return iStmtRet;
}

//向预处理环境句柄添加带占位符的sql语句
MISBIOINT MISBIO_StmtPrepare(MISBIOSTMT iStmtRet, MISBIOSTR uSql, MISBIOLONG uSqLen)
{
	MISBIOINT    iRet;
	if(iStmtRet == NULL || uSql == NULL || uSqLen == 0)
	{
		printf("iStmtRet, uSql and uSqLen is NULL");
		return MISBIO_StmtErrno(iStmtRet);
	}
	iRet = mysql_stmt_prepare(iStmtRet, uSql, uSqLen);
	if(iRet != 0)
  {
	  printf("stmt prepare error\n");
	  return MISBIO_StmtErrno(iStmtRet);
	}
	return iRet;
}

//返回预处理语句中参数标记符的数目
MISBIOLONG MISBIO_StmtParamCount(MISBIOSTMT iStmtRet)
{
	MISBIOLONG       iParamCount;
	if(iStmtRet == NULL)
	{
		printf("iStmtRet is NULL\n");	
		exit(0);
	}
	iParamCount = mysql_stmt_param_count(iStmtRet);
	if(iParamCount == 0)
	{
		printf("iParamCount == 0");	
		exit(0);
	}
	return iParamCount;
}

//把赋值以后的数组添加到预处理环境句柄
MISBIOBOOL MISBIO_StmtBindParam(MISBIOSTMT iStmtRet, MISBIOBIND IBind)
{
	MISBIOBOOL  iBool;
	if(iStmtRet == NULL || IBind == NULL)
	{
		printf("iStmtRet and IBind is NULL\n");	
		return _FALSE_;
	}
	iBool = mysql_stmt_bind_param(iStmtRet, IBind);
	if(iBool)
	{
		printf( "bind param failed\n");
	  printf("%s\n", MISBIO_StmtErrno(iStmtRet));
		return _FALSE_;
	}
	return iBool;
}

//将结果集中的列与数据缓冲和长度缓冲关联绑定起来
MISBIOBOOL MISBIO_StmtBindResult(MISBIOSTMT iStmtRet, MISBIOBIND IBind)
{
	MISBIOBOOL  iBool;
	if(iStmtRet == NULL || IBind == NULL)
	{
		printf("iStmtRet and IBind is NULL\n");	
		return _FALSE_;
	}
	iBool = mysql_stmt_bind_result(iStmtRet, IBind);
	if(iBool)
	{
		printf( "bind result failed\n");
		return _FALSE_;
	}
	return _TRUE_;
}

//执行已经完全填好值的SQL语句mysql_stmt_store_result
MISBIOINT MISBIO_StmtExecute(MISBIOSTMT iStmtRet)
{
	MISBIOINT  iRet;
	if(iStmtRet == NULL)
	{
		printf("iStmtRet is NULL\n");	
		return -1;
	}
	iRet = mysql_stmt_execute(iStmtRet);
	if(iRet != 0)
	{
		printf("mysql_stmt_execute err\n");	
		return -1;
	}
	return iRet;
}

//生成结果集
MISBIOINT MISBIO_StmtStoreResult(MISBIOSTMT iStmtRet)
{
	MISBIOINT  iRet;
	if(iStmtRet == NULL)
	{
		printf("iStmtRet is NULL\n");
		return 0;	
	}
	iRet = mysql_stmt_store_result(iStmtRet);
	if(iRet != 0)
	{
		printf("mysql_stmt_store_result error\n");
		return 0;	
	}
	return -1;
}

MISBIOINT MISBIO_StmtFetch(MISBIOSTMT iStmtRet)
{
	if(iStmtRet == NULL)
	{
		printf("iStmtRet is NULL\n");	
		return -1;
	}
  return mysql_stmt_fetch(iStmtRet);
}

MISBIOBOOL MISBIO_StmtClose(MISBIOSTMT iStmtRet)
{
	return mysql_stmt_close(iStmtRet);
}
