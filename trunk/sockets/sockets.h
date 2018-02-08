#ifndef _SOCKETS_H_
#define _SOCKETS_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
		
typedef int tsocket;

tsocket tcp_listen(unsigned short port);
tsocket tcp_accept(tsocket fd);
tsocket tcp_connect(unsigned short port, char *addr);
int tcp_read(tsocket fd, void *buffer, int nbytes);
int tcp_write(tsocket fd, void *buffer, int nbytes);
void tcp_close(tsocket s);

int udp_open(unsigned short port,struct sockaddr_in *s_addr, tsocket *fd);						//创建udp server
int udp_connect(unsigned short to_port, struct sockaddr_in *s_addr, int addr, tsocket *fd);	//udp连接
int udp_close(tsocket fd);

#endif
