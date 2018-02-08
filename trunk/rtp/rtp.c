#include "rtp.h"
#include "fnc_log.h"

static unsigned char packet[RTP_MAX_PACKET_SIZE];

int rtp_send(RTP_session* session, unsigned char* data, unsigned int len)
{
	unsigned int packetLen;
	
	rtp_build_packet(session, data, len, packet, &packetLen);
	rtp_send_packet(session, packet, packetLen);

	return 0;
}


int rtp_build_packet(RTP_session* session, unsigned char* data, unsigned int len, unsigned char* packet, unsigned int *packetLen)
{
	session->head.seq_no++;
	//fnc_log(FNC_LOG_DEBUG, "seq_no:%d\n",session->head.seq_no);
	
	memset(packet, 0, RTP_MAX_PACKET_SIZE);
	memcpy(packet,&session->head,sizeof(RTP_header));
	memcpy(packet+sizeof(RTP_header),data,len);

	*packetLen = len + sizeof(RTP_header);
	
	return 0;
}


int rtp_send_packet(RTP_session* session,unsigned char* packet, unsigned int packetLen)
{
	int n = sendto(session->transport.rtp_fd, packet, packetLen, 0, (struct sockaddr*)&session->transport.srv_addr, sizeof(session->transport.srv_addr));
	if(n > 0)
	{
		//fnc_log(FNC_LOG_DEBUG,"send mesg:%d\n",packetLen);
	}
	
	return 0;
}


int rtp_recv(RTP_session* session,unsigned char* packet, unsigned int maxsize, unsigned int *len)
{
	socklen_t client_addr_length = sizeof(session->transport.client_addr);  
	int n = recvfrom(session->transport.rtp_fd, packet, maxsize, 0, (struct sockaddr*)&session->transport.client_addr, &client_addr_length);
	if(n > 0)
	{
//		fnc_log(FNC_LOG_DEBUG,"recv packet len:%d\n",n);
	}
	*len = n;
	
	return 0;
}

