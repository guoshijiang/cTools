/*=============================================================
 *文件名称: misbio_stmt_mysql.h
 *创建时间: 2017.9.13
 *作    者: 郭世江
 *修改记录:
 *2017.9.13 创建文件
 *功能描述：mysql预处理封装 
 * ===========================================================*/
#ifndef __MISBIOSTMTMYSQL_H
#define  __MISBIOSTMTMYSQL_H
#include <stdio.h>
#include <mysql.h>
#include "misbio_mysql.h"
#include "misbio_stmt_mysql.h"

#define _TRUE_    0
#define _FALSE_   -1

typedef   MYSQL_STMT*     MISBIOSTMT;
typedef   my_bool         MISBIOBOOL;
typedef   MYSQL_BIND*     MISBIOBIND;
typedef   MYSQL_BIND      MISBIOBINDSTR;

MISBIOUNSIGNINT MISBIO_StmtErrno(MISBIOSTMT iStmt);

MISBIOSTR MISBIO_StmtError(MISBIOSTMT iStmt);

MISBIOSTMT MISBIO_StmtInit(MISBIOSQL uSql); 

MISBIOINT MISBIO_StmtPrepare(MISBIOSTMT iStmtRet, MISBIOSTR uSql, MISBIOLONG uSqLen);

MISBIOLONG MISBIO_StmtParamCount(MISBIOSTMT iStmtRet);

MISBIOBOOL MISBIO_StmtBindParam(MISBIOSTMT iStmtRet, MISBIOBIND IBind); 

MISBIOBOOL MISBIO_StmtBindResult(MISBIOSTMT iStmtRet, MISBIOBIND IBind);

MISBIOINT MISBIO_StmtExecute(MISBIOSTMT iStmtRet);

MISBIOINT MISBIO_StmtStoreResult(MISBIOSTMT iStmtRet);

MISBIOINT MISBIO_StmtFetch(MISBIOSTMT iStmtRet);

MISBIOBOOL MISBIO_StmtClose(MISBIOSTMT iStmtRet);

#endif