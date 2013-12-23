#include <kchashdb.h>
#include <kcprotodb.h>
//#include <string>
//#include <cstring>
//#include <sys/time.h>
#include <stdlib.h>

//#include <netdb.h>
#include "zht_util.h"
//#include "meta.pb.h"
//#include "d3_transport.h"
//#include "d3_sys_globals.h"

#include <pthread.h>
#include <iomanip>
using namespace std;
using namespace kyotocabinet;

#define MAX_THREADS 400 //from 400
//#define MAX_MSG_SIZE 1024//a fixed package is neither reasonable nor practicable.

//HashDB db;//file based hash
ProtoHashDB db; //in-mem hash

//int NUM_REPLICAS;
//int REPLICATION_TYPE;//1 for Client-side replication

#if TRANS_PROTOCOL==USE_TCP
struct threaddata {
	int socket;
};

#elif TRANS_PROTOCOL==USE_UDP
struct threaddata {
	struct sockaddr_in sockinfo;
	char buffer[MAX_MSG_SIZE];
};
#endif

/*
 struct HostEntity {
 struct sockaddr_in si;
 int sock;
 string host;
 int port;
 bool valid;
 vector<unsigned long long> ringID;
 };
 */

vector<struct HostEntity> hostList;
int nHost;

//raman-sigpipe-s
bool sigpipe_flag = false;
uint32_t sigpipe_count = 0;
static void sigpipe_handler(int signum, siginfo_t *siginfo, void *context) {
	cout << "SIGPIPE Caught!" << endl;
	sigpipe_count++;
	sigpipe_flag = true;
}
//raman-sigpipe-e
static int server_sock = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int numthreads = 0;

int HB_insert(DB &db, Package package) {
	//int opt = package.operation();//opt not be used?
	string package_str = package.SerializeAsString();
	int ret = db.set(package.virtualpath(), package_str); //virtualpath as key
	if (ret == 0) {
		return -3; //insert fail.
	}
	/*
	 cout << "String insted: " << package_str << endl;
	 */
	return 0;
}

string HB_lookup(DB &db, Package package) {
	string value;
	db.get(package.virtualpath(), &value); //new kyoto version: 60
	return value;
}

/*
 string * HB_lookup(DB &db, Package package) {
 string* result = db.get(package.virtualpath());//This work for old version of kyotocabinet 30

 return result;
 }
 */

int HB_remove(DB &db, string key) {
	int ret = db.remove(key); //True for success False for fail.
	if (!ret) {
		cout << "DB Error: fail to remove :ret= " << ret << endl;
		return -2;
	}
	return 0; //succeed.
}
//raman-replication-s
/*
 int myhash(const char *str, int mod) { //old hash
 unsigned long hash = 5381;
 int c;

 while (c = *str++)
 hash = ((hash << 5) + hash) + c;

 return hash % mod;
 }
 */
/*
 int myhash(const char *str, int mod){

 unsigned long hash = 0;
 int c;

 while (c = *str++)
 hash = c + (hash << 6) + (hash << 16) - hash;

 return hash% mod;
 }
 */
/*
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
 ivalue = kyotocabinet::atoi(svalue);

 if ((strcmp(key, "REPLICATION_TYPE")) == 0) {
 REPLICATION_TYPE = ivalue;
 cout << "REPLICATION_TYPE = " << REPLICATION_TYPE << endl;
 }//other config options follow this way(if).

 if ((strcmp(key, "NUM_REPLICAS")) == 0) {
 NUM_REPLICAS = ivalue;
 cout << "NUM_REPLICAS = " << NUM_REPLICAS << endl;
 }

 }
 return 0;
 }
 */

