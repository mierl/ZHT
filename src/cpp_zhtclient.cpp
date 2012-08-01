#include "cpp_zhtclient.h"
#include "lru_cache.h"
#include <stdint.h>

/*******************************
 * zhouxb
 */
//================================ Global and constant ===============================
struct timeval tp;
int MAX_FILE_SIZE = 10000; //1GB, too big, use dynamic memory malloc.

int const MAX_MSG_SIZE = 65535; //transferd string maximum size

int REPLICATION_TYPE; //1 for Client-side replication

int NUM_REPLICAS;
//====================================================================================

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
/*******************************
 * zhouxb
 */

int UDP_SOCKET = -1;
int CACHE_SIZE = 1024;
LRUCache<string, int> connectionCache(CACHE_SIZE); //initialized here.

ZHTClient::ZHTClient() { // default all invalid value, so the client must be initialized to set the variables.
	//Since constructor can't return anything, we must have an initialization function that can return possible error message.
	this->NUM_REPLICAS = -1;
	this->REPLICATION_TYPE = -1;
	this->protocolType = -1;
}

int ZHTClient::initialize(string configFilePath, string memberListFilePath,
		bool tcp) {

	this->TCP = tcp;
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
	if (TCP == true) {
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
	} else { //UDP
		if (UDP_SOCKET < 0) {
			UDP_SOCKET = makeClientSocket(dest.host.c_str(), dest.port, TCP);
		}
		return UDP_SOCKET;

	}

}

//This store limited connections in a LRU cache, one item is one host VS one sock
int ZHTClient::str2SockLRU(string str, bool tcp) {
	struct HostEntity dest = this->str2Host(str);
	int sock = 0;
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
		} //end if sock<0
	} else { //UDP

		if (UDP_SOCKET <= 0) {
			sock = makeClientSocket(dest.host.c_str(), dest.port, TCP);
			UDP_SOCKET = sock;
		} else
			sock = UDP_SOCKET;
	}

	return sock;
}

int ZHTClient::tearDownTCP() {
	if (TCP == true) {
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
	Package package;
	package.ParseFromString(str);

	if (package.virtualpath().empty()) //empty key not allowed.
		return -1;
	if (package.realfullpath().empty()) //coup, to fix ridiculous bug of protobuf!
		package.set_realfullpath(" ");

	package.set_operation(3); //1 for look up, 2 for remove, 3 for insert
	package.set_replicano(5); //5: original, 3 not original
	str = package.SerializeAsString();

	int sock = this->str2SockLRU(str, TCP);
	reuseSock(sock);
//	cout<<"sock = "<<sock<<endl;
//	int sentSize = generalSendTCP(sock, str.c_str());
	struct HostEntity dest = this->str2Host(str);
	sockaddr_in recvAddr;
	int sentSize = generalSendTo(dest.host.data(), dest.port, sock, str.c_str(),
			TCP);
//	cout <<"Client inseret sent: "<<sentSize<<endl;
	int32_t* ret_buf = (int32_t*) malloc(sizeof(int32_t));

//	generalReveiveTCP(sock, (void*) ret_buf, sizeof(int32_t), 0);
	generalReceive(sock, (void*) ret_buf, 4, recvAddr, 0, TCP);
	int ret = *(int32_t*) ret_buf;
	if (ret < 0) {
//		cerr << "zht_util.h: Failed to insert." << endl;
	}
//cout <<"Returned status: "<< *(int32_t*) ret<<endl;
//	d3_closeConnection(sock);
	free(ret_buf);

	//return ret_1;
//	cout<<"insert got: "<< *ret <<endl;
	return ret;
}

int ZHTClient::lookup(string str, string &returnStr) {

	Package package;
	package.ParseFromString(str);

	if (package.virtualpath().empty()) //empty key not allowed.
		return -1;
	if (package.realfullpath().empty()) //coup, to fix ridiculous bug of protobuf!
		package.set_realfullpath(" ");

	package.set_operation(1); // 1 for look up, 2 for remove, 3 for insert
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
	int sentSize = generalSendTo(dest.host.data(), dest.port, sock, str.c_str(),
			TCP);
//	int ret = generalSendTCP(sock, str.c_str());

//	cout << "ZHTClient::lookup: simpleSend return = " << ret << endl;
	char buff[MAX_MSG_SIZE]; //MAX_MSG_SIZE
	memset(buff, 0, sizeof(buff));
	int rcv_size = -1;
	int status = -1;
	string sRecv;
	string sStatus;

	if (sentSize == str.length()) { //this only work for TCP. UDP need to make a new one so accept returns from server.
//		cout << "before protocol judge" << endl;

		rcv_size = generalReceive(sock, (void*) buff, sizeof(buff), recvAddr, 0,
				TCP);

		if (rcv_size < 0) {
			cout << "Lookup receive error." << endl;
		} else {
			sRecv.assign(buff);
			returnStr = sRecv.substr(3); //the left is real thing need to be deserilized.
			sStatus = sRecv.substr(0, 3); //the first three chars means status code, like -1, -2, 0, -98, -99 and so on.
		}

//		cout << "after protocol judge" << endl;
	}
//	d3_closeConnection(sock);

	if (!sStatus.empty())
		status = atoi(sStatus.c_str());

	return status;
}

int ZHTClient::remove(string str) {

	Package package;
	package.ParseFromString(str);

	if (package.virtualpath().empty()) //empty key not allowed.
		return -1;
	if (package.realfullpath().empty()) //coup, to fix ridiculous bug of protobuf!
		package.set_realfullpath(" ");

	package.set_operation(2); //1 for look up, 2 for remove, 3 for insert
	package.set_replicano(3); //5: original, 3 not original
	str = package.SerializeAsString();

	int sock = this->str2SockLRU(str, TCP);
	reuseSock(sock);
//	cout<<"sock = "<<sock<<endl;
	struct HostEntity dest = this->str2Host(str);
	sockaddr_in recvAddr;
	int sentSize = generalSendTo(dest.host.data(), dest.port, sock, str.c_str(),
			TCP);
//		cout<<"remove sentSize "<< sentSize <<endl;
	int32_t* ret_buf = (int32_t*) malloc(sizeof(int32_t));

	//	generalReveiveTCP(sock, (void*) ret_buf, sizeof(int32_t), 0);
	generalReceive(sock, (void*) ret_buf, sizeof(int32_t), recvAddr, 0, TCP);

//	generalSendTCP(sock, str.c_str());

//	int32_t* ret = (int32_t*) malloc(sizeof(int32_t));

//	generalReveiveTCP(sock, (void*) ret, sizeof(int32_t), 0);
	int ret_1 = *(int32_t*) ret_buf;
//	cout<<"remove got: "<< ret_1 <<endl;
//cout <<"Returned status: "<< *(int32_t*) ret<<endl;
//	d3_closeConnection(sock);
	free(ret_buf);

	return ret_1;
}
