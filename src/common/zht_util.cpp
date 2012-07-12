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
#include <string.h>
//#include <boost/serialization/vector.hpp>
//#include <boost/serialization/string.hpp>
#include "meta.pb.h"
//#include "d3_transport.h"
#include "novoht.h" //Kevin's persistent hash table
//#include "lru_cache.h"
#include<signal.h>
# include <errno.h>
#include "../../inc/net_util.h"
#include "../../inc/zht_util.h"
//struct timeval tp;
using namespace std;

//================================ Global and constant ===============================
//int MAX_FILE_SIZE = 10000; //1GB, too big, use dynamic memory malloc.

//int const MAX_MSG_SIZE = 1024; //transferd string maximum size

//int REPLICATION_TYPE; //1 for Client-side replication

//int NUM_REPLICAS;
//====================================================================================

double getTime_usec() {

	struct timeval tp;

	gettimeofday(&tp, NULL);
	return static_cast<double>(tp.tv_sec) * 1E6
			+ static_cast<double>(tp.tv_usec);
}

double getTime_msec() {

	struct timeval tp;

	gettimeofday(&tp, NULL);
	return static_cast<double>(tp.tv_sec) * 1E3
			+ static_cast<double>(tp.tv_usec) / 1E3;
}

double getTime_sec() {

	struct timeval tp;

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

//execute shell scripts and return results as a string
string executeShell(string str) {
	char* cmd = (char*) str.c_str();
	FILE* pipe = popen(cmd, "r");
	if (!pipe)
		return "ERROR";
	char buffer[128];
	string result = "";
	while (!feof(pipe)) {
		if (fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}
	pclose(pipe);
	return result;
}

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

struct HostEntity str2Host(string str, vector<struct HostEntity> memberList,
		int &index) {
	Package pkg;
	pkg.ParseFromString(str);
	int index_inner = myhash(pkg.virtualpath().c_str(), memberList.size());
	struct HostEntity host = memberList.at(index_inner);
	index = index_inner;
	return host;
}

int str2Sock(string str, vector<struct HostEntity> &memberList) { //give socket and update the vector of network entity
	int sock = 0;
	int index = -1;
	struct HostEntity dest = str2Host(str, memberList, index);
//	cout<<"str2Sock: dest.sock = "<<dest.sock<<endl;
	if (dest.sock < 0) {
		sock = makeClientSocket((char*) dest.host.c_str(), dest.port, true);
		reuseSock(sock);
		dest.sock = sock;
		memberList.erase(memberList.begin() + index);
		memberList.insert(memberList.begin() + index, dest);

	}

//	cout<<"str2Sock: after update: sock = "<<this->str2Host(str).sock<<endl;
	return dest.sock;
}

int tearDownTCP(vector<struct HostEntity> memberList) {
	int size = memberList.size();
	for (int i = 0; i < size; i++) {
		struct HostEntity dest = memberList.at(i);
		int sock = dest.sock;
		if (sock > 0) {
			close(sock);
		}
	}

	return 0;
}

bool myCompare(struct HostEntity i, struct HostEntity j) {
	return (i.ringID.begin() < j.ringID.begin()); //potential problem: here only consider each node has one string ID.

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
		aHost.sock = -1;
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
	/*	//First implementation is based on gossip-protocol
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

	 int sock1 =  d3_makeConnection(neighbor1.host.c_str(), neighbor1.port);

	 //	int ret1 = simpleSend(msg, neighbor1, sock1);

	 int sock2 = d3_makeConnection(neighbor2.host.c_str(), neighbor2.port);
	 //	int ret2 = simpleSend(msg, neighbor2, sock2);
	 return 0;*/
}

bool areYouAlive(string hostName, int port) { //this one only detect if physical link alive
	/*	int sock = d3_makeConnection(hostName.c_str(), port);
	 if (sock > 0) {
	 d3_closeConnection(sock);
	 return true;
	 } else
	 return false;
	 */
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

