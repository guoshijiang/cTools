/****************************************************************************
 *文件名称: 			base_station.cpp     
 *创建日期：			2017-9-14      
 *作    者:       郭世江          
 *修改记录：			2017-9-14  首次创建
 *功能描述：      业务处理文件代码
 *****************************************************************************/
#include "base_station.h"
#include "misbio_stmt_mysql.h"
#include "parse_json.h"
#include "parse_json.cpp"

//宏定义
#define MACSIZE 40960000
#define STRSZIE 1024

//全局变量
map<string, string> config_item;
MISBIOSQL usql_handle;
//database_mmr_id  db_mmr_id[1024] = {{0}};

//初始化配置文件
void get_config()
{
	bool blok = ReadConfig(LOG_FILE_DIR, config_item);
	if(blok == false)
	{
		cout << "read config file error" << endl;
		return ; 
	}
	cout << "read config file success" << endl;
}

//获取数据库配置项
void get_db_config(database_info *db_info)
{
	 strcpy(db_info->db_host, config_item["dbhost"].c_str());
	 strcpy(db_info->db_user, config_item["dbuser"].c_str());    
	 strcpy(db_info->db_passwd, config_item["dbpasswd"].c_str());
	 strcpy(db_info->db_name, config_item["dbname"].c_str());
	 db_info->db_port = atoi(config_item["dbport"].c_str());
	 strcpy(db_info->db_unixsock, config_item["dbunixsock"].c_str());
	 db_info->db_clientflag = atoi(config_item["dbclientflag"].c_str());
}

//获取服务器配置项
void get_server_config(server_info* svr_info)
{
	strcpy(svr_info->tcp_serverid, config_item["tcpserverid"].c_str());	
	strcpy(svr_info->tcp_serverip, config_item["tcpserverip"].c_str());
  svr_info->tcp_severport = atoi(config_item["tcpserverport"].c_str()); 
	strcpy(svr_info->udp_serverid, config_item["udpserverid"].c_str());
	strcpy(svr_info->udp_serverip, config_item["tcpserverip"].c_str());
	svr_info->udp_serverport = atoi(config_item["udpserverport"].c_str());
	svr_info->thread_num = atoi(config_item["threadnum"].c_str());
	svr_info->start_id = atoi(config_item["startid"].c_str()); 
	svr_info->req_timeprid = atoi(config_item["reqtimeprid"].c_str());
}

//获取日志配置项
void get_log_config(log_info* l_info)
{
	strcpy(l_info->log_path, config_item["logpath"].c_str());	
	l_info->is_write_log = atoi(config_item["iswritelog"].c_str());
	l_info->log_is_full  = atoi(config_item["logisfull"].c_str());
	l_info->log_level    = atoi(config_item["loglevel"].c_str());
	l_info->login_disk_time = atoi(config_item["logindisktime"].c_str());
	l_info->max_log_size = atoi(config_item["maxlogsize"].c_str());
	l_info->logto_stderr = atoi(config_item["logtostderr"].c_str());
	strcpy(l_info->log_extension_name, config_item["logextensionname"].c_str());	
}

//数据库初始化
int init_database(database_info *db_info)
{
	usql_handle = MISBIODB_init(NULL);
	if(usql_handle == NULL) 
	{
		cout << "MISBIODB_init error" << endl;
		return -1;
	}
	cout << "MISBIODB_init ok" << endl;
	cout << db_info->db_port << endl;
	usql_handle = MISBIODB_Connect(usql_handle, "192.168.90.241", "root", "123456", "hisee_common", 3306, NULL, 0);
	//usql_handle = MISBIODB_Connect(usql_handle, db_info->db_host, db_info->db_user, db_info->db_passwd, db_info->db_name, 3306, NULL, 0);
	if(usql_handle == NULL)
	{
		cout << "MISBIODB_Connect error" << endl;
		return -1;	
	}
	cout << "connnect database ok" << endl;
	return 0;
}

void init_all(base_station* bs)
{
	bs->buffer_len = 0;
	bs->buffer_cap = 512;
	bs->buffer = new unsigned char[bs->buffer_cap];
	bs->pkg = new package();
	bs->msg = new message;
	bs->reg_req = new msg_reg_req;
	bs->reg_rpd = new msg_reg_rpd;
	bs->reg_cfm = new msg_reg_cfm;
	bs->hb_req = new msg_hb_req;
	bs->hb_rpd = new msg_hb_rpd;
	bs->data = new msg_tag_data_resp;
}

