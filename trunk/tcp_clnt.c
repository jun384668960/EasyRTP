#include "stdio.h"
#include "fnc_log.h"
#include "sockets.h"

#define SVR_PORT 	14433
#define MAXLINE		1024
#define SERVER_ADDR	"127.0.0.1"

int main(int argc, char* argv[])
{
	int ret = 0;
	tsocket srv_fd;
	char mesg[MAXLINE];

	fnc_init(FNC_LOG_DEBUG);
	
	srv_fd = tcp_connect(SVR_PORT,SERVER_ADDR);	
	if(srv_fd <= 0)
	{
		fnc_log(FNC_LOG_ERR, "tcp_connect error!\n");
		return -1;
	}

	int count = 0;
	while(1)
	{
		memset(mesg, 0x0, MAXLINE);
		sprintf(mesg,"count:%d",count++);
		
		int n = tcp_write(srv_fd, mesg, MAXLINE);
		if(n > 0)
		{
			fnc_log(FNC_LOG_DEBUG,"send mesg:%s\n",mesg);
		}
		fnc_log(FNC_LOG_DEBUG,"sendto test\n");
		
		sleep(5);
	}

	tcp_close(srv_fd);
	
	return 0;
}


