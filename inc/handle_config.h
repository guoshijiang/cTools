/****************************************************************************
 *文件名称: 			handle_config.h     
 *创建日期：			2017-9-13      
 *作    者:       郭世江          
 *修改记录：			2017-6-23  首次创建
 *功能描述：      读取配置文件的信息，以map的形式存入;
 *                配置文件的格式，以#作为行注释，配置的形式是key = value；
 *                中间可有空格，也可没有空格
 *****************************************************************************/
#ifndef _HANDLE_CONFIG_H_
#define _HANDLE_CONFIG_H_

#include <string>
#include <map>
using namespace std;

#define COMMENT_CHAR '#'

bool ReadConfig(const string & filename, map<string, string> & m);
void PrintConfig(const map<string, string> & m);

#endif
