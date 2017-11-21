#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <glog/logging.h>
//#include "write_log.h"
#include "parse_json.h"
//#include "handle_config.h"

using namespace std;

parse_json_data  json_data_array[1024] = {{0}};

char* ReadFile(char *filename)  
{  
  char *buffer = NULL;  
  int string_size,read_size;  
  FILE *handler = fopen(filename,"r");  

  if(handler)  
  {  
     fseek(handler,0,SEEK_END);  
     string_size = ftell(handler);  
     rewind(handler);  
     buffer = (char*) malloc (sizeof(char) * (string_size + 1));  
     read_size = fread(buffer,sizeof(char),string_size,handler);  
     buffer[string_size+1] = '\0';  

     if (string_size != read_size) 
     {  
         free(buffer);  
         buffer = NULL;  
     }  
   }  
   return buffer;  
} 

/*
int func(int argc, char* argv[])
{
	//int log_init(const char* program, const char *log_path, bool log_bool, uint32_t log_level)
  int ret = log_init(argv[0], "../log/", true, 0);
  if(ret == -1)
  {
  	cout << "log_init error" << endl;	
  }
	LOG(INFO) << "I am guosj, I am from China!!!";	
	return 0;
}
*/


int main()
{
	char *json_str = ReadFile("./testdata.json");
	if(json_str == NULL)
	{
		cout << "read file fail" << endl;
		return -1;
	}
	
	int ret = parse_json(json_str, json_data_array);
	if(ret == PARSEJSONERR)
	{
		cout << "parse json error" <<endl;	
		return -1;
	}
	
	for(int i = 0; i < 3; ++i)
	{
		cout << json_data_array[i].coor_x << endl;
		cout << json_data_array[i].coor_y << endl;
		cout << json_data_array[i].coor_z << endl;
		cout << json_data_array[i].speed_a << endl;
		cout << json_data_array[i].mr_id   << endl;
		cout << json_data_array[i].uwb_id << endl;
	}
	return 0;
}

