#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "cross_platform_type.h"

#if CROSS_PLATFORM == PLATFORM_WINDOWS
#else
#include <string.h>
#endif

#include <vector>
/**
 * @brief Little end.
 * @param v
 * @param b
 * @return
 */
template<typename T>
static unsigned int serialize_int( T v, unsigned char* b )
{
    for (unsigned int i = 0; i < sizeof(v); ++i )
    {
        b[i] = 0;
        b[i] |=  v >> i*8;
    }
    return  sizeof(v);
}

/**
 * @brief Little end.
 * @param b
 * @param v
 * @return
 */
template<typename T>
static unsigned int deserialize_int( const unsigned char* b, T& v )
{
    for (unsigned int i = 0; i <sizeof(v); ++i )
    {
        v |= b[i] << i*8;
    }
    return sizeof(v);
}

typedef struct date_time
{
	cp_uint32 year_;
	cp_uint32 month_;
	cp_uint32 day_;
	cp_uint32 hour_;
	cp_uint32 minute_;
	cp_uint32 second_;
	cp_uint32 msecond_;
} date_time;

const unsigned char g_head_flags[4] = { 0X7E, 0X7E, 0X7E, 0X7E };
#define PACKAGE_LEN 4+2+1+1+4+4
#define MESSAGE_LEN 1+1+1+1
#define MSG_REG_REQ 2
#define MSG_REG_RPD 2+1+1+1+1+1+1+2
#define MSG_REG_CFM 2
#define MSG_HB_REQ 2
#define MSG_HB_RPD 2
#define MSG_TAG_DATA_HEAD_LEN 1+1+1+1+1+1+1+1+2
#define ISPD_STATIC_LEN 4+2+2+2+2+2+2+2+1
#define ISPD_DYNAMIC_LEN 4+4

class package
{
public:
	package() {}
	virtual ~package() {}

	int serialize( unsigned char* buf, cp_size_t len )
	{
		cp_size_t off = 0;
		memcpy( buf+off, g_head_flags, 4 );
		off += 4;

		cp_uint16 tempu16 = this->pkg_len_;
		off += serialize_int( tempu16, buf+off );

		buf[off++] = this->pkg_cnt_;
		buf[off++] = this->pkg_id_;

		cp_uint32 tempu32 = this->msg_id_;
		off += serialize_int(tempu32, buf+off);
		
		cp_uint32 tempu64 = this->mmr_id_;
		off += serialize_int(tempu64, buf+off);

		return (int)off;
	}

	int deserialize( const unsigned char* buf, cp_uint len )
	{
		if ( memcmp(buf, g_head_flags, 4) != 0 )
			return -1;

		size_t off = 4;

		cp_uint16 tempu16 = 0;
		off += deserialize_int( buf+off, tempu16 );
		this->pkg_len_ = tempu16;

		if ( this->pkg_len_ < len )
			return -2;

		this->pkg_cnt_ = buf[off++];
		this->pkg_id_ = buf[off++];

		cp_uint32 tempu32 = 0;
		off += deserialize_int( buf+off, tempu32 );
		this->msg_id_ = tempu32;

		cp_uint32 tempu64 = 0;
		off += deserialize_int( buf+off, tempu64 );
		this->mmr_id_ = tempu64;

		return (int)off;
	}

public:
	unsigned char pkg_flag_[4];
	cp_uint32 pkg_len_;
	cp_uint32 pkg_cnt_;
	cp_uint32 pkg_id_;
	cp_uint32 msg_id_;
	cp_uint32 mmr_id_;
};

class message
{
public:
	message(){}
	virtual ~message() {}

	int serialize( unsigned char* buf, size_t len )
	{
		int off = 0;
		buf[off++] = this->type_;
		buf[off++] = this->ack_;
		buf[off++] = this->ver_;
		buf[off++] = this->res_;
		return off;
	}

	int deserialize( const unsigned char* buf, size_t len )
	{
		int off = 0;
		this->type_ = buf[off++];
		this->ack_ = buf[off++];
		this->ver_ = buf[off++];
		this->res_ = buf[off++];
		return off;
	}
public:
	unsigned char type_;
	unsigned char ack_;
	unsigned char ver_;
	unsigned char res_;
};

class msg_reg_req
{
public:
	msg_reg_req() : status_(0){}
	virtual ~msg_reg_req() {}

	virtual int serialize( unsigned char* buf, size_t len )
	{
		size_t off = 0;

		cp_uint16 tempu16 = this->status_;
		off += serialize_int( tempu16, buf+off );

		return (int)off;
	}

	virtual int deserialize( const unsigned char* buf, size_t len ) 
	{
		size_t off = 0;

		cp_uint16 tempu16 = 0;
		off += deserialize_int( buf+off, tempu16 );
		this->status_ = tempu16;

		return (int)off;
	}
public:
	cp_uint32 status_;
};

class msg_reg_rpd
{
public:
	msg_reg_rpd() {}
	virtual ~msg_reg_rpd() {}

	virtual int serialize( unsigned char* buf, size_t len )
	{
		size_t off = 0;

		cp_uint16 tempu16 = this->error_;
		off += serialize_int( tempu16, buf+off );

		buf[off++] = this->year_;
		buf[off++] = this->month_;
		buf[off++] = this->day_;
		buf[off++] = this->hour_;
		buf[off++] = this->minute_;
		buf[off++] = this->second_;

		tempu16 = this->msecond_;
		off += serialize_int( tempu16, buf+off );

		return (int)off;
	}

