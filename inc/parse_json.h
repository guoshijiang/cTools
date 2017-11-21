/****************************************************************************
 *文件名称: 			parse_json.h     
 *创建日期：			2017-9-14      
 *作    者:       郭世江          
 *修改记录：			2017-9-14  首次创建
 *功能描述：      使用rapidjson库解析从数据库中查出来的json数组
 *****************************************************************************/
#ifndef _PARSE_JSON_H_
#define _PARSE_JSON_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>  
#include <iostream>  
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h" 
using namespace rapidjson;
using namespace std;

#define JSONLEN 128

//错误码定义
#define PARSEJSONERR 1000

//结构体定义
typedef struct _parse_json_data
{
	double coor_x;
	double coor_y;
	double coor_z;
	char speed_a[64];
	int mr_id;
	char uwb_id[64];
}parse_json_data;

typedef std::vector<parse_json_data*> json_array_vector;

int parse_json(char *json_str, parse_json_data json_data_array[]);

#endif