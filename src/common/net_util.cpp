/*
 * net_util.cpp
 *
 *  Created on: Feb 15, 2012
 *      Author: tony
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/time.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include "../../inc/net_util.h"
using namespace std;

//make socket for server, include bind and listen(if TCP), return socket
int makeSvrSocket(int port, bool tcp) { //only for svr
	struct sockaddr_in svrAdd_in; /* socket info about our server */
	int svrSock;

	memset(&svrAdd_in, 0, sizeof(struct sockaddr_in)); /* zero the struct before filling the fields */
	svrAdd_in.sin_family = AF_INET; /* set the type of connection to TCP/IP */
	svrAdd_in.sin_addr.s_addr = INADDR_ANY; /* set our address to any interface */
	svrAdd_in.sin_port = htons(port); /* set the server port number */

	if (tcp == true) { //make socket
		svrSock = socket(AF_INET, SOCK_STREAM, 0); /* OS will return a fd for network stream connection*/
	} else { //UDP

		svrSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}

	if (svrSock < 0) {
		printf(
				"Error occurred when creating the socket:%d to the server port:%d\n",
				svrSock, port);
		printf("%s\n", strerror(errno));
		close(svrSock);
		return -1;
	}

	if (tcp == true) {
		//for TCP works. may not work for UDP
		if (bind(svrSock, (struct sockaddr *) &svrAdd_in,
				sizeof(struct sockaddr)) < 0) {
			printf(
					"Error occurred binding the socket:%d to the server port:%d\n",
					svrSock, port);
			printf("%s", strerror(errno));
			close(svrSock);
			return -1;
		}
	} else { //UDP bind: exactly same as TCP??
//		memset(&svrAdd_in, 0, sizeof(svrAdd_in));
//		svrAdd_in.sin_family = AF_INET;
//		svrAdd_in.sin_addr.s_addr = INADDR_ANY;
//		svrAdd_in.sin_port = htons(port);

		if (bind(svrSock, (struct sockaddr*) &svrAdd_in,
				sizeof(struct sockaddr)) < 0) {
			/*		printf(	"Error occurred when binding the socket:%d to the server port:%d\n",sock, port);
			 printf("%s\n", strerror(errno));*/
			close(svrSock);
			return -1;
		}
	}

	if (tcp == true) { //TCP needs listen, UDP does not.
		/* start listening, allowing a queue of up to 1 pending connection */
		if (listen(svrSock, 1000) < 0) {
			printf("Error occurred while enabling listen on the socket:%d\n",
					svrSock);
			printf("%s", strerror(errno));
			close(svrSock);
			return -1;
		}
	}
	return svrSock;
}

//accept request, return working socket
int svr_accept(int sock, bool tcp) {

	if (tcp == true) {
		struct sockaddr_in dest; /* socket info about the machine connecting to us */
		int socksize = sizeof(struct sockaddr_in);
		int consocket;
		consocket = accept(sock, (struct sockaddr *) &dest,
				(socklen_t *) &socksize);
		if (consocket < 0) {
			printf("Error occurred while accepting on the socket:%d\n", sock);
			printf("%s\n", strerror(errno));
		}
		return consocket;
	} else {
		return sock;
	}
}

//make socket for client to send/receive
int makeClientSocket(const char* host, int port, bool tcp) {
	int to_sock;
	if (tcp == true) {
		struct sockaddr_in dest;
		memset(&dest, 0, sizeof(struct sockaddr_in)); /*zero the struct*/
		struct hostent * hinfo = gethostbyname(host);
		if (hinfo == NULL)
			printf("getbyname failed!\n");
		dest.sin_family = PF_INET; /*storing the server info in sockaddr_in structure*/
		dest.sin_addr = *(struct in_addr *) (hinfo->h_addr); /*set destination IP number*/
		dest.sin_port = htons(port);
		to_sock = socket(PF_INET, SOCK_STREAM, 0); //try change here.................................................
		if (to_sock < 0) {
			cerr << "net_util: error on socket(): " << strerror(errno) << endl;
			return -1;
		}

		int ret_con = connect(to_sock, (struct sockaddr *) &dest,
				sizeof(sockaddr));
		if (ret_con < 0) {
			cerr << "net_util: error on connect(): " << strerror(errno) << endl;
			return -1;
		}

	} else { //UDP
		to_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (to_sock < 0) {
			printf("Error occurred when creating the socket:%d\n", to_sock);
			printf("%s\n", strerror(errno));
			close(to_sock);
			return -1;
		}
	}
	return to_sock;
}

//general send, include TCP & UDP
int generalSendTo(const char* host, int port, int to_sock, const char* buff,
		bool tcp) {
	int buff_size = strlen(buff);
	int sentSize;

//	cout << buff_size << "{" << buff << "}" << endl;

	if (tcp == true) {
		sentSize = send(to_sock, (const void*) buff, buff_size, 0); // may try other flags......................
		if (sentSize < 0) {
			cerr << "net_util: generalSendTo: error on generalSendTo: "
					<< strerror(errno) << endl;
			return -1;
		}
	} else { //UDP
		sentSize = udpSendTo(to_sock, host, port, buff);
	}
	return sentSize;
}

