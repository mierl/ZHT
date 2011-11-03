#include <kchashdb.h>
#include <kcprotodb.h>
#include <string>
#include <cstring>
#include <sys/time.h>
#include <stdlib.h>
using namespace std;
using namespace kyotocabinet;

#include "meta.pb.h"
#include "d3_transport.h"
HashDB db;
//---------thread -----------
#include <pthread.h>
#define MAX_THREADS 400
struct threaddata {
	int socket;
};
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int numthreads = 0;
//---------thread function-----------


struct timeval tp;

double getTime_usec() {
	gettimeofday(&tp, NULL);
	return static_cast<double> (tp.tv_sec) * 1E6
			+ static_cast<double> (tp.tv_usec);

}

double getTime_msec() {
	gettimeofday(&tp, NULL);
	return static_cast<double> (tp.tv_sec) * 1E3
			+ static_cast<double> (tp.tv_usec) / 1E3;

}

double getTime_sec() {
	gettimeofday(&tp, NULL);
	return static_cast<double> (tp.tv_sec) + static_cast<double> (tp.tv_usec)
			/ 1E6;

}

string randomString(int len) {
	string s(len, ' ');
	static const char alphanum[] = "0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	//s[len] = 0;
	return s;
}

int HB_insert(DB &db, Package package) {
	int opt = package.operation();
	//0=insert, 1=find, 2=remove.
	string package_str = package.SerializeAsString();
	//---------

	int ret = db.set(package.virtualpath(), package_str); //virtualpath as key
	if (ret == 0) {
		//	cerr << "set error: " << db.error().name() << endl;
		return -3;
	}
	return 0;

}
/*
 string HB_find(DB &db, string key) {//&
 return *(db.get(key));

 }
 */

int HB_remove(DB &db, string key) {
	int ret = db.remove(key);
	if (ret == 0) {
		//		cerr << "remove error: " << db.error().name() << endl;
		return -2;
	}
	return 0;
}