	virtual int deserialize( const unsigned char* buf, size_t len ) 
	{
		size_t off = 0;

		cp_uint16 tempu16 = 0;
		off += deserialize_int( buf+off, tempu16 );
		this->error_ = tempu16;

		this->year_ = buf[off++];
		this->month_ = buf[off++];
		this->day_ = buf[off++];
		this->hour_ = buf[off++];
		this->minute_ = buf[off++];
		this->second_ = buf[off++];

		tempu16 = 0;
		off += deserialize_int( buf+off, tempu16 );
		this->msecond_ = tempu16;

		return (int)off;
	}
public:
	cp_uint32 error_;
	cp_uint32 year_;
	cp_uint32 month_;
	cp_uint32 day_;
	cp_uint32 hour_;
	cp_uint32 minute_;
	cp_uint32 second_;
	cp_uint32 msecond_;
};

class msg_reg_cfm
{
public:
	msg_reg_cfm() {}
	virtual ~msg_reg_cfm() {}

	virtual int serialize( unsigned char* buf, cp_size_t len )
	{
		cp_size_t off = 0;

		cp_uint16 tempu16 = this->error_;
		off += serialize_int( tempu16, buf+off );

		return (int)off;
	}

	virtual int deserialize( const unsigned char* buf, cp_size_t len )
	{
		cp_size_t off = 0;

		cp_uint16 tempu16 = 0;
		off += deserialize_int( buf+off, tempu16 );
		this->error_ = tempu16;

		return (int)off;
	}
public:
	cp_uint32 error_;
};

class msg_hb_req
{
public:
	msg_hb_req() {}
	virtual ~msg_hb_req() {}
	virtual int serialize( unsigned char* buf, cp_size_t len )
	{
		cp_size_t off = 0;

		cp_uint16 tempu16 = this->status_;
		off += serialize_int( tempu16, buf+off );

		return (int)off;
	}
	virtual int deserialize( const unsigned char* buf, cp_size_t len )
	{
		cp_size_t off = 0;

		cp_uint16 tempu16 = 0;
		off += deserialize_int( buf+off, tempu16 );
		this->status_ = tempu16;

		return (int)off;
	}
public:
	cp_uint32 status_;
};

class msg_hb_rpd
{
public:
	msg_hb_rpd() {}
	virtual ~msg_hb_rpd() {}
	virtual int serialize( unsigned char* buf, cp_size_t len )
	{
		cp_size_t off = 0;

		cp_uint16 tempu16 = this->error_;
		off += serialize_int( tempu16, buf+off );

		return (int)off;
	}
	virtual int deserialize( const unsigned char* buf, cp_size_t len )
	{
		cp_size_t off = 0;

		cp_uint16 tempu16 = 0;
		off += deserialize_int( buf+off, tempu16 );
		this->error_ = tempu16;

		return (int)off;
	}
public:
	cp_uint32 error_;
};

class msg_tag_data_resp : public message
{
public:
	msg_tag_data_resp() {}
	virtual ~msg_tag_data_resp() {}
	virtual int serialize( unsigned char* buf, cp_size_t len )
	{
		cp_size_t off = 0;

		buf[off++] = this->mr_cnt_;
		buf[off++] = this->tag_cnt_;

		buf[off++] = this->date_time_.year_;
		buf[off++] = this->date_time_.month_;
		buf[off++] = this->date_time_.day_;
		buf[off++] = this->date_time_.hour_;
		buf[off++] = this->date_time_.minute_;
		buf[off++] = this->date_time_.second_;

		cp_uint16 tempu16 = this->date_time_.msecond_;
		off += serialize_int( tempu16, buf+off );

		for ( size_t i = 0; i < this->data_array_.size(); ++i )
		{
			cp_uint32 tempu32 = this->data_array_[i].id;
			off += serialize_int( tempu32, buf+off );
			
			time_diff_array& tda = this->data_array_[i].td_array;
			for ( size_t j = 0; j < this->data_array_.size(); ++j )
			{
				cp_uint32 tempu64 = tda[j].mr_id;
				off += serialize_int( tempu64, buf+off );
				tempu32 = tda[j].value;
				off += serialize_int( tempu32, buf+off );
			}

			buf[off++] = 15;

			for ( unsigned int i = 0; i < 3; ++i )
			{
				if ( 0 == i ) tempu16 = this->data_array_[i].acce.x;
				if ( 1 == i ) tempu16 = this->data_array_[i].acce.y;
				if ( 2 == i ) tempu16 = this->data_array_[i].acce.z;
				off += serialize_int( tempu16, buf+off );
			}

			for ( unsigned int i = 0; i < 3; ++i )
			{
				if ( 0 == i ) tempu16 = this->data_array_[i].gyro.x;
				if ( 1 == i ) tempu16 = this->data_array_[i].gyro.y;
				if ( 2 == i ) tempu16 = this->data_array_[i].gyro.z;
				off += serialize_int( tempu16, buf+off );
			}

			tempu16 = this->data_array_[i].heart_rate;
			off += serialize_int( tempu16, buf+off );

			buf[off++] = 1;
		}

		return (int)off;
	}
public:
	typedef struct time_diff
	{
		cp_uint32 mr_id;
		cp_uint32 value;
	} time_diff;

	typedef std::vector<time_diff> time_diff_array;

	typedef struct point_3n
	{
		int x;
		int y;
		int z;
	} point_3n;

	typedef struct ispd_data
	{
		cp_uint32 id;
		time_diff_array td_array;

		cp_uint32 s_len;
		point_3n acce;  //x,y方向的加速度
		point_3n gyro;  //x,y方向的陀螺仪
		cp_uint32 heart_rate;
		cp_uint32 power;
		cp_uint32 charge;
	}ispd_data;

	typedef std::vector<ispd_data> ispd_data_array;

	cp_uint32 mr_cnt_;  //从终端个数
	cp_uint32 tag_cnt_; //标签
	date_time date_time_;
	ispd_data_array data_array_;
};

#endif
