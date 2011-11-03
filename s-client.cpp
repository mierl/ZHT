/*
 * s-client.cpp
 *
 *  Created on: Oct 29, 2011
 *      Author: leo
 */

#include <string>
#include <iostream>
#include "d3_transport.h"
using namespace std;

struct HostEntity {
	struct sockaddr_in si;
	int sock;
	string host;
	int port;
	bool valid;
};


int simpleSend(string str, struct HostEntity destination, int &current_sock) {

	//cout<< "contactReplica: newly received msg: package.replicano= " << package.replicano()<< ", package.ByteSize()="<< package.ByteSize() <<endl;//here the correct package was received: -1

	int i = 0, port, client_sock, r = 0;
	int32_t str_size = str.length();
	string hostName;
	sockaddr_in toAddr, recv_addr;

#if TRANS_PROTOCOL == USE_TCP
	client_sock = d3_makeConnection((destination.host).c_str(),
			destination.port);
#elif TRANS_PROTOCOL == USE_UDP

	cout << endl << "simpleSend makeSocket start-----" << endl;
	client_sock = d3_svr_makeSocket((time(NULL) % 10000) + rand() % 10000); //client can use any port to talk to server
//	client_sock = d3_svr_makeSocket(destination.port);
	cout << "simpleSend makeSocket end-----" << endl << endl;
#endif

	if (client_sock < 0) { //only report error, doesn't handle it
		cout << " " << endl;
		return -9;
	}

	toAddr = d3_make_sockaddr_in((destination.host).c_str(), destination.port);

	cout << "simpleSend trying to reach host:" << destination.host
			<< ", port:" << destination.port << endl;


	r = d3_send_data(client_sock, (void*) str.c_str(), str_size, 0, &toAddr);

	if (r < 0) {
		cerr << "Sending data failed." << endl;
		return -7;
	}



	current_sock = client_sock;
	//d3_closeConnection(client_sock);//not close here?
	return 0;
}





int main(int argc, char* argv[]) {

	string str = "123456789asdfghjklzxcvbnnm,.";
	HostEntity destination;
	destination.host = "localhost";
	destination.port = 7000;
	int current_sock = -1;
	simpleSend(str, destination, current_sock);
	d3_closeConnection(current_sock);

	cout << str <<endl;
	return 0;
}
