#ifndef _RTP_H_
#define _RTP_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <sockets.h>

#define RTP_MAX_PACKET_SIZE 300*1024

typedef struct _RTP_transport
{
	tsocket rtp_fd;
	struct sockaddr_in srv_addr;
	struct sockaddr_in client_addr;
}RTP_transport;

typedef struct _RTP_header 
{
    /* byte 0 */
    unsigned char csrc_len:4;   /* expect 0 */
    unsigned char extension:1;  /* expect 1, see RTP_OP below */
    unsigned char padding:1;  /* expect 0 */
    unsigned char version:2;  /* expect 2 */
    /* byte 1 */
    unsigned char payload:7;/* RTP_PAYLOAD_RTSP */
    unsigned char marker:1;/* expect 1 */
    /* bytes 2, 3 */
    unsigned short seq_no;
    /* bytes 4-7 */
    unsigned int timestamp;
    /* bytes 8-11 */
    unsigned int ssrc;/* stream number is used here. */
} RTP_header;

typedef struct _RTP_session 
{	
	RTP_transport transport;
	int sessionId;
	RTP_header head;
} RTP_session;

int rtp_send(RTP_session* session, unsigned char* data, unsigned int len);
int rtp_build_packet(RTP_session* session, unsigned char* data, unsigned int len, unsigned char* packet, unsigned int *packetLen);
int rtp_send_packet(RTP_session* session,unsigned char* packet, unsigned int packetLen);
int rtp_recv(RTP_session* session,unsigned char* packet, unsigned int maxsize, unsigned int *len);

#endif

