

#include <stdlib.h>


#include "zht_util.h"

#include <pthread.h>
#include <iomanip>
#include "novoht.h" //Kevin's implementation of persistent hash table
using namespace std;





#define MAX_THREADS 1000 //from 400

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

int HB_insert(NoVoHT &map, Package &package) {
	//int opt = package.operation();//opt not be used?
	string package_str = package.SerializeAsString();
	//int ret = db.set(package.virtualpath(), package_str); //virtualpath as key
//	cout<<"Insert to pmap..."<<endl;
	string key = package.virtualpath();
//	cout<<"key:"<<key<<endl;
	string value = package_str;
//	cout<<"value:"<<value<<endl;
//	cout<<"Insert: k-v ready. put..."<<endl;
	int ret = map.put(key,value);
	cout<<"end inserting, ret = "<<ret<<endl;

	if (ret != 0) {	return -3; 	}
	/*
	 cout << "String insted: " << package_str << endl;
	 */
	else
		return 0;
}




string HB_lookup(NoVoHT &map, Package &package) {
	string value;
	cout<<"lookup in HB_lookup"<<endl;
	string key = package.virtualpath();
	cout<<"key:"<<key<<endl;
	string *strP = map.get(key); //problem
	cout<<"lookup end."<<endl;

  if(strP == NULL){
	  cout<<"lookup find nothing."<<endl;
	  string nullString = "Empty";
	  return nullString;}
	return *strP;
}

int HB_remove(NoVoHT &map, Package &package) {
	string key = package.virtualpath();
	int ret = map.remove(key);// return 0 means correct.
	if (ret != 0) {
		cout << "DB Error: fail to remove :ret= " << ret << endl;
		return -2;
	}
	else
		return 0; //succeed.
}



