/****************************************************************************
 *文件名称: 			base_main.cpp     
 *创建日期：			2017-9-15      
 *作    者:       郭世江          
 *修改记录：			2017-9-15  首次创建
 *功能描述：      主函数文件
 *****************************************************************************/
#include "base_station.h"
#include "common.h"
#include "parse_json.h"


log_info l_info[100]             = {{0}};
server_info svr_info[100]        = {{0}};
database_info db_info[100]       = {{0}};
database_mmr_id  db_mmr_id[1024] = {{0}};
database_stmt_info dbstmt_mmr_id[1024] = {{0}};

const char *welcome_str = "--Welcome you use our test tool--";

#if CROSS_PLATFORM == PLATFORM_WINDOWS
DWORD WINAPI thread_fun(LPVOID p)
#else
void* thread_fun(void* p)
#endif
{
	char          dest[12];
	int           ret_value;
	cp_uint32     mmr_mr_id;
	thread_param* tp = (thread_param*)p;
	base_station  bs;
	bs.mmrid         = tp->mmr_id;  
  ret_value = gethandle_onerow_data_from_database(dbstmt_mmr_id, tp->mmr_id);	
  if(ret_value == -1)
  {
  	printf("gethandle_onerow_data_from_database exec fail\n");
  	return NULL;
  }
  
  memset(dest, 0, sizeof(dest));
  strncpy(dest, dbstmt_mmr_id[0].master_uwb_id + 2, 10);  
  mmr_mr_id = atoi(dest);
 
	init_all(&bs);

	bool bconnect = false;
	bool bregist = false; 

	int tag_data_cnt = 1;

	while(true)
	{
		if(!bconnect)
		{
			int rtn = open_folder_file(&bs);
			//if (0 != rtn) break;

			tp->tcp_count = 0;
			tp->udp_count = 0;

			bs.tcp_cli = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if( bs.tcp_cli < 0 )
			{
				const char* perr = get_system_error( (char*)bs.buffer, bs.buffer_cap );
				//fprintf( bs.log, "%lu tcp socket() failed, %s\n", bs.mmrid, perr );
				//fprintf( stderr, "%lu tcp socket() failed, %s\n", bs.mmrid, perr );
				//fflush(bs.log);
				break;
			}

			bs.udp_cli = socket( AF_INET, SOCK_DGRAM, 0 );
			if ( bs.udp_cli < 0 )
			{
				const char* perr = get_system_error( (char*)bs.buffer, bs.buffer_cap );
				//fprintf( bs.log, "%lu tcp socket() failed, %s\n", bs.mmrid, perr );
				//fprintf( stderr, "%lu tcp socket() failed, %s\n", bs.mmrid, perr );
				//fflush(bs.log);
				break;
			}

#if CROSS_PLATFORM == PLATFORM_WINDOWS
			cp_int timeout = 6*1000;
#else
			struct timeval timeout = {6, 0};
#endif
			int ret = setsockopt( bs.tcp_cli, SOL_SOCKET, SO_RCVTIMEO, (char*)(&timeout), sizeof(timeout) );
			if ( ret < 0 )
			{
				const char* perr = get_system_error( (char*)bs.buffer, bs.buffer_cap);
				//fprintf( bs.log, "%lu tcp socket() failed, %s\n", bs.mmrid, perr );
				//fprintf( stderr, "%lu tcp socket() failed, %s\n", bs.mmrid, perr );
				//fflush(bs.log);
			}
			ret = setsockopt( bs.tcp_cli, SOL_SOCKET, SO_SNDTIMEO, (char*)(&timeout), sizeof(timeout) );
			if ( ret < 0 )
			{
				const char* perr = get_system_error( (char*)bs.buffer, bs.buffer_cap);
				//fprintf( bs.log, "%lu tcp socket() failed, %s\n", bs.mmrid, perr );
				//fprintf( stderr, "%lu tcp socket() failed, %s\n", bs.mmrid, perr );
				//fflush(bs.log);
			}
		
			bs.addr_in.sin_family = AF_INET;
			bs.addr_in.sin_port = htons( g_server_port );
#if CROSS_PLATFORM == PLATFORM_WINDOWS
			bs.addr_in.sin_addr.S_un.S_addr = inet_addr( g_server_host.c_str() );
#else
			bs.addr_in.sin_addr.s_addr = inet_addr( g_server_host.c_str() );
#endif

			if(connect( bs.tcp_cli, (sockaddr*)(&bs.addr_in), sizeof(bs.addr_in)) != 0)
			{
				const char* perr = get_system_error( (char*)bs.buffer, bs.buffer_cap );
				//fprintf( bs.log, "%lu tcp socket() failed, %s\n", bs.mmrid, perr );
				//fprintf( stderr, "%lu tcp socket() failed, %s\n", bs.mmrid, perr );
				//fflush(bs.log);

				if ( NULL != bs.info ) fclose( bs.info ); bs.info = NULL;
				if ( NULL != bs.info ) fclose( bs.log ); bs.log = NULL;

#if CROSS_PLATFORM == PLATFORM_WINDOWS
				closesocket( bs.tcp_cli );
				closesocket( bs.udp_cli );
				Sleep( 10*60*1000 ); // Sleep 10 minutes.
#else
				close( bs.tcp_cli );
				close( bs.udp_cli );
				sleep( 10*60 ); // Sleep 10 minutes.
#endif
				continue;
			}
#if 0
			LOG_DATE_TIME(bs.buffer, bs.buffer_cap, bs.mmrid);
			//fprintf( bs.log, "%s tcp connect successfully\n", bs.buffer );
			//fprintf(stdout, "%s tcp connect successfully\n", bs.buffer );
#endif	
			bconnect = true;
			tp->begin_time = time(NULL);
		}

		if (!bregist)
		{
			bs.pkg->pkg_len_ = PACKAGE_LEN+MESSAGE_LEN+MSG_REG_REQ;
			bs.pkg->pkg_cnt_ = 1;
			bs.pkg->pkg_id_ = 0;
			bs.pkg->msg_id_ = ++tp->tcp_count;
			bs.pkg->mmr_id_ = mmr_mr_id;   
			bs.msg->type_ = 0X03;
			bs.msg->ack_ = 0;
			bs.msg->ver_ = 0;
			bs.msg->res_ = 0;
			bs.reg_req->status_ = 0;
			bs.buffer_len = bs.pkg->serialize( bs.buffer, bs.buffer_cap );
			bs.buffer_len += bs.msg->serialize( bs.buffer+bs.buffer_len, bs.buffer_cap-bs.buffer_len );
			bs.buffer_len += bs.reg_req->serialize( bs.buffer+bs.buffer_len, bs.buffer_cap-bs.buffer_len );	
			int res = send_tcp_data(&bs);
			if(0 != res)
			{
                const char* res = get_system_error( (char*)bs.buffer, bs.buffer_cap );
                //fprintf( stderr, "%lu send tcp data(register) failed, %s\n", bs.mmrid, res );
                //fprintf( bs.log, "%lu send tcp data(register) failed, %s\n", bs.mmrid, res );
                //fflush( bs.log );

				LOG_DATE_TIME( bs.buffer, bs.buffer_cap, bs.mmrid );
				fprintf( stderr, "%s tcp count: %lu, udp count: %lu\n", bs.buffer, tp->tcp_count, tp->udp_count );
				//fprintf( bs.log, "%s tcp count: %lu, udp count: %lu\n", bs.buffer, tp->tcp_count, tp->udp_count );
				//fflush(bs.log);

				if ( NULL != bs.info ) fclose( bs.info ); bs.info = NULL;
				if ( NULL != bs.log ) fclose( bs.log ); bs.log = NULL;

#if CROSS_PLATFORM == PLATFORM_WINDOWS
				closesocket( bs.tcp_cli );
				closesocket( bs.udp_cli );
				Sleep(60*1000); 
#else
				close(bs.tcp_cli);
				close(bs.udp_cli);
				sleep(60); 
#endif
                bconnect = bregist = false; continue;
			}
#if 0
			fprintf(stdout, "send tcp register request successfully\n" );
#endif	

			res = recv_tcp_data( &bs );
			if ( 0 != res )
			{
                const char* res = get_system_error( (char*)bs.buffer, bs.buffer_cap );
                fprintf( stderr, "%lu receive tcp data(register) failed, %s\n", bs.mmrid, res );
                fprintf( bs.log, "%lu receive tcp data(register) failed, %s\n", bs.mmrid, res );
                fflush( bs.log );

				LOG_DATE_TIME( bs.buffer, bs.buffer_cap, bs.mmrid );
				//fprintf( stderr, "%s tcp count: %lu, udp count: %lu\n", bs.buffer, tp->tcp_count, tp->udp_count );
				//fprintf( bs.log, "%s tcp count: %lu, udp count: %lu\n", bs.buffer, tp->tcp_count, tp->udp_count );
				//fflush(bs.log);

				tp->tcp_count = tp->udp_count = 0;
				if ( NULL != bs.info ) fclose( bs.info ); bs.info = NULL;
				if ( NULL != bs.log ) fclose( bs.log ); bs.log = NULL;

#if CROSS_PLATFORM == PLATFORM_WINDOWS
				closesocket( bs.tcp_cli );
				closesocket( bs.udp_cli );
				Sleep( 60*1000 ); 
#else
				close( bs.tcp_cli );
				close( bs.udp_cli );
				sleep( 60 ); 
#endif
				bconnect = bregist = false; continue;
			}
#if 1
			fprintf( stdout, "%lu, recv tcp register response successfully\n", bs.mmrid );
#endif	

			bs.pkg->pkg_len_ = PACKAGE_LEN+MESSAGE_LEN+MSG_REG_CFM;
			bs.pkg->pkg_cnt_ = 1;
			bs.pkg->pkg_id_ = 0;
			bs.pkg->msg_id_ = ++tp->tcp_count;
			bs.pkg->mmr_id_ = mmr_mr_id; 
			bs.msg->type_ = 0X05;
			bs.msg->ack_ = 0;
			bs.msg->ver_ = 0;
			bs.msg->res_ = 0;
			
			bs.reg_cfm->error_ = 0;
			bs.buffer_len = bs.pkg->serialize( bs.buffer, bs.buffer_cap );
			bs.buffer_len += bs.msg->serialize( bs.buffer+bs.buffer_len, bs.buffer_cap-bs.buffer_len );
			bs.buffer_len += bs.reg_cfm->serialize( bs.buffer+bs.buffer_len, bs.buffer_cap-bs.buffer_len );
			res = send_tcp_data(&bs);
			if ( 0 != res )
			{
                const char* res = get_system_error( (char*)bs.buffer, bs.buffer_cap );
                //fprintf( stderr, "%lu send tcp data(confirm) failed, %s\n", bs.mmrid, res );
                //fprintf( bs.log, "%lu send tcp data(confirm) failed, %s\n", bs.mmrid, res );
                //fflush( bs.log );

				LOG_DATE_TIME( bs.buffer, bs.buffer_cap, bs.mmrid );
				//fprintf( stderr, "%s tcp count: %lu, udp count: %lu\n", bs.buffer, tp->tcp_count, tp->udp_count );
				//fprintf( bs.log, "%s tcp count: %lu, udp count: %lu\n", bs.buffer, tp->tcp_count, tp->udp_count );
				//fflush(bs.log);

				tp->tcp_count = tp->udp_count = 0;
				if ( NULL != bs.info ) fclose( bs.info ); bs.info = NULL;
				if ( NULL != bs.log ) fclose( bs.log ); bs.log = NULL;

#if CROSS_PLATFORM == PLATFORM_WINDOWS
				closesocket( bs.tcp_cli );
				closesocket( bs.udp_cli );
				Sleep( 60*1000 ); // Sleep 1 minutes.
#else
				close( bs.tcp_cli );
				close( bs.udp_cli );
				sleep( 60 ); // Sleep 1 minutes.
#endif
				bconnect = bregist = false; continue;
			}
#if 0
			fprintf(stdout, "send tcp register confirm successfully\n");
#endif	
			bregist = true;
		}
		else 
		{
			if (tp->udp_count % 6 == 0)
			{
				bs.pkg->pkg_len_ = PACKAGE_LEN+MESSAGE_LEN+MSG_HB_REQ;
				bs.pkg->pkg_cnt_ = 1;
				bs.pkg->pkg_id_ = 0;
				bs.pkg->msg_id_ = ++tp->tcp_count;
				bs.pkg->mmr_id_ = mmr_mr_id;  
				bs.msg->type_ = 0X07;  
				bs.msg->ack_ = 0;
				bs.msg->ver_ = 0;
				bs.msg->res_ = 0;
				bs.hb_req->status_ = 0;
				bs.buffer_len = bs.pkg->serialize( bs.buffer, bs.buffer_cap );
				bs.buffer_len += bs.msg->serialize( bs.buffer+bs.buffer_len, bs.buffer_cap-bs.buffer_len );
				bs.buffer_len += bs.hb_req->serialize( bs.buffer+bs.buffer_len, bs.buffer_cap-bs.buffer_len );
				int res = send_tcp_data(&bs);
				if ( 0 != res )
				{
                    const char* res = get_system_error((char*)bs.buffer, bs.buffer_cap );
                    //fprintf( stderr, "%lu send tcp data(heart beat) failed, %s\n", bs.mmrid, res );
                    //fprintf( bs.log, "%lu send tcp data(heart beat) failed, %s\n", bs.mmrid, res );
                    //fflush( bs.log );

					LOG_DATE_TIME( bs.buffer, bs.buffer_cap, bs.mmrid );
					//fprintf( stderr, "%s tcp count: %lu, udp count: %lu\n", bs.buffer, tp->tcp_count, tp->udp_count );
					//fprintf( bs.log, "%s tcp count: %lu, udp count: %lu\n", bs.buffer, tp->tcp_count, tp->udp_count );
					//fflush(bs.log);

					tp->tcp_count = tp->udp_count = 0;
					if ( NULL != bs.info ) fclose( bs.info ); bs.info = NULL;
					if ( NULL != bs.log ) fclose( bs.log ); bs.log = NULL;

#if CROSS_PLATFORM == PLATFORM_WINDOWS
					closesocket(bs.tcp_cli);
					closesocket(bs.udp_cli);
					Sleep(60*1000); // Sleep 1 minutes.
#else
					close(bs.tcp_cli);
					close(bs.udp_cli);
					sleep( 60 ); // Sleep 1 minutes.
#endif
					bconnect = bregist = false; continue;
				}
#if 0
				fprintf(stdout, "send tcp heart beat request successfully\n");
#endif	
				res = recv_tcp_data( &bs );
				if ( 0 != res )
				{
                    const char* res = get_system_error( (char*)bs.buffer, bs.buffer_cap );
                    //fprintf( stderr, "%lu receive tcp data(heart beat) failed, %s\n", bs.mmrid, res );
                    //fprintf( bs.log, "%lu receive tcp data(heart beat) failed, %s\n", bs.mmrid, res );
                    //fflush( bs.log );

					LOG_DATE_TIME( bs.buffer, bs.buffer_cap, bs.mmrid );
					//fprintf( stderr, "%s tcp count: %lu, udp count: %lu\n", bs.buffer, tp->tcp_count, tp->udp_count );
					//fprintf( bs.log, "%s tcp count: %lu, udp count: %lu\n", bs.buffer, tp->tcp_count, tp->udp_count );
					//fflush(bs.log);

					tp->tcp_count = tp->udp_count = 0;
					if ( NULL != bs.info ) fclose( bs.info ); bs.info = NULL;
					if ( NULL != bs.log ) fclose( bs.log ); bs.log = NULL;

#if CROSS_PLATFORM == PLATFORM_WINDOWS
					closesocket( bs.tcp_cli );
					closesocket( bs.udp_cli );
					Sleep( 60*1000 ); // Sleep 1 minutes.
#else
					close( bs.tcp_cli );
					close( bs.udp_cli );
					sleep(60); // Sleep 1 minutes.
#endif
					bconnect = bregist = false; continue;
				}
#if 0
				fprintf(stdout, "recv tcp heart beat response successfully\n" );
#endif	
			}

			init_send_tag_data(&bs, tp, mmr_mr_id, dbstmt_mmr_id[0].slave_mr_json, dbstmt_mmr_id[0].slave_num);
#if 1
            if ( tag_data_cnt%1000 == 0 )
                fprintf( stdout, "send udp tag data[%d] successfully\n",  tag_data_cnt++);
#endif

#if CROSS_PLATFORM == PLATFORM_WINDOWS
			Sleep( 200 );
#else
			usleep( 200000 );
#endif

			time_t current_time = time( NULL );
			if ( current_time-tp->begin_time > g_total_time*60 ) // 60 seconds * x = x minutes. 
			{
				LOG_DATE_TIME( bs.buffer, bs.buffer_cap, bs.mmrid );
				//fprintf(stderr, "%s tcp count: %lu, udp count: %lu\n", bs.buffer, tp->tcp_count, tp->udp_count );
				//fprintf( bs.log, "%s tcp count: %lu, udp count: %lu\n", bs.buffer, tp->tcp_count, tp->udp_count );
				//fflush(bs.log);

#if 0
                //fprintf( bs.log, "%s ,udp_cnt > g_total_time, have sent all the udp message\n", bs.buffer);
                fprintf(stdout, "%s ,udp_cnt > g_total_time, have sent all the udp message\n", bs.buffer);
#endif

				tp->tcp_count = tp->udp_count = 0;
				if ( NULL != bs.info ) fclose( bs.info ); bs.info = NULL;
				if ( NULL != bs.log ) fclose( bs.log ); bs.log = NULL;
#if CROSS_PLATFORM == PLATFORM_WINDOWS
				closesocket( bs.tcp_cli );
				closesocket( bs.udp_cli );
				Sleep( 30*60*1000 ); 
#else
				close( bs.tcp_cli );
				close( bs.udp_cli );
				sleep( 30*60 ); 
#endif
				bconnect = bregist = false;
			}
		}
	}
	free_all( &bs );
#if CROSS_PLATFORM == PLATFORM_WINDOWS
	return 0;
#else
	return NULL;
#endif
}

