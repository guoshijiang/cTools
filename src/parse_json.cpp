/****************************************************************************
 *文件名称: 			parse_json.cpp    
 *创建日期：			2017-9-14      
 *作    者:       郭世江          
 *修改记录：			2017-9-14  首次创建
 *功能描述：      使用rapidjson库解析从数据库中查出来的json数组
 *****************************************************************************/
#include "parse_json.h" 

//解析JSON数组
int parse_json(char *json_str, parse_json_data json_data_array[])
{
	if(json_str == NULL)
	{
		cout << "json array is null" << endl;	
		return PARSEJSONERR;
	}
	rapidjson::Document doc;
	int json_size = 0;
	doc.Parse<rapidjson::kParseDefaultFlags>(json_str);
	if (doc.HasParseError() || !doc.IsArray())
	{
		cout << "parson json error or json string is not array" << endl;
		return PARSEJSONERR;
	}
	json_size = doc.Size();
	for(unsigned int i = 0; i < doc.Size(); ++i)
	{
		rapidjson::Value &v = doc[i];
		if (v.HasMember("x") && v.HasMember("y") && v.HasMember("z") && v.HasMember("a") && v.HasMember("mrId") && v.HasMember("uwbId"))
		{  
			json_data_array[i].coor_x  = v["x"].GetDouble();
			json_data_array[i].coor_y  = v["y"].GetDouble();
			json_data_array[i].coor_z  = v["z"].GetDouble();
			memcpy(json_data_array[i].speed_a, v["a"].GetString(), strlen(v["a"].GetString()));
			json_data_array[i].mr_id   = v["mrId"].GetInt();
		  memcpy(json_data_array[i].uwb_id, v["uwbId"].GetString(), strlen(v["uwbId"].GetString()));
	  }
	}
	return json_size;	
}
