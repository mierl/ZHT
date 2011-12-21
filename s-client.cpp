/*
 * s-client.cpp
 *
 *  Created on: Oct 29, 2011
 *      Author: leo
 */

#include <string>
#include <iostream>
//#include "d3_transport.h"
#include<fstream>
#include <netdb.h>
#include <stdlib.h>
#include "zht_util.h"
using namespace std;

int main(int argc, char* argv[]) {

//	string str = "123456789asdfghjklzxcvbnnm,.";
	HostEntity destination;
	destination.host = "localhost";
	destination.port = 50000;
	int current_sock = -1;

/*	Package package, package_ret;
	package.set_virtualpath(randomString(25)); //as key
	package.set_isdir(false);
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
*/
	Package package, package_ret;
			package.set_virtualpath(randomString(50)); //as key
			package.set_isdir(true); //this is crucial: true for not breaking the string.
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
			cout<<"package size = "<<str.size()<<endl;

	string stri = package.SerializeAsString();

	cout << "Initial string in String, size= "<< stri.size()<<", content: "<<stri<<endl<<endl;

	cout << "Initial string in c_str, strlen=  "<< strlen(stri.c_str())<<", content: "<<stri.c_str()<<endl<<endl;

	cout << "Initial string in const char * data(), length=  "<< stri.length()<<", content: "<<stri.data()<<endl;

	int r = simpleSend(stri, destination, current_sock); //break down this function.

//	cout<<"simpleSend ended, return."<<endl;
	return 0;




















/*


	//--------------------------
//========================= simpleSend========================================================================
	//int simpleSend(const char* str, struct HostEntity destination, int &current_sock)
	int i = 0, port, client_sock,	r = 0;
	const char * str = stri.c_str();
		//int32_t str_size = strlen(str);
	int32_t str_size = stri.size();
		string hostName;
		sockaddr_in toAddr, recv_addr;
		cout<<"before send, length= "<< str_size << endl;
	#if TRANS_PROTOCOL == USE_TCP
		client_sock = d3_makeConnection((destination.host).c_str(),
				destination.port);
	#elif TRANS_PROTOCOL == USE_UDP

		cout << endl << "simpleSend makeSocket start-----" << endl;
	//	client_sock = d3_svr_makeSocket((time(NULL) % 10000) + rand() % 10000); //old, with consideration on multiple port conflicts.

		client_sock = d3_makeConnection(); //possible right one.

		cout << "simpleSend makeSocket end-----" << endl << endl;
	#endif

		if (client_sock < 0) { //only report error, doesn't handle it
			cout << " " << endl;
			return -9;
		}

		toAddr = d3_make_sockaddr_in_client((destination.host).c_str(), destination.port);

		cout << "simpleSend trying to reach host:" << destination.host << ", port:"
				<< destination.port << endl;


		//r = d3_send_data(client_sock, (void*) str.c_str(), str_size, 0, &toAddr);
		cout<<"before send, length= "<< str_size << endl;
		//r = d3_send_data(client_sock, (void*) str, str_size, 0, &toAddr);
		r = d3_send_data(client_sock, (void*)stri.c_str(), str_size, 0, &toAddr);

	cout << "simpleSend: sent "<<r<<" bytes:"<<stri.c_str()<<endl;
		if (r < 0) {
			cerr << "Sending data failed." << endl;
			return -7;
		}
*/
/*
		//receive return status.
		void *buff_return = (void*) malloc(sizeof(int32_t));
		r = d3_recv_data(client_sock, buff_return, sizeof(int32_t), 0);

		if (r < 0) {
			cerr << "zht_util.h: Receiving return state failed." << endl;
			return -7;
		}
		int32_t ret = *(int32_t*) buff_return;
		switch (ret) {
		case 0:
			break;
		case 1:
			break;
		case -2:
			cerr << "zht_util.h: Failed to remove from replica." << endl;
			break;
		case -3:
			cerr << "zht_util.h: Failed to insert into replica." << endl;
			break;
		default:
			cerr << "zht_util.h: What the hell was that? ret = " <<ret<< endl;
			break;
		}
*/
	//	current_sock = client_sock;


	//========================= simpleSend================================







//	cout << r << " bytes sent." << endl;
//	d3_closeConnection(current_sock);

//	cout << stri << endl;
	return 0;
}
