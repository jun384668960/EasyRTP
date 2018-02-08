BUILD_TOOL_PREFIX = 
CC = $(BUILD_TOOL_PREFIX)gcc

INCLUDES = -I./log -I./sockets -I./rtp -I./rtcp -I./h264 -I./utils
CFLAGS = -lpthread $(INCLUDES) -I. -O3 -g -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64

all:udp_clnt udp_srv

CSRCS = $(wildcard  ./log/*.c ./sockets/*.c ./rtp/*.c ./rtcp/*.c ./h264/*.c ./utils/*.c)
COBJS := $(CSRCS:.c=.o) 

$(COBJS) : %.o: %.c  
	$(CC) -c -fPIC $< -o $@ $(CFLAGS)  

tcp_srv:$(COBJS) tcp_srv.o
	$(CC) -o tcp_srv $(COBJS) tcp_srv.o $(CFLAGS)  

tcp_clnt:$(COBJS) tcp_clnt.o
	$(CC) -o tcp_clnt $(COBJS) tcp_clnt.o $(CFLAGS)  
	
udp_clnt:$(COBJS) udp_clnt.o
	$(CC) -o udp_clnt $(COBJS) udp_clnt.o $(CFLAGS)  

udp_srv:$(COBJS) udp_srv.o
	$(CC) -o udp_srv $(COBJS) udp_srv.o $(CFLAGS)  

rtp_srv:$(COBJS) rtp_srv.o
	$(CC) -o rtp_srv $(COBJS) rtp_srv.o $(CFLAGS)  
	
rtp_clnt:$(COBJS) rtp_clnt.o
	$(CC) -o rtp_clnt $(COBJS) rtp_clnt.o $(CFLAGS)  

clean:
	-rm -rf *.o
	-rm -rf ./log/*.o
	-rm -rf ./sockets/*.o
	-rm -rf ./rtp/*.o
	-rm -rf ./rtcp/*.o
	-rm -rf ./h264/*.o
	-rm -rf ./utils/*.o
	-rm -rf udp_srv udp_clnt rtp_srv rtp_clnt tcp_srv tcp_clnt