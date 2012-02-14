#include <unistd.h>
#include <stdio.h>
//#include <cstdlib>
#include <stdlib.h>
//#include <cstring>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "d3_trans_globals.h"

int d3_makeConnection(const char *rmserver,int port);
int d3_makeConnection();
int d3_makeConnection_tcp(const char *rmserver,int port,int nmspace, int style, int protocol);
int d3_makeConnection_udp();
int d3_send_data(int sock,void *buffer,size_t size,int flags, sockaddr_in *toAddr);
int d3_send_data_tcp(int sock,void *buffer,size_t size,int flags);
int d3_send_data_udp(int sock,void *buffer,size_t size,int flags, sockaddr_in *toAddr);
int d3_recv_data(int sock,void *buffer,size_t size,int flags);
int d3_recv_data_tcp(int sock,void *buffer,size_t size,int flags);
//int d3_recv_data_udp(int sock,void *buffer,size_t size,int flags);
int d3_closeConnection(int sock);
int d3_closeConnection_tcp(int sock);
int d3_closeConnection_udp(int sock);
int d3_svr_makeSocket(int port);
int d3_svr_makeSocket_tcp(int port);
int d3_svr_makeSocket_udp(int port);
int d3_svr_accept(int sock);
int d3_svr_accept_tcp(int sock);
int d3_svr_accept_udp(int sock);
int d3_svr_recv(int sock,void *buffer,size_t size,int flags, sockaddr_in *toAddr);
int d3_svr_recv_tcp(int sock,void *buffer,size_t size,int flags);
int d3_svr_recv_udp(int sock,void *buffer,size_t size,int flags, sockaddr_in *toAddr);
struct sockaddr_in d3_make_sockaddr_in_client(const char *rmserver, int port);
/* udp unique send mode for acknowledge */
int d3_send_via_sockaddr_in( int sock, void *buffer, size_t size, int flags, sockaddr_in *toAddr );
struct sockaddr_in d3_make_sockaddr_in( const char *rmserver, int port );

/*    This part was moved to d3_tcp.cpp so to solve multiple definition problem.
int d3_makeConnection(const char *rmserver,int port){
        return d3_makeConnection_tcp(rmserver,port,NMSPACE,STYLE,PROTOCOL);
}
int d3_makeConnection(){
        return d3_makeConnection_udp();
}

int d3_send_data(int sock,void *buffer,size_t size,int flags, sockaddr_in *toAddr){
	if( TRANS_PROTOCOL == USE_TCP )
		return d3_send_data_tcp( sock, buffer, size, flags );
	else if( TRANS_PROTOCOL == USE_UDP )
		return d3_send_via_sockaddr_in( sock, buffer, size, flags, toAddr );
}

int d3_recv_data(int sock,void *buffer,size_t size,int flags){
    if(TRANS_PROTOCOL==USE_TCP){
        return d3_recv_data_tcp(sock,buffer,size,flags);    
    }else if(TRANS_PROTOCOL==USE_UDP){
//        return d3_recv_data_udp(sock,buffer,size,flags);
    }
}
int d3_closeConnection(int sock){
    if(TRANS_PROTOCOL==USE_TCP){
        return d3_closeConnection_tcp(sock);    
    }else if(TRANS_PROTOCOL==USE_UDP){
        return d3_closeConnection_udp(sock);
    }
}
int d3_svr_makeSocket(int port){
    if(TRANS_PROTOCOL==USE_TCP){
        return d3_svr_makeSocket_tcp(port);
    }else if(TRANS_PROTOCOL==USE_UDP){
       return d3_svr_makeSocket_udp(port);
    }
}
int d3_svr_accept(int sock){
    if(TRANS_PROTOCOL==USE_TCP){
        return d3_svr_accept_tcp(sock);
    }else if(TRANS_PROTOCOL==USE_UDP){
       return d3_svr_accept_udp(sock);
    }
}
        
int d3_svr_recv(int sock,void *buffer,size_t size,int flags,sockaddr_in *toAddr) {
	if( TRANS_PROTOCOL == USE_TCP )
		return d3_svr_recv_tcp(sock,buffer,size,flags);
	if( TRANS_PROTOCOL == USE_UDP )
		return d3_svr_recv_udp(sock,buffer,size,flags,toAddr);
}
*/
