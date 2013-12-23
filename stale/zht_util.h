/*
 * zht_util.h
 *
 *  Created on: Nov 26, 2011
 *      Author: tony
 */

#include <string>
#include <cstring>
#include <vector>
#include <sys/time.h>
#include <iostream>
#include <arpa/inet.h>
#include <algorithm>
#include <fstream>
#include <netdb.h>
#include <stdlib.h>
//#include <boost/serialization/vector.hpp>
//#include <boost/serialization/string.hpp>
#include "meta.pb.h"
#include "d3_transport.h"
#include "novoht.h" //Kevin's persistent hash table
#include<signal.h>
# include <errno.h>
struct timeval tp;
using namespace std;

//================================ Global and constant ===============================
int MAX_FILE_SIZE = 10000; //1GB, too big, use dynamic memory malloc.

int const MAX_MSG_SIZE = 1024; //transferd string maximum size

int REPLICATION_TYPE; //1 for Client-side replication

int NUM_REPLICAS;
//====================================================================================

double getTime_usec() {
	gettimeofday(&tp, NULL);
	return static_cast<double>(tp.tv_sec) * 1E6
			+ static_cast<double>(tp.tv_usec);
}

double getTime_msec() {
	gettimeofday(&tp, NULL);
	return static_cast<double>(tp.tv_sec) * 1E3
			+ static_cast<double>(tp.tv_usec) / 1E3;
}

double getTime_sec() {
	gettimeofday(&tp, NULL);
	return static_cast<double>(tp.tv_sec)
			+ static_cast<double>(tp.tv_usec) / 1E6;
}

string randomString(int len) {
	string s(len, ' ');

	static const char alphanum[] = "0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	return s;
}
struct HostEntity {
	struct sockaddr_in si;
	int sock;
	string host;
	int port;
	bool valid;
	vector<unsigned long long> ringID; //assume each node has multiple ringID.---problem?
};

int myhash(const char *str, int mod) { //int type return
	unsigned long hash = 0;
	int c;

	while (c = *str++) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}
	return hash % mod;
}

unsigned long long hash_64bit_ring(const char *str) { //unsigned long long: 64 bit
	unsigned long long hash = 0;
	unsigned long long c;

	while (c = *str++) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}
	return hash;
}

bool myCompare(struct HostEntity i, struct HostEntity j) {
	return (i.ringID.begin() < j.ringID.begin()); //potential problem: here only consider each node has one string ID.

}

int setconfigvariables(string cfgFile) {
	FILE *fp;
	char line[100], *key, *svalue;
	int ivalue;

	fp = fopen(cfgFile.data(), "r");
	if (fp == NULL) {
		cout << "Error opening the file." << endl;
		return -1;
	}
	while (fgets(line, 100, fp) != NULL) {
		key = strtok(line, "=");
		svalue = strtok(NULL, "=");
		ivalue = atoi(svalue);

		if ((strcmp(key, "REPLICATION_TYPE")) == 0) {
			REPLICATION_TYPE = ivalue;
			//cout<<"REPLICATION_TYPE = "<< REPLICATION_TYPE <<endl;
		} //other config options follow this way(if).

		if ((strcmp(key, "NUM_REPLICAS")) == 0) {
			NUM_REPLICAS = ivalue;
			//cout<<"NUM_REPLICAS = "<< NUM_REPLICAS <<endl;
		}

	}
	return 0;
}

vector<struct HostEntity> getMembership(string fileName) {
	vector<struct HostEntity> hostList;
	ifstream in(fileName.c_str(), ios::in);
	string host;
	int port;
	HostEntity aHost;
	struct sockaddr_in si_other;
	hostent *record;
	in_addr *address;
	string ip_address;
	if (!in.is_open()) {
		cout << "Membership File read failed." << endl;
		return hostList;
	}
	if (!in.eof()) {
		in >> host >> port;
	}
	record = gethostbyname(host.c_str());
	address = (in_addr *) record->h_addr;
	ip_address = inet_ntoa(*address);
	while (!in.eof()) {
//		int s, i, slen = sizeof(si_other);
		memset((char *) &si_other, 0, sizeof(si_other));
		si_other.sin_family = AF_INET;
		si_other.sin_port = htons(port);
		if (inet_aton(ip_address.c_str(), &si_other.sin_addr) == 0) {
			fprintf(stderr, "inet_aton() failed\n");
		}
		aHost.si = si_other;
		aHost.host = host;
		aHost.port = port;
		aHost.valid = true;
		hostList.push_back(aHost);
		in >> host >> port;
		record = gethostbyname(host.c_str());
		address = (in_addr *) record->h_addr;
		ip_address = inet_ntoa(*address);
	}
	in.close();
	cout << "finished reading membership info, " << hostList.size() << " nodes"
			<< endl;
	return hostList;
}

