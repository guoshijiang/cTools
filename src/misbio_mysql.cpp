/*=============================================================
 *文件名称: MISBIOMysql.c
 *创建时间: 2017.9.13
 *作    者: 郭世江
 *修改记录:
 *2017.9.13 创建文件
 *功能描述：
 *  mysql
 * ===========================================================*/
#include <assert.h>
#include <mysql.h>
#include "misbio_mysql.h"

//获取数据库的错误号
MISBIOUNSIGNINT MISBIODB_Erron(MISBIOSQL uSql)
{
	if(uSql != NULL)
	{
		return mysql_errno(uSql);
	}
	return -1;
}

//初始化数据库
MISBIOSQL MISBIODB_init(MISBIOSQL uSql)
{
	MISBIOSQL iSqlHandle = mysql_init(uSql);
	if(iSqlHandle == NULL)
	{
		printf("database init error\n");
		return SQLINITERR;
	}
	return iSqlHandle;
}

//连接数据库
MISBIOSQL MISBIODB_Connect(MISBIOSQL uSql, MISBIOSTR iHost, MISBIOSTR iUser, MISBIOSTR iPasswd, MISBIOSTR iDbName, MISBIOUNSIGNINT iPort,MISBIOSTR iUnixSock, MISBIOLONG iClientFlag)
{
	if(uSql == NULL || iHost == NULL || iUser == NULL || iPasswd == NULL) 
	{
		printf("uSql, iHost, iUser and iPasswd is NULL\n");	
		return SQLCONNECTERR;
	} 
  MISBIOSQL iSqlHandle = mysql_real_connect(uSql, iHost, iUser, iPasswd, iDbName, iPort, iUnixSock, iClientFlag);
  if(iSqlHandle == NULL)
  {
 		printf("database connect err%d\n", SQLCONNECTERR);
		return SQLCONNECTERR;
  }
  return iSqlHandle;
}

//执行非查询的SQL语句
int MISBIODB_ExecSql(MISBIOSQL uSql, MISBIOSTR iSql)
{
	MISBIOINT iExecNum;
	MISBIOINT iErrNum;
	if(uSql == NULL || iSql == NULL)
	{
		printf("uSql and iSql is NULL");
		return NOSELECTSQLERR;	
	}
	iExecNum = mysql_query(uSql, iSql);
	if(iExecNum != 0)
	{
		
		iErrNum = MISBIODB_Erron(uSql);
		/*
		if(iErrNum == CR_COMMANDS_OUT_OF_SYNC)
		{
			printf("Exec cmd not by order:%d\n", iErrNum);	
			return NOSELECTSQLERR;
		}
		if(iErrNum == CR_SERVER_GONE_ERROR)
		{
			printf("MySQL Server disable:%d\n", iErrNum);	
			return NOSELECTSQLERR;
		}
		if(iErrNum == CR_SERVER_LOST)
		{
			printf("connecting with server err:%d\n", iErrNum);	
			return NOSELECTSQLERR;
		}
		if(iErrNum == CR_UNKNOWN_ERROR)
		{
			printf("unknown err:%d\n", iErrNum);	
			return NOSELECTSQLERR;
		}
		*/
		printf("mysql_query query err:%d\n", iErrNum);
		return NOSELECTSQLERR;
	}
	return 0;
}

//执行查询的SQL语句
MISBIOSQLRES MISBIODB_ExecQuerySqlRes(MISBIOSQL uSql, MISBIOSTR iSql)
{
	MISBIOINT           i, ret;
	MISBIOSQLRES        iResult;

	ret = MISBIODB_ExecSql(uSql, iSql);
	if(ret == NOSELECTSQLERR)
	{
		printf("exec select no table head result sql sentence\n");
		return NULL;	
	}
	
	iResult = mysql_store_result(uSql);
	if(iResult == NULL)
	{
		printf("store result err:%d\n", MISBIODB_Erron(uSql));
		return NULL;
	}
	return iResult;
}

//获取列数
MISBIOUNSIGNINT MISBIODB_GetSqlCount(MISBIOSQL uSql)
{
	MISBIOINT iCount;
	if(uSql == NULL)
	{
		printf("uSql is NULL\n");
		return -1;
	}	
	iCount= mysql_field_count(uSql);
	if(iCount != 0)
	{
		return iCount;
	}
	else
	{
		printf("No Count\n");	
	}
	return 0;
}

//打印表头
MISBIOFIELD MISBIODB_iFields(MISBIOSQLRES iResult)
{
	MISBIOFIELD iFields;
	if(iResult == NULL)
	{
		printf("iResult is NULL\n");
		return NULL;	
	}
	iFields = mysql_fetch_fields(iResult);
	if(iFields != NULL)
	{
		return iFields;
	}
	return NULL;
}

MISBIOFIELD MISBIODB_iField(MISBIOSQLRES iResult)
{
	MISBIOFIELD iField;
	if(iResult == NULL)
	{
		printf("iResult is NULL\n");
		return NULL;	
	}
	iField = mysql_fetch_field(iResult);
	if(iField != NULL)
	{
		return iField;
	}
	return NULL;
}

//解析结果集
MISBIOSQLROW MISBIODB_FetchRow(MISBIOSQLRES iResult)
{
	MISBIOSQLROW iRow;
	if(iResult == NULL)
	{
		printf("iResult is NULL\n");
		return NULL;	
	}
	iRow = mysql_fetch_row(iResult);
	if( iRow != NULL) 
	{
		return iRow;	
	}
	return NULL;
}
	
//释放结果集
void MISBIOFree_DBRes(MISBIOSQLRES iResult)
{
	mysql_free_result(iResult);
} 

//关闭数据库
void MISBIODB_Close(MISBIOSQL uSql)
{
	if(uSql == NULL)
	{
		mysql_close(uSql);
	}
}

//开启事务
MISBIOINT MISBIODB_StartTrans(MISBIOSQL uSql)  			
{
	MISBIOINT        iRet;
	iRet = mysql_query(uSql, "start transaction");  
	if (iRet != 0) 
	{
		printf("mysql_OperationTran query start err");
		return iRet;
	}
	return iRet;
}

//设置事务为手动提交
MISBIOINT MISBIODB_OperationTran(MISBIOSQL uSql) 
{
	MISBIOINT        iRet;
	iRet = mysql_query(uSql, SET_TRAN);
	if (iRet != 0)
	{
		printf("mysql_OperationTran query set err");
		return iRet;
	}
	return iRet;
}

//设置事务为自动提交
MISBIOINT MISBIODB_AutoTran(MISBIOSQL uSql)
{
	MISBIOINT        iRet;
	iRet = mysql_query(uSql, UNSET_TRAN);
	if (iRet != 0)
  {
		printf("mysql_OperationTran query set err");
		return iRet;
	}
	return iRet;
}

//手动提交事务
MISBIOINT MISBIODB_Commit(MISBIOSQL uSql)
{
	MISBIOINT        iRet;
	iRet = mysql_query(uSql, "COMMIT"); 
	if(iRet != 0) 
	{
		printf("commit err\n");
		return iRet;
	}
	return iRet;
}

//回滚事务		
MISBIOINT MISBIODB_Rollback(MISBIOSQL uSql)
{
	MISBIOINT        iRet;
	iRet = mysql_query(uSql, "ROLLBACK");
	if (iRet != 0) 
	{
		printf("rollback err");
		return iRet;
	}
	return iRet;
}