#ifndef _SOCKS5_H
#define _SOCKS5_H

#include<stdio.h>
#include<stdint.h>

#ifndef bool
typedef unsigned char bool;
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#define SOCKS5_BUFSIZE 65536 // 1024 * 64 = 64K

char socks5_buf[SOCKS5_BUFSIZE];

#define SOCKS5_VER      0x05

#define TCP (uint8_t)0x00
#define UPD (uint8_t)0x01

typedef struct _proxy_info
{
	bool isauthenticated;           // client wether authenticated, true or false
	int client_fd;             // client socket fd
	int remote_fd;             // remote socket fd
	struct _proxy_info *next;
	struct _proxy_info *previous;
}proxy_info_t, *pproxy_info_t;


/*
 * phrase one
 *
 * authentication
 */

// the way of authentication in socket 5
#define NO_AUTH_REQUIRED   0x00
#define GASSAPI            0x01
#define USR_PWD            0x02
#define NO_ACCEPTED_METHOD 0xff


/*
 * +---------+----------+---------+
 * | version | nmethods | methods |
 * +---------+----------+---------+
 * |  0x05   |  1 byte  | 1 ~ 255 | 
 * +---------+----------+---------+
 *
 */

typedef struct _auth_req
{
	uint8_t version;
	uint8_t nmethods;
	uint8_t methods[256];
}auth_req_t, *pauth_req_t;


typedef struct _auth_rsp
{
	uint8_t version;
	uint8_t method;
}auth_rsp_t, *pauth_rsp_t;



/*

+----+------+----------+------+----------+
|VER | ULEN |  UNAME   | PLEN |  PASSWD  |
+----+------+----------+------+----------+
| 1  |  1   | 1 to 255 |  1   | 1 to 255 |
+----+------+----------+------+----------+
*/

typedef struct _usrpawd_auth
{
	uint8_t ver;
	uint8_t ulen;
	uint8_t plen;
	char uname[256];
	char passwd[256]; 
}usrpwd_auth_data_t, *pusrpwd_auth_data_t;


/*
 * phrase two
 *
 * setup proxy
 */

// address type in phrase two (setup proxy phrase)
#define SOCK5_ADDR_TYPE_IPV4    0x01
#define SOCK5_ADDR_TYPE_DOMAIN  0x03
#define SOCK5_ADDR_TYPE_IPV6    0x04

// requrest type in phrase two (setup proxy phrase)
#define SOCK5_CMD_TYPE_CONNECT        0x01
#define SOCK5_CMD_TYPE_BIND           0x02
#define SOCK5_CMD_TYPE_UDP_ASSOCIATE  0x03

// status code in phrase two (setup proxy phrase)
#define SUCCESS            0x00
#define FAILURE            0x01
#define NOT_ALLOWED        0x02
#define NET_UNREACHABLE    0x03
#define HOST_UNREACHABLE   0x04
#define CONN_REFUSED       0x05
#define TTL_EXPIRED        0x06
#define CMD_NOT_SUPPORT    0x07
#define ADDR_TYPE_NOT_SUPPORT 0x08
#define UNASIGNED          0x09


/*

+----+-----+-------+------+----------+----------+
|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
+----+-----+-------+------+----------+----------+
| 1  |  1  | X'00' |  1   | Variable |    2     |
+----+-----+-------+------+----------+----------+

*/

typedef struct _conn_req
{
	uint8_t ver;
	uint8_t cmd;
	uint8_t rsv;
	uint8_t atyp;
	uint8_t addrlen;
	uint16_t port;
	union
	{
		uint32_t ipv4;
		uint8_t ipv6[16];
	}addr;
}conn_req_t, *pconn_req_t;



/*
 * phrase three
 *
 * datas relay
 *

        +----+-----+-------+------+----------+----------+
        |VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
        +----+-----+-------+------+----------+----------+
        | 1  |  1  | X'00' |  1   | Variable |    2     |
        +----+-----+-------+------+----------+----------+
*/

int socks5_init();
int socks5_shakehands(int client_fd); 

void socket_run();

#endif