//内存释放
void free_all(base_station* bs)
{
	delete bs->data;
	delete bs->hb_rpd;
	delete bs->hb_req;
	delete bs->reg_cfm;
	delete bs->reg_rpd;
	delete bs->reg_req;
	delete bs->msg;
	delete bs->pkg;
	delete [] bs->buffer;
}

//获取系统错误
const char* get_system_error(char * buf, size_t length)
{
#if CROSS_PLATFORM == PLATFORM_WINDOWS
    INT32 err_no = WSAGetLastError();
	LPVOID lpMsgBuf = (LPVOID)"unknown error";	
	if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err_no, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL)) 
	{			
		sprintf_s(buf , length,  "ErrorCode %d: %s", err_no, lpMsgBuf);
	} 
	else
		sprintf_s(buf, length, "ErrorCode: %d", err_no);
	LocalFree(lpMsgBuf);
    return buf;
#else
    return strerror_r(errno, buf, length );
#endif
}

//写日志函数,打开目录文件,如果没有则创建
int open_folder_file(base_station* bs)
{
	/*
#if CROSS_PLATFORM == PLATFORM_WINDOWS
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf_s((char*)(bs->buffer), bs->buffer_cap, ".\\%4d%02d%02d", st.wYear, st.wMonth, st.wDay );
#else
    struct timeval tv;
    gettimeofday( &tv, NULL );
    struct tm ltv;
    localtime_r( &(tv.tv_sec), &ltv );
    snprintf((char*)(bs->buffer), bs->buffer_cap, "./%4d%02d%02d", ltv.tm_year+1900, ltv.tm_mon+1, ltv.tm_mday );
#endif
	std::string folder_name = (char*)(bs->buffer);
#if CROSS_PLATFORM == PLATFORM_WINDOWS
	if (0 != _access(folder_name.c_str(), 0))
	{
		if (0 != _mkdir(folder_name.c_str()))
		{
#else
    if (0!= access(folder_name.c_str(), 0))
    {
        if( 0 != mkdir(folder_name.c_str(), 755))
        {
#endif
            const char* perr = get_system_error( (char*)bs->buffer, bs->buffer_cap);
			//fprintf(stderr, "%lu tcp data send failed, %s\n", bs->mmrid, perr );
			return -1;
		}
	}

	std::string data_name = folder_name;
	data_name += "\\ispd_data_";
	size_t mmrid = bs->mmrid;
#if CROSS_PLATFORM == PLATFORM_WINDOWS
    sprintf_s( (char*)(bs->buffer), bs->buffer_cap, "%u_%02d%02d%02d.txt", mmrid, st.wHour, st.wMinute, st.wSecond );
#else
    snprintf( (char*)(bs->buffer), bs->buffer_cap, "%lu_%02u%02u%02u.txt", mmrid, ltv.tm_hour, ltv.tm_min, ltv.tm_sec );
#endif
	data_name += (char*)(bs->buffer);

	std::string log_name = folder_name;
	log_name += "\\log_";
#if CROSS_PLATFORM == PLATFORM_WINDOWS
	sprintf_s( (char*)(bs->buffer), bs->buffer_cap, "%u.txt", bs->mmrid );
#else
    snprintf( (char*)(bs->buffer), bs->buffer_cap, "%lu.txt", bs->mmrid );
#endif
	log_name += (char*)(bs->buffer);

	bs->info = NULL;
	//bs->info = fopen( data_name.c_str(), "w" );
	//if ( NULL == bs->info )
	//{
	//	strerror_s( (char*)bs->buffer, bs->buffer_cap, errno );
	//	fprintf( stderr, "file(%s) fopen() failed, %s\n", data_name.c_str(), (char*)bs->buffer );
	//	return -1;
	//}

	bs->log = fopen( log_name.c_str(), "a+"	);
	if ( NULL == bs->log )
	{
		const char* res = get_system_error( (char*)bs->buffer, bs->buffer_cap );
		//fprintf( stderr, "%lu file(%s) fopen() failed, %s\n", bs->mmrid, log_name.c_str(), res );
		return -1;
	}
	*/
	return -100;
}


