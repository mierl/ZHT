#ifndef CPP_ZHTCLIENT_H_
#define CPP_ZHTCLIENT_H_

#include "zht_util.h"



class ZHTClient {
public:

	bool TCP;
	int REPLICATION_TYPE; //serverside or client side. -1:error
	int NUM_REPLICAS; //-1:error
	int protocolType; //1:1TCP; 2:UDP; 3.... Reserved.  -1:error
	vector<struct HostEntity> memberList;
	ZHTClient();

	int initialize(string configFilePath, string memberListFilePath, bool tcp);
	struct HostEntity str2Host(string str);
	struct HostEntity str2Host(string str, int &index);
	int str2Sock(string str);
	int str2SockLRU(string str, bool tcp);
	int insert(string str); //following functions should only know string, address where to can be calculated.
	int insert(string str, int sock); // only for test
	int lookup(string str, string &returnStr);
	int lookup(string str, string &returnStr, int sock); // only for test
	int remove(string str);
	int tearDownTCP(); //only for TCP

};

#endif