// This function is to send a package to a GIVEN replica, it dones't choose which to send
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
 int dbService(int socket, NoVoHT *pmap) {
	srand(getpid() + clock());
	//srand(kyotocabinet::getpid() + clock());
	//cout << "Current service thread ID = " << pthread_self()<< ", dbService() begin..." << endl;

	char buff[MAX_MSG_SIZE];
	int32_t operation_status;
	sockaddr_in toAddr;
	int client_sock, r;
	void* buff1;
	buff1 = (void*) malloc(sizeof(int32_t));
	//NoVoHT *pmap = ((struct threaddata *) threadarg)->p_pmap;

#if TRANS_PROTOCOL ==USE_TCP
	//client_sock = ((struct threaddata *) threadarg)->socket;
	client_sock = socket;
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

	case 99: //shut the server
		cout << "Server will be shut shortly."<<endl;


	case 3: //insert
		cout << "Insert..."<< endl;
		//operation_status = HB_insert(db, package);
		operation_status = HB_insert(*pmap, package);
		cout<<"insert finished, return: "<<operation_status<<endl;
		buff1 = &operation_status;
					r = d3_send_data(client_sock, buff1, sizeof(int32_t), 0, &toAddr);
						if (r <= 0) {
							cout << "Server could not send acknowledgement to client" << endl;
						}
		break;
	case 1: //lookup
		cout << "Lookup..."<< endl;
		if (package.virtualpath().empty()) {
			cerr << "Bad key: nothing to find" << endl;
			operation_status = -1;
		} else {
			//result = HB_lookup(db, package);
			cout << "Lookup...2"<< endl;
			result = HB_lookup(*pmap, package);
			cout << "Lookup...3"<< endl;
			//don't really send result back to client now, do it latter.
			if (result.compare("Empty")==0) {
				operation_status = -2;
				buff1 = &operation_status;

			} else { //find result, send back here

				cout << "Lookup: Result found, sending back..." << endl;
				cout << "What I found on server: \n";
				cout << result.c_str() <<endl;
				if (TRANS_PROTOCOL == USE_TCP) {
					d3_send_data(client_sock, (void*) result.c_str(), result.length(), 0, &toAddr); //TCP reuse the socket and connection.

				} else if (TRANS_PROTOCOL == USE_UDP) {

					d3_send_data(server_sock, (void*)(result.c_str()), result.length()+1, 0, &toAddr);

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
			//operation_status = HB_remove(db, package);
			operation_status = HB_remove(*pmap, package);
			buff1 = &operation_status;
			r = d3_send_data(client_sock, buff1, sizeof(int32_t), 0, &toAddr);
				if (r <= 0) {
					cout << "Server could not send acknowledgement to client" << endl;
				}
			cout << "Remove succeeded, return "<< operation_status<< endl;
		}
		break;
	default:
		operation_status = -99;//no this operation
		break;
	}
	buff1 = &operation_status;



	if (package.replicano() == 5) { //only forward orginal copy:5--Tony
		cout << "tell if new package..." << endl;

		if (package.operation() == 3) { //insert
//			int32_t numReplica = package.replicano();
			int i = NUM_REPLICAS;
			unsigned int n;
			package.set_replicano(3); //3 means it's not an original(5) request.:------here the definition of replicano are different:
			//mine is that replicano is a indicator for original copy, raman's is that it is the num of replicas, given by user.
			while (i > 0) { //numReplica was given by package.replicano,and this is always <==.

				n = myhash((package.virtualpath()).c_str(), nHost) + i;
				n = n % hostList.size();
				struct HostEntity destination = hostList.at(n);
				cout << "Replica insert: sent to " << destination.port
						<< " and before send replicano() = "
						<< package.replicano() << endl;
				//sleep(3);
				int ret = contactReplica(package, destination);

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
				contactReplica(package, destination);
				cout << "Replica remove: sent to " << destination.port
						<< " and before send replicano() = "
						<< package.replicano() << endl;

//				cout << "Replica Remove: i = " << i << endl;
				//numReplica--;
				i--;
			}


		}

	}
	//raman-replication-e
#if TRANS_PROTOCOL==USE_TCP
	int closingSock = socket;//= ((struct threaddata *) threadarg)->socket;
	close(closingSock);
//	((struct threaddata *) threadarg)->socket = -1; //this should work but seems it donesn't.-----------------????
#elif TRANS_PROTOCOL==USE_UDP
			memset( &(((struct threaddata *) threadarg)->sockinfo), 0, sizeof(sockaddr_in) );
			memset( (((struct threaddata *) threadarg)->buffer), 0, sizeof(MAX_MSG_SIZE) );
#endif
	//cout << "leaving thread..."<<endl;
//	pthread_mutex_lock(&mutex1);
//	numthreads--;
//	pthread_mutex_unlock(&mutex1);
	//cout << "Ending thread: "<< pthread_self() <<endl;

//	pthread_detach(pthread_self()); //end thread.
	cout << "After detach thread..." << endl;
	cout<<"End operation: "<<package.operation()<< endl << endl;
	free(buff1);
//	pthread_exit(NULL);
	return 0;
}
//raman-replication-s


int main(int argc, char* argv[]) {

	cout << "Use: hash <port> <neighbor_list_file>" << endl;
	cout << "Start Server..." << endl;

	string cfgFile(argv[3]);
	srand(getpid() + clock());
	if (setconfigvariables(cfgFile) != 0) {
		cout << "Server: Not able to read configuration file." << endl;
		exit(1);
	}

	int client_sock, new_req_indicator, r, thread_status;

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


	string randStr = randomString(5);
    string fileName = "";//= "hashmap.data."+randStr;
    NoVoHT pmap; //new
	pmap = NoVoHT(fileName, 100, 10, 0.7);


	string membershipFile(argv[2]);
	hostList = getMembership(membershipFile);
	nHost = hostList.size();

	for (int i = 0; i < MAX_THREADS; i++) { //all 400 slots are set to -1
#if TRANS_PROTOCOL==USE_TCP
		threaddata_array[i].socket = -1;
		threaddata_array[i].p_pmap = &pmap;
#elif TRANS_PROTOCOL==USE_UDP
		memset( &(threaddata_array[i].sockinfo), 0, sizeof(sockaddr_in) );
		memset( &(threaddata_array[i].buffer), 0, sizeof(MAX_MSG_SIZE) );
		threaddata_array[i].p_pmap = &pmap;
#endif
	}
	cout << endl << "main: makeSocket -----" << endl;
	server_sock = d3_svr_makeSocket(svrPort);

	int turn_off = 0;

	while (server_sock > 0) {
		new_req_indicator = 0;
#if TRANS_PROTOCOL==USE_TCP
		cout<<"begin accept"<<endl;
		client_sock = d3_svr_accept(server_sock);
		cout<<"end accept"<<endl;
		new_req_indicator = client_sock;
#elif TRANS_PROTOCOL==USE_UDP
		memset( tmp_buf, 0, sizeof(tmp_buf) );
		memset( &tmp_sockaddr, 0, sizeof(sockaddr_in) );
		rcv_size = d3_svr_recv( server_sock, tmp_buf, MAX_MSG_SIZE, 0, &tmp_sockaddr );
		new_req_indicator = tmp_sockaddr.sin_port;
#endif
		if (new_req_indicator > 0) { //

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

						//r = pthread_create(&thread[i], NULL, dbService,	(void *) &threaddata_array[i]);
						r = dbService(client_sock, &pmap);


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
/*
	if (!db.close()) {
		cerr << "close error: " << db.error().name() << endl;
	}*/

	pmap.~NoVoHT(); //free.

	return 0;
}

