int configtest()
{
	database_info db_info[100] = {{0}};
	server_info svr_info[100] = {{0}};
	log_info l_info[100] = {{0}};
	
	get_config_ini();
  get_db_config(db_info);
	get_server_config(svr_info);
	get_log_config(l_info);
	//db
	printf("db_info->db_host = %s\n", db_info->db_host);
	printf("db_info->db_user = %s\n", db_info->db_user);
	printf("db_info->db_passwd = %s\n", db_info->db_passwd);
	printf("db_info->db_name = %s\n", db_info->db_name);
	printf("db_info->db_port = %d\n", db_info->db_port);
	printf("db_info->db_unixsock = %s\n", db_info->db_unixsock);
	printf("db_info->db_clientflag = %d\n", db_info->db_clientflag);
	
	//server
	printf("svr_info->tcp_serverid = %s\n", svr_info->tcp_serverid);
	printf("svr_info->tcp_serverip = %s\n", svr_info->tcp_serverip);
	printf("svr_info->tcp_severport = %d\n", svr_info->tcp_severport);
	printf("svr_info->udp_serverid = %s\n", svr_info->udp_serverid);
	printf("svr_info->udp_serverip = %s\n", svr_info->udp_serverip);
	printf("svr_info->udp_serverport = %d\n", svr_info->udp_serverport);
	printf("svr_info->thread_num = %d\n", svr_info->thread_num);
	printf("svr_info->req_timeprid = %d\n", svr_info->req_timeprid);
	
	cout << "----------log------------" << endl;
	
	printf("l_info->log_path = %s\n", l_info->log_path);
	printf("l_info->is_write_log = %d\n", l_info->is_write_log); //bool
	printf("l_info->log_is_full = %d\n", l_info->log_is_full);   //bool
	printf("l_info->log_level = %d\n", l_info->log_level);
	printf("l_info->login_disk_time = %d\n", l_info->login_disk_time);
	printf("l_info->max_log_size = %d\n", l_info->max_log_size);
	printf("l_info->logto_stderr = %d\n", l_info->logto_stderr);  //bool
	printf("l_info->log_extension_name = %s\n", l_info->log_extension_name);
	
	return 0;	
}