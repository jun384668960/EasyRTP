#include "stdio.h"
#include "fnc_log.h"
#include "sockets.h"

#define SVR_PORT 	44332
#define MAXLINE		1024
int main(int argc, char* argv[])
{
	int ret = 0;
	struct sockaddr_in srv_add;
	struct sockaddr_in addr_client;
	tsocket srv_fd;
	char mesg[MAXLINE];

	fnc_init(FNC_LOG_DEBUG);
	
	ret = udp_open(SVR_PORT,&srv_add, &srv_fd);	
	if(ret != 0)
	{
		fnc_log(FNC_LOG_ERR, "udp_open error!\n");
		return -1;
	}

	while(1)
	{
		socklen_t client_addr_length = sizeof(addr_client);  
		int n = recvfrom(srv_fd, mesg, MAXLINE, 0, (struct sockaddr*)&addr_client, &client_addr_length);
		if(n > 0)
		{
			fnc_log(FNC_LOG_DEBUG,"recv mesg:%s\n",mesg);
		}
		fnc_log(FNC_LOG_DEBUG,"srv test n=%d\n",n);
		memset(mesg, 0x0, MAXLINE);
		
		sleep(5);
	}
	
	ret = udp_close(srv_fd);
	if(ret != 0)
	{
		fnc_log(FNC_LOG_ERR, "udp_close error!\n");
		return -1;
	}
	
	return 0;
}
