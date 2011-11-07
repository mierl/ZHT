//#include <kchashdb.h>
//#include <kcprotodb.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include "d3_transport.h"
#include "meta.pb.h"
#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <netdb.h>
#include "d3_sys_globals.h"
//raman-client-replication-s
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
//raman-client-replication-e

struct timeval tp;

//raman-configfile-s
int REPLICATION_TYPE; //1 for Client-side replication
//raman-configfile-e

int NUM_REPLICAS; //=0;

double getTime_usec() {
	gettimeofday(&tp, NULL);
	return static_cast<double>(tp.tv_sec) * 1E6
			+ static_cast<double>(tp.tv_usec);
}
double getTime_msec() {
	gettimeofday(&tp, NULL);
	return static_cast<double>(tp.tv_sec) * 1E3
			+ static_cast<double>(tp.tv_usec) / 1E3;
}
double getTime_sec() {
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
struct HostEntity {
	struct sockaddr_in si;
	int sock;
	string host;
	int port;
	bool valid;
	vector<unsigned long long> ringID;
};

vector<struct HostEntity> getMembership(string fileName) {
	vector<struct HostEntity> hostList;
	ifstream in(fileName.c_str(), ios::in); //make a file input stream
	string host;
	int port;
	HostEntity aHost;
	struct sockaddr_in si_other;
	hostent *record;
	in_addr *address;
	string ip_address;
	if (!in.is_open()) {
		cout << "File read failed." << endl;
		return hostList;
	}
	if (!in.eof()) {
		in >> host >> port;
	}
	record = gethostbyname(host.c_str());
	address = (in_addr *) record->h_addr;
	ip_address = inet_ntoa(*address);
	while (!in.eof()) {
		int s, i, slen = sizeof(si_other);
		memset((char *) &si_other, 0, sizeof(si_other));
		si_other.sin_family = AF_INET;
		si_other.sin_port = htons(port);
		if (inet_aton(ip_address.c_str(), &si_other.sin_addr) == 0) {
			fprintf(stderr, "inet_aton() failed\n"); //address to number
		}
		aHost.si = si_other;
		aHost.host = host;
		aHost.port = port;
		aHost.valid = true;
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
/*
 int hash(const char *str, int mod) { //djb2
 unsigned long hash = 5381;
 int c;
 while (c = *str++)
 hash = ((hash << 5) + hash) + c; // hash * 33 + c
 return hash % mod;
 }
 */
int hash(const char *str, int mod){

	unsigned long hash = 0;
	int c;

	while (c = *str++)
	hash = c + (hash << 6) + (hash << 16) - hash;

	return hash % mod;
}

//raman-sigpipe-s
void sig_pipe(int signum) {
	printf("SIGPIPE Caught!\n");
	signal(SIGPIPE, sig_pipe);
}
//raman-sigpipe-e

//raman-configfile-s
int setconfigvariables() {
	FILE *fp;
	char line[100], *key, *svalue;
	int ivalue;
	fp = fopen("zht.cfg", "r");
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
//raman-configfile-e

//raman-client-replication-s
void *replicator(void *) {
	int nHost = hostList.size();
	vector<struct HostEntity> sendDestList;
	list<string>::iterator it;
	int fail = 0;
	int client_sock_r;
	sockaddr_in toAddr_r;
	sockaddr_in recvAddr_r;
#if TRANS_PROTOCOL == USE_UDP
	client_sock_r = d3_svr_makeSocket((rand() % 10000 + 10000));
#endif
	for (it = myPackagelist.begin(); it != myPackagelist.end(); it++) {
		//---------------------map key to nodes: make connection with destination arguments----------------------
		string hostName;
		int port;
		struct HostEntity destHost;
		string str = *it;
		//-------------------repilication----------------------
		for (int t = 1; t <= NUM_REPLICAS; t++) {
			Package package;
			package.ParseFromString(str);
			//package.set_replicano(NUM_REPLICAS - t);
			package.set_replicano(3); //just a backup, not original(5).
			int index = hash((package.virtualpath()).c_str(), nHost) + t;
			index = index % nHost;
			destHost = hostList.at(index);
#if TRANS_PROTOCOL == USE_TCP
			client_sock_r = d3_makeConnection((destHost.host).c_str(),
					destHost.port);
#endif
			void* buff1 = (void*) malloc(sizeof(int32_t)); //send an int for presenting size of following package
			memset(buff1, 0, sizeof(int32_t)); //init of size
			int32_t str_size = str.length(); //strlen(str);
			buff1 = &str_size;
			toAddr_r = d3_make_sockaddr_in((destHost.host).c_str(),
					destHost.port);
			int returnV = d3_send_data(client_sock_r, (void*) str.c_str(),
					str.length(), 0, &toAddr_r);
			if (returnV == -1) {
				fail++;
				cout << "Replication error: sending request error." << endl;
				continue;
			}
			void *buff_return = (void*) malloc(sizeof(int32_t));
			d3_svr_recv(client_sock_r, buff_return, sizeof(int32_t), 0,
					&recvAddr_r);
			int32_t ret = *(int32_t*) buff_return;
			switch (ret) {
			case 0:
				break;
			case -1:
				cout << "Find failed." << endl;
				break;
			case -2:
				cout << "Remove failed." << endl;
				break;
			case -3:
				fail++;
				cout << "Replication error: insert: server return error."
						<< endl;
				break;
			default:
				cout << "Replicator: What the hell was that? ret= " << ret
						<< endl;
				break;
			}
#if TRANS_PROTOCOL == USE_TCP
			d3_closeConnection(client_sock_r);
#endif
		} //-----------end inner for loop----------------
	} //--------------end outer for loop--------------------
#if TRANS_PROTOCOL == USE_UDP
	d3_closeConnection(client_sock_r);
#endif
}
//raman-client-replication-e

//----------------------------------Making API---------------------------------------

//seperate TCP with UDP??

//	int simpleReceive();
//Send a plain string to dest while keep the socket for future receive, NOT closing the socket in this function, so it must be closed somewhere else.
int simpleSend(string str, struct HostEntity destination, int &current_sock) {

	//cout<< "contactReplica: newly received msg: package.replicano= " << package.replicano()<< ", package.ByteSize()="<< package.ByteSize() <<endl;//here the correct package was received: -1

	int i = 0, port, client_sock, r = 0;
	int32_t str_size = str.length();
	string hostName;
	sockaddr_in toAddr, recv_addr;

#if TRANS_PROTOCOL == USE_TCP
	client_sock = d3_makeConnection((destination.host).c_str(),
			destination.port);
#elif TRANS_PROTOCOL == USE_UDP

	cout << endl << "simpleSend makeSocket start-----" << endl;
	client_sock = d3_svr_makeSocket((time(NULL) % 10000) + rand() % 10000); //client can use any port to talk to server
//	client_sock = d3_svr_makeSocket(destination.port);
	cout << "simpleSend makeSocket end-----" << endl << endl;
#endif

	if (client_sock < 0) { //only report error, doesn't handle it
		cout << " " << endl;
		return -9;
	}

	toAddr = d3_make_sockaddr_in((destination.host).c_str(), destination.port);

	cout << "simpleSend trying to reach host:" << destination.host << ", port:"
			<< destination.port << endl;

	r = d3_send_data(client_sock, (void*) str.c_str(), str_size, 0, &toAddr);
	/*
	 if (sigpipe_flag) {
	 sigpipe_flag = false;
	 cout << "Primary got SIGPIPE while inserting package to a replica. "
	 << client_sock << " : " << r << endl;
	 return -8;
	 }
	 */
	if (r < 0) {
		cerr << "Sending data failed." << endl;
		return -7;
	}

	void *buff_return = (void*) malloc(sizeof(int32_t));
	r = d3_recv_data(client_sock, buff_return, sizeof(int32_t), 0);
	//d3_recv_data(int sock,void *buffer,size_t size,int flags);
	/*
	 if (sigpipe_flag) {
	 cout
	 << "Error: Got SIGPIPE while receiving return state."
	 << endl;
	 return -8;
	 }
	 */
	if (r < 0) {
		cerr << "Receiving return state failed." << endl;
		return -7;
	}
	int32_t ret = *(int32_t*) buff_return;
	switch (ret) {
	case 0:
		break;
	case 1:
		break;
	case -2:
		cerr << "Failed to remove from replica." << endl;
		break;
	case -3:
		cerr << "Failed to insert into replica." << endl;
		break;
	default:
		cerr << "What the hell was that?" << endl;
		break;
	}
	current_sock = client_sock;
	//d3_closeConnection(client_sock);//not close here?
	return ret;
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
			this->NUM_REPLICAS = ivalue +1; //note: +1 is must
			//cout<<"NUM_REPLICAS = "<< NUM_REPLICAS <<endl;
		} else {
			cout << "Config file is not correct." << endl;
			return -2;
		}

	}
	return 0;

}

struct HostEntity ZHTClient::str2Host(string str) {
	Package pkg;
	pkg.ParseFromString(str);
	int index = hash(pkg.virtualpath().c_str(), this->NUM_REPLICAS);
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
	int sock = -1;
	struct HostEntity dest = this->str2Host(str);
	int ret = simpleSend(str, dest, sock);
	char buff[134];
	if (ret == 0) {
		d3_recv_data(sock, buff, 134, 0);
		returnStr.assign(buff);
	}
	d3_closeConnection(sock);

	return ret;
}

int ZHTClient::remove(string str) {
	int sock = -1;
	struct HostEntity dest = this->str2Host(str);
	int ret = simpleSend(str, dest, sock);
	d3_closeConnection(sock);
	return ret;
}

//This is an example.
int main() {

	string cfgFile = "zht.cfg";
	string memberList = "neighbor";

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
	package.set_realfullpath("Some-Real-longer-longer-and-longer-Path--------");
	package.add_listitem("item1-1234567890001");
	package.add_listitem("item2-1234567890001");
	package.add_listitem("item3-1234567890001");
	package.add_listitem("item4-123456789");
	package.add_listitem("item5-123456789");
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