//send back when you receive something
int generalSendBack(int to_sock, const char* buff_sendback,
		struct sockaddr_in sendbackAddr, int flag, bool tcp) {
	int sentSize = 0;
	int buff_size = strlen(buff_sendback);
	if (tcp == true) {
		sentSize = send(to_sock, (const void*) buff_sendback, buff_size, 0);

	} else { //UDP
		sentSize = udpSendBack(to_sock, buff_sendback, sendbackAddr, flag);

	}
	if (sentSize < 0) {
		cerr << "net_util: generalSendBack:  error on generalSendBack(): "
				<< strerror(errno) << endl;
		//return -1;
	}
	return sentSize;

}

int generalSendUDP(char* host, int port, int to_sock, char* buff) {

	/*
	 struct hostent *hp;
	 struct sockaddr_in server;
	 server.sin_family = AF_INET;
	 hp = gethostbyname(host);


	 bcopy((char *)hp->h_addr,
	 (char *)&server.sin_addr,
	 hp->h_length);
	 server.sin_port = htons(port);
	 int length = sizeof(struct sockaddr_in);
	 int n =sendto(to_sock,buff, strlen(buff),0,(const struct sockaddr *)&server,length);
	 */

	int buff_size = strlen(buff);
	int sentSize;
	struct sockaddr_in dest;
	memset(&dest, 0, sizeof(struct sockaddr_in)); /*zero the struct*/
	struct hostent * hinfo = gethostbyname(host);
	if (hinfo == NULL)
		printf("getbyname failed!\n");
	dest.sin_family = AF_INET; /*storing the server info in sockaddr_in structure*/
	dest.sin_addr = *(struct in_addr *) (hinfo->h_addr); /*set destination IP number*/
	dest.sin_port = htons(port);
	sentSize = sendto(to_sock, (const void*) buff, buff_size, 0,
			(struct sockaddr*) &dest, sizeof(dest));
	if (sentSize != buff_size) {
		printf("generalSend:udp: send failed. error:");
		printf("%s\n", strerror(errno));
		close(to_sock);
		return -1;
	}
	return sentSize;
}

int generalReceive(int sock, void* recvBuff, int maxRecvSize,
		struct sockaddr_in & recvAddr, int flag, bool tcp) {
	int recvSize = 0;
	if (tcp == true) {
		recvSize = recv(sock, recvBuff, maxRecvSize, flag);
	} else { //UDP
		recvSize = udpRecvFrom(sock, recvBuff, maxRecvSize, recvAddr, flag);
	}

	if (recvSize < 0) {
		cerr << "net_util: generalReceive(): error on receive: "
				<< strerror(errno) << endl;
		return -1;
	}

	return recvSize;

}

int generalSendTCP(int to_sock, const char* buff) {
	int buff_size = strlen(buff);
	int sentSize;

	sentSize = send(to_sock, (const void*) buff, buff_size, 0);

	if (sentSize < 0) {
		cerr << "net_util: error on generalSendTCP: " << strerror(errno)
				<< endl;
		return -1;
	}

	return sentSize;
}

/*
 int generalReceive(int sock, char* host, int port, void *buffer, size_t size,
 int flags, bool tcp) {
 memset(buffer, 0, size);
 if (tcp == true) {
 return recv(sock, buffer, size, flags);
 } else {
 unsigned int addrLen;

 //Here I strongly adopt that this will work, I made a sockaddr_in with specified address: host and port
 struct sockaddr_in rcvFromAddr;
 int recvLen;
 memset(&rcvFromAddr, 0, sizeof(struct sockaddr_in));
 struct hostent * hinfo = gethostbyname(host);
 if (hinfo == NULL)
 printf("getbyname failed!\n");
 rcvFromAddr.sin_family = PF_INET; //storing the server info in sockaddr_in structure
 rcvFromAddr.sin_addr = *(struct in_addr *) (hinfo->h_addr); //set destination IP number
 rcvFromAddr.sin_port = htons(port);
 //--------------------------------------------------
 addrLen = sizeof(rcvFromAddr);
 recvLen = recvfrom(sock, buffer, size, 0,
 (struct sockaddr*) &rcvFromAddr, &addrLen); //this cast from sockaddr_in to sockaddr could be wrong.
 if (recvLen < 0) {
 printf("d3_svr_recv_udp recvfrom failed, error: %s\n",
 strerror(errno));
 close(sock);
 return -1;
 }
 return recvLen;
 }
 }
 */

int generalReveiveTCP(int sock, void *buffer, size_t size, int flags) {
	return recv(sock, buffer, size, flags);
}

