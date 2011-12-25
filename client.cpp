//#include <kchashdb.h>
//#include <kcprotodb.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>
//#include "d3_transport.h"
#include "meta.pb.h"
#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>
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
		cout << "Error opening the file." << endl;
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
			int z = rand() % 10000 + 10000;
			cout<<"Client:lookup: random port: "<<z<<endl;
			int server_sock = d3_svr_makeSocket(rand() % 10000 + 10000);// use random port to send, and receive lookup result from it too.
			cout<<"Client:lookup: UDP socket: "<<server_sock<<endl;
			sockaddr_in tmp_sockaddr;

			memset(&tmp_sockaddr, 0, sizeof(sockaddr_in));
			cout << "lookup: before receive..." << endl;
			rcv_size = d3_svr_recv(server_sock, buff, MAX_MSG_SIZE, 0,
					&tmp_sockaddr);//receive lookup result
			d3_closeConnection(server_sock);
			cout << "lookup received " << rcv_size << " bytes." << endl;

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
//		cout<<"c_str(): "<<str.c_str()<<endl;
//		cout<<"data(): "<< str.data()<<endl;

		pkgList.push_back(str);
	}

	double start = 0;
	double end = 0;
	start = getTime_msec();
	int errCount = 0;
	vector<string>::iterator it;
	for (it = pkgList.begin(); it != pkgList.end(); it++) {
		if (client.insert((*it)) < 0)
			errCount++;
	}
	end = getTime_msec();

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
		if (client.lookup((*it), result) < 0) {
			errCount++;
		} else if (result.empty()) { //empty string
			errCount++;
		}
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
	int para = strLen - 128;
	return 0;
}
int main(int argc, char* argv[]) {

	/*
	 cout<<"======================================================="<<endl<<endl;
	 Package package, package_ret;
	 package.set_virtualpath(randomString(25)); //as key
	 package.set_isdir(true);
	 package.set_replicano(5); //orginal--Note: never let it be nagative!!!
	 package.set_operation(3); // 3 for insert, 1 for look up, 2 for remove

	 package.add_listitem("item--1");
	 package.add_listitem("item--2");
	 package.add_listitem("item--3");
	 package.add_listitem("item--4");
	 package.add_listitem("item--5");
	 package.set_realfullpath(
	 "Some- Real-longer- longer-  and-longer-  Paths--------1item--1item--1item--123456789");

	 cout << "package size: " << package.ByteSize() << endl;

	 //string str = package.SerializeAsString();
	 char array[package.ByteSize()];


	 string stri = package.SerializeAsString();

	 cout << "Initial string in String, size= "<< stri.size()<<", content: "<<stri<<endl<<endl;

	 cout << "Initial string in c_str, strlen=  "<< strlen(stri.c_str())<<", content: "<<stri.c_str()<<endl<<endl;

	 cout << "Initial string in const char * data(), length=  "<< stri.length()<<", content: "<<stri.data()<<endl;

	 cout<<"======================================================="<<endl<<endl;



	 */
	int numOper = atoi(argv[1]);
	string cfgFile = "zht.cfg";
	string memberList = "neighbor";
	vector<string> pkgList;
	ZHTClient testClient;
	benchmarkInsert(cfgFile, memberList, pkgList, testClient, numOper, 15); //25fro 128bytes.
	benchmarkLookup(pkgList, testClient);
	benchmarkRemove(pkgList, testClient);

	/*
	 ZHTClient cl;

	 if (cl.initialize(cfgFile, memberList) != 0) {
	 cout << "Crap! ZHTClient initialization failed, program exits." << endl;
	 return -1;
	 }

	 Package package, package_ret;
	 package.set_virtualpath(randomString(25)); //as key
	 //package.set_virtualpath("Virtual/some");
	 package.set_isdir(false);
	 //package.set_replicano(NUM_REPLICAS);
	 package.set_replicano(5); //orginal--Note: never let it be nagative, it will increase the package size to 143!!!
	 package.set_realfullpath(
	 "Some-Real-longer-longer-and-longer-Paths--------");
	 package.add_listitem("item--1");
	 package.add_listitem("item--2");
	 package.add_listitem("item--3");
	 package.add_listitem("item--4");
	 package.add_listitem("item--5");
	 package.set_operation(3);

	 string str = package.SerializeAsString();

	 if (cl.insert(str) < 0) {
	 cout << "Insert failed. " << endl;
	 return -1;
	 };
	 cout << "Insert succeeded." << endl;

	 string result;
	 package.set_operation(1); //1:lookup, 2:remove, 3:insert
	 string str_lookup = package.SerializeAsString();
	 cout << "Lookup package size = " << str_lookup.length() << endl;
	 if (cl.lookup(str_lookup, result) != 0) { //stupid.
	 cout << "Lookup failed." << endl;
	 return -1;
	 } else {
	 package_ret.ParseFromString(result);
	 cout << "Result real path = " << package_ret.realfullpath() << endl;
	 }

	 package.set_operation(2);
	 string str_remove = package.SerializeAsString();
	 if (cl.remove(str_remove) < 0) {
	 cout << "Remove failed." << endl;
	 return -1;
	 } else
	 cout << "Remove succeeded." << endl;
	 return 0;
	 */

}

