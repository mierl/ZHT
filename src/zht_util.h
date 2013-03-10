/*
 * zht_util.h
 *
 *  Created on: Nov 26, 2011
 *      Author: tony
 */

#ifndef ZHT_UTIL_H_
#define ZHT_UTIL_H_

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
#include "meta.pb.h"

#include "novoht.h"

#include<signal.h>
# include <errno.h>
#include "net_util.h"

using namespace std;

class Env {
public:
	Env();
	virtual ~Env();

	static int setconfigvariables(string cfgFile);

public:
	static const int MAX_MSG_SIZE; //max size of a message in each transfer

	static const uint BUF_SIZE; //size of blob transfered from client to server each time

	static const int TOTAL_MSG_SIZE; //total size of a message transfered

	static int REPLICATION_TYPE; //1 for Client-side replication

	static int NUM_REPLICAS;
};

double getTime_usec();

double getTime_msec();

double getTime_sec();

string randomString(int len);

struct HostEntity {
	struct sockaddr_in si;
	int sock; //for used by client/server replicas only
	string host;
	int port;
	bool valid;
	vector<unsigned long long> ringID; //assume each node has multiple ringID.---problem?
};

//execute shell scripts and return results as a string
string executeShell(string str);

int myhash(const char *str, int mod);

uint64_t genHash(const char *pc);

unsigned long long hash_64bit_ring(const char *str);

int tearDownTCP(vector<struct HostEntity> memberList);

bool myCompare(struct HostEntity i, struct HostEntity j);

vector<struct HostEntity> getMembership(string fileName);

HostEntity getHostEntity(const string& host, const int& port);

int myIndex(vector<struct HostEntity> memberList, struct HostEntity aHost);

class IdHelper {
public:
	IdHelper();
	virtual ~IdHelper();

	static uint64_t genId();

public:
	static const uint ID_LEN;
};

#endif /* ZHT_UTIL_H_ */
