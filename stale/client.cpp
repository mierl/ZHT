//#include <kchashdb.h>
//#include <kcprotodb.h>
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>

#include <algorithm>
//#include "d3_transport.h"
#include "meta.pb.h"
#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <vector>
#include <netdb.h>
#include "zht_util.h"
//raman-client-replication-s
//#include "d3_sys_globals.h"
#include <pthread.h>
#include <error.h>
using namespace std;



//int NUM_REPLICAS = 0;
struct thread_data {
	vector<struct HostEntity> hostList;
	list<string> packageList;
};
vector<struct HostEntity> hostList;
list<string> myPackagelist;


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
	int insert(string str); //following functions should only know string, address where to can be calculated.
	int lookup(string str, string &returnStr);
	int remove(string str);

};

ZHTClient::ZHTClient() { // default all invalid value, so the client must be initialized to set the variables.
	//Since constructor can't return anything, we must have an initialization function that can return possible error message.
	this->NUM_REPLICAS = -1;
	this->REPLICATION_TYPE = -1;
	this->protocolType = -1;
}

int ZHTClient::initialize(string configFilePath, string memberListFilePath) {

	//read cfg file
	this->memberList = getMembership(memberListFilePath);

	FILE *fp;
	char line[100], *key, *svalue;
	int ivalue;

	fp = fopen(configFilePath.c_str(), "r");
	if (fp == NULL) {
		cout << "Error opening the config file." << endl;
		return -1;
	}
	while (fgets(line, 100, fp) != NULL) {
		key = strtok(line, "=");
		svalue = strtok(NULL, "=");
		ivalue = atoi(svalue);

		if ((strcmp(key, "REPLICATION_TYPE")) == 0) {
			this->REPLICATION_TYPE = ivalue;
			//cout<<"REPLICATION_TYPE = "<< REPLICATION_TYPE <<endl;
		} //other config options follow this way(if).

		else if ((strcmp(key, "NUM_REPLICAS")) == 0) {
			this->NUM_REPLICAS = ivalue + 1; //note: +1 is must
			//cout<<"NUM_REPLICAS = "<< NUM_REPLICAS <<endl;
		} else {
			cout << "Config file is not correct." << endl;
			return -2;
		}

	}
	return 0;

}

//transfer a key to a host index where it should go
struct HostEntity ZHTClient::str2Host(string str) {
	Package pkg;
	pkg.ParseFromString(str);
	int index = myhash(pkg.virtualpath().c_str(), this->memberList.size());
	struct HostEntity host = this->memberList.at(index);

	return host;
}

//send a plain string to destination, receive return state.
int ZHTClient::insert(string str) {
	int sock = -1;
	struct HostEntity dest = this->str2Host(str);
	int ret = simpleSend(str, dest, sock);
	d3_closeConnection(sock);
	return ret;
}

int ZHTClient::lookup(string str, string &returnStr) {

	Package package;
			package.ParseFromString(str);
			package.set_operation(1); // 3 for insert, 1 for look up, 2 for remove
			package.set_replicano(3); //5: original, 3 not original


			str = package.SerializeAsString();

	int sock = -1;
	struct HostEntity dest = this->str2Host(str);
//	cout << "client::lookup is called, now send request..." << endl;

	Package pack;
	pack.ParseFromString(str);
//	cout<<"ZHTClient::lookup: operation = "<<pack.operation()<<endl;

	int ret = simpleSend(str, dest, sock);
//	cout << "ZHTClient::lookup: simpleSend return = " << ret << endl;
	char buff[MAX_MSG_SIZE]; //MAX_MSG_SIZE
	memset(buff, 0, sizeof(buff));
	if (ret == str.length()) { //this only work for TCP. UDP need to make a new one so accept returns from server.
//		cout << "before protocol judge" << endl;

		int rcv_size = -1;
		if (TRANS_PROTOCOL == USE_TCP) {

			rcv_size = d3_recv_data(sock, buff, MAX_MSG_SIZE, 0); //MAX_MSG_SIZE

		} else if (TRANS_PROTOCOL == USE_UDP) {
			//int svrPort = dest.port;//surely wrong, it's just the 50000 port
			//int svrPort = 50001;
			srand(getpid() + clock());
//			int z = rand() % 10000 + 10000;
//			cout<<"Client:lookup: random port: "<<z<<endl;
//			int server_sock = d3_svr_makeSocket(rand() % 10000 + 10000);// use random port to send, and receive lookup result from it too.
//			cout<<"Client:lookup: UDP socket: "<<server_sock<<endl;
			sockaddr_in tmp_sockaddr;

			memset(&tmp_sockaddr, 0, sizeof(sockaddr_in));
//			cout << "lookup: before receive..." << endl;
			rcv_size = d3_svr_recv(sock, buff, MAX_MSG_SIZE, 0,
					&tmp_sockaddr);//receive lookup result
			//d3_closeConnection(server_sock);
//			cout << "lookup received " << rcv_size << " bytes." << endl;

		}
		if (rcv_size < 0) {
			cout << "Lookup receive error." << endl;
			return rcv_size;
		} else {
			returnStr.assign(buff);
		}

//		cout << "after protocol judge" << endl;
	}
	d3_closeConnection(sock);

	return ret;
}

