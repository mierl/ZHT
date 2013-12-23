//#include <kchashdb.h>
//#include <kcprotodb.h>
//#include <string>
//#include <cstring>
//#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
//#include <netdb.h>
#include "zht_util.h"
//#include "meta.pb.h"
//#include "d3_transport.h"
//#include "d3_sys_globals.h"
#include <fcntl.h>
#include <pthread.h>
#include <iomanip>
#include <map>

#include "novoht.h" //Kevin's implementation of persistent hash table

using namespace std;
//using namespace kyotocabinet;

#define MAX_THREADS 1000 //from 400
//#define MAX_MSG_SIZE 1024//a fixed package is neither reasonable nor practicable.

//HashDB db;//file based hash
//ProtoHashDB db; //in-mem hash
NoVoHT *pmap; //move to main().
map<string, string> hmap;
//An in-memory hash table



int turn_off;



//int NUM_REPLICAS;
//int REPLICATION_TYPE;//1 for Client-side replication

void setnonblock(int fd)
{
  int flags;

  flags = fcntl(fd, F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(fd, F_SETFL, flags);
}


#if TRANS_PROTOCOL==USE_TCP
struct threaddata {
	int socket;
	NoVoHT *p_pmap;
};

#elif TRANS_PROTOCOL==USE_UDP
struct threaddata {
	struct sockaddr_in sockinfo;
	char buffer[MAX_MSG_SIZE];
	NoVoHT *pmap;
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
/*
 int HB_insert(DB &db, Package package) {
 //int opt = package.operation();//opt not be used?
 string package_str = package.SerializeAsString();
 int ret = db.set(package.virtualpath(), package_str); //virtualpath as key
 if (ret == 0) {
 return -3; //insert fail.
 }

 //cout << "String insted: " << package_str << endl;
 return 0;
 }
 */

int HB_insert(NoVoHT *map, Package &package) {
	//int opt = package.operation();//opt not be used?
	string package_str = package.SerializeAsString();
	//int ret = db.set(package.virtualpath(), package_str); //virtualpath as key
//	cout<<"Insert to pmap..."<<endl;
	string key = package.virtualpath();
//	cout<<"key:"<<key<<endl;
	string value = package_str;
//	cout<<"value:"<<value<<endl;
//	cout<<"Insert: k-v ready. put..."<<endl;
	int ret = map->put(key, value);
//	cout << "end inserting, ret = " << ret << endl;

	if (ret != 0) {
		return -3;
	}
	/*
	 cout << "String insted: " << package_str << endl;
	 */
	else
		return 0;
}

int HB_insert(map<string, string> &hmap, Package &package){
	//int opt = package.operation();//opt not be used?
		string package_str = package.SerializeAsString();
		//int ret = db.set(package.virtualpath(), package_str); //virtualpath as key
	//	cout<<"Insert to pmap..."<<endl;
		string key = package.virtualpath();
	//	cout<<"key:"<<key<<endl;
		string value = package_str;
	//	cout<<"value:"<<value<<endl;
	//	cout<<"Insert: k-v ready. put..."<<endl;


		//pair<map<string,string>::iterator,bool> ret;
		pair<map<string,string>::iterator,bool> ret;
		ret = hmap.insert(pair<string, string> (key, value));

		if (ret.second == false) {
			return -3;
		}
		else
			return 0;
}

string HB_lookup(map<string, string> &hmap, Package &package) {
	string value;
//		cout << "lookup in HB_lookup" << endl;
		string key = package.virtualpath();
//		cout << "key:" << key << endl;
		map<string, string>::iterator it;
		it = hmap.find(key);
		if (it == hmap.end()){
			string nullString = "Empty";
					return nullString;
		}
		return (*it).second;
}

/*
 string HB_lookup(DB &db, Package package) {
 string value;
 db.get(package.virtualpath(), &value); //new kyoto version: 60
 return value;
 }*/

string HB_lookup(NoVoHT *map, Package &package) {
	string value;
//	cout << "lookup in HB_lookup" << endl;
	string key = package.virtualpath();
//	cout << "key:" << key << endl;
	string *strP = map->get(key); //problem
//	cout << "lookup end." << endl;

	if (strP == NULL) {
		cout << "lookup find nothing." << endl;
		string nullString = "Empty";
		return nullString;
	}
	return *strP;
}



int HB_remove(NoVoHT *map, Package &package) {
	string key = package.virtualpath();
	int ret = map->remove(key); // return 0 means correct.
	if (ret != 0) {
		cout << "DB Error: fail to remove :ret= " << ret << endl;
		return -2;
	} else
		return 0; //succeed.
}

int HB_remove(map<string, string> &hmap, Package &package){
	unsigned int r = hmap.erase(package.virtualpath());
			if(r==0){
				cout<<"Remove nothing, no match found."<<endl;
				return -1;
			}
	return 0;

}
// This function is to send a package to a GIVEN replica, it dones't choose which to send.
// It works as a client.
int contactReplica(Package package, struct HostEntity destination) {

	//cout<< "contactReplica: newly received msg: package.replicano= " << package.replicano()<< ", package.ByteSize()="<< package.ByteSize() <<endl;//here the correct package was received: -1

	int client_sock, r = 0;
	int32_t str_size;
	string hostName, str;
	sockaddr_in toAddr, recv_addr;

#if TRANS_PROTOCOL == USE_TCP
	client_sock = d3_makeConnection((destination.host).c_str(),
			destination.port);
#elif TRANS_PROTOCOL == USE_UDP

//	cout<<endl<<"contactReplica makeSocket start-----"<<endl;
	client_sock = d3_svr_makeSocket( (time(NULL)%10000)+rand()%10000 ); //client can use any port to talk to server
//	client_sock = d3_svr_makeSocket(destination.port);
//	cout<<"contactReplica makeSocket end-----"<<endl<<endl;
#endif       
	if (client_sock < 0) { //what's this mechanism: only report error, doesn't handle it
		int me = myhash((package.virtualpath()).c_str(), nHost);
		//since dbService already take over the routing, this will to extra map which is unnecessary and wrong.

//		cout << "contactReplica:get msg: package.replicano= " << package.replicano() << endl;
		me = me % hostList.size();
		struct HostEntity source = hostList.at(me);
		cerr << "Error connecting to replica: " << (destination.host).c_str()
				<< " : " << destination.port << " .Primary sever: "
				<< (source.host).c_str() << " : " << source.port << ". "
				<< strerror(errno) << endl; //This error msg is misleading.
		return -1;
	}
	str = package.SerializeAsString();
	str_size = str.length();
	toAddr = d3_make_sockaddr_in_client((destination.host).c_str(),
			destination.port);
//	cout << "contactReplica trying to reach host:" << destination.host << ", port:" << destination.port << endl;
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




int socket_replica(Package package, struct HostEntity destination){
	string str = package.SerializeAsString();

	int to_sock = socket(PF_INET, SOCK_STREAM, 0);//try change here.................................................
	int optval=1;

	if(setsockopt(to_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval)<0) cerr<<"replica: reuse failed."<<endl;
	if(to_sock<0){
			cerr<<"socket_replica: error on socket(): "<< strerror(errno) << endl;
			return -1;
		}
	//socket(nmspace,style,protocol), originally be socket(AF_INET, SOCK_STREAM, 0)



	struct sockaddr_in dest, recv_addr;
	memset(&dest, 0, sizeof(struct sockaddr_in));                        /*zero the struct*/
	struct hostent * hinfo = gethostbyname(destination.host.c_str());
	    if(hinfo == NULL)printf("getbyname failed!\n");
	    dest.sin_family = PF_INET;                                          /*storing the server info in sockaddr_in structure*/
	    dest.sin_addr =  *(struct in_addr *)(hinfo->h_addr);                /*set destination IP number*/
	    dest.sin_port = htons(destination.port);

	int ret_con = connect(to_sock, (struct sockaddr *)&dest, sizeof(sockaddr));
	if(ret_con<0){
		cerr<<"socket_replica: error on connect(): "<< strerror(errno) << endl;
		return -1;
	}

	int ret_snd = send(to_sock, (const void*)str.c_str(),str.size(),0 );// may try other flags......................
	if(ret_snd<0){
		cerr<<"socket_replica: error on socket(): "<< strerror(errno) << endl;
					return -1;

	}

	void *buff_return = (void*) malloc(sizeof(int32_t));
	int r = d3_svr_recv(to_sock, buff_return, sizeof(int32_t), 0, &recv_addr);
	//connect (int socket, struct sockaddr *addr, size_t length)
	if(r<0){
		cerr<<"socket_replica: got bad news from relica: "<< r <<endl;
	}

	close(to_sock);
}


void *dbService(void *threadarg) {
	srand(getpid() + clock());
	//srand(kyotocabinet::getpid() + clock());
	//cout << "Current service thread ID = " << pthread_self()<< ", dbService() begin..." << endl;

	char buff[MAX_MSG_SIZE];
	int32_t operation_status;
	sockaddr_in toAddr;
	int client_sock, r;
	void* buff1;
	//buff1 = (void*) malloc(sizeof(int32_t));
	NoVoHT *pmap = ((struct threaddata *) threadarg)->p_pmap;

#if TRANS_PROTOCOL ==USE_TCP
	client_sock = ((struct threaddata *) threadarg)->socket;
#elif TRANS_PROTOCOL == USE_UDP
	toAddr = ((struct threaddata *) threadarg)->sockinfo;
	strcpy( buff, ((struct threaddata *) threadarg)->buffer ); //here the buffer(it seems a global variable) should be refreshed so clean it.

#endif

	//raman-s
	//	if (size > 0) { //if size < 0, consider it as a "quit command"
	//raman-e
//	cout << "Thread: receive request from client..." << endl;
#if TRANS_PROTOCOL == USE_TCP
	r = d3_svr_recv(client_sock, buff, MAX_MSG_SIZE * sizeof(char), 0, &toAddr);
	if (r <= 0) {
		cout << "Server could not recv data" << endl;
	}
#endif
	Package package;
	package.ParseFromArray(buff, MAX_MSG_SIZE);
	string result;
//	cout << endl << endl << "in dbService: received replicano = " << package.replicano() << endl;

//	cout << "package size: " << package.ByteSize() << endl;
//	cout <<"Package content: "<< buff<<endl;
	switch (package.operation()) {

	case 3: //insert
//		cout << "Insert..." << endl;
		//operation_status = HB_insert(db, package);
		operation_status = HB_insert(hmap, package);
//cout<<"Inserted: key: "<< package.virtualpath()<<endl;
//		cout << "insert finished, return: " << operation_status << endl;
		buff1 = &operation_status;
		r = d3_send_data(client_sock, buff1, sizeof(int32_t), 0, &toAddr);
		if (r <= 0) {
			cout << "Server could not send acknowledgement to client" << endl;
		}
		break;
	case 1: //lookup
//		cout << "Lookup..." << endl;
		if (package.virtualpath().empty()) {
			cerr << "Bad key: nothing to find" << endl;
			operation_status = -1;
		} else {
			//result = HB_lookup(db, package);
//			cout << "Lookup...2" << endl;
//cout<<"Will lookup key: "<< package.virtualpath()<<endl;
			result = HB_lookup(hmap, package);
//			cout << "Lookup...3" << endl;
			//don't really send result back to client now, do it latter.
			if (result.compare("Empty") == 0) {
				operation_status = -2;
				buff1 = &operation_status;

			} else { //find result, send back here

//				cout << "Lookup: Result found, sending back..." << endl;
//				cout << "What I found on server: \n";
//				cout << result.c_str() << endl;
				//=====================================
				//how to know where to send back? Use the one that created when receive request as above.

				//d3_send_data(client_sock, (void*)result.c_str(),result.length(), 0, &toAddr);
				//just a try, not use simpleSend, because no need to make a new socket and connection..
				if (TRANS_PROTOCOL == USE_TCP) {
					d3_send_data(client_sock, (void*) result.c_str(),
							result.length(), 0, &toAddr); //TCP reuse the socket and connection.

				} else if (TRANS_PROTOCOL == USE_UDP) {

					d3_send_data(server_sock, (void*) (result.c_str()),
							result.length() + 1, 0, &toAddr);

				}

			}
		}
		break;
	case 2: //remove
//cout << "Remove..." << endl;
//cout << "Package:key "<<package.virtualpath()<<endl;
		if (package.virtualpath().empty()) {
			cerr << "Bad key: nothing to remove" << endl;
			operation_status = -1;
		} else {
			//operation_status = HB_remove(db, package);
			operation_status = HB_remove(hmap, package);
			buff1 = &operation_status;
			r = d3_send_data(client_sock, buff1, sizeof(int32_t), 0, &toAddr);
			if (r <= 0) {
				cout << "Server could not send acknowledgement to client"
						<< endl;
			}
//			cout << "Remove succeeded, return " << operation_status << endl;
		}
		break;
	case 99: //shut the server
//		cout << "Server will be shut shortly." << endl;
		turn_off = 1; //turn off service.
		break;
	default:
		operation_status = -99; //no this operation
		break;
	}
	buff1 = &operation_status;

#if TRANS_PROTOCOL == USE_TCP
	/*	cout << "Final returned: "<<operation_status<<endl;
	 r = d3_send_data(client_sock, buff1, sizeof(int32_t), 0, &toAddr);
	 if (r <= 0) {
	 cout << "Server could not send acknowledgement to client" << endl;
	 }*/
	/*	if (!result.empty()) { //return lookup result.
	 r = d3_send_data(client_sock, (void*) result.c_str(),
	 MAX_MSG_SIZE * sizeof(char), 0, &toAddr); //RCV_MSG_SIZE = 134
	 }*/

#elif TRANS_PROTOCOL == USE_UDP
	/* feedback twice here, commented	
	 d3_send_data( server_sock, buff1, sizeof(int32_t), 0, &toAddr );
	 if(!result.empty()) { //return lookup result.
	 r = d3_send_data(client_sock, (void*)result.c_str(), MAX_MSG_SIZE*sizeof(char), 0, &toAddr);//RCV_MSG_SIZE = 134
	 }
	 */
#endif

#if TRANS_PROTOCOL==USE_TCP
	int closingSock = ((struct threaddata *) threadarg)->socket;
	close(closingSock);
	((struct threaddata *) threadarg)->socket = -1; //this should work but seems it donesn't.-----------------????
#elif TRANS_PROTOCOL==USE_UDP
			memset( &(((struct threaddata *) threadarg)->sockinfo), 0, sizeof(sockaddr_in) );
			memset( (((struct threaddata *) threadarg)->buffer), 0, sizeof(MAX_MSG_SIZE) );
#endif

	if (package.replicano() == 5) { //only forward orginal copy:5--Tony
//		cout << "tell if new package..." << endl;

		if (package.operation() == 3) { //insert
//			int32_t numReplica = package.replicano();
			int i = NUM_REPLICAS;
			unsigned int n;
			package.set_replicano(3); //3 means it's not an original(5) request.:------here the definition of replicano are different:
			//mine is that replicano is a indicator for original copy, raman's is that it is the num of replicas, given by user.
			while (i > 0) { //numReplica was given by package.replicano,and this is always <==.
				//loop condition variable should be i
				n = myhash((package.virtualpath()).c_str(), nHost) + i;
				n = n % hostList.size();
				struct HostEntity destination = hostList.at(n);
//				cout << "Replica insert: sent to " << destination.port	<< " and before send replicano() = " << package.replicano() << endl;
				//sleep(3);
				int ret = socket_replica(package, destination);
				//the only thing this function does is to send a package to dest, that's it!!!!
				//seems don't need to handle failure here
				//	cout << "Replica Insert: i = " << i << endl;
				//numReplica--;
				i--;
			}

		}

		if (package.operation() == 2) { //remove
//			int32_t numReplica = package.replicano();
			int i = NUM_REPLICAS;
			unsigned int n;
			package.set_replicano(3);
			while (i > 0) { //change from numReplica to i
				n = myhash((package.virtualpath()).c_str(), nHost) + i;
				n = n % hostList.size();
				struct HostEntity destination = hostList.at(n);
				socket_replica(package, destination);
//				cout << "Replica remove: sent to " << destination.port 	<< " and before send replicano() = "<< package.replicano() << endl;

//				cout << "Replica Remove: i = " << i << endl;
				//numReplica--;
				i--;
			}
		}
	}
	//raman-replication-e

	//cout << "leaving thread..."<<endl;
	pthread_mutex_lock(&mutex1);
	numthreads--;
	pthread_mutex_unlock(&mutex1);
	//cout << "Ending thread: "<< pthread_self() <<endl;

	pthread_detach(pthread_self()); //end thread.
//	cout << "After detach thread..." << endl;
//	cout << "End operation: " << package.operation() << endl << endl;
	//free(buff1);
	pthread_exit(NULL);
}
//raman-replication-s

int main(int argc, char* argv[]) {

//	cout << "Use: hash <port> <neighbor_list_file>" << endl;
//	cout << "Start Server..." << endl;
//	ProtoHashDB db; //in-mem hash
	string cfgFile(argv[3]);
	

//	const string cmd = "cat /proc/personality.sh | grep BG_PSETORG";
//      string torusID = exec(cmd);
//        torusID.resize(torusID.size()-1);
 //       srand( getTime_msec()+ myhash(torusID.c_str(), 10000000) );
		srand(getTime_msec() + getpid());

	if (setconfigvariables(cfgFile) != 0) {
		cout << "Server: Not able to read configuration file." << endl;
		exit(1);
	}

	int client_sock, new_req_indicator, r, thread_status;
//	char tmp_buf[MAX_MSG_SIZE];
//	sockaddr_in tmp_sockaddr;
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
	int svrPort = atoi(argv[1]);

//	string randStr = randomString(5);
//	string fileName = "hashmap.data"; //= "hashmap.data."+randStr;
//	pmap = new NoVoHT(fileName, 100, 10, 0.7);
//	map<string, string> hashMap;
//	*hmap = hashMap;
	//raman-replication-s
	string membershipFile(argv[2]);
	hostList = getMembership(membershipFile);
	nHost = hostList.size();
	//raman-replication-e
	for (int i = 0; i < MAX_THREADS; i++) { //all 400 slots are set to -1
#if TRANS_PROTOCOL==USE_TCP
		threaddata_array[i].socket = -1;
		threaddata_array[i].p_pmap = pmap;
#elif TRANS_PROTOCOL==USE_UDP
		memset( &(threaddata_array[i].sockinfo), 0, sizeof(sockaddr_in) );
		memset( &(threaddata_array[i].buffer), 0, sizeof(MAX_MSG_SIZE) );
		threaddata_array[i].p_pmap = pmap;
#endif
	}
//	cout << endl << "main: makeSocket -----" << endl;
	server_sock = d3_svr_makeSocket(svrPort);

	turn_off = 0;
	if (server_sock > 0) {
	
		//report: I'm now serving!
		//ofstream myfile;
		//stringstream ss;

		//string tmpStr = "/intrepid-fs0/users/tonglin/persistent/Register_";
		//ss << tmpStr << nHost;
		//string nNodes = ss.str();
		//myfile.open(ss.str().c_str());
		//myfile << torusID << "\n";
		//myfile.close();
//		system("echo $IP >> /intrepid-fs0/users/tonglin/persistent/Register_$NNODE");

		
		while (turn_off == 0) {
//			cout << "while: turn_off = " << turn_off << endl;

			new_req_indicator = 0;
#if TRANS_PROTOCOL==USE_TCP
//			cout << "begin accept" << endl;
			client_sock = d3_svr_accept(server_sock);


			//set reuse socket
			int optval=1;
			setsockopt(client_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
			//setnonblock(client_sock);//set non-blocking: cause all error.
//			cout << "end accept" << endl;
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
					srand(getpid() + clock() + getTime_usec());
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
								cout << "Error creating thread: "
										<< strerror(errno) << endl;
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

			if (turn_off == 1) {
				d3_closeConnection(server_sock);
				cout << "Server turn off." << endl;
				return 0;
			}
		}

	}//--------end if (server_sock > 0)---------

	/*
	 if (!db.close()) {
	 cerr << "close error: " << db.error().name() << endl;
	 }*/
	cout << "Server turn off." << endl;
	return 0;
}

