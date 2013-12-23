/*
 * serverConsole.cpp
 *
 *  Created on: Mar 31, 2011
 *      Author: tony
 */

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include "d3_transport.h"
#include <list>
using namespace std;

struct HostEntity {
	struct sockaddr_in si;
	int sock;
	string host;
	int port;
	bool valid;
};

list<struct HostEntity> getMembership(string fileName) {

	list<struct HostEntity> hostList;
	ifstream in(fileName.c_str(), ios::in);//make a file input stream
	string host;
	int port;

	if (!in.is_open()) {
		cout << "File read failed." << endl;
		return hostList;
	}

	if (!in.eof()) {
		in >> host >> port;
	}

	while (!in.eof()) {
		HostEntity aHost;

		struct sockaddr_in si_other;

		int s, i, slen = sizeof(si_other);

		memset((char *) &si_other, 0, sizeof(si_other));
		si_other.sin_family = AF_INET;
		si_other.sin_port = htons(port);
		if (inet_aton(host.c_str(), &si_other.sin_addr) == 0) {
			fprintf(stderr, "inet_aton() failed\n"); //address to number
			//exit(1);
		}

		aHost.si = si_other;

		aHost.host = host;
		aHost.port = port;
		aHost.valid = true;
		hostList.push_back(aHost);
		/*
		 if (isCurrentNode(host)) {
		 setId(getNumberOfNodes() - 1);
		 }
		 */
		in >> host >> port;

	}
	in.close();

	cout << "finished reading membership info, " << hostList.size() << " nodes"
			<< endl;
	return hostList;

}



int main(int argc, char* argv[]){
	string fileName(argv[1]);
	list<struct HostEntity> hostList = getMembership(fileName);
	list<struct HostEntity>::iterator it;
	int numHost = hostList.size();
	int32_t sig = -9;//atoi(argv[2]);

	int port;
	struct HostEntity destHost;
	string hostName;
	string str;
	int32_t str_size;
	int client_sock;
	int i=1;
	void* buff1 = (void*) malloc(sizeof(int32_t));
	int ret1;
	for(it = hostList.begin(); it != hostList.end(); it++){



					cout << "|" << 0 << "|";
				//struct HostEntity destHost = sendDestList.at(i);
				destHost = *it;

					cout << "# " << i << "port: " << destHost.port;
				client_sock = d3_makeConnection((destHost.host).c_str(), destHost.port);

					cout << "|" << 1 << "|";

				memset(buff1, 0, sizeof(int32_t));//init of size

				//string str = *it;


				buff1 = &sig;

				ret1 = d3_send_data(client_sock, buff1, sizeof(int32_t), 0);//send the sig
				if (ret1 < 0) {
					cout << "Send command errer: return " << ret1 << endl;

					continue;
				}

					cout << "|" << 2 << "|"<<endl;

					i++;

	}

}
