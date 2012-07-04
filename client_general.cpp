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
#include "zht_util.h"
//raman-client-replication-s
//#include "d3_sys_globals.h"
#include <pthread.h>
#include <error.h>
#include "lru_cache.h"

using namespace std;

//int NUM_REPLICAS = 0;
struct thread_data {
	vector<struct HostEntity> hostList;
	list<string> packageList;
};
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

struct HostEntity ZHTClient::str2Host(string str, int &index) {
	Package pkg;
	pkg.ParseFromString(str);
	int index_inner = myhash(pkg.virtualpath().c_str(),
			this->memberList.size());
	struct HostEntity host = this->memberList.at(index_inner);
	index = index_inner;
	return host;
}

//This store all connections
/*
int ZHTClient::str2Sock(string str) { //give socket and update the vector of network entity
	int sock = 0;
	int index = -1;
	struct HostEntity dest = this->str2Host(str, index);
	cout<<"str2Sock: dest.sock = "<<dest.sock<<endl;
	if (dest.sock < 0) {
		sock = makeClientSocket(dest.host.c_str(), dest.port, 1);
		reuseSock(sock);
		dest.sock = sock;
		this->memberList.erase(this->memberList.begin() + index);
		this->memberList.insert(this->memberList.begin() + index, dest);

	}

	cout<<"str2Sock: after update: sock = "<<this->str2Host(str).sock<<endl;
	return dest.sock;
}
*/
int ZHTClient::str2Sock(string str) { //give socket and update the vector of network entity
	int sock = 0;
        int index = -1;
        struct HostEntity dest = this->str2Host(str, index);
	if(TCP==true){
	//	int sock = 0;
//		int index = -1;
//		struct HostEntity dest = this->str2Host(str, index);
//		cout<<"str2Sock: dest.sock = "<<dest.sock<<endl;
		if (dest.sock < 0) {
			sock = makeClientSocket(dest.host.c_str(), dest.port, TCP);
			reuseSock(sock);
			dest.sock = sock;
			this->memberList.erase(this->memberList.begin() + index);
			this->memberList.insert(this->memberList.begin() + index, dest);
		}
//		cout<<"str2Sock: after update: sock = "<<this->str2Host(str).sock<<endl;
		return dest.sock;
	}else{//UDP
		if(UDP_SOCKET < 0){
			UDP_SOCKET = makeClientSocket(dest.host.c_str(), dest.port, TCP);
		}
		return UDP_SOCKET;
			
	}
	
}


//This store limited connections in a LRU cache, one item is one host VS one sock
int ZHTClient::str2SockLRU(string str, bool tcp) {
	struct HostEntity dest = this->str2Host(str);
	int sock=0;
	if (tcp == true) {
		sock = connectionCache.fetch(dest.host, tcp);
		if (sock <= 0) {
//			cout << "host not found in cache, making connection..." << endl;
			sock = makeClientSocket(dest.host.c_str(), dest.port, tcp);
//			cout << "created sock = " << sock << endl;
			if (sock <= 0) {
				cerr << "Client insert:making connection failed." << endl;
				return -1;
			} else {
				int tobeRemoved = -1;
				connectionCache.insert(dest.host, sock, tobeRemoved);
				if (tobeRemoved != -1) {
//					cout << "sock " << tobeRemoved	<< ", will be removed, which shouldn't be 0."<< endl;
					close(tobeRemoved);
				}
			}
		}//end if sock<0
	}else{//UDP

		if(UDP_SOCKET<=0){
			sock = makeClientSocket(dest.host.c_str(), dest.port, TCP);
			UDP_SOCKET = sock;
		}else
			sock = UDP_SOCKET;	
	}

	return sock;
}

int ZHTClient::tearDownTCP(bool tcp) {
	if (tcp == true) {
		int size = this->memberList.size();
		for (int i = 0; i < size; i++) {
			struct HostEntity dest = this->memberList.at(i);
			int sock = dest.sock;
			if (sock > 0) {
				close(sock);
			}
		}
	}
	return 0;
}