// This function is to send a package to a GIVEN replica, it dones't choose which to send
int contactReplica(Package package, struct HostEntity destination) {

	//cout<< "contactReplica: newly received msg: package.replicano= " << package.replicano()<< ", package.ByteSize()="<< package.ByteSize() <<endl;//here the correct package was received: -1

	int i = 0, port, client_sock, r = 0;
	int32_t str_size;
	string hostName, str;
	sockaddr_in toAddr, recv_addr;

#if TRANS_PROTOCOL == USE_TCP
	client_sock = d3_makeConnection((destination.host).c_str(),
			destination.port);
#elif TRANS_PROTOCOL == USE_UDP

	cout<<endl<<"contactReplica makeSocket start-----"<<endl;
	client_sock = d3_svr_makeSocket( (time(NULL)%10000)+rand()%10000 ); //client can use any port to talk to server
//	client_sock = d3_svr_makeSocket(destination.port);
	cout<<"contactReplica makeSocket end-----"<<endl<<endl;
#endif       
	if (client_sock < 0) { //what's this mechanism: only report error, doesn't handle it
		int me = myhash((package.virtualpath()).c_str(), nHost);
		//since dbService already take over the routing, this will to extra map which is unnecessary and wrong.

		cout << "contactReplica:get msg: package.replicano= "
				<< package.replicano() << endl;
		me = me % hostList.size();
		struct HostEntity source = hostList.at(me);
		cerr << "Error connecting to replica: " << (destination.host).c_str()
				<< " : " << destination.port << " .Primary sever: "
				<< (source.host).c_str() << " : " << source.port << ". "
				<< strerror(errno) << endl;
		return -1;
	}
	str = package.SerializeAsString();
	str_size = str.length();
	toAddr = d3_make_sockaddr_in_client((destination.host).c_str(),
			destination.port);
	cout << "contactReplica trying to reach host:" << destination.host
			<< ", port:" << destination.port << endl;
	r = d3_send_data(client_sock, (void*) str.c_str(), str_size, 0, &toAddr);

	if (sigpipe_flag) {
		sigpipe_flag = false;
		cout << "Primary got SIGPIPE while inserting package to a replica. "
				<< client_sock << " : " << r << endl;
		return -1;
	}
	if (r < 0) {
		cerr << "Primary Server could not send data to secondary." << endl;
		return -1;
	}
	void *buff_return = (void*) malloc(sizeof(int32_t));
	r = d3_svr_recv(client_sock, buff_return, sizeof(int32_t), 0, &recv_addr);
	if (sigpipe_flag) {
		cout
				<< "Primary got SIGPIPE while receiving replica insert status from a secondary."
				<< endl;
		return -1;
	}
	if (r < 0) {
		cerr
				<< "Primary Server could not receive request status from secondary."
				<< endl;
		return -1;
	}
	int32_t ret = *(int32_t*) buff_return;
	switch (ret) {
	case 0:
		break;
	case 1:
		break;
	case -2:
		cerr << "Primary server failed to remove from replica." << endl;
		break;
	case -3:
		cerr << "Primary server failed to insert into replica." << endl;
		break;
	default:
		cerr << "What the hell was that?" << endl;
		break;
	}
	d3_closeConnection(client_sock);
	return ret;
}

