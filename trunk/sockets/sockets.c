#include <stdio.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "sockets.h"
#include "fnc_log.h"
#include <netinet/tcp.h>
#include <errno.h>

int set_socket_live(int fd, int liveSec)
{
	int keep_alive = 1;
	int keep_idle = liveSec, keep_interval = 1, keep_count = 3;
	int ret = -1;
	unsigned int timeout = 10000;

	unsigned char  service_type = 0xe0;
	if((ret = setsockopt(fd, SOL_IP/*IPPROTO_IP*/, IP_TOS, (void *)&service_type, sizeof(service_type))) < 0)			
	{
		fnc_log(FNC_LOG_ERR, "setsockopt(IP_TOS) failed");
		return ret;
	}

	if (-1 == (ret = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, sizeof(keep_alive)))) {
		fnc_log(FNC_LOG_ERR, "set socket to keep alive error");
		return ret;
	}

	if (-1 == (ret = setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof(keep_idle)))) {
		fnc_log(FNC_LOG_ERR, "set socket keep alive idle error");
		return ret;
	}

	if (-1 == (ret = setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &keep_interval, sizeof(keep_interval)))) {
		fnc_log(FNC_LOG_ERR, "set socket keep alive interval error");
		return ret;
	}

	if (-1 == (ret = setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &keep_count, sizeof(keep_count)))) {
		fnc_log(FNC_LOG_ERR, "set socket keep alive count error");
		return ret;
	}

	if (-1 == setsockopt(fd, IPPROTO_TCP, TCP_USER_TIMEOUT, &timeout, sizeof(timeout))) {
		fnc_log(FNC_LOG_ERR, "set TCP_USER_TIMEOUT option error");
		return ret;
	}
	return ret;
}

char *sock_ntop_host(const struct sockaddr *sa, socklen_t salen, char *str, size_t len)
{
    // static char str[128];		/* Unix domain is largest */
    // size_t len = sizeof(str);

	switch(sa->sa_family) 
	{
	case AF_INET: {
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

		if (inet_ntop(AF_INET, &sin->sin_addr, str, len) == NULL)
			return(NULL);
		return(str);
	}

#ifdef	IPV6
	case AF_INET6: {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, len) == NULL)
			return(NULL);
		return(str);
	}
#endif
	default:
		snprintf(str, len, "sock_ntop_host: unknown AF_xxx: %d, len %d",
				 sa->sa_family, salen);
		return(str);
	}
    return (NULL);
}


tsocket tcp_listen(unsigned short port)
{
	tsocket f;
    int on=1;

    struct sockaddr_in s;
    int v = 1;

    /*创建套接字*/
    if ((f = socket(AF_INET, SOCK_STREAM, 0))<0) 
    {
        fnc_log(FNC_LOG_ERR,"socket() error in tcp_listen.\n" );
        return -1;
    }

    /*设置socket的可选参数*/
    setsockopt(f, SOL_SOCKET, SO_REUSEADDR, (char *) &v, sizeof(int));

    s.sin_family = AF_INET;
    s.sin_addr.s_addr = htonl(INADDR_ANY);
    s.sin_port = htons(port);

    /*绑定socket*/
    if (bind (f, (struct sockaddr *)&s, sizeof (s))) 
    {
        fnc_log(FNC_LOG_ERR,"bind() error in tcp_listen" );
        return -1;
    }

    /*设置为非阻塞方式*/
    if (ioctl(f, FIONBIO, &on) < 0) 
    {
        fnc_log(FNC_LOG_ERR,"ioctl() error in tcp_listen.\n" );
        return -1;
    }

    /*监听*/
    if (listen(f, SOMAXCONN) < 0) 
    {
        fnc_log(FNC_LOG_ERR,"listen() error in tcp_listen.\n" );
        return -1;
    }

    return f;
}

tsocket tcp_accept(tsocket fd)
{
    tsocket f;
    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);

    memset(&addr,0,sizeof(addr));
    addrlen=sizeof(addr);

    /*接收连接，创建一个新的socket,返回其描述符*/
    f = accept (fd, (struct sockaddr *)&addr, &addrlen);
	if(f > 0)
	{
		set_socket_live(f, 5);
	}
    return f;
}