//send a plain string to destination, receive return state.
int ZHTClient::insert(string str) {

	/*	int sock = -1;

	 struct HostEntity dest = this->str2Host(str);

	 //	cout<<"Client: will send to "<<dest.host<<endl;
	 //int ret = simpleSend(str, dest, sock);

	 sock = makeClientSocket(dest.host.c_str(), dest.port, 1);
	 //	cout<<"client sock = "<< sock<<endl;
	 reuseSock(sock);
	 generalSendTCP(sock, str.c_str());
	 */

	int sock = this->str2SockLRU(str, TCP);
	reuseSock(sock);
//	cout<<"sock = "<<sock<<endl;
//	int sentSize = generalSendTCP(sock, str.c_str());
	struct HostEntity dest = this->str2Host(str);
	sockaddr_in recvAddr;
	int sentSize = generalSendTo(dest.host.data(),dest.port, sock, str.c_str(), TCP);
//	cout <<"Client inseret sent: "<<sentSize<<endl;
	int32_t* ret_buf = (int32_t*) malloc(sizeof(int32_t));

//	generalReveiveTCP(sock, (void*) ret_buf, sizeof(int32_t), 0);
	generalReceive(sock, (void*)ret_buf, 4,recvAddr,0, TCP);
	int ret = *(int32_t*) ret_buf;

	if (ret < 0) {
//		cerr << "zht_util.h: Failed to insert." << endl;
//		cout << "Insert: ret = "<< *ret <<endl;
	}
//cout <<"Returned status: "<< *(int32_t*) ret<<endl;
//	d3_closeConnection(sock);
	free(ret_buf);

	//return ret_1;
//	cout<<"insert got: "<< *ret <<endl;
	return ret;
}
/*
 int ZHTClient::insert(string str, int sock) {


 //cout << "in insert-----1" << endl;
 int r_s = generalSendTCP(sock, str.c_str());
 //	cout << "in insert-----2, ret_send = " << r_s << endl;
 int32_t* ret = (int32_t*) malloc(sizeof(int32_t));

 generalReveiveTCP(sock, (void*) ret, sizeof(int32_t), 0);
 //	cout << "in insert-----3" << endl;
 int ret_1 = *(int32_t*) ret;
 //cout <<"Returned status: "<< *(int32_t*) ret<<endl;
 //	d3_closeConnection(sock);
 free(ret);
 //	cout << "in insert-----4" << endl;
 return ret_1;
 //return 0;
 }
 */

