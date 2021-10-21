#define _GNU_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<errno.h>
#include<netdb.h>
#include<pthread.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "../third-pard/sockets/rdwrn.h"
#include "../third-pard/cJSON/cJSON.h"
#include "../utils/logger.h"
#include "../utils/io.h"
#include "socks5.h"


/*
* parse username and password authentication
* @client_fd  socket5 client socket file descriptor
* @pauth_data a pointer points to usrpwd_auth_data_t structure
* @return:
		return 0 in default
		return -1 if incorrect socket5 version is detected

*/

static int parse_auth_req(int client_fd, pusrpwd_auth_data_t pauth_data)
{
	readn(client_fd, (char *)pauth_data, 2);
	if (pauth_data->ver != SOCKS5_VER) {
		LOG_ERROR("Socket5 version field incorrect: 0x%02x", pauth_data->ver);
		return -1;
	}
	readn(client_fd, pauth_data->uname, pauth_data->ulen);
	readn(client_fd, pauth_data->plen, 1);
	readn(client_fd, pauth_data->passwd, pauth_data->plen);
	return 0;
}


/*
*
*
*/
static bool verify_usrpwd(usrpwd_auth_data_t auth_data)
{
	char uname[256], passwd[256];
	uint16_t i;
	for(i = 0; i < auth_data.ulen; i++) {
		if (uname[i] ^ auth_data.uname[i]) {
			LOG_DEBUG("username incorrect, username: %s", uname);
			return false;
		}
	}
	for (i = 0; i < auth_data.plen; i++) {
		if (passwd[i] ^ auth_data.passwd[i]) {
			LOG_DEBUG("password incorrect, password: %s", passwd);
			return false;
		}
	}
	return true;
}


/*
* socks5 authentication
*
* success 
*
*/


static int socks5_auth(int client_fd)
{
	int i, n, method;
	auth_req_t auth_req;
	n = 0;
	readn(client_fd, &auth_req, 2);
	readn(client_fd, auth_req.methods, auth_req.nmethods);
	if (auth_req.version != SOCKS5_VER) {
		LOG_ERROR("unsupported socket version: 0x%02x", auth_req.version);
		return -1;
	}
	while (n < auth_req.nmethods) {
		method = auth_req.methods[n];
		switch(method) {
			case NO_AUTH_REQUIRED:
				return 0;
			case USR_PWD:
				return 0;
		}
	}
	return 1;
}


/*
 *  read data from client and parse it
 *  @pconn_req_data: a structure pointer point to following format
 *	+----+-----+-------+------+----------+----------+
 *	|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
 *	+----+-----+-------+------+----------+----------+
 *	| 1  |  1  | X'00' |  1   | Variable |    2     |
 *	+----+-----+-------+------+----------+----------+
 #  
 *
*/

static int parse_conn_req(int client_fd, pconn_req_t pconn_req_data)
{
	int ret, i;
	uint8_t domain_len;
	char buf[256], ip[16], *domain;
	struct hostent *host;
	readn(client_fd, pconn_req_data, 4);
	if (pconn_req_data->ver != 0x05) {
		LOG_ERROR("Invalid socket version, version field is %d", pconn_req_data->ver);
		return -1;
	}
	if (pconn_req_data->cmd == 0 || pconn_req_data->cmd > 0x03) {
		LOG_ERROR("Invalid request cmd, cmd field is %d", pconn_req_data->cmd);
		return -1;
	}
	if (pconn_req_data->rsv != 0) {
		LOG_WARNING("The reverse field of request isn't zero!");
	}
	if (pconn_req_data->atyp == SOCK5_ADDR_TYPE_IPV4) {
		readn(client_fd, &pconn_req_data->addr, 4);
		readn(client_fd, &pconn_req_data->port, 2);
	}
	else if (pconn_req_data->atyp == SOCK5_ADDR_TYPE_DOMAIN)
	{
		domain = buf;
		readn(client_fd, &domain_len, 1);
		readn(client_fd, domain, domain_len);
		domain[domain_len] = '\0';
		readn(client_fd, &pconn_req_data->port, 2);
		host = gethostbyname(domain); 
		if (host == NULL) {
			LOG_ERROR("gethostbyname(%s) failed: ", domain, hstrerror(h_errno));
			return -1;
		}
		memcpy(&pconn_req_data->addr, host->h_addr_list[0], 4);
		LOG_DEBUG("domain: %s --> IP address: %s", domain, inet_ntop(host->h_addrtype, host->h_addr_list[0], ip, 256));
	}
	else if (pconn_req_data->atyp == SOCK5_ADDR_TYPE_IPV6) {
		// IPv6 + port
		readn(client_fd, &pconn_req_data->addr, 16); // IPv6
		readn(client_fd, &pconn_req_data->port, 2);  // port
	}
	else {
		LOG_ERROR("Invalid atyp, atyp is %d\n", pconn_req_data->atyp);
		return -1;
	}
}