//发送TCP数据
int send_tcp_data( base_station* bs)
{
	/*
	printf("--------------------------------\n");
	printf("Tcp read data(%lu):\n", sizeof((char*)bs->buffer));
  for (size_t i = 0; i < sizeof((char*)bs->buffer); ++i )
  {
    printf(" %02X", (char*)bs->buffer[i]);
  }
  printf("\n--------------------------------\n");
  */
	int rtn = send( bs->tcp_cli, (char*)bs->buffer, bs->buffer_len, 0 );
	if ( rtn < 0 )
	{
		const char* perr = get_system_error( (char*)bs->buffer, bs->buffer_cap );
		//fprintf( bs->log, "%lu tcp data send failed, %s\n", bs->mmrid, perr );
		//fprintf( stderr, "%lu tcp data send failed, %s\n", bs->mmrid, perr );
		return -1;
	}
	else if ( (size_t)rtn != bs->buffer_len )
	{
    const char* perr = get_system_error( (char*)bs->buffer, bs->buffer_cap );
		//fprintf( bs->log, "tcp data send failed, %s\n", perr );
		//fprintf( stderr, "tcp data send failed, %s\n", perr );
		return -2;
	}
	else
	{
		return 0;
	}
}

//接受TCP数据
int recv_tcp_data( base_station* bs)
{
	int rtn = recv( bs->tcp_cli, (char*)bs->buffer, bs->buffer_cap, 0 );
	if ( rtn <= 0 )
	{
		const char* perr = get_system_error( (char*)bs->buffer, bs->buffer_cap );
		//fprintf( bs->log, "%lu tcp data recv failed, %s\n", bs->mmrid, perr );
		//fprintf( stderr, "%lu tcp data recv failed, %s\n", bs->mmrid, perr );
		return -1;
	}
	else
	{
		bs->buffer_len = rtn;
		return 0;
	}
}

//发送UDP数据
void send_udp_data(base_station* bs)
{
	int rtn = sendto( bs->udp_cli, (char*)bs->buffer, bs->buffer_len, 0, (sockaddr*)&(bs->addr_in), sizeof(bs->addr_in) );
	if ((cp_size_t)rtn != bs->buffer_len )
	{
    const char* perr = get_system_error( (char*)bs->buffer, bs->buffer_cap );
		//fprintf( bs->log, "%lu udp data send failed, %s\n", bs->mmrid, perr );
		//fprintf( stderr, "%lu udp data send failed, %s\n", bs->mmrid, perr );
		++bs->udp_faiure_cnt;
	}
}

//初始化UDP数据
void init_send_tag_data(base_station* bs, thread_param* tp, cp_uint32 mmr_mr_id, char slave_mr_json[], int slave_num)
{
	unsigned int ncount = 0;
	unsigned int jsonlen = 0;
	char buf_uwbid[12];
	int  uwbId;
	parse_json_data json_data_array[1024] = {{0}};
	while (ncount < 60)
	{
		bs->pkg->pkg_cnt_ = 1;
		bs->pkg->pkg_id_ = 0;
		bs->pkg->msg_id_ = ++(tp->udp_count);
		bs->pkg->mmr_id_ = mmr_mr_id;   
		bs->msg->type_ = 0XB5;
		bs->msg->ack_ = 0;
		bs->msg->ver_ = 0;
		bs->msg->res_ = 0;
	
		bs->data->tag_cnt_ = 5;                  //rand() % 6; 标签数量
		if ( bs->data->tag_cnt_ == 0 )
			bs->data->tag_cnt_ = 5;
		bs->data->mr_cnt_ = slave_num;           //rand() % 6; 从基站数量
		if ( 0 == bs->data->mr_cnt_ )
			bs->data->mr_cnt_ = slave_num;

		ncount += bs->data->tag_cnt_;

		bs->data->data_array_.clear();
		for (int i = 0; i < bs->data->tag_cnt_; ++i)
		{
			msg_tag_data_resp::ispd_data ispd;
			ispd.id = i + 1;
			ispd.s_len = 15;
			jsonlen = parse_json(slave_mr_json, json_data_array);
			if(jsonlen < 0)
			{
				printf("parse_json function exec fail\n");
				return ;	
			}
	
			msg_tag_data_resp::time_diff td;
			for (int j = 0; j < bs->data->mr_cnt_; ++j) 
			{
				strncpy(buf_uwbid, json_data_array[j].uwb_id + 2, 11);
				uwbId  = atoi(buf_uwbid);
				td.mr_id = uwbId;                         
				td.value = td.mr_id;     
				ispd.td_array.push_back(td);
			}
			
			for (int k = 0; k < 3; ++k )
			{
				if ( 0 == k )
				{
					ispd.acce.x = td.mr_id % 65536; //rand() % 65536;  
					ispd.gyro.x = td.mr_id % 65536;  //rand() % 65536;
				}
				else if ( 1 == k )
				{
					ispd.acce.y = td.mr_id % 65536;        //rand() % 65536;  
					ispd.gyro.y = td.mr_id % 65536;        //rand() % 65536;  
				}
				else if ( 2 == k )
				{
					ispd.acce.z =  td.mr_id % 65536;  //rand() % 65536; 
					ispd.acce.z =  td.mr_id % 65536; //rand() % 65536; 
				}
			}
			ispd.heart_rate = td.mr_id % 256;   //rand() % 256;    
			ispd.power = 1;
			ispd.charge = 1;
			bs->data->data_array_.push_back(ispd);
		}
#if CROSS_PLATFORM == PLATFORM_WINDOWS
		SYSTEMTIME st;
		GetLocalTime( &st );
		bs->data->date_time_.year_ = st.wYear;
		bs->data->date_time_.month_ = st.wMonth;
		bs->data->date_time_.day_ = st.wDay;
		bs->data->date_time_.hour_ = st.wHour;
		bs->data->date_time_.minute_ = st.wMinute;
		bs->data->date_time_.second_ = st.wSecond;
		bs->data->date_time_.msecond_ = st.wMilliseconds;
#else
        struct timeval tv;
        gettimeofday( &tv, NULL );
        struct tm ltv;
        localtime_r( &(tv.tv_sec), &ltv );
        bs->data->date_time_.year_ = ltv.tm_year+1900;
        bs->data->date_time_.month_ = ltv.tm_mon+1;
        bs->data->date_time_.day_ = ltv.tm_mday;
        bs->data->date_time_.hour_ = ltv.tm_hour;
        bs->data->date_time_.minute_ = ltv.tm_min;
        bs->data->date_time_.second_ = ltv.tm_sec;
        bs->data->date_time_.msecond_ = tv.tv_usec * 0.001;
#endif
		bs->pkg->pkg_len_ = PACKAGE_LEN+MESSAGE_LEN+2+8+bs->data->tag_cnt_*(4+15+1+bs->data->mr_cnt_*(4+4));
		bs->buffer_len = bs->pkg->serialize(bs->buffer, bs->buffer_cap);
		bs->buffer_len += bs->msg->serialize(bs->buffer+bs->buffer_len, bs->buffer_len-bs->buffer_len);
		bs->buffer_len += bs->data->serialize(bs->buffer+bs->buffer_len, bs->buffer_len-bs->buffer_len);
		send_udp_data(bs);
	}
}

