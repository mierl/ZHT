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



double getTime_usec();

double getTime_msec();

double getTime_sec();

string randomString(int len);

struct HostEntity {
	struct sockaddr_in si;
	int sock;//for used by client/server replicas only
	string host;
	int port;
	bool valid;
	vector<unsigned long long> ringID; //assume each node has multiple ringID.---problem?
};

//execute shell scripts and return results as a string
string executeShell(string str);

int myhash(const char *str, int mod);

unsigned long long hash_64bit_ring(const char *str);

int tearDownTCP(vector<struct HostEntity> memberList);

bool myCompare(struct HostEntity i, struct HostEntity j);

vector<struct HostEntity> getMembership(string fileName);

int myIndex(vector<struct HostEntity> memberList, struct HostEntity aHost);

#endif /* ZHT_UTIL_H_ */