int generalReceiveUDP(int sock, char* host, int port, void *buffer, size_t size,
		int flags, bool tcp) {
	unsigned int addrLen;

	//Here I strongly adopt that this will work, I made a sockaddr_in with specified address: host and port
	struct sockaddr_in rcvFromAddr;
	int recvLen;
	memset(&rcvFromAddr, 0, sizeof(struct sockaddr_in)); /*zero the struct*/
	struct hostent * hinfo = gethostbyname(host);
	if (hinfo == NULL)
		printf("getbyname failed!\n");
	rcvFromAddr.sin_family = PF_INET; /*storing the server info in sockaddr_in structure*/
	rcvFromAddr.sin_addr = *(struct in_addr *) (hinfo->h_addr); /*set destination IP number*/
	rcvFromAddr.sin_port = htons(port);
	//--------------------------------------------------
	addrLen = sizeof(rcvFromAddr);
	recvLen = recvfrom(sock, buffer, size, flags,
			(struct sockaddr*) &rcvFromAddr, &addrLen); //this cast from sockaddr_in to sockaddr could be wrong.
	if (recvLen < 0) {
		printf("net_util: generalReceiveUDP failed, error: %s\n",
				strerror(errno));
		close(sock);
		return -1;
	}
	return recvLen;
}

int simpleRecvUDP(int sock, void *buffer, size_t size, int flag,
		struct sockaddr_in *rcvFromAddr) {

	int recvLen = recvfrom(sock, buffer, size, flag,
			(struct sockaddr*) rcvFromAddr, NULL);
	return recvLen;
}

// toSock can be made by makeClientSock()
int udpSendTo(int toSock, const char* host, int port, const char* buff) {
	struct hostent *hp;
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	hp = gethostbyname(host);

	bcopy((char *) hp->h_addr, (char *) &server.sin_addr, hp->h_length);
	server.sin_port = htons(port);
	int ret = sendto(toSock, buff, strlen(buff), 0, (struct sockaddr*) &server,
			sizeof(struct sockaddr));
	if (ret < 0) {
		cerr << "net_util.cpp: udpSendTo error: " << strerror(errno) << endl;
	}
	return ret;
}

int udpRecvFrom(int sock, void* recvBuff, int maxRecvSize,
		struct sockaddr_in & recvAddr, int flag) {

	socklen_t addr_len = sizeof(struct sockaddr);
	int ret = recvfrom(sock, recvBuff, maxRecvSize, flag,
			(struct sockaddr *) &recvAddr, &addr_len);
	if (ret < 0) {
		cerr << "net_util.cpp: udpRecvFrom error: " << strerror(errno) << endl;
	}
	return ret;
}

int udpSendBack(int sock, const char* buff_sendback,
		struct sockaddr_in sendbackAddr, int flag) {

	int ret = sendto(sock, buff_sendback, strlen(buff_sendback), flag,
			(struct sockaddr *) &sendbackAddr, sizeof(struct sockaddr));
//	cout<<"UDP sendback: ret =  "<<ret<<endl;
	if (ret < 0) {
		cerr << "net_util.cpp: udpSendBack error: " << strerror(errno) << endl;
	}
	return ret;

}

//may not work
int serverReceive(int sock, void *buffer, size_t size, int flags, bool tcp) {
	if (tcp == true) {
		return recv(sock, buffer, size, flags);
	} else { //---UDP
		unsigned int addrLen;
		struct sockaddr_in rcvFromAddr;
		int recvLen;
		memset(&rcvFromAddr, 0, sizeof(struct sockaddr_in)); /*zero the struct*/
		//--------------------------------------------------
		addrLen = sizeof(rcvFromAddr);
		recvLen = recvfrom(sock, buffer, size, 0, (sockaddr *) &rcvFromAddr,
				&addrLen);
		if (recvLen < 0) {
			printf("d3_svr_recv_udp recvfrom failed, error: %s\n",
					strerror(errno));
			close(sock);
			return -1;
		}

		return recvLen;
	}
}

int reuseSock(int sock) {
	int reuse_addr = 1;
	int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr,
			sizeof(reuse_addr));
	if (ret < 0) {
		cerr << "resuse socket failed: " << strerror(errno) << endl;
		return -1;
	} else
		return 0;
}
/*
 int setTCPLowLatency(int sock){
 int flag = 1;
 int ret = setsockopt( sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );
 if (ret == -1) {
 printf("Couldn't setsockopt(TCP_NODELAY)\n");
 exit( EXIT_FAILURE );
 }
 return ret;
 }
 */
int setRecvTimeout(int sockfd, unsigned int sec, unsigned int usec) {
	struct timeval tv;

	tv.tv_sec = sec; /* 30 Secs Timeout */
	tv.tv_usec = 0; // Not init'ing this can cause strange errors

	return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv,
			sizeof(struct timeval));
}
