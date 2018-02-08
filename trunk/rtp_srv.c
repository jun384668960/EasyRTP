#include <fcntl.h> 
#include "stdio.h"
#include "fnc_log.h"
#include "sockets.h"
#include "rtp.h"

#define SVR_PORT 	44332
int main(int argc, char* argv[])
{
	RTP_session session;
	int ret = 0;
	struct sockaddr_in srv_add;
	struct sockaddr_in client_addr;
	tsocket srv_fd;
	char packet[RTP_MAX_PACKET_SIZE];
	int fd = 0;
	char startBit[4] = {0x00, 0x00, 0x00, 0x01};

	fnc_init(FNC_LOG_DEBUG);
	
	ret = udp_open(SVR_PORT,&srv_add, &srv_fd);	
	if(ret != 0)
	{
		fnc_log(FNC_LOG_ERR, "udp_open error!\n");
		return -1;
	}

	if((fd = open("rtpRecv.file",O_WRONLY|O_CREAT,0777))==-1)  
    {  
        fnc_log(FNC_LOG_ERR,"The file of rtpRecv.file don't exist!\n");  
        return -1;
    } 

	memset(&session, 0, sizeof(session));
	session.sessionId = 1;
	session.transport.rtp_fd = srv_fd;
	session.transport.srv_addr = srv_add;
	session.transport.client_addr = client_addr;

	while(1)
	{
		int recvLen = 0;
		int n = rtp_recv(&session, packet, RTP_MAX_PACKET_SIZE, &recvLen);
		if(recvLen > 0)
		{
			//something deal with
			RTP_header head = {0};
			memcpy(&head, packet, sizeof(head));
//			fnc_log(FNC_LOG_DEBUG,"rtp_recv seq_no:%d ssrc:%x payload:%d timestamp:%ld\n"
//				,head.seq_no,head.ssrc,head.payload,head.timestamp);

			//if(head.payload == 96 && head.ssrc == 0x89)
			{	
				if(head.marker == 1)
				{
					write(fd,startBit,4);
				}
				int write_size = write(fd,packet + sizeof(RTP_header),recvLen - sizeof(RTP_header));
				if(write_size < recvLen - sizeof(RTP_header))
				{
					fnc_log(FNC_LOG_ERR,"Warning , write file miss packets!\n");  
				}
			}
		}
		memset(packet, 0x0, RTP_MAX_PACKET_SIZE);
		
		usleep(200);
	}

	close(fd);
	
	ret = udp_close(srv_fd);
	if(ret != 0)
	{
		fnc_log(FNC_LOG_ERR, "udp_close error!\n");
		return -1;
	}
	
	return 0;
}