//--------------------------------Making API End-------------------------------------

/*
 int main(int argc, char* argv[]) {
 cout << "Use: ./client numOption neighborListFile" << endl;
 srand(getpid() + clock());
 int numOper = atoi(argv[1]); //how many operations you want to do?
 int TEST_SIZE = numOper * 1;
 string fileName(argv[2]);
 //raman-configfile-s

 if (setconfigvariables() != 0) {
 cout << "Not able to read configuration file." << endl;
 exit(1);
 }
 //raman-configfile-e
 cout << "ZHT_REPLICATION = " << REPLICATION_TYPE << endl;
 cout << "NUM_REPLICAS = " << NUM_REPLICAS << endl;
 //raman-client-replication-s
 struct thread_data *threaddata;
 pthread_t threadId;
 //raman-client-replication-e
 vector<struct HostEntity> hostList = getMembership(fileName);
 int nHost = hostList.size();
 list<string> myPackagelist;
 vector<struct HostEntity> sendDestList;
 list<string>::iterator it;
 //raman-sigpipe-s
 signal(SIGPIPE, sig_pipe);
 //raman-sigpipe-e
 for (int i = 0; i < TEST_SIZE; i++) {
 Package package;
 package.set_virtualpath(randomString(25)); //as key
 package.set_isdir(true);
 //package.set_replicano(NUM_REPLICAS);
 package.set_replicano(5); //orginal--Note: never let it be nagative, it will increase the package size to 143!!!
 package.set_realfullpath("Some-Real-Path");
 package.add_listitem("item1-123456789");
 package.add_listitem("item2-123456789");
 package.add_listitem("item3-123456789");
 package.add_listitem("item4-123456789");
 package.add_listitem("item5-123456789");
 package.set_operation(3); // 3=insert, 1=find, 2=remove
 myPackagelist.push_back(package.SerializeAsString());
 //hash key to various nodes
 //		cout << "package size: " << package.ByteSize() << endl;//just actual size of package, not the size of string which is sent.
 struct HostEntity destHost = hostList.at(
 hash((package.virtualpath()).c_str(), nHost)); //vector start from 0, not 1.
 sendDestList.push_back(destHost);
 }

 //======================================= insert  ========================================
 double start = 0;
 double end = 0;
 int i = 0;
 int client_sock = 0;
 start = getTime_msec();
 int fail = 0;
 int returnV;
 sockaddr_in toAddr;
 sockaddr_in recvAddr;
 //raman-client-replication-s
 if (REPLICATION_TYPE == 1) {
 if (pthread_create(&threadId, NULL, replicator, NULL) != 0) {
 cout
 << "Creation of Replicator thread failed. No replicas will be created."
 << endl;
 }
 }
 //raman-client-replication-e
 #if TRANS_PROTOCOL == USE_UDP
 client_sock = d3_svr_makeSocket((rand() % 10000 + 10000));
 #endif
 for (it = myPackagelist.begin(); it != myPackagelist.end(); it++, i++) {
 //---------------------map key to nodes: make connection with destination arguments----------------------
 //===================================start communication======================================
 cout << "Insert: Package str size=" << (*it).length() << endl;
 string hostName;
 int port;
 struct HostEntity destHost = sendDestList.at(i);
 string str = *it;
 #if TRANS_PROTOCOL == USE_TCP
 client_sock = d3_makeConnection((destHost.host).c_str(), destHost.port);
 //raman-client-replication-s
 //			if (client_sock < 0) { //connection refused: bad node: turn to repilicas
 if (REPLICATION_TYPE == 0 && client_sock < 0) { //connection refused: bad primary node:only pick ONE replica to send
 //turn to repilicas, 0 means server-side replication
 //raman-client-replication-e
 //in this situation primary server fail, client sent to all replicas by its own
 for (int t = 1; t <= NUM_REPLICAS; t++) { //if NUM_REPLICAS is 0, do nothing.
 Package package;
 package.ParseFromString(str);
 //package.set_replicano(NUM_REPLICAS-t);//  ????
 package.set_replicano(3);//not original: not handle all replicas, only store one(need to be fixed)
 str = package.SerializeAsString();

 int index = hash((package.virtualpath()).c_str(), nHost) + t;//t is the replica offset
 index = index % nHost;
 struct HostEntity destHost_replica = hostList.at(index);
 cout << "send to replica:  " << (destHost.host).c_str() << ": "
 << destHost.port << endl;//
 int client_sock_replica = d3_makeConnection(
 (destHost_replica.host).c_str(), destHost_replica.port);//only make connection, not send, sending done by common part:
 // may has logical problem: if primary server fail, data will be sent to the last replica,because client_sock is updated but data is not sent to them. Only last one works.
 if (client_sock_replica > 0) { //???----Tony added below
 cout
 << "Warning: One of the nodes failed. Message forwarded to replicas."
 << endl;
 client_sock = client_sock_replica;
 break;// break for(): escape from replica-for loop, here it doesn't finish scaning all replicas
 //send to replica, receive return here. what's needed to do so?

 //Tony-sendToAllReplicas-e
 } // end if-

 if (client_sock_replica < 0) { // replica can't be reached
 cout << "Error: Failed to connect one of the replicas."
 << endl;
 //fail++;
 continue;//continue to next insert-for loop
 } //endif: replica can't be reached
 //cout<<"All repilicas are failed, message lost. "<<endl;
 } //end replica-for

 } //endwhile: server-side replication activated, all replication sending must finished in this scope.
 #endif //end TCP #if scope
 //cout << "msg will be send to " << destHost.port << ", content: " << str	<< ", length " << str.length() << endl;

 //sleep(3);
 if (client_sock > 0) {

 toAddr = d3_make_sockaddr_in((destHost.host).c_str(),
 destHost.port); //make_socket means make a socket locally ready to be used
 //	cout << "NUM_REPLICAS is " << NUM_REPLICAS <<endl;
 //		cout << "send to server: str length= "<< str.length() <<endl;
 //Here UDP doen't has connection, so no make_connection() to tell if server is alive.
 returnV = d3_send_data(client_sock, (void*) str.c_str(),
 str.length(), 0, &toAddr); //normal insert sending.why need both client_sock and toAdd?
 if (returnV == -1) {
 fail++;
 continue;
 }
 void *buff_return = (void*) malloc(sizeof(int32_t));
 d3_svr_recv(client_sock, buff_return, sizeof(int32_t), 0,
 &recvAddr);
 int32_t ret = *(int32_t*) buff_return;
 switch (ret) {
 case 0:
 break;
 case -3:
 cout << "Insert failed." << endl;
 fail++;
 break;
 default:
 cout << "insert(main): What the hell was that? ret = " << ret
 << endl;
 break;
 }
 #if TRANS_PROTOCOL == USE_TCP
 d3_closeConnection(client_sock);

 #endif
 } else {
 cout << "Client: All repilicas are failed, message lost. " << endl;
 }

 } //--------------end for: insert--------------------
 end = getTime_msec();
 cout << "Sent " << TEST_SIZE - fail << " packages out of" << TEST_SIZE
 << " cost " << end - start << " ms" << endl;
 #if TRANS_PROTOCOL == USE_UDP
 d3_closeConnection(client_sock);
 #endif
 //raman-client-replication-s
 if (REPLICATION_TYPE == 1) {
 if (pthread_join(threadId, NULL) != 0) {
 cout << "Problem joining the replicator thread: " << endl;
 }
 }
 //raman-client-replication-e

 //======================================= find  ========================================
 list<string> findList;
 for (it = myPackagelist.begin(); it != myPackagelist.end(); it++) {
 Package package;
 package.ParseFromString(*it);
 package.set_operation(1); // 3=insert, 1=find, 2=remove
 package.set_replicano(3); //==========Tony not tested: finds don't need to forward at server-side
 findList.push_back(package.SerializeAsString());
 }
 myPackagelist = findList;
 start = 0;
 end = 0;
 i = 0;
 start = getTime_msec();
 fail = 0;
 #if TRANS_PROTOCOL == USE_UDP
 client_sock = d3_svr_makeSocket((rand() % 10000 + 10000));
 #endif
 for (it = myPackagelist.begin(); it != myPackagelist.end(); it++, i++) {
 //---------------------map key to nodes: make connection with destination arguments----------------------
 //		cout<<"Lookup Package No."<< i <<endl;
 string hostName;
 int port;
 struct HostEntity destHost = sendDestList.at(i);
 string str = *it;
 #if TRANS_PROTOCOL == USE_TCP
 int client_sock = d3_makeConnection((destHost.host).c_str(),
 destHost.port);
 if (client_sock < 0) { //bad primary node: turn to replicas
 for (int t = 1; t <= NUM_REPLICAS; t++) {
 Package package;
 package.ParseFromString(str);
 int index = hash((package.virtualpath()).c_str(), nHost) + t;
 index = index % nHost;
 destHost = hostList.at(index);
 client_sock = d3_makeConnection((destHost.host).c_str(),
 destHost.port);
 if (client_sock > 0) { //????????
 cout
 << "Warning: One of the nodes failed. Message forwarded to replicas."
 << endl;
 break;
 }
 }
 if (client_sock < 0) { // no replica alive, failed.
 cout << "Find Error: Failed to connect any of the replicas."
 << endl;
 fail++;
 continue;//continue to next for loop
 }
 }
 #endif
 toAddr = d3_make_sockaddr_in((destHost.host).c_str(), destHost.port);
 returnV = d3_send_data(client_sock, (void*) str.c_str(), str.length(),
 0, &toAddr);
 //cout<<"Return value of send:"<<returnV<<endl;
 if (returnV == -1) {
 fail++;
 continue;
 }
 void *buff_return = (void*) malloc(sizeof(int32_t));
 d3_svr_recv(client_sock, buff_return, sizeof(int32_t), 0, &recvAddr);
 int32_t ret = *(int32_t*) buff_return;
 switch (ret) {
 case 0:
 break;
 case -1:
 cout << "Bad key: nothing to find" << endl;
 fail++;
 break;
 case -2:
 cout << "Lookup failed: no match found." << endl;
 fail++;
 break; //----?
 default:
 cout << "find: What the hell was that? ret= " << ret << endl;
 break;
 }
 #if TRANS_PROTOCOL == USE_TCP
 d3_closeConnection(client_sock);
 #endif
 } //--------------end for--------------------
 end = getTime_msec();
 cout << "Found " << TEST_SIZE - fail << " packages out of" << TEST_SIZE
 << " cost " << end - start << " ms" << endl;
 #if TRANS_PROTOCOL == USE_UDP
 d3_closeConnection(client_sock);
 #endif

 //======================================= Remove  ========================================
 list<string> removeList;
 for (it = myPackagelist.begin(); it != myPackagelist.end(); it++) {
 Package package;
 package.ParseFromString(*it);
 package.set_replicano(5); // 5: original; 3: non-original
 package.set_operation(2); // 0=insert, 1=find, 2=remove
 removeList.push_back(package.SerializeAsString());
 }
 myPackagelist = removeList;
 start = 0;
 end = 0;
 i = 0;
 start = getTime_msec();
 fail = 0;
 #if TRANS_PROTOCOL == USE_UDP
 client_sock = d3_svr_makeSocket((rand() % 10000 + 10000));
 #endif
 for (it = myPackagelist.begin(); it != myPackagelist.end(); it++, i++) {
 //---------------------map key to nodes: make connection with destination arguments----------------------
 //		cout<<"Package No."<< i <<endl;
 string hostName;
 int port;
 string str = *it;
 struct HostEntity destHost = sendDestList.at(i);
 #if TRANS_PROTOCOL == USE_TCP
 int client_sock = d3_makeConnection((destHost.host).c_str(),
 destHost.port);

 if (ZHT_REPLICATION == 0 && client_sock < 0 ) { //connection refused: bad node: turn to repilicas
 for (int t = 1; t <= NUM_REPLICAS; t++) {
 Package package;
 package.ParseFromString(str);
 package.set_replicano(3);
 str = package.SerializeAsString();
 int index = hash((package.virtualpath()).c_str(), nHost) + t;
 index = index % nHost;
 destHost = hostList.at(index);
 struct HostEntity destHost_replica = hostList.at(index);
 int client_sock_replica = d3_makeConnection((destHost.host).c_str(),
 destHost.port);
 if (client_sock_replica > 0) {
 cout
 << "Warning: One of the nodes failed. Message forwarded to replicas."
 << endl;
 //break;

 //Tony-sendToAllReplicas-s
 sockaddr_in toAddr_replica = d3_make_sockaddr_in(
 (destHost_replica.host).c_str(),
 destHost_replica.port);
 int return_replica = d3_send_data(client_sock_replica,
 (void*) str.c_str(), str.length(), 0,
 &toAddr_replica);//normal insert sending.

 //copied from below: general sending
 void *buff_return_replica = (void*) malloc(sizeof(int32_t));
 sockaddr_in recvAddr_replica;
 d3_svr_recv(client_sock_replica, buff_return_replica,
 sizeof(int32_t), 0, &recvAddr_replica);
 int32_t ret = *(int32_t*) buff_return_replica;
 switch (ret) {
 case 0:
 break;
 case -3:
 cout << "Insert failed." << endl;
 fail++;TCP
 break;
 default:
 cout << "What the hell was that? ret= " << ret << endl;
 break;
 }
 #if TRANS_PROTOCOL == USE_TCP
 d3_closeConnection( client_sock_replica);
 #endif

 //Tony-sendToAllReplicas-e

 }
 }
 if (client_sock < 0) { // no replica alive, failed.
 cout
 << "Remove Error: Failed to connect any of the replicas. This certain message lost."
 << endl;
 continue;//continue to next for loop
 }
 }
 #endif

 toAddr = d3_make_sockaddr_in((destHost.host).c_str(), destHost.port);
 returnV = d3_send_data(client_sock, (void*) str.c_str(), str.length(),
 0, &toAddr);
 if (returnV == -1) {
 fail++;
 continue;
 }
 void *buff_return = (void*) malloc(sizeof(int32_t));
 d3_svr_recv(client_sock, buff_return, sizeof(int32_t), 0, &recvAddr);
 int32_t ret = *(int32_t*) buff_return;
 switch (ret) {
 case 0:
 break;
 case 1:
 break;
 case -2:
 cout << "Remove failed." << endl;
 fail++;
 break;
 default:
 cout << "What the hell was that?" << endl;
 break;
 }
 #if TRANS_PROTOCOL == USE_TCP
 d3_closeConnection(client_sock);
 #endif
 } //--------------end for--------------------
 end = getTime_msec();
 cout << "Removed " << TEST_SIZE - fail << " packages out of" << TEST_SIZE
 << " cost " << end - start << " ms" << endl;
 #if TRANS_PROTOCOL == USE_UDP
 d3_closeConnection(client_sock);
 #endif

 }

 */
