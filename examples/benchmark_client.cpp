//#include <kchashdb.h>
//#include <kcprotodb.h>
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>

#include <algorithm>
//#include "d3_transport.h"
//#include "meta.pb.h"
#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <vector>
#include <netdb.h>
//#include "zht_util.h" //zhouxb
//raman-client-replication-s
//#include "d3_sys_globals.h"
#include <pthread.h>
#include <error.h>
#include "lru_cache.h"

#include "cpp_zhtclient.h" //zhouxb
using namespace std;

//int NUM_REPLICAS = 0;
struct thread_data {
	vector<struct HostEntity> hostList;
	list<string> packageList;
};
vector<struct HostEntity> hostList;
list<string> myPackagelist;
bool TCP;
int UDP_SOCKET = -1;
int CACHE_SIZE = 1024;
LRUCache<string, int> connectionCache(CACHE_SIZE); //initialized here.

void sig_pipe(int signum) {
	printf("SIGPIPE Caught!\n");
	signal(SIGPIPE, sig_pipe);
}

/*

 //zhouxb

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
 */

int benchmarkInsert(string cfgFile, string memberList, vector<string> &pkgList,
		ZHTClient &clientRet, int numTest, int lenString) {

//	ZHTClient client;

//	if (clientRet.initialize(cfgFile, memberList) != 0) { //zhouxb
	if (clientRet.initialize(cfgFile, memberList, TCP) != 0) {
		cout << "Crap! ZHTClient initialization failed, program exits." << endl;
		return -1;
	}

//	clientRet = client; //reserve this client object for other benchmark(lookup/remove) to use.

//vector<string> pkgList;
//	int i = 0; //zhouxb
	for (int i = 0; i < numTest; i++) {
		Package package, package_ret;
		package.set_virtualpath(randomString(lenString)); //as key
		package.set_isdir(true);
		package.set_replicano(5); //orginal--Note: never let it be nagative!!!
		package.set_operation(3); // 3 for insert, 1 for look up, 2 for remove
		package.set_realfullpath(
				"Some-Real-longer-longer-and-longer-Paths--------");
		package.add_listitem("item-----2");
		package.add_listitem("item-----3");
		package.add_listitem("item-----4");
		package.add_listitem("item-----5");
		package.add_listitem("item-----6");
		string str = package.SerializeAsString();
//		cout << "package size = " << str.size() << endl;
//		cout<<"Client.cpp:benchmarkInsert: "<<endl;
//		cout<<"string: "<<str<<endl;
//		cout<<"Insert str: "<<str.c_str()<<endl;
//		cout<<"data(): "<< str.data()<<endl;

		pkgList.push_back(str);
	}

	double start = 0;
	double end = 0;
	start = getTime_msec();
	int errCount = 0;
	vector<string>::iterator it;
	int c = 0;
//	cout << "-----2" << endl;

//	string sampleString  = *(pkgList.begin());
//	struct HostEntity aHost = client.str2Host(sampleString);
	/*
	 int sock = makeClientSocket("localhost", 50000, 1);
	 cout<<"client sock = "<< sock<<endl;
	 reuseSock(sock);
	 */

	for (it = pkgList.begin(); it != pkgList.end(); it++) {

//		cout <<"insert count "<< c << endl;

		c++;

		string str_ins = *it;
//cout << "-----1" << endl;
		int ret = clientRet.insert(str_ins);
//cout << "-----2" << endl;
		if (ret < 0) {

			errCount++;
		}

	}
//close(sock);
	end = getTime_msec();

	cout << "Inserted " << numTest - errCount << " packages out of " << numTest
			<< ", cost " << end - start << " ms" << endl;

	return 0;

}

int benmarkTimeAnalize(string cfgFile, string memberList,
		vector<string> &pkgList, ZHTClient &clientRet, int numTest,
		int lenString, string Recordpath) {
	ZHTClient client;

//	if (client.initialize(cfgFile, memberList) != 0) {
	if (client.initialize(cfgFile, memberList, TCP) != 0) {
		cout << "Crap! ZHTClient initialization failed, program exits." << endl;
		return -1;
	}

	double timeRecord[numTest]; //={0};
//	bzero(timeRecord, sizeof(timeRecord));
	clientRet = client; //reserve this client object for other benchmark(lookup/remove) to use.

	//vector<string> pkgList;
	int i = 0;
	for (i = 0; i < numTest; i++) {
		Package package, package_ret;
		package.set_virtualpath(randomString(lenString)); //as key
		package.set_isdir(true);
		package.set_replicano(5); //orginal--Note: never let it be nagative!!!
		package.set_operation(3); // 3 for insert, 1 for look up, 2 for remove
		package.set_realfullpath(
				"Some-Real-longer-longer-and-longer-Paths--------");
		package.add_listitem("item-----1");
		package.add_listitem("item-----2");
		package.add_listitem("item-----3");
		package.add_listitem("item-----4");
		package.add_listitem("item-----5");
		string str = package.SerializeAsString();
//		cout << "package size = " << str.size() << endl;
//		cout<<"Client.cpp:benchmarkInsert: "<<endl;
//		cout<<"string: "<<str<<endl;
//		cout<<"Insert str: "<<str.c_str()<<endl;
//		cout<<"data(): "<< str.data()<<endl;

		pkgList.push_back(str);
	}

	double start = 0;
	double end = 0;
	double istart = 0;
	double iend = 0;
	int errCount = 0;
	vector<string>::iterator it;
	int c = 0;

	ofstream record;
	record.open(Recordpath.c_str());

	start = getTime_msec();
	for (it = pkgList.begin(); it != pkgList.end(); it++) {
//		cout<<c<<endl;
		c++;
		double interval = 0;
		istart = getTime_usec();
		int op_ret = client.insert((*it));
		iend = getTime_usec();

		if (op_ret < 0) {
			errCount++;
			interval = -1;
		} else
			interval = iend - istart;
		record << interval << endl;
		timeRecord[c] = interval;

	}
	end = getTime_msec();
	record.close();

	cout << "Inserted " << numTest - errCount << " packages out of " << numTest
			<< ", cost " << end - start << " ms" << endl;

	return 0;
}