//string str
int simpleSend(string str, struct HostEntity destination, int &current_sock) {

	//cout<< "contactReplica: newly received msg: package.replicano= " << package.replicano()<< ", package.ByteSize()="<< package.ByteSize() <<endl;//here the correct package was received: -1

	int client_sock, sent_size = 0;
	int32_t str_size = str.length();
//	int32_t str_size = len;//strlen(str);
	string hostName;
	sockaddr_in toAddr;
	int ret = 0;
#if TRANS_PROTOCOL == USE_TCP

	//set reuse socket

	client_sock = d3_makeConnection((destination.host).c_str(),
			destination.port);
	int optval=1;
	if(setsockopt(client_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval)<0) cerr<<"simpleSend: reuse failed."<<endl;
#elif TRANS_PROTOCOL == USE_UDP

//	cout << endl << "simpleSend makeSocket start-----" << endl;
//	client_sock = d3_svr_makeSocket((time(NULL) % 10000) + rand() % 10000); //old, with consideration on multiple port conflicts.

	client_sock = d3_makeConnection(); //possible right one.

//	cout << "simpleSend makeSocket end-----" << endl << endl;
#endif

	if (client_sock < 0) { //only report error, doesn't handle it
		cerr << "zht_util.h: make connection failed: " << strerror(errno)<<endl;
		return -9;
	}

	toAddr = d3_make_sockaddr_in_client((destination.host).c_str(),
			destination.port);

//	cout << "simpleSend trying to reach host:" << destination.host << ", port:" << destination.port << endl;
//r = d3_send_data(client_sock, (void*) str.c_str(), str_size, 0, &toAddr);
//	cout<<"simpleSend:string:"<<endl;
//			cout<<"string: "<<str<<endl;
//			cout<<"c_str(): "<<str.c_str()<<endl;
//			cout<<"(void*)c_str(): "<< (void*)str.c_str()<<endl;
//			printf("printf %s \n\n\n",str.c_str());
	sent_size = d3_send_data(client_sock, (void*) str.data(), str_size, 0, &toAddr);
//	cout << "simpleSend: sent " << r << " bytes:" << str << endl;
	if (sent_size < 0) {
		cerr << "Sending data failed." << endl;
		return -7;
	}
	if (TRANS_PROTOCOL == USE_TCP) {
		void *buff_return = malloc(sizeof(int32_t));//int32_t
		int r = d3_recv_data(client_sock, buff_return, sizeof(int32_t), 0);
		//this is the return status, not search result.
		if (r < 0) {
			cerr << "zht_util.h: Receiving return state failed." << endl;
			return -7;
		}
		ret = *(int32_t*) buff_return;
		free(buff_return);
		switch (ret) {
		case 0:
			break;
		case 1:
			break;
		case -2:
			cerr << "zht_util.h: Failed to remove." << endl;
			break;
		case -3:
			cerr << "zht_util.h: Failed to insert." << endl;
			break;
		case -99:
			cerr <<"zht_util.h: such operation is not supported." <<endl;
			break;
		default:
//			cerr << "zht_util.h: default:  What the hell was that? ret = " << ret << endl;
			break;
		}
	} else if (TRANS_PROTOCOL == USE_UDP) {
		//ret = sent_size;
	}
	current_sock = client_sock;
	//d3_closeConnection(client_sock);//not close here?
	return sent_size;
}

int myIndex(vector<struct HostEntity> memberList, struct HostEntity aHost) { //give host position on the list, not consider ringID

	//int pos = std::find(memberList.begin(), memberList.end(), host) - memberList.begin();
	//return pos;
	int i = 0;
	for (i = 0; i < memberList.size(); i++) {
		if (memberList.at(i).host == aHost.host
				&& memberList.at(i).port == aHost.port)
			return i;
	}
	return -1; //find no match
}

int broadcast_ST(vector<struct HostEntity> memberList, struct HostEntity me,
		string msg) {
	//First implementation is based on gossip-protocol
	//how to serialize multiple different objects?
	//Notice that the first element has a position of 0, not 1.
	//Problem: the local member list could be out of date, so some of the nodes on list may not exist::handle it later.
	//Problem: the order of member list could be not certain.
	//--Member list is determined by list file, so this file itself must be deterministic.
	//--It shoud be sorted in some way: do this later: assume it is in order already.
	//n:2n+1, 2n+2. Find my position in the list.

	int self = myIndex(memberList, me);
	HostEntity neighbor1, neighbor2;

	neighbor1 = memberList.at(2 * self + 1);
	neighbor2 = memberList.at(2 * self + 2);

	int sock1 = d3_makeConnection(neighbor1.host.c_str(), neighbor1.port);
//	int ret1 = simpleSend(msg, neighbor1, sock1);

	int sock2 = d3_makeConnection(neighbor2.host.c_str(), neighbor2.port);
//	int ret2 = simpleSend(msg, neighbor2, sock2);
	return 0;
}

bool areYouAlive(string hostName, int port) { //this one only detect if physical link alive
	int sock = d3_makeConnection(hostName.c_str(), port);
	if (sock > 0) {
		d3_closeConnection(sock);
		return true;
	} else
		return false;
}
/*
 class MsgSys {
 private:
 friend class boost::serialization::access;
 template<class Archive>
 void serialize(Archive & ar, const unsigned int version) {
 ar & msgType;
 ar & memberList;
 ar & mics;
 }
 int msgType; //
 vector<struct HostEntity> memberList;
 string mics;
 };
 */
int sendFile(string readPath, struct HostEntity toHost) {
	return 0;
}

int receiveFile(string storePath, int fromSocket) {
	return 0;
}

