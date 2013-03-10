/*
 * Copyright (C) 2010-2020 DatasysLab@iit.edu(http://datasys.cs.iit.edu/index.html).
 *      Director: Ioan Raicu(iraicu@cs.iit.edu)
 *	 
 * This file is part of ZHT library(http://datasys.cs.iit.edu/projects/ZHT/index.html).
 *      Ioan Raicu(iraicu@cs.iit.edu),
 *      Tonglin Li(tli13@hawk.iit.edu) with nickname tony,
 *      Xiaobing Zhou(xzhou40@hawk.iit.edu) with nickname xiaobingo.
 * 
 * The ZHT Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The ZHT Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the ZHT Library; if not, write to the 
 * Data-Intensive Distributed Systems Laboratory, 10 W. 31st Street,
 * Stuart Building, Room 003B, Chicago, IL 60616 USA.
 *
 * udtfile_test.cpp
 *
 *  Created on: Sep 24, 2012
 *      Author: tony, xiaobingo
 */

#include "udtfile_test.h"

#include <vector>
using namespace std;

#ifndef WIN32
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <udt.h>

using namespace UDT;

FileClient::FileClient() {
}

FileClient::FileClient(const string& fileClient, const string& host,
		const string& port, const string& remoteFilename,
		const string& localFileName) :
		_fileClient(fileClient), _host(host), _port(port), _remoteFilename(
				remoteFilename), _localFileName(localFileName) {
}
/*

 FileClient::FileClient(const char* const fileClient, const char* const host,
 const char* const port, const char* const remoteFilename,
 const char* const localFileName) :
 _fileClient(string(fileClient)), _host(string(host)), _port(
 string(port)), _remoteFilename(string(remoteFilename)), _localFileName(
 string(localFileName)) {
 }
 */

FileClient::~FileClient() {
}

int FileClient::getFileFromZHT() {

	vector<const char*> argv;
	argv.push_back(_fileClient.c_str());
	argv.push_back(_host.c_str());
	argv.push_back(_port.c_str());
	argv.push_back(_remoteFilename.c_str());
	argv.push_back(_localFileName.c_str());

	return getFileFromZHTInternal(argv.size(), &argv[0]);

}

int FileClient::getFileFromZHTInternal(const int& argc, const char** argv) {

	if ((argc != 5) || (0 == atoi(argv[2]))) {
		cout
				<< "usage: recvfile server_ip server_port remote_filename local_filename"
				<< endl;
		return -1;
	}

	// use this function to initialize the UDT library
	UDT::startup();

	struct addrinfo hints, *peer;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	UDTSOCKET fhandle = UDT::socket(hints.ai_family, hints.ai_socktype,
			hints.ai_protocol);

	if (0 != getaddrinfo(argv[1], argv[2], &hints, &peer)) {
		cout << "incorrect server/peer address. " << argv[1] << ":" << argv[2]
				<< endl;
		return -1;
	}

	// connect to the server, implict bind
	if (UDT::ERROR == UDT::connect(fhandle, peer->ai_addr, peer->ai_addrlen)) {
		cout << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
		return -1;
	}

	freeaddrinfo(peer);

	// send name information of the requested file
	int len = strlen(argv[3]);

	if (UDT::ERROR == UDT::send(fhandle, (char*) &len, sizeof(int), 0)) {
		cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
		return -1;
	}

	if (UDT::ERROR == UDT::send(fhandle, argv[3], len, 0)) {
		cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
		return -1;
	}

	// get size information
	int64_t size;

	if (UDT::ERROR == UDT::recv(fhandle, (char*) &size, sizeof(int64_t), 0)) {
		cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
		return -1;
	}

	if (size < 0) {
		cout << "no such file " << argv[3] << " on the server\n";
		return -1;
	}

	// receive the file
	fstream ofs(argv[4], ios::out | ios::binary | ios::trunc);
	int64_t recvsize;
	int64_t offset = 0;

	if (UDT::ERROR == (recvsize = UDT::recvfile(fhandle, ofs, offset, size))) {
		cout << "recvfile: " << UDT::getlasterror().getErrorMessage() << endl;
		return -1;
	}

	UDT::close(fhandle);

	ofs.close();

	// use this function to release the UDT library
	UDT::cleanup();

	return 0;
}

int main(int argc, char** argv) {

	/*	char * argv[6];
	 argv[0] = (char*) ConfigHandler::NC_FILECLIENT_PATH.c_str(); //fileclient
	 argv[1] = (char*) fsHost.c_str(); //192.168.1.1
	 argv[2] = (char*) Const::toString(fsPort).c_str(); //9000
	 argv[3] = (char*) remoteFileName.c_str(); //hello.txt
	 argv[4] = (char*) localFileName.c_str(); //hello2.txt
	 argv[5] = NULL;*/

	if ((argc != 5) || (0 == atoi(argv[2]))) {
		cout
				<< "usage: recvfile server_ip server_port remote_filename local_filename"
				<< endl;
		return -1;
	}

	FileClient fc(argv[0], argv[1], argv[2], argv[3], argv[4]);
	int rcode = fc.getFileFromZHT();

	if (rcode == 0)
		return 0;
	else
		return -2;
}