float benchmarkLookup(vector<string> strList, ZHTClient &client) {
	vector<string>::iterator it;

	/*
	 for (it = strList.begin(); it != strList.end(); it++) {
	 Package package;
	 package.ParseFromString((*it));
	 package.set_operation(1); // 3 for insert, 1 for look up, 2 for remove
	 package.set_replicano(3); //5: original, 3 not original

	 strList.erase(it);
	 string newStr = package.SerializeAsString();
	 strList.push_back(newStr);
	 }
	 */
	double start = 0;
	double end = 0;
	start = getTime_msec();
	int errCount = 0;
//cout << "Client: benchmarkLookup: start lookup \n";
	int c = 0;
	for (it = strList.begin(); it != strList.end(); it++) {
		string result;
//		cout <<"insert count "<< c << endl;
		c++;

//		cout<<"lookup: "<<i<<endl;
//		cout << "Client: What I want to find: \n";
//		cout <<"Lookup: "<< (*it).c_str() << endl;
		if (client.lookup((*it), result) < 0) {

			errCount++;
		} else if (result.empty()) { //empty string
			errCount++;
		}
//		cout << "Client: What I get: ";
//		cout << result.c_str() << endl;
	}

	end = getTime_msec();

	cout << "Lookup " << strList.size() - errCount << " packages out of "
			<< strList.size() << ", cost " << end - start << " ms" << endl;
	return 0;
}

float benchmarkRemove(vector<string> strList, ZHTClient &client) {
	vector<string>::iterator it;

	for (it = strList.begin(); it != strList.end(); it++) {
		Package package;
		package.ParseFromString((*it));
		package.set_operation(2); // 3 for insert, 1 for look up, 2 for remove
		package.set_replicano(5); //5: original, 3 not original

		strList.erase(it);
		string newStr = package.SerializeAsString();
		strList.push_back(newStr);
	}

	double start = 0;
	double end = 0;
	start = getTime_msec();
	int errCount = 0;
	int c = 0;
	for (it = strList.begin(); it != strList.end(); it++) {
		string result;
		c++;
//		cout <<"Remove count "<< c << endl;

//		cout <<"Remove: "<< (*it).c_str() << endl;
		if (client.remove((*it)) < 0) {
			errCount++;
		}

	}

	end = getTime_msec();

	cout << "Remove " << strList.size() - errCount << " packages out of "
			<< strList.size() << ", cost " << end - start << " ms" << endl;
	return 0;

	return 0;
}
//This is an example.

int benchmarkALL(int numTest, int strLen) { //103+length
//	int para = strLen - 128;
	return 0;
}
int main(int argc, char* argv[]) {
//	cout << "Usage: ./client <num_operations> <memberList> <configFile>"<< endl;
	/*//	For BG/P
	 const string cmd = "cat /proc/personality.sh | grep BG_PSETORG";
	 string torusID = executeShell(cmd);
	 torusID.resize(torusID.size() - 1);
	 int pid = getpid();
	 unsigned int v = myhash(torusID.c_str(), 1000000) + pid;
	 //cout<<"client: pid = "<<pid<<endl;

	 //cout<<"Client random n = "<<v<<endl;
	 srand(v);
	 */
	char* isTCP = argv[4];
//	cout<<"Protocol = "<<isTCP<<endl;
	if (!strcmp("TCP", isTCP)) {
		TCP = true;
	} else {
		TCP = false;
	}

	int numOper = atoi(argv[1]);
//cout<<"numOper = "<<numOper<<endl;	
	string cfgFile(argv[3]);
//cout<<"cfgFile = "<<cfgFile<<endl;
	string memberList(argv[2]);
//cout<<"memberList: "<<memberList<<endl;
	vector<string> pkgList;
	ZHTClient testClient;
//	int pid = getpid();
	char* tmpStr;
	stringstream ss; //create a stringstream
//	ss << pid;

//	string recordFile = "record." + ss.str();
//	benmarkTimeAnalize(cfgFile, memberList, pkgList, testClient, numOper, 15, recordFile);
//cout<<"start to insert..."<<endl;
	benchmarkInsert(cfgFile, memberList, pkgList, testClient, numOper, 15); //25fro 128bytes.
//cout << "Client:main, start lookup \n";
	benchmarkLookup(pkgList, testClient);
	benchmarkRemove(pkgList, testClient);
//	testClient.tearDownTCP(TCP);
	testClient.tearDownTCP(); //zhouxb
	return 0;

}