int ZHTClient::lookup(string str, string &returnStr) {

	Package package;
	package.ParseFromString(str);
	package.set_operation(1); // 3 for insert, 1 for look up, 2 for remove
	package.set_replicano(3); //5: original, 3 not original

	str = package.SerializeAsString();

	struct HostEntity dest = this->str2Host(str);
//	cout << "client::lookup is called, now send request..." << endl;

	Package pack;
	pack.ParseFromString(str);
//	cout<<"ZHTClient::lookup: operation = "<<pack.operation()<<endl;

//	int ret = simpleSend(str, dest, sock);
//	sock = makeClientSocket(dest.host.c_str(), dest.port, 1);
//	cout<<"client sock = "<< sock<<endl;

	int sock = this->str2SockLRU(str, TCP);
	reuseSock(sock);
//	cout<<"sock = "<<sock<<endl;
	sockaddr_in recvAddr;
	int sentSize = generalSendTo(dest.host.data(),dest.port, sock, str.c_str(), TCP);
//	int ret = generalSendTCP(sock, str.c_str());

//	cout << "ZHTClient::lookup: simpleSend return = " << ret << endl;
	char buff[MAX_MSG_SIZE]; //MAX_MSG_SIZE
	memset(buff, 0, sizeof(buff));
	int rcv_size = -1;
	if (sentSize == str.length()) { //this only work for TCP. UDP need to make a new one so accept returns from server.
//		cout << "before protocol judge" << endl;

		rcv_size = generalReceive(sock, (void*)buff, sizeof(buff),recvAddr,0, TCP);

	/*	if (TRANS_PROTOCOL == USE_TCP) {

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
			rcv_size = d3_svr_recv(sock, buff, MAX_MSG_SIZE, 0, &tmp_sockaddr); //receive lookup result
			//d3_closeConnection(server_sock);
//			cout << "lookup received " << rcv_size << " bytes." << endl;

		}*/
		if (rcv_size < 0) {
			cout << "Lookup receive error." << endl;
			return rcv_size;
		} else {
			returnStr.assign(buff);
		}

//		cout << "after protocol judge" << endl;
	}
//	d3_closeConnection(sock);

	return rcv_size;
}
/*
int ZHTClient::lookup(string str, string &returnStr, int to_sock) {

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

//	int ret = simpleSend(str, dest, sock);
	sock = makeClientSocket(dest.host.c_str(), dest.port, 1);
//	cout<<"client sock = "<< sock<<endl;

	reuseSock(sock);
	int ret = generalSendTCP(sock, str.c_str());

//	cout << "ZHTClient::lookup: simpleSend return = " << ret << endl;
	char buff[MAX_MSG_SIZE]; //MAX_MSG_SIZE
	memset(buff, 0, sizeof(buff));
	int rcv_size = -1;
	if (ret == str.length()) { //this only work for TCP. UDP need to make a new one so accept returns from server.
//		cout << "before protocol judge" << endl;

		if (TRANS_PROTOCOL == USE_TCP) {

			rcv_size = d3_recv_data(sock, buff, MAX_MSG_SIZE, 0); //MAX_MSG_SIZE

		} else if (TRANS_PROTOCOL == USE_UDP) {
			//int svrPort = dest.port;//surely wrong, it's just the 50000 port
			//int svrPort = 50001;
			//srand(getpid() + clock());
//			int z = rand() % 10000 + 10000;
//			cout<<"Client:lookup: random port: "<<z<<endl;
//			int server_sock = d3_svr_makeSocket(rand() % 10000 + 10000);// use random port to send, and receive lookup result from it too.
//			cout<<"Client:lookup: UDP socket: "<<server_sock<<endl;
			sockaddr_in tmp_sockaddr;

			memset(&tmp_sockaddr, 0, sizeof(sockaddr_in));
//			cout << "lookup: before receive..." << endl;
			rcv_size = d3_svr_recv(sock, buff, MAX_MSG_SIZE, 0, &tmp_sockaddr); //receive lookup result
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

	return rcv_size;
}
*/
int ZHTClient::remove(string str) {

	Package package;
	package.ParseFromString(str);
	package.set_operation(2); // 3 for insert, 1 for look up, 2 for remove
	package.set_replicano(3); //5: original, 3 not original
	str = package.SerializeAsString();

//	struct HostEntity dest = this->str2Host(str);
	//int ret = simpleSend(str, dest, sock);
	/*	sock = makeClientSocket(dest.host.c_str(), dest.port, 1);
	 reuseSock(sock);
	 int ret_s = generalSendTCP(sock, str.c_str());

	 int32_t* ret = (int32_t*) malloc(sizeof(int32_t));

	 generalReveiveTCP(sock, (void*) ret, sizeof(int32_t), 0);
	 int ret_1 = *(int32_t*) ret;
	 //cout <<"Returned status: "<< *(int32_t*) ret<<endl;
	 d3_closeConnection(sock);
	 free(ret);

	 return ret_1;
	 */

	int sock = this->str2SockLRU(str, TCP);
	reuseSock(sock);
//	cout<<"sock = "<<sock<<endl;
	struct HostEntity dest = this->str2Host(str);
		sockaddr_in recvAddr;
		int sentSize = generalSendTo(dest.host.data() ,dest.port, sock, str.c_str(), TCP);
//		cout<<"remove sentSize "<< sentSize <<endl;
		int32_t* ret_buf = (int32_t*) malloc(sizeof(int32_t));

	//	generalReveiveTCP(sock, (void*) ret_buf, sizeof(int32_t), 0);
		generalReceive(sock, (void*)ret_buf, sizeof(int32_t),recvAddr,0, TCP);
		int ret_1 = *(int32_t*) ret_buf;



//	generalSendTCP(sock, str.c_str());

//	int32_t* ret = (int32_t*) malloc(sizeof(int32_t));

//	generalReveiveTCP(sock, (void*) ret, sizeof(int32_t), 0);

//	cout<<"remove got: "<< ret_1 <<endl;
//cout <<"Returned status: "<< *(int32_t*) ret<<endl;
//	d3_closeConnection(sock);
	free(ret_buf);

	return ret_1;
}

int benchmarkInsert(string cfgFile, string memberList, vector<string> &pkgList,
		ZHTClient &clientRet, int numTest, int lenString) {

//	ZHTClient client;

	if (clientRet.initialize(cfgFile, memberList) != 0) {
		cout << "Crap! ZHTClient initialization failed, program exits." << endl;
		return -1;
	}

//	clientRet = client; //reserve this client object for other benchmark(lookup/remove) to use.

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
		cout << "Insert: ret = "<< ret <<endl;
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

	if (client.initialize(cfgFile, memberList) != 0) {
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
int c=0;
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
int c=0;
	for (it = strList.begin(); it != strList.end(); it++) {
		string result;
		c++;
//		cout <<"Remove count "<< c << endl;

//		cout <<"Remove: "<< (*it).c_str() << endl;
		int ret = client.remove((*it));
		cout << "Remove: ret = "<< ret <<endl;
		if (ret < 0) {
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
	}else {
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
//	sleep(10);
	benchmarkLookup(pkgList, testClient);
	benchmarkRemove(pkgList, testClient);
	testClient.tearDownTCP(TCP);
	return 0;

}

