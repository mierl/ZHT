/**
 * File name: ffsnetd.cpp
 *
 * Function: daemon process for FusionFS network transfer
 *
 * Author: dzhao8@hawk.iit.edu
 *
 * Update history:
 *		- 06/18/2012: initial development
 *
 * To compile this single file:
 * 		g++ ffsnetd.cpp -o ffsnetd -I../src -L../src -ludt -lstdc++ -lpthread
 *		NOTE: -I../src (similarly to -L../src) means to include the directory of the udt library, i.e. libudt.so 
 */

#include <cstdlib>
#include <netdb.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <stdlib.h>
#include <stddef.h>
#include <udt.h>

using namespace std;

void* transfile(void*);

int main(int argc, char* argv[]) {
	/* usage: ffsd [server_port] */
	if ((2 < argc) || ((2 == argc) && (0 == atoi(argv[1])))) {
		cout << "usage: ffsd [server_port]" << endl;
		return 0;
	}

	/* use this function to initialize the UDT library */
	UDT::startup();

	addrinfo hints;
	addrinfo* res;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	string service("30000"); /* default server port */
	if (2 == argc)
		service = argv[1];

	if (0 != getaddrinfo(NULL, service.c_str(), &hints, &res)) {
		cout << "illegal port number or port is busy.\n" << endl;
		return 0;
	}

	UDTSOCKET serv = UDT::socket(res->ai_family, res->ai_socktype,
			res->ai_protocol);

	if (UDT::ERROR == UDT::bind(serv, res->ai_addr, res->ai_addrlen)) {
		cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
		return 0;
	}

	freeaddrinfo(res);

	cout << "FusionFS file transfer is ready at port: " << service << endl;

	UDT::listen(serv, 10);

	sockaddr_storage clientaddr;
	int addrlen = sizeof(clientaddr);

	UDTSOCKET fhandle;

	while (true) {
		if (UDT::INVALID_SOCK
				== (fhandle = UDT::accept(serv, (sockaddr*) &clientaddr,
						&addrlen))) {
			cout << "accept: " << UDT::getlasterror().getErrorMessage() << endl;
			return 0;
		}

		char clienthost[NI_MAXHOST];
		char clientservice[NI_MAXSERV];
		getnameinfo((sockaddr *) &clientaddr, addrlen, clienthost,
				sizeof(clienthost), clientservice, sizeof(clientservice),
				NI_NUMERICHOST | NI_NUMERICSERV);
		/* cout << "new connection: " << clienthost << ":" << clientservice << endl; */

		pthread_t filethread;
		pthread_create(&filethread, NULL, transfile, new UDTSOCKET(fhandle));
		pthread_detach(filethread);
	}

	UDT::close(serv);

	/* use this function to release the UDT library */
	UDT::cleanup();

	return 0;
}

string getDir(const string& str) {
	size_t found;
//	cout << "Splitting: " << str << endl;
	found = str.find_last_of("/\\");
	return str.substr(0, found);
}

void createFullPath(const string& file) {

	string dir = getDir(file);

	if (dir.compare(file) != 0) {

		string cmd = "mkdir -p ";
		cmd.append(dir);
		system(cmd.c_str());
	}
}

/**
 * Thread to accept file request: download or upload
 */
void* transfile(void* usocket) {
	UDTSOCKET fhandle = *(UDTSOCKET*) usocket;
	delete (UDTSOCKET*) usocket;

	/* aquiring file name information from client */
	char file[1024];
	int len;
	int is_recv; /* 0: download, 1: upload */

	/* get the request type: download or upload */
	if (UDT::ERROR == UDT::recv(fhandle, (char*) &is_recv, sizeof(int), 0)) {
		cout << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
		return 0;
	}

	if (is_recv) {
		if (UDT::ERROR == UDT::recv(fhandle, (char*) &len, sizeof(int), 0)) {
			cout << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
			return 0;
		}

		if (UDT::ERROR == UDT::recv(fhandle, file, len, 0)) {
			cout << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
			return 0;
		}
		file[len] = '\0';

		/* open the file to write */
		createFullPath(file);
		fstream ofs(file, ios::out | ios::binary | ios::trunc);
		int64_t recvsize;
		int64_t offset = 0;

		/* get size information */
		int64_t size;

		if (UDT::ERROR
				== UDT::recv(fhandle, (char*) &size, sizeof(int64_t), 0)) {
			cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
			return 0;
		}

		if (size < 0) {
			cout << "cannot open file " << file << " on the server\n";
			return 0;
		}

		/* receive the file */
		if (UDT::ERROR
				== (recvsize = UDT::recvfile(fhandle, ofs, offset, size))) {
			cout << "recvfile: " << UDT::getlasterror().getErrorMessage()
					<< endl;
			return 0;
		}

		UDT::close(fhandle);

		ofs.close();

		return NULL;
	}

	/* the following is for download */
	if (UDT::ERROR == UDT::recv(fhandle, (char*) &len, sizeof(int), 0)) {
		cout << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
		return 0;
	}

	if (UDT::ERROR == UDT::recv(fhandle, file, len, 0)) {
		cout << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
		return 0;
	}
	file[len] = '\0';

	/* open the file */
	fstream ifs(file, ios::in | ios::binary);

	ifs.seekg(0, ios::end);
	int64_t size = ifs.tellg();
	ifs.seekg(0, ios::beg);

	/* send file size information */
	if (UDT::ERROR == UDT::send(fhandle, (char*) &size, sizeof(int64_t), 0)) {
		cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
		return 0;
	}

	UDT::TRACEINFO trace;
	UDT::perfmon(fhandle, &trace);

	/* send the file */
	int64_t offset = 0;
	if (UDT::ERROR == UDT::sendfile(fhandle, ifs, offset, size)) {
		cout << "sendfile: " << UDT::getlasterror().getErrorMessage() << endl;
		return 0;
	}

	UDT::perfmon(fhandle, &trace);
	/* cout << "speed = " << trace.mbpsSendRate << "Mbits/sec" << endl; */

	UDT::close(fhandle);

	ifs.close();

	return NULL;
}
