#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#include "d3_trans_globals.h"
#include "d3_transport.h"
#define MAX_INT_MSG_SIZE 8

struct sockaddr_in dest;                                            /*holds info about server m/c*/

int d3_svr_makeSocket_tcp(int port){
    struct sockaddr_in serv;                                            /* socket info about our server */
    int mysocket,r;
    mysocket = socket(AF_INET, SOCK_STREAM, 0);                         /* OS will return a fd for network stream connection*/
    memset(&serv, 0, sizeof(struct sockaddr_in));                                     /* zero the struct before filling the fields */
    serv.sin_family = AF_INET;                                          /* set the type of connection to TCP/IP */
    serv.sin_addr.s_addr = INADDR_ANY;                                  /* set our address to any interface */
    serv.sin_port = htons(port);                                        /* set the server port number */    
    r = bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));  /* bind serv information to mysocket */
    if(r<0) {
        printf("Error occurred binding the socket:%d to the server port:%d\n",mysocket,port);
        printf("%s",strerror(errno));
        close(mysocket);
        return -1;
    }
    r = listen(mysocket, 5000);                                            /* start listening, allowing a queue of up to 1 pending connection */
    if(r<0) {
        printf("Error occurred while enabling listen on the socket:%d\n",mysocket);
        printf("%s",strerror(errno));
        close(mysocket);
        return -1;
    }
    return mysocket;
}
int d3_svr_accept_tcp(int sock){
    struct sockaddr_in dest;                                            /* socket info about the machine connecting to us */
    int socksize=sizeof(struct sockaddr_in);
    int consocket;
    consocket = accept(sock, (struct sockaddr *)&dest, (socklen_t *)&socksize);
    if(consocket<0) {
        printf("Error occurred while accepting on the socket:%d\n",sock);
        printf("%s\n",strerror(errno));
    }
    return consocket;
}
int d3_svr_recv_tcp(int sock,void *buffer,size_t size,int flags){//problem: can only receive fixed length

/*
	int index=0,len=0;
    char *buf;
    buf=(char *)memset(buffer,0,size);
    while(index<size){
	len = recv(sock,&buf[index],size-index,flags);
	index=index+len;
    }
    return index;
*/
	//This old code was created by Roman, problem: can only receive fixed length.
	return recv(sock, buffer, size, flags);

}
int d3_makeConnection_tcp(const char *rmserver,int port,int nmspace, int style, int protocol){
    int sock = socket(nmspace,style,protocol);                         /*returns a fd for a TCP socket*/
    int ret;
    struct hostent *hinfo;                                              /*sturcture to hold the dest m/c IP address*/
    if(sock<0){
    	printf("Error occurred while creating socket:%d\n",sock);
    	printf("%s\n",strerror(errno));
    	return -1;
    }
    memset(&dest, 0, sizeof(struct sockaddr_in));                        /*zero the struct*/
    hinfo = gethostbyname(rmserver);
    if(hinfo == NULL)
    	printf("getbyname failed!\n");
    dest.sin_family = AF_INET;                                          /*storing the server info in sockaddr_in structure*/
    dest.sin_addr =  *(struct in_addr *)(hinfo->h_addr);                /*set destination IP number*/
    dest.sin_port = htons(port);                                        /*set destination port number*/
    ret = connect(sock, (struct sockaddr *)&dest, sizeof(struct sockaddr)); /*OS creates a connection to server and returns the fd in mysocket */
    if(ret<0){
    	printf("Error occurred while connecting on the socket:%d\n",sock);
    	printf("%s\n",strerror(errno));
    	close(sock);
    	return -1;
    }
    return sock;
}
int d3_send_data_tcp(int sock,void *buffer,size_t size,int flags){
    int r;    
    r = send(sock,buffer,size,flags);
    if (r < 0){
        printf("d3_tcp.cpp:90 Error sending data: %s\n", strerror(errno));
    }
    return r;
}
int d3_recv_data_tcp(int sock,void *buffer,size_t size,int flags){
    return recv(sock,buffer,size,flags);
}
int d3_closeConnection_tcp(int sock){
    return close(sock);
}




//-----------------------------------------------------------------------------------------------------

//----------------------------Moved from d3_transport.h------------------------------------------------




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
