#include <fcntl.h> 
#include "stdio.h"
#include "fnc_log.h"
#include "sockets.h"
#include "rtp.h"
#include "H264Analyzer.h"

#define SVR_PORT 	44332
#define MAXLINE		1500
#define SERVER_ADDR	"127.0.0.1"

int main(int argc, char* argv[])
{
	RTP_session session;
	int ret = 0;
	struct sockaddr_in srv_add;
	tsocket srv_fd;
	unsigned char mesg[MAXLINE];
	int count = 0;
	int fd = 0;
	int fd2 = 0;

	unsigned char nal_type = 0;
	int offset = 0;
	int nal_len = 0;
	int send_len = 0;
	int end_bit = 0;
	
	struct timeval tBegin;
	
	fnc_init(FNC_LOG_DEBUG);
	
	ret = udp_connect(SVR_PORT,&srv_add, inet_addr(SERVER_ADDR),&srv_fd);	
	if(ret != 0)
	{
		fnc_log(FNC_LOG_ERR, "udp_connect error!\n");
		return -1;
	}

	if((fd = open("rtpSend.file",O_RDONLY))==-1)  
    {  
        fnc_log(FNC_LOG_ERR,"The file of rtpRecv.file don't exist!\n");  
        return -1;
    } 
	if((fd2 = open("rtpRecv11.file",O_WRONLY|O_CREAT,0777))==-1)  
    {  
        fnc_log(FNC_LOG_ERR,"The file of rtpRecv.file don't exist!\n");  
        return -1;
    } 

	memset(&session, 0x0, sizeof(RTP_session));
	session.sessionId = 1;
	session.transport.rtp_fd = srv_fd;
	session.transport.srv_addr = srv_add;
	
	while(1)
	{
		memset(mesg, 0x0, MAXLINE);
		//something to send
		//修改成 读一个完整nalu BUF[最大帧大小]
		int read_size=read(fd,mesg,MAXLINE);
		if(read_size <= 0)
		{
			break;
		}
		send_len = 0;
		
		while(send_len < read_size)
		{
			nal_type = 0;
			offset = 0;
			nal_len = 0;
	
			//H264NaluAnalyzer
			ret = H264_nalu_analyzer(&mesg[send_len], read_size - send_len, &nal_type, &offset,&nal_len, &end_bit);
			fnc_log(FNC_LOG_DEBUG,"read_size:%d send_len:%d offset:%d nal_len:%d\n",read_size,send_len,offset,nal_len);
			if(nal_len > 0)
			{	
				unsigned char _sps[256];
				unsigned char _pps[256];
				int get_sps = H264_base64_sps(_sps);
				int get_pps = H264_base64_pps(_pps);
				if(get_sps == 0 && get_sps == 0)
				{
					fnc_log(FNC_LOG_DEBUG,"get_sps:%s get_pps:%s \n",_sps,_pps);
				}
				int tmp = 0;
				if(ret == -1)
				{
					tmp = nal_len;
					fnc_log(FNC_LOG_DEBUG,"send_len:%d nal_len:%d offset:%d\n",send_len,nal_len,offset);
					//exit(1);
				}
				else if(ret == 0)
				{
					tmp = nal_len +  4;
				}
				if(end_bit == 1)
				{
					session.head.marker = 1;
				}
				else
				{
					session.head.marker = 0;
				}
				write(fd2,mesg + send_len + offset,nal_len);
				
				gettimeofday(&tBegin, NULL);
				session.head.version = 2;
				session.head.ssrc = 0x89;
				session.head.payload = 96;
				session.head.timestamp = 1000000L * tBegin.tv_sec  + tBegin.tv_usec;
			
				int n = rtp_send(&session, mesg + send_len + offset, nal_len);
				
				send_len += tmp;
				usleep(500);
				continue;
			}
			else
			{
				if(end_bit == 1)
				{
					session.head.marker = 1;
				}
				else
				{
					session.head.marker = 0;
				}
				
				gettimeofday(&tBegin, NULL);
				session.head.version = 2;
				session.head.ssrc = 0x89;
				session.head.payload = 96;
				session.head.timestamp = 1000000L * tBegin.tv_sec  + tBegin.tv_usec;
			
				int n = rtp_send(&session, "", 0);
				
				send_len += 4;
				usleep(500);
				continue;
			}
//			ret = H264NaluAnalyzer(&mesg[send_len], read_size - send_len, &nal_type, &offset,&nal_len);
//			if(ret == 0)
//			{
//				if(nal_type != 0 && nal_type != 255 && nal_type == 9 && nal_len != 6)
//				{
//					fnc_log(FNC_LOG_DEBUG, "offset:%d read_size:%d send_len:%d nal_type:%d nal_len:%d mesg[send_len+4]:%x,mesg[send_len+5]:%x,mesg[send_len+6]:%x\n"
//											,offset,read_size,send_len,nal_type,nal_len,mesg[send_len+4],mesg[send_len+5],mesg[send_len+6]);
//				}
//			}
			
			//nal_len =  read_size;

			if(nal_len > 0)
			{
				write(fd2,mesg + send_len,nal_len);
				
				gettimeofday(&tBegin, NULL);
				session.head.version = 2;
				session.head.ssrc = 0x89;
				session.head.payload = 96;
				session.head.timestamp = 1000000L * tBegin.tv_sec  + tBegin.tv_usec;
			
				int n = rtp_send(&session, mesg+send_len, nal_len);
				//if(n == 0)
				//{
					
					send_len += nal_len;
					
	//				fnc_log(FNC_LOG_DEBUG,"rtp_send send_len:%d nal_len:%d\n",send_len,nal_len);
				//}
			}

			usleep(500);
		}

		
		usleep(500);
	}

	close(fd2);
	close(fd);

	ret = udp_close(srv_fd);
	if(ret != 0)
	{
		fnc_log(FNC_LOG_ERR, "udp_close error!\n");
		return -1;
	}
	
	return 0;
}




