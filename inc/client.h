#ifndef PHASHMAP_H
#define PHASHMAP_H
#include "client.h"
#include <string>
#include <stdio.h>
using namespace std;

struct thread_data {
	vector<struct HostEntity> hostList;
	list<string> packageList;
};

//int NUM_REPLICAS = 0;
vector<struct HostEntity> hostList;
list<string> myPackagelist;
bool TCP;
int UDP_SOCKET=-1;
int CACHE_SIZE = 1024;
LRUCache <string, int> connectionCache(CACHE_SIZE);//initialized here.

void sig_pipe(int signum) {
	printf("SIGPIPE Caught!\n");
	signal(SIGPIPE, sig_pipe);
}

class ZHTClient {
public:
	int REPLICATION_TYPE; //serverside or client side. -1:error
	int NUM_REPLICAS; //-1:error
	int protocolType; //1:1TCP; 2:UDP; 3.... Reserved.  -1:error
	vector<struct HostEntity> memberList;
	ZHTClient();

	int initialize(string configFilePath, string memberListFilePath);
	struct HostEntity str2Host(string str);
	struct HostEntity str2Host(string str, int &index);
	int str2Sock(string str);
	int str2SockLRU(string str, bool tcp);
	int insert(string str); //following functions should only know string, address where to can be calculated.
	int insert(string str, int sock); // only for test
	int lookup(string str, string &returnStr);
	int lookup(string str, string &returnStr, int sock); // only for test
	int remove(string str);
	int tearDownTCP(bool tcp); //only for TCP

};