int start_server()
{
	int      ret;
	get_config();
	get_server_config(svr_info);
	get_log_config(l_info);
	get_db_config(db_info);
	ret = init_database(db_info);
	if(ret == -1)
	{
		cout << "error" << endl;	
		return -1;
	}
		
	g_server_host = svr_info->tcp_serverip;
	g_server_port = svr_info->tcp_severport;
	cp_uint32 start_id = svr_info->start_id;  
	unsigned int ncount = svr_info->thread_num;   
	g_total_time = svr_info->req_timeprid;
	
#if CROSS_PLATFORM == PLATFORM_WINDOWS
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,0),&wsa);

	HANDLE* handles = new HANDLE[ncount];
#else
	pthread_t* handles = new pthread_t[ncount];
#endif

	thread_param* params = new thread_param[ncount];

	for ( unsigned int i = 0; i < ncount; ++i )
	{
		params[i].mmr_id = i + start_id;       //数据库中field_id
#if CROSS_PLATFORM == PLATFORM_WINDOWS
		handles[i] = CreateThread( NULL, 0, thread_fun, (LPVOID)&params[i], 0, NULL );
        Sleep(200);
#else
		pthread_create(&handles[i], NULL, thread_fun, &params[i]);
        usleep(200000);
#endif
	}

#if CROSS_PLATFORM == PLATFORM_WINDOWS
	WaitForMultipleObjects( ncount, handles, true, INFINITE );
#else
	for ( unsigned int i = 0; i < ncount; ++i )
	{
		pthread_join( handles[i], NULL );
	}
#endif
	delete [] params;
	delete [] handles;
#if CROSS_PLATFORM == PLATFORM_WINDOWS
	WSACleanup();
#endif
	return 0;
}


int main()
{
	printf("helll");
	start_server();
	return 0;
}
