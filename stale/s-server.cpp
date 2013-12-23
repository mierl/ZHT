/*
 * s-server.cpp
 *
 *  Created on: Oct 29, 2011
 *      Author: leo
 */

//#include "d3_transport.h"
#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include<fstream>
#include "zht_util.h"
using namespace std;


//simple_send is good to go. stuck at receive. now tcp only.
int simple_receive(int sock,void *buffer,size_t size,int flags){
	int ret = recv(sock, buffer, size, flags);
	return ret;
}

int main(int argc, char* argv[]){

cout<<"Simple server with d3 lib..."<<endl;
	int port = 50000;
	//all for TCP
	int server_sock = d3_svr_makeSocket(port);
	cout <<"port: "<<port<<endl;
	sockaddr_in toAddr;
	char buff[1000];
	while(server_sock > 0){
		int client_sock = d3_svr_accept(server_sock);
		if (client_sock > 0){//correct,
			int t = d3_svr_recv(client_sock, buff, sizeof(buff), 0, &toAddr);
			//int r = simple_receive(client_sock, buff, 100, 0);
			cout <<"Received " << t << " bytes!" << endl;
			cout << buff <<endl;
		}

	}


	return 0;
}