//raman-replication-e
void *dbService(void *threadarg) {
	srand(kyotocabinet::getpid() + clock());
	//cout << "Current service thread ID = " << pthread_self()<< ", dbService() begin..." << endl;

	char buff[MAX_MSG_SIZE];
	int32_t operation_status;
	sockaddr_in toAddr;
	int client_sock, r;
	void* buff1 = (void*) malloc(sizeof(int32_t));
#if TRANS_PROTOCOL ==USE_TCP
	client_sock = ((struct threaddata *) threadarg)->socket;
#elif TRANS_PROTOCOL == USE_UDP
	toAddr = ((struct threaddata *) threadarg)->sockinfo;
	strcpy( buff, ((struct threaddata *) threadarg)->buffer ); //here the buffer(it seems a global variable) should be refreshed so clean it.

#endif

	//raman-s
	//	if (size > 0) { //if size < 0, consider it as a "quit command"
	//raman-e
	cout << "Thread: receive request from client..." << endl;
#if TRANS_PROTOCOL == USE_TCP
	r = d3_svr_recv(client_sock, buff, MAX_MSG_SIZE * sizeof(char), 0, &toAddr);
	if (r <= 0) {
		cout << "Server could not recv data" << endl;
	}
#endif
	Package package;
	package.ParseFromArray(buff, MAX_MSG_SIZE);
	string result;
	cout << endl << endl << "in dbService: received replicano = "
			<< package.replicano() << endl;

	cout << "package size: " << package.ByteSize() << endl;
//	cout <<"Package content: "<< buff<<endl;
	switch (package.operation()) {
	case 3: //insert
		cout << "Insert..."<< endl;
		operation_status = HB_insert(db, package);
		break;
	case 1: //lookup
		cout << "Lookup..."<< endl;
		if (package.virtualpath().empty()) {
			cerr << "Bad key: nothing to find" << endl;
			operation_status = -1;
		} else {
			result = HB_lookup(db, package);
			//don't really send result back to client now, do it latter.
			if (result.empty()) {
				operation_status = -2;
				buff1 = &operation_status;

			} else { //find result, send back here

				cout << "Lookup: Result found, sending back..." << endl;
				//=====================================
				//how to know where to send back? Use the one that created when receive request as above.

				//d3_send_data(client_sock, (void*)result.c_str(),result.length(), 0, &toAddr);
				//just a try, not use simpleSend, because no need to make a new socket and connection..
				if (TRANS_PROTOCOL == USE_TCP) {
					d3_send_data(client_sock, (void*) result.c_str(), result.length(), 0, &toAddr); //TCP reuse the socket and connection.

				} else if (TRANS_PROTOCOL == USE_UDP) {

					HostEntity dest;

					char host[INET_ADDRSTRLEN];
					cout << "before inet_ntop" << endl;
					inet_ntop(AF_INET, &(toAddr.sin_addr), host, INET_ADDRSTRLEN); //get a hostname from toAddr

					cout << "before inet_ntop" << endl;
					string hostName = string(host);
					cout << "string made" << endl;
					dest.host = hostName;
					dest.port = toAddr.sin_port; //this is arandom port used by client just once, it was discarded after once use.
					//dest.port = 50001; working, but ugly and dangerous
					cout << "send back result to host: " << dest.host
							<< "; port: " << dest.port << endl;
					int sock = 0;
					cout << "host entity ready, now simpleSend..." << endl;
					simpleSend(result, dest, sock);
					//d3_closeConnection(sock);//may be necessary.
					// Do nothing below.----------------
					operation_status = 0;

				}

			}
		}
		break;
	case 2: //remove
		cout << "Remove..."<< endl;
		if (package.virtualpath().empty()) {
			cerr << "Bad key: nothing to remove" << endl;
			operation_status = -1;
		} else {
			operation_status = HB_remove(db, package.virtualpath());
		}
		break;
	default:
		operation_status = -3;
		break;
	}
	buff1 = &operation_status;

#if TRANS_PROTOCOL == USE_TCP
	r = d3_send_data(client_sock, buff1, sizeof(int32_t), 0, &toAddr);
	if (r <= 0) {
		cout << "Server could not send acknowledgement to client" << endl;
	}
	if (!result.empty()) { //return lookup result.
		r = d3_send_data(client_sock, (void*) result.c_str(),
				MAX_MSG_SIZE * sizeof(char), 0, &toAddr); //RCV_MSG_SIZE = 134
	}

#elif TRANS_PROTOCOL == USE_UDP
	d3_send_data( server_sock, buff1, sizeof(int32_t), 0, &toAddr );
	if(!result.empty()) { //return lookup result.
		r = d3_send_data(client_sock, (void*)result.c_str(), MAX_MSG_SIZE*sizeof(char), 0, &toAddr);//RCV_MSG_SIZE = 134
	}
#endif
	//raman-s
	//	}
	//	if (size < 0) {
	//		cout << "Received QUIT command." << endl;
	//		//do something joining/exiting...
	//	}
	//raman-e
	//raman-replication-s
	//raman-the insert/remove have not been unit tested when two or three servers
	//have failed or when a random server failes (not the primary)
	//	cout << "Package replication process begin: ..." << endl;

//v----------------------------- do replication ------------------------------------v
	//cout <<"in dbService: replicano() = "<< package.replicano()<<endl;
//	cout<<"in dbService: replicano() = "<< package.replicano() <<endl;//here problem: replicano is always 0, where it come from???

	/*
	 //Tony: this is a stupid and ugly patch for UDP: the package size 132 problem is still not solved.
	 #if	TRANS_PROTOCOL == USE_UDP
	 if(package.replicano() == 0){
	 cout<<"Stupid patch working: replicano changed from 0 to 5."<<endl;
	 package.set_replicano(5);
	 }
	 #endif
	 //----------------------------------------------
	 */

	if (package.replicano() == 5) { //only forward orginal copy:5--Tony
		cout << "tell if new package..." << endl;

		if (package.operation() == 3) { //insert
			int32_t numReplica = package.replicano();
			int i = NUM_REPLICAS;
			unsigned int n;
			package.set_replicano(3); //3 means it's not an original(5) request.:------here the definition of replicano are different:
			//mine is that replicano is a indicator for original copy, raman's is that it is the num of replicas, given by user.
			while (i > 0) { //numReplica was given by package.replicano,and this is always <==.
				//loop condition variable should be i
				n = myhash((package.virtualpath()).c_str(), nHost) + i;
				n = n % hostList.size();
				struct HostEntity destination = hostList.at(n);
				cout << "Replica insert: sent to " << destination.port
						<< " and before send replicano() = "
						<< package.replicano() << endl;
				//sleep(3);
				int ret = contactReplica(package, destination);
				//seems don't need to handle failure here
				//	cout << "Replica Insert: i = " << i << endl;
				//numReplica--;
				i--;
			}
			//return anything?
			/*			//-------------copied from end, for stop the thread
			 #if TRANS_PROTOCOL==USE_TCP
			 int closingSock = ((struct threaddata *) threadarg)->socket;
			 close(closingSock);
			 ((struct threaddata *) threadarg)->socket = -1;
			 #elif TRANS_PROTOCOL==USE_UDP
			 memset( &(((struct threaddata *) threadarg)->sockinfo), 0, sizeof(sockaddr_in) );
			 memset( (((struct threaddata *) threadarg)->buffer), 0, sizeof(RCV_MSG_SIZE) );
			 #endif
			 cout << "leaving thread..."<<endl;
			 pthread_mutex_lock(&mutex1);
			 numthreads--;
			 pthread_mutex_unlock(&mutex1);
			 cout << "After unlock mutex..."<<endl;
			 pthread_detach(pthread_self());//end thread.
			 cout << "After detach thread..."<<endl;
			 pthread_exit(NULL);
			 //-------------copied from end
			 */
		}

		if (package.operation() == 2) { //remove
			int32_t numReplica = package.replicano();
			int i = NUM_REPLICAS;
			unsigned int n;
			package.set_replicano(3);
			while (i > 0) { //change from numReplica to i
				n = myhash((package.virtualpath()).c_str(), nHost) + i;
				n = n % hostList.size();
				struct HostEntity destination = hostList.at(n);
				contactReplica(package, destination);
				cout << "Replica remove: sent to " << destination.port
						<< " and before send replicano() = "
						<< package.replicano() << endl;

//				cout << "Replica Remove: i = " << i << endl;
				//numReplica--;
				i--;
			}

			/*			//-------------copied from end, for stop the thread
			 #if TRANS_PROTOCOL==USE_TCP
			 int closingSock = ((struct threaddata *) threadarg)->socket;
			 close(closingSock);
			 ((struct threaddata *) threadarg)->socket = -1;
			 #elif TRANS_PROTOCOL==USE_UDP
			 memset( &(((struct threaddata *) threadarg)->sockinfo), 0, sizeof(sockaddr_in) );
			 memset( (((struct threaddata *) threadarg)->buffer), 0, sizeof(RCV_MSG_SIZE) );
			 #endif
			 cout << "leaving thread..."<<endl;
			 pthread_mutex_lock(&mutex1);
			 numthreads--;
			 pthread_mutex_unlock(&mutex1);
			 cout << "After unlock mutex..."<<endl;
			 pthread_detach(pthread_self());//end thread.
			 cout << "After detach thread..."<<endl;
			 pthread_exit(NULL);
			 //-------------copied from end
			 */
		}

	}
	//raman-replication-e
#if TRANS_PROTOCOL==USE_TCP
	int closingSock = ((struct threaddata *) threadarg)->socket;
	close(closingSock);
	((struct threaddata *) threadarg)->socket = -1; //this should work but seems it donesn't.-----------------????
#elif TRANS_PROTOCOL==USE_UDP
			memset( &(((struct threaddata *) threadarg)->sockinfo), 0, sizeof(sockaddr_in) );
			memset( (((struct threaddata *) threadarg)->buffer), 0, sizeof(MAX_MSG_SIZE) );
#endif
	//cout << "leaving thread..."<<endl;
	pthread_mutex_lock(&mutex1);
	numthreads--;
	pthread_mutex_unlock(&mutex1);
	//cout << "Ending thread: "<< pthread_self() <<endl;

	pthread_detach(pthread_self()); //end thread.
	cout << "After detach thread..." << endl << endl << endl;
	pthread_exit(NULL);
}
//raman-replication-s

