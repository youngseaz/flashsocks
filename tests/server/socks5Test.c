#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<netdb.h>
#include<pthread.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>


#include "../../src/server/socks5.h"
#include "../../src/server/handler.h"


int main()
{
	proxy_handler();
	return 0;
}