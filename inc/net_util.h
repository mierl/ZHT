/*
 * net_util.h
 *
 *  Created on: Feb 15, 2012
 *      Author: tony
 */

#ifndef NET_UTIL_H_
#define NET_UTIL_H_

int makeSvrSocket(int port, bool tcp);
int svr_accept(int sock, bool tcp);
int makeClientSocket(const char* host, int port, bool tcp);

int generalSendTo(const char* host, int port, int to_sock, const char* buff, bool tcp);
int generalSendBack(int to_sock, const char* buff_sendback, struct sockaddr_in sendbackAddr, int flag, bool tcp);
int generalReceive(int sock, void* recvBuff, int maxRecvSize, struct sockaddr_in & recvAddr, int flag, bool tcp);



int generalSendTCP(int to_sock, const char* buff);
int generalReveiveTCP(int sock, void *buffer, size_t size, int flags);


//int serverReceive(int sock, void *buffer, size_t size, int flags, bool tcp);

int udpSendTo(int toSock, const char* host, int port, const char* buff);
int udpRecvFrom(int sock, void* recvBuff, int maxRecvSize, struct sockaddr_in & recvAddr, int flag);
int udpSendBack(int sock, const char* buff_sendback, struct sockaddr_in sendbackAddr, int flag);

int reuseSock(int sock);
int setRecvTimeout(int sockfd, unsigned int sec, unsigned int usec);



#endif /* NET_UTIL_H_ */