/*
 * connect to remote for forwarding data to it from client
 * @conn_req_data: the datas of remote server, ip, port etc...
 * @return:
 * 		return fd connected to remote
 *		return 1 
 *
 */

static int connect_remote(conn_req_t conn_req_data)
{
	int ret, i, fd = 0;
	pconn_req_t req = &conn_req_data;
	struct sockaddr_in addr;
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		LOG_ERROR("create socket fail: %s", strerror(errno));
		return -1;
	}
	setnonblocking(fd);
	// here set nonblock will cause "Operation now in progress"
	int sockopt = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = req->port;
	addr.sin_addr.s_addr = req->addr.ipv4;
	if (req->cmd == SOCK5_CMD_TYPE_CONNECT) {
		ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
		/*
		if (ret != 0 && errno != EINPROGRESS) {
			LOG_ERROR("failed to connect to %s: %s", "ip", strerror(errno));
			close(fd);
			return -1;
		}
		*/
	}
	LOG_INFO("connected to remote ip %s", inet_ntoa(addr.sin_addr));
	return fd;
}



/*
* socket5 shake hands
* @return:
* 		return true in success
*		return false in failure
*/

int socks5_shakehands(int client_fd)
{
	char buf[32];
	int ret, remote_fd;
	conn_req_t conn_req_data;

	readn(client_fd, buf, 3); // receive auth data

	//send(client_fd, "\x05\x00", 2, MSG_DONTWAIT);
	writen(client_fd, "\x05\x00", 2);  // no auth method
	
	ret = parse_conn_req(client_fd, &conn_req_data);
	if(ret == -1) {
		close(client_fd);
		return -1;
	}
	remote_fd = connect_remote(conn_req_data);
	if(remote_fd == -1) {
		LOG_ERROR("failed to connect to remote: %s", strerror(errno));
		return -1;
	}
	memcpy(buf, "\x05\x00\x00\x01", 4);
	memcpy(buf + 4, &conn_req_data.addr.ipv4, 4);
	memcpy(buf + 8, &conn_req_data.port, 2);
	writen(client_fd, buf, 10);
	return remote_fd;
}


int socks5_init()
{
	int ret, n;
	int socks5_fd;
	struct sockaddr_in socks5_addr, client_addr;
	conn_req_t conn_req_data;
	LOG_INFO("socket5 initial...");

	socks5_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socks5_fd == -1) {
		LOG_CRITICAL("create socket fail: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	setnonblocking(socks5_fd);

	int sockopt = 1;
	setsockopt(socks5_fd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));

	if (ret == -1) {
		LOG_ERROR("set listening socket fd reuse error: %s", strerror(errno));
	}
	
	memset(&socks5_addr, 0, sizeof(socks5_addr));
	socks5_addr.sin_family = AF_INET;
	socks5_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	socks5_addr.sin_port = htons(2233);
	ret = bind(socks5_fd, (struct sockaddr*)&socks5_addr, sizeof(socks5_addr));
	if (ret == -1) {
		LOG_CRITICAL("bind error: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	ret = listen(socks5_fd, 4096);
	if (ret == -1) {
		LOG_CRITICAL("listening port error: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	LOG_INFO("socks5 server started.");
	return socks5_fd;
}
