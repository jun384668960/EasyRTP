#include <stdio.h>
#include <pthread.h>
#include "fnc_log.h"
#include "sockets.h"

#define SVR_PORT 	14433
#define MAXLINE		1024

struct ThreadArgs {  
    int client_sock;  
}; 

void *thread_func( void* arg)
{  
    struct ThreadArgs* thread_args = (struct ThreadArgs**)arg;
    unsigned char buf[MAXLINE] = {0};
    while(1)
    {
    	fnc_log(FNC_LOG_DEBUG, "tcp_read thread_args->client_sock:%d \n",thread_args->client_sock);
    	memset(buf, 0x0, MAXLINE);
		int n = tcp_read(thread_args->client_sock, buf, 1024);
		if(n > 0)
		{
			fnc_log(FNC_LOG_DEBUG, "buf:%s \n",buf);
		}
		else
		{
			break;
		}
		sleep(2);
	}
} 

int main(int argc, char* argv[])
{
	int ret = 0;
	struct sockaddr_in srv_add;
	struct sockaddr_in addr_client;
	tsocket srv_fd;
	char mesg[MAXLINE];

	fnc_init(FNC_LOG_DEBUG);
	
	srv_fd = tcp_listen(SVR_PORT);
	if(srv_fd < 0)
	{
		fnc_log(FNC_LOG_DEBUG, "tcp_listen error!\n");
		exit(0);
	}

	while(1)
	{
		tsocket clnt_fd = tcp_accept(srv_fd);
		if(clnt_fd <= 0)
		{
			fnc_log(FNC_LOG_DEBUG, "tcp_accept timeout!\n");
		}
		else
		{
			fnc_log(FNC_LOG_DEBUG,"tcp_accept clnt_fd:%d\n",clnt_fd);
			pthread_t id; 
			struct ThreadArgs* thread_args = (struct ThreadArgs*) malloc(sizeof (struct ThreadArgs)); 
			thread_args->client_sock = clnt_fd;
			ret=pthread_create(&id, NULL, (void *) thread_func, thread_args);  
		    if(ret!=0)  
		    {  
		        fnc_log (FNC_LOG_DEBUG,"Create pthread error!\n");  
		        return 0;  
		    } 
		}
		usleep(1500 * 1000);
	}
	
	return 0;
}