int ZHTClient::remove(string str) {
	int sock = -1;

	Package package;
	package.ParseFromString(str);
	package.set_operation(2); // 3 for insert, 1 for look up, 2 for remove
	package.set_replicano(3); //5: original, 3 not original
	str = package.SerializeAsString();

	struct HostEntity dest = this->str2Host(str);
	int ret = simpleSend(str, dest, sock);
	d3_closeConnection(sock);
	return ret;
}

int benchmarkInsert(string cfgFile, string memberList, vector<string> &pkgList,
		ZHTClient &clientRet, int numTest, int lenString) {

	ZHTClient client;

	if (client.initialize(cfgFile, memberList) != 0) {
		cout << "Crap! ZHTClient initialization failed, program exits." << endl;
		return -1;
	}

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
	start = getTime_msec();
	int errCount = 0;
	vector<string>::iterator it;
	int c=0;
	for (it = pkgList.begin(); it != pkgList.end(); it++) {
//		cout<<c<<endl;
		c++;
		if (client.insert((*it)) < 0)
			errCount++;
	}
	end = getTime_msec();

	cout << "Inserted " << numTest - errCount << " packages out of " << numTest
			<< ", cost " << end - start << " ms" << endl;

	return 0;
}

int benmarkTimeAnalize(string cfgFile, string memberList, vector<string> &pkgList,
		ZHTClient &clientRet, int numTest, int lenString, string Recordpath){
	ZHTClient client;

	if (client.initialize(cfgFile, memberList) != 0) {
		cout << "Crap! ZHTClient initialization failed, program exits." << endl;
		return -1;
	}

	double timeRecord[numTest];//={0};
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
	int c=0;

	ofstream record;
	record.open(Recordpath.c_str());

	start = getTime_msec();
	for (it = pkgList.begin(); it != pkgList.end(); it++) {
//		cout<<c<<endl;
		c++;
		double interval=0;
		istart = getTime_usec();
		int op_ret = client.insert((*it));
		iend = getTime_usec();

		if (op_ret < 0){
			errCount++;
			interval = -1;
		}else
			interval = iend - istart;
		record << interval <<endl;
		timeRecord[c] = interval;

	}
	end = getTime_msec();
	record.close();

	cout << "Inserted " << numTest - errCount << " packages out of " << numTest
			<< ", cost " << end - start << " ms" << endl;

	return 0;
}

float benchmarkLookup(vector<string> strList, ZHTClient client) {
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

	for (it = strList.begin(); it != strList.end(); it++) {
		string result;
//		cout << "Client: What I want to find: \n";
//		cout <<"Lookup: "<< (*it).c_str() << endl;
		if (client.lookup((*it), result) < 0) {

			errCount++;
		} else if (result.empty()) { //empty string
			errCount++;
		}
//		cout << "Client: What I get: \n";
//		cout << result.c_str() << endl;
	}

	end = getTime_msec();

	cout << "Lookup " << strList.size() - errCount << " packages out of "
			<< strList.size() << ", cost " << end - start << " ms" << endl;
	return 0;
}

float benchmarkRemove(vector<string> strList, ZHTClient client) {
	vector<string>::iterator it;

	for (it = strList.begin(); it != strList.end(); it++) {
		Package package;
		package.ParseFromString((*it));
		package.set_operation(2); // 3 for insert, 1 for look up, 2 for remove
		package.set_replicano(3); //5: original, 3 not original

		strList.erase(it);
		string newStr = package.SerializeAsString();
		strList.push_back(newStr);
	}

	double start = 0;
	double end = 0;
	start = getTime_msec();
	int errCount = 0;

	for (it = strList.begin(); it != strList.end(); it++) {
		string result;
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
	cout<<"Usage: ./client <num_operations> <memberList> <configFile>"<<endl;


	srand(getpid() + clock() + getTime_usec());
	int numOper = atoi(argv[1]);
	string cfgFile(argv[3]);
	string memberList(argv[2]);
	vector<string> pkgList;
	ZHTClient testClient;
	int pid = getpid();
	char* tmpStr;
	stringstream ss;//create a stringstream
	ss << pid;

	string recordFile = "record." + ss.str();
	benmarkTimeAnalize(cfgFile, memberList, pkgList, testClient, numOper, 15, recordFile);
//	benchmarkInsert(cfgFile, memberList, pkgList, testClient, numOper, 15); //25fro 128bytes.
//	benchmarkLookup(pkgList, testClient);
//	benchmarkRemove(pkgList, testClient);


}