int gethandle_data_from_database(database_mmr_id db_mmr_id[])
{
	MISBIOSQLRES data_result;
	MISBIOSQLROW data_rows;
	MISBIOINT    data_count;
	MYSQL_FIELD  *field;
	char         *buf_mmr    = NULL;
	char         *buf_uwb    = NULL;
	char         *buf_mmr_id = NULL;
	char         *buf_uwbid  = NULL;
	char         *mmr_buffer = NULL;
	char         *uwb_buffer = NULL;
	unsigned int  num_fields;
	unsigned int  mmr_item    = 0;
	unsigned int  uwb_item    = 0;
  unsigned int  mmr_id_size = 0;
  unsigned int  uwbid_size  = 0;
  MYSQL_FIELD  *fields;
  MYSQL_FIELD_OFFSET offset;
  
  buf_mmr  = (char*)malloc(MACSIZE);
  if(buf_mmr == NULL)
	{
		cout << "malloc buf_mmr fail" << endl;
		return -1;
	}
	
	buf_uwb = (char*)malloc(MACSIZE);
  if(buf_mmr == NULL)
	{
		cout << "malloc buf_mmr fail" << endl;
		return -1;
	}
		
	data_result = MISBIODB_ExecQuerySqlRes(usql_handle, SQL_SENTENCE);
	if(data_result == NULL)
	{
		cout << "query database error" << endl;	
		return -1;
	}
	
	data_count = MISBIODB_GetSqlCount(usql_handle);
	if(data_count < 0)
	{
		cout << "there is no query result in this select\n" << endl;
		return -1;
	}
	
	while((data_rows = MISBIODB_FetchRow(data_result)) != NULL)
	{
		mmr_id_size = strlen(data_rows[0]);
		buf_mmr_id  = (char*)malloc(mmr_id_size);
		if(buf_mmr_id == NULL)
		{
			cout << "malloc buf_mmr_id fail" << endl;
			return -1;
		}
		sprintf(buf_mmr_id, "%s%s", data_rows[0], "|");
		strcat(buf_mmr, buf_mmr_id);
		
		uwbid_size  = strlen(data_rows[1]);
		buf_uwbid   = (char*)malloc(uwbid_size);
		if(buf_uwbid == NULL)
		{
			cout << "malloc buf_uwbid fail" << endl;
			return -1;
		}
		sprintf(buf_uwbid, "%s%s", data_rows[1], "|");
		strcat(buf_uwb,  buf_uwbid);		
	}

	mmr_buffer = strtok(buf_mmr, "|");
 	while(mmr_buffer != NULL)
 	{	
 		memcpy(db_mmr_id[mmr_item].buf_mmr_id, mmr_buffer, strlen(mmr_buffer)); 
 		mmr_buffer = strtok(NULL, "|");
 		mmr_item++;
 	}
 		 	
 	uwb_buffer = strtok(buf_uwb, "|");
 	while(uwb_buffer != NULL)
 	{	
 		memcpy(db_mmr_id[uwb_item].buf_uwbid, uwb_buffer, strlen(uwb_buffer)); 
 		uwb_buffer = strtok(NULL, "|");
 		uwb_item++;
 	}
 	free(mmr_buffer);
 	free(uwb_buffer);
	free(buf_mmr);
	free(buf_uwb);
	free(buf_mmr_id);
  free(buf_uwbid);
  MISBIOFree_DBRes(data_result);
	MISBIODB_Close(usql_handle);
	return uwb_item;
}


