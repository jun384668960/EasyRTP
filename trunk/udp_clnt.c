#include "stdio.h"
#include "fnc_log.h"
#include "sockets.h"

#define SVR_PORT 	44332
#define MAXLINE		1024
#define SERVER_ADDR	"127.0.0.1"

int main(int argc, char* argv[])
{
	int ret = 0;
	struct sockaddr_in srv_add;
	tsocket srv_fd;
	char mesg[MAXLINE];

	fnc_init(FNC_LOG_DEBUG);
	
	ret = udp_connect(SVR_PORT,&srv_add, inet_addr(SERVER_ADDR),&srv_fd);	
	if(ret != 0)
	{
		fnc_log(FNC_LOG_ERR, "udp_connect error!\n");
		return -1;
	}

	int count = 0;
	while(1)
	{
		memset(mesg, 0x0, MAXLINE);
		sprintf(mesg,"count:%d",count++);
		
		int n = sendto(srv_fd, mesg, MAXLINE, 0, (struct sockaddr*)&srv_add, sizeof(srv_add));
		if(n > 0)
		{
			fnc_log(FNC_LOG_DEBUG,"send mesg:%s\n",mesg);
		}
		fnc_log(FNC_LOG_DEBUG,"sendto test\n");
		
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

