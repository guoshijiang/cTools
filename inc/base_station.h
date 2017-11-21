/****************************************************************************
 *文件名称: 			base_station.h     
 *创建日期：			2017-9-13      
 *作    者:       郭世江          
 *修改记录：			2017-9-13  首次创建
 *功能描述：      业务处理文件代码
 *****************************************************************************/
#ifndef _WRITE_LOG_H_
#define _WRITE_LOG_H_
#include "protocol.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#if CROSS_PLATFORM == PLATFORM_WINDOWS
	#include <io.h>
	#include <direct.h>
	#include <WINSOCK2.H>
	#include <STDIO.H>
#elif CROSS_PLATFORM == PLATFORM_LINUX
	#include <sys/time.h>
	#include <sys/stat.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <pthread.h>
	#include <string.h>
	#include <errno.h>
	#include <zconf.h>
#elif CROSS_PLATFORM == PLATFORM_APPLE
#elif CROSS_PLATFORM == PLATFORM_FREEBSD
#else
#endif
#include <string>
#include <iostream>

#include "handle_config.h"
#include "parse_json.h"
#include "write_log.h"
#include "misbio_mysql.h"

using namespace std;

#define SQL_SENTENCE "SELECT field_id, master_uwb_id, slave_mr_json, slave_num FROM c_field_mr WHERE field_id = ?"

#define LOG_FILE_DIR "../etc/config.ini"
#define DATARELATIONLEN 128

#if CROSS_PLATFORM == PLATFORM_WINDOWS
    #pragma  comment(lib,"ws2_32.lib")
#endif

#if CROSS_PLATFORM == PLATFORM_WINDOWS
#define LOG_DATE_TIME( buf, cap, mmrid ) { \
	UINT32 id = mmrid; \
	SYSTEMTIME st; \
	GetLocalTime( &st ); \
	sprintf_s( (char*)(buf), cap, "[%4d%02d%02d %02d:%02d:%02d:%d] mmrid:%u ", \
	st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, id ); \
	}
#elif CROSS_PLATFORM == PLATFORM_LINUX
    #define LOG_DATE_TIME( buf, cap, mmrid ) { \
    struct timeval tv; \
    gettimeofday( &tv, NULL ); \
    struct tm ltv; \
    localtime_r( &(tv.tv_sec), &ltv ); \
    snprintf( (char*)buf, cap, "[%4d%02d%02d %02d:%02d:%02d:%lu] mmrid:%lu", \
              ltv.tm_year+1900, ltv.tm_mon+1, ltv.tm_mday, \
              ltv.tm_hour, ltv.tm_min, ltv.tm_sec, tv.tv_usec, mmrid ); \
    }
#elif CROSS_PLATFORM == PLATFORM_APPLE
#elif CROSS_PLATFORM == PLATFORM_FREEBSD
#else
#endif

static std::string g_server_host;
static cp_uint g_server_port = 0;
static cp_uint g_total_time = 30; 

typedef struct _database_stmt_info
{
	char master_uwb_id[128];
	char slave_mr_json[10240];
	int slave_num;
}database_stmt_info;

typedef struct thread_param
{
	cp_uint32 mmr_id;
	size_t tcp_count;
	size_t udp_count;
	time_t begin_time;
} thread_param;

typedef struct base_station
{
  cp_uint32 mmrid;
	unsigned char* buffer;
	cp_size_t buffer_len;
	cp_size_t buffer_cap;
	
	package* pkg;
	message* msg;
	msg_reg_req* reg_req;
	msg_reg_rpd* reg_rpd;
	msg_reg_cfm* reg_cfm;
	msg_hb_req* hb_req;
	msg_hb_rpd* hb_rpd;
	msg_tag_data_resp* data;
	database_stmt_info* db_stmt_data;

  cp_socket_t tcp_cli;
  cp_socket_t udp_cli;

	sockaddr_in addr_in;
	cp_uint32 udp_faiure_cnt;

	FILE* log;
	FILE* info;
} base_station;

typedef struct _database_info
{
	char db_host[DATARELATIONLEN];    
  char db_user[DATARELATIONLEN];     
  char db_passwd[DATARELATIONLEN];    
  char db_name[DATARELATIONLEN];      
  unsigned int db_port;    
  char db_unixsock[DATARELATIONLEN];  
  long db_clientflag;
}database_info;

typedef struct _server_info
{
	char tcp_serverid[DATARELATIONLEN];
	char tcp_serverip[DATARELATIONLEN];
	int  tcp_severport;
	char udp_serverid[DATARELATIONLEN];
	char udp_serverip[DATARELATIONLEN];
	int  udp_serverport;
	int  thread_num;
	int  start_id;
	int  req_timeprid;
}server_info;

typedef struct _log_info
{
	char log_path[128];                //日志的路径
	bool is_write_log;                 //设置是否写日志	
	bool log_is_full;                  //当磁盘满时，停止日志输出
	uint32_t log_level;                //日志的级别
	unsigned int login_disk_time;      //将日志写入硬盘的时间间隔
	unsigned int max_log_size;         //日志的大小
	bool logto_stderr;                 //是否将错误日志输出到标准出错
	char log_extension_name[64];       //日志的扩展名
}log_info;

typedef struct _database_mmr_id
{
	char buf_mmr_id[128];
	char buf_uwbid[10240];
}database_mmr_id;

void get_config();

void get_db_config(database_info *db_info);

void get_server_config(server_info* svr_info);

void get_log_config(log_info* l_info);

int init_database(database_info *db_info);

void init_all(base_station* bs);

void free_all(base_station* bs);

const char* get_system_error(char * buf, size_t length);

int open_folder_file(base_station* bs);

int send_tcp_data( base_station* bs);

int recv_tcp_data( base_station* bs);

void send_udp_data( base_station* bs);

void init_send_tag_data(base_station* bs, thread_param* tp, cp_uint32 mmr_mr_id, char slave_mr_json[], int slave_num);

//void init_send_tag_data(base_station* bs, thread_param* tp);

int gethandle_data_from_database(database_mmr_id db_mmr_id[]);

int gethandle_onerow_data_from_database(database_stmt_info *dbstmt_mmr_id, int field_id);

#endif