int returnState(int client_sock, int32_t state) {
	void* buff1 = (void*) malloc(sizeof(int32_t));
	memset(buff1, 0, sizeof(int32_t));
	buff1 = &state;
	if (!d3_send_data(client_sock, buff1, sizeof(int32_t), 0)) {
//		d3_closeConnection(client_sock);
		return -4;
	};
//	d3_closeConnection(client_sock);
	return 0;
}
void *dbService(void *threadarg) {
	cout<<"New thread created: "<<pthread_self()<<" | ";
	int client_sock = ((struct threaddata*) threadarg)->socket;
	//	while ((client_sock = d3_svr_accept(server_sock)) > 0) {//
	int32_t size = 0;
	//----------------working part----------------------------------
	//-----------receive packages
	void *buff = (void*) malloc(sizeof(int32_t));//this many bytes
//	cout<<pthread_self()<<" recv size"<<" | ";
//	d3_svr_recv(client_sock, buff, sizeof(int32_t), 0);
//	size = *(int32_t*) buff;//A package will come, size.
size=132;
//        cout<<pthread_self()<<buff<<" | ";
	if (size > 0) { //if size < 0, consider it as a "quit command"
		buff = (void*) realloc(buff, size * sizeof(char));
//		cout<<"Thread "<<pthread_self()<<"out of realloc"<<" | ";
		d3_svr_recv(client_sock, buff, size * sizeof(char), 0);
//		cout<<"Thread "<<pthread_self()<<"recvd msg"<<" | ";
		//			d3_closeConnection(client_sock);
		//store in DB
		Package package;
		package.ParseFromArray(buff, size);//extract package
		if (package.operation() == 0) {
//		cout<<"Thread "<<pthread_self()<<"ins op"<<" | ";
			int32_t ret = HB_insert(db, package);
//		cout<<"Thread "<<pthread_self()<<"ins ret:"<<ret<<" | ";			
			returnState(client_sock, ret);
		} else if (package.operation() == 1) {//find and return result to client
			if (package.virtualpath().empty()) {
				cerr << "Bad key: nothing to find" << endl;
				returnState(client_sock, -1);
			} else {
				string result;// = HB_find(package.virtualpath());
				//don't really send result back to client now, do it latter.
				if (result.empty()) {
					returnState(client_sock, 0);
				} else {
					returnState(client_sock, -1);
				}
			}
		} else if (package.operation() == 2) {
			if (package.virtualpath().empty()) {
				cerr << "Bad key: nothing to remove" << endl;
				returnState(client_sock, -2);
			}
			int32_t ret = HB_remove(db, package.virtualpath());
			returnState(client_sock, ret);
			if (!ret) {
				cerr << "Remove fail" << endl;
			}
		}
/*
		 //----------------check content of the package-----------------
			 Package packageT;
			 packageT.ParseFromArray(buff, size);//extract package
			 cout << "Operation num is: " << packageT.operation() << endl;
			 cout << "Result real path form parsing is: " << packageT.realfullpath()
			 << endl;

			 cout << "There are " << packageT.listitem_size() << "items in the list"
			 << endl;

			 for (int i = 0; i < packageT.listitem_size(); i++) {
			 cout << "list item No." << i << ": " << packageT.listitem(i) << endl;
			 }
		//------------------------------------------------------

*/
		//---DB end---
		free(buff);
	}
	//------------------working part------------------------------
	if (size < 0) {
		cout << "Received QUIT command." << endl;
		//do something joining/exiting...
	}

	//	}// ------------end while--------------
	//	}
	int closingSock = ((struct threaddata *) threadarg)->socket;
	close(closingSock);
	((struct threaddata *) threadarg)->socket=-1;
	cout<<"Thread "<<pthread_self()<<"closed client sock"<<" | ";	
	pthread_mutex_lock(&mutex1);
	numthreads--;
	pthread_mutex_unlock(&mutex1);
	cout<<"Thread "<<pthread_self()<<"out of mutex"<<" | "<<endl;
        pthread_detach(pthread_self());
	pthread_exit(NULL);
}
//----------------------thread end----------------
int main(int argc, char* argv[]) {
	cout << "Usage: ./hash <port>" << endl;
	int svrPort = kyotocabinet::atoi(argv[1]);
	//int numOper = kyotocabinet::atoi(argv[1]);
	//structHDBTest(numOper);
	//	ProtoHashDB db;
	// open the database
	if (!db.open("mybase", HashDB::OWRITER | HashDB::OCREATE)) {
		cerr << "open error: " << db.error().name() << endl;
	}
	//--------------------------------Thread declaration
	pthread_t thread[MAX_THREADS];
	struct threaddata threaddata_array[MAX_THREADS];
	for (int i = 0; i < MAX_THREADS; i++) {
		threaddata_array[i].socket = -1;
	}
	//-------------------------------
	int server_sock = d3_svr_makeSocket(svrPort);
	cout<<"Main"<<"created svr sock"<<server_sock<<" | ";
	int client_sock;// = d3_svr_accept(server_sock);
	while (server_sock>0) {
		cout<<"Main "<<"before accept"<<" | ";
		client_sock = d3_svr_accept(server_sock);
		cout<<"Main "<<"after accept"<<client_sock<<" | ";
		if (client_sock > 0) {//
			if (numthreads < MAX_THREADS) {
				pthread_mutex_lock(&mutex1);
				numthreads++;
				pthread_mutex_unlock(&mutex1);
				cout<<"Main "<<"after mutex"<<" | ";
				for (int i = 0; i < MAX_THREADS; i++) {
				        cout<<"Main "<<"finding free thread:"<<i<<" | ";
					if (threaddata_array[i].socket == -1) {
						threaddata_array[i].socket = client_sock;
						cout<<"Main "<<"found free thread:"<<i<<" | "<<endl;
						int r = pthread_create(&thread[i], NULL, dbService,
								(void *) &threaddata_array[i]);
						cout<<"Main "<<"pthread_create() returned: "<<r<<",parameters: "<<(void *) &threaddata_array[i]<<
						",thread id: "<<thread[i]<<endl;
						if(r!=0){
						    cout<<"Error creating thread: "<<strerror(errno)<<endl;
						}
						if(r==0) break;
					}
				}//--------end for--------------
			} else {
				cout<<"Connection refused because no more threads can be formed\n"<<endl;
			}//----------end else---------------
		}// ------------end if--------------
	}//--------end while---------
	d3_closeConnection(server_sock);
	if (!db.close()) {
		cerr << "close error: " << db.error().name() << endl;
	}
	return 0;
}