tsocket tcp_connect(unsigned short port, char *addr)
{
	int ret = 0;
	tsocket f;
	int on=1;
	int one = 1;/*used to set SO_KEEPALIVE*/
	
	struct sockaddr_in s;
	int v = 1;
	if ((f = socket(AF_INET, SOCK_STREAM, 0/*IPPROTO_TCP*/))<0) {
		fnc_log(FNC_LOG_ERR,"socket() error in tcp_connect.\n" );
		return -1;
	}
	setsockopt(f, SOL_SOCKET, SO_REUSEADDR, (char *) &v, sizeof(int));
	s.sin_family = AF_INET;
	s.sin_addr.s_addr = inet_addr(addr);//htonl(addr);
	s.sin_port = htons(port);
	fnc_log(FNC_LOG_ERR, "port:%d , addr:%s\n",port,addr);
	memset(&s.sin_zero,0,8);
	
	// set to non-blocking
//	if (ioctl(f, FIONBIO, &on) < 0) {
//		fnc_log(FNC_LOG_ERR,"ioctl() error in tcp_connect.\n" );
//		return -1;
//	}	
	
	if ((ret = connect(f,(struct sockaddr*)&s, sizeof(struct sockaddr))) < 0) {
		fnc_log(FNC_LOG_ERR,"connect() error in tcp_connect.ret:%d\n", ret);
		return -1;
	}        

//	if(setsockopt(f, SOL_SOCKET, SO_KEEPALIVE, &one, sizeof(one))<0){
//		fnc_log(FNC_LOG_ERR,"setsockopt() SO_KEEPALIVE error in tcp_connect.\n" );
//		return -1;
//	}
	set_socket_live(f, 5);

	return f;
}

int tcp_read(tsocket fd, void *buffer, int nbytes)
{
	int n;
    struct sockaddr_storage name;
    socklen_t namelen = sizeof(name);
    char addr_str[128];   /* Unix domain is largest */

	n = recv(fd,buffer,nbytes,0);
	fnc_log(FNC_LOG_DEBUG,"- - - :%d\n",n);
	
	if(n>0)
    {
        if( getpeername(fd, (struct sockaddr*)&name, &namelen) < 0 )
            fnc_log(FNC_LOG_DEBUG,"- - - \n");
        else
        {
            fnc_log(FNC_LOG_DEBUG,"%s - - \n", sock_ntop_host((struct sockaddr *)&name, namelen, addr_str, sizeof(addr_str)) );
        }
    }
	else// if(errno == EAGAIN)
	{
		//fnc_log(FNC_LOG_DEBUG,"errno == EAGAIN\n");
		tcp_close(fd);
	}

    return n;
	
}

int tcp_write(tsocket fd, void *buffer, int nbytes)
{
	int n;
	n = send(fd, buffer, nbytes,0);
	return n;
}

void tcp_close(tsocket s)
{
	fnc_log(FNC_LOG_ERR,"tcp_close() socket:%d.\n", s);
	static struct linger ling = {1, 0};
	setsockopt((socket), SOL_SOCKET, SO_LINGER, (void*)&ling, sizeof(ling));
	
	close(s);
}

/*********************************************************************************************/
int udp_open(unsigned short port,struct sockaddr_in *s_addr, tsocket *fd)		//创建udp server
{
	struct sockaddr_in addr;
    int on = 1; 

    if ((*fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        fnc_log(FNC_LOG_ERR,"socket() error in udp_open.\n" );
        return -1;
    }

	bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port =  htons(port);
	
    if (bind (*fd, (struct sockaddr *)&addr, sizeof (addr))) 
    {
        fnc_log(FNC_LOG_ERR,"bind() error in udp_open.\n" );
        return -1;
    }
    *s_addr=*((struct sockaddr_in*)&addr);

    return 0;
}


int udp_connect(unsigned short to_port, struct sockaddr_in *s_addr, int addr, tsocket *fd)	//udp连接
{
	struct sockaddr_in s;
    int on = 1; //,v=1;

    if ((*fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        fnc_log(FNC_LOG_ERR,"socket() error in udp_connect.\n" );
        return -1;
    }
	
	bzero(&s, sizeof(s));
    s.sin_family = AF_INET;
    s.sin_addr.s_addr = addr;
    s.sin_port = htons(to_port);
	
    if (connect(*fd,(struct sockaddr*)&s,sizeof(s))<0) 
    {
        fnc_log(FNC_LOG_ERR,"connect() error in udp_connect.\n" );
        return -1;
    }
    *s_addr=*((struct sockaddr_in*)&s);

    return 0;
}


int udp_close(tsocket fd)
{
	return close(fd);
}