/****************************************************************************
 *文件名称: 			write_log.h     
 *创建日期：			2017-9-13      
 *作    者:       郭世江          
 *修改记录：			2017-6-23  首次创建
 *功能描述：      调用glog库打日志的一些处理
 *****************************************************************************/
#include <iostream>
#include <string>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <glog/logging.h>
#include "write_log.h"
using namespace std;

/*
int log_init(const char* program, log_info l_info)
{
	if(program == NULL)
	{
		cout << "program is null" << endl;
		return -1;
	}
	
	char       logInfoPath[100]; 
	char       logWarnpath[100];
  char       logErrPath[100];  
  char       logFatalpath[100];
   
	//判断指定的log_path是否存在，如果不存在，创建相应的日志目录  
	if(access(log_path, F_OK))
  {  
      cerr << "log_path not wxist,we will create it!" << endl;  
      mkdir(log_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);  
      std::cout << "log_path created,filePath:" << log_path << ",mode:"  
              << (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) << std::endl;  
  }
	google::InitGoogleLogging(program);
  snprintf(logInfoPath,  sizeof(logInfoPath), "%s%s", l_info.log_path, "/INFO_");  	
	snprintf(logWarnpath,  sizeof(logWarnpath), "%s%s", l_info.log_path, "/WARNING_");  	
	snprintf(logErrPath,   sizeof(logErrPath),  "%s%s", l_info.log_path, "/ERROR_");  	
	snprintf(logFatalpath, sizeof(logFatalpath),"%s%s", l_info.log_path, "/FATAL_");
	  	
	//实时刷新日志到硬盘，默认是会缓冲的 ,等于0时表示立即输出 
	FLAGS_logbufsecs = l_info.login_disk_time;
	
	//日志大小大于30M时，创建新的日志 
  FLAGS_max_log_size = l_info.max_log_size; 
  
  //当磁盘满时，停止日志输出
  FLAGS_stop_logging_if_full_disk = l_info.log_is_full;  
  
  //不输出到文件,输出到stderr	
  FLAGS_alsologtostderr = l_info.logto_stderr;
  
  //设置日志扩展名
  google::SetLogFilenameExtension(l_info.log_extension_name);
		  
	if (log_level == google::GLOG_INFO)  
  {  
    google::SetLogDestination(google::INFO, logInfoPath);
  }  
  else if(log_level == google::GLOG_WARNING)
  {  
    google::SetLogDestination(google::WARNING, logWarnpath);
  }
  else if (log_level == google::GLOG_ERROR)  
  {  
    google::SetLogDestination(google::ERROR, logErrPath);  
  }  
  else if(log_level == google::GLOG_FATAL)
  {
  	google::SetLogDestination(google::FATAL, logFatalpath);  
  }
  else
	{
    google::ShutdownGoogleLogging();  	
	}       
  return 0;  
}
*/