int gethandle_onerow_data_from_database(database_stmt_info *dbstmt_mmr_id, int field_id)
{
	MISBIOSQLRES         data_result;
	MISBIOSQLROW         data_rows;
	MISBIOINT            data_count;
	MISBIOSTMT           sqlStmt;
	MYSQL_FIELD          *field;
  MYSQL_FIELD          *fields;
  MYSQL_FIELD_OFFSET   offset;
  MISBIOLONG           iParamCount;
  MISBIOBINDSTR        data_Bind[10];
  
	unsigned int         num_fields;
	unsigned int         mmr_item    = 0;
	unsigned int         uwb_item    = 0;
  unsigned int         mmr_id_size = 0;
  unsigned int         uwbid_size  = 0;
  int                  return_value;
  char                 master_uwb_id[128];
  char                 slave_mr_json[10240];
  int                  slave_num;
 
	sqlStmt = MISBIO_StmtInit(usql_handle); 
	if(sqlStmt == NULL)
	{
		printf("MISBIO_StmtInit fail\n");
	  return -1;
	}
	
	if(MISBIO_StmtPrepare(sqlStmt, SQL_SENTENCE, strlen(SQL_SENTENCE))) 
	{
		printf("MISBIO_StmtPrepare error\n");
	  return -1;
	}

	iParamCount = MISBIO_StmtParamCount(sqlStmt);		
	if (iParamCount != 1)
	{ 	
		printf("MISBIO_StmtParamCount error\n");
	  return -1;
	}
		
	data_Bind[0].buffer_type = MYSQL_TYPE_LONG;
	data_Bind[0].buffer = &field_id;	
	
	data_Bind[1].buffer_type = MYSQL_TYPE_STRING;
  data_Bind[1].buffer = master_uwb_id;
  data_Bind[1].buffer_length = sizeof(master_uwb_id);
    
  data_Bind[2].buffer_type = MYSQL_TYPE_STRING;
  data_Bind[2].buffer = slave_mr_json;
  data_Bind[2].buffer_length = sizeof(slave_mr_json);
  
  data_Bind[3].buffer_type = MYSQL_TYPE_LONG;
	data_Bind[3].buffer = &slave_num;	
	
	
	if(MISBIO_StmtBindParam(sqlStmt, data_Bind))
	{ 
		printf("MISBIO_StmtBindParam error\n");
	  return -1;
	}
	
	if(MISBIO_StmtBindResult(sqlStmt, data_Bind))
	{ 
		printf("mysql_stmt_bind_result error\n");
	  return -1;
	}

  if(MISBIO_StmtExecute(sqlStmt)) 
  { 
  	printf("MISBIO_StmtExecute error\n");
		return -1;
  }			
		     	                           
	MISBIO_StmtStoreResult(sqlStmt);
	
  while(MISBIO_StmtFetch(sqlStmt) == 0) 
  {
  	memcpy(dbstmt_mmr_id->master_uwb_id, master_uwb_id, strlen(master_uwb_id));
  	memcpy(dbstmt_mmr_id->slave_mr_json, slave_mr_json, strlen(slave_mr_json));
  	dbstmt_mmr_id->slave_num = slave_num;
  }
	
  if(MISBIO_StmtClose(sqlStmt))	
	{
		printf("MISBIO_StmtClose error\n");
	  return -1;
	}	
	return 0;
}



 