/*
 vector<struct HostEntity> getMembership(string fileName) {
 vector<struct HostEntity> hostList;
 ifstream in(fileName.c_str(), ios::in);
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
 fprintf(stderr, "inet_aton() failed\n");
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
 }*/

//raman-replication-e
int main(int argc, char* argv[]) {

	cout << "Use: hash <port> <neighbor_list_file>" << endl;
	cout << "Start Server..." << endl;
//	ProtoHashDB db; //in-mem hash
	srand(kyotocabinet::getpid() + clock());
	if (setconfigvariables() != 0) {
		cout << "Not able to read configuration file." << endl;
		exit(1);
	}

	int rcv_size, i, client_sock, new_req_indicator, r, thread_status;
	char tmp_buf[MAX_MSG_SIZE];
	sockaddr_in tmp_sockaddr;
	struct threaddata threaddata_array[MAX_THREADS];
	pthread_t thread[MAX_THREADS];
	//raman-sigpipe-s
	struct sigaction act;
	memset(&act, '\0', sizeof(act));
	act.sa_sigaction = &sigpipe_handler;
	act.sa_flags = SA_SIGINFO;
	if (sigaction(SIGPIPE, &act, NULL) < 0) {
		cout << "Error in sigaction." << endl;
		exit(0);
	}
	//raman-sigpipe-e
	int svrPort = kyotocabinet::atoi(argv[1]);
	if (!db.open("mybase", ProtoHashDB::OWRITER | ProtoHashDB::OCREATE)) {
		cerr << "open error: " << db.error().name() << endl;
	}
	//raman-replication-s
	string membershipFile(argv[2]);
	hostList = getMembership(membershipFile);
	nHost = hostList.size();
	//raman-replication-e
	for (int i = 0; i < MAX_THREADS; i++) { //all 400 slots are set to -1
#if TRANS_PROTOCOL==USE_TCP
		threaddata_array[i].socket = -1;
#elif TRANS_PROTOCOL==USE_UDP
		memset( &(threaddata_array[i].sockinfo), 0, sizeof(sockaddr_in) );
		memset( &(threaddata_array[i].buffer), 0, sizeof(MAX_MSG_SIZE) );
#endif
	}
	cout << endl << "main: makeSocket -----" << endl;
	server_sock = d3_svr_makeSocket(svrPort);

	while (server_sock > 0) {
		new_req_indicator = 0;
#if TRANS_PROTOCOL==USE_TCP
		client_sock = d3_svr_accept(server_sock);
		new_req_indicator = client_sock;
#elif TRANS_PROTOCOL==USE_UDP
		memset( tmp_buf, 0, sizeof(tmp_buf) );
		memset( &tmp_sockaddr, 0, sizeof(sockaddr_in) );
		rcv_size = d3_svr_recv( server_sock, tmp_buf, MAX_MSG_SIZE, 0, &tmp_sockaddr );
		new_req_indicator = tmp_sockaddr.sin_port;
#endif
		if (new_req_indicator > 0) { //
			//can we use the rcv_size in udp, for this if loop.that looks little more appropriate
			if (numthreads < MAX_THREADS) {
				pthread_mutex_lock(&mutex1);
				numthreads++;
				pthread_mutex_unlock(&mutex1);
				for (int i = 0; i < MAX_THREADS; i++) { //??? problem is here ~~~~~~~~~~
#if TRANS_PROTOCOL == USE_TCP
					thread_status = threaddata_array[i].socket;
#elif TRANS_PROTOCOL == USE_UDP
					thread_status = threaddata_array[i].sockinfo.sin_port;
#endif
					if (thread_status <= 0) { // sock <= 0
#if TRANS_PROTOCOL == USE_TCP
						threaddata_array[i].socket = client_sock; //----------???maybe reason why repeat?
						//cout << "new socket= "<<threaddata_array[i].socket<<endl;
						r = pthread_create(&thread[i], NULL, dbService,
								(void *) &threaddata_array[i]);
						//cout << "Main: new thread created: " << r << endl;
						//cout << " i=  " << i<< endl;

						//cout << "Thread ended, now socket= "<<threaddata_array[i].socket<<endl;

#elif TRANS_PROTOCOL == USE_UDP
						threaddata_array[i].sockinfo = tmp_sockaddr;
						strcpy( threaddata_array[i].buffer, tmp_buf );
						r = pthread_create(&thread[i], NULL, dbService, (void *) &threaddata_array[i]);
#endif
						if (r != 0) {
							cout << "Error creating thread: " << strerror(errno)
									<< endl;
						}
						if (r == 0)
							break;
					}
				} //--------end for--------------
			} else {
				cout
						<< "Connection refused because no more threads can be formed\n"
						<< endl;
			} //----------end else---------------
		} // ------------end if--------------
	} //--------end while---------
	d3_closeConnection(server_sock);
	if (!db.close()) {
		cerr << "close error: " << db.error().name() << endl;
	}
	return 0;
}

