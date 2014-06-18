/*
 * Copyright 2010-2020 DatasysLab@iit.edu(http://datasys.cs.iit.edu/index.html)
 *      Director: Ioan Raicu(iraicu@cs.iit.edu)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is part of ZHT library(http://datasys.cs.iit.edu/projects/ZHT/index.html).
 *      Tonglin Li(tli13@hawk.iit.edu) with nickname Tony,
 *      Xiaobing Zhou(xzhou40@hawk.iit.edu) with nickname Xiaobingo,
 *      Ke Wang(kwang22@hawk.iit.edu) with nickname KWang,
 *      Dongfang Zhao(dzhao8@@hawk.iit.edu) with nickname DZhao,
 *      Ioan Raicu(iraicu@cs.iit.edu).
 *
 * tcp_proxy_stub.cpp
 *
 *  Created on: Jun 21, 2013
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include "tcp_proxy_stub.h"

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "lock_guard.h"
//#include <thread>
//#include <mutex>

#include "Env.h"
#include "Util.h"
#include "ZHTUtil.h"
#include "bigdata_transfer.h"

using namespace std;
using namespace iit::datasys::zht::dm;

/*LRUCache<string, int> TCPProxy::CONN_CACHE = LRUCache<string, int>(
 TCPProxy::CACHE_SIZE);*/

//TCPProxy::MAP TCPProxy::CONN_CACHE = TCPProxy::MAP();
TCPProxy::TCPProxy() :
		CONN_CACHE() {

}

TCPProxy::~TCPProxy() {
}

bool TCPProxy::sendrecv(const void *sendbuf, const size_t sendcount,
		void *recvbuf, size_t &recvcount) {

	/*get client sock fd*/
	ZHTUtil zu;
	string msg((char*) sendbuf, sendcount);
	HostEntity he = zu.getHostEntityByKey(msg);

	int sock = getSockCached(he.host, he.port);

	reuseSock(sock);

	/*get mutex to protected shared socket*/
	pthread_mutex_t *sock_mutex = getSockMutex(he.host, he.port);
	LockGuard lock(sock_mutex);

	/*send message to server over client sock fd*/
	int sentSize = sendTo(sock, sendbuf, sendcount);
	int sent_bool = sentSize == sendcount;

	/*receive response from server over client sock fd*/
	recvcount = recvFrom(sock, recvbuf);
	int recv_bool = recvcount >= 0;

	/*combine flags as value to be returned*/
	return sent_bool && recv_bool;
}

bool TCPProxy::teardown() {

	bool result = true;

	MIT it;
	for (it = CONN_CACHE.begin(); it != CONN_CACHE.end(); it++) {

		int rc = close(it->second);

		result &= rc == 0;
	}

	CONN_CACHE.clear();

	result &= IPProtoProxy::teardown();

	return result;
}

int TCPProxy::getSockCached(const string& host, const uint& port) {

	int sock = 0;

#ifdef SOCKET_CACHE
	LockGuard lock(&CC_MUTEX);

	string hashKey = HashUtil::genBase(host, port);

	MIT it = CONN_CACHE.find(hashKey);

	if (it == CONN_CACHE.end()) {

		sock = makeClientSocket(host, port);

		if (sock <= 0) {

			cerr << "TCPProxy::getSockCached(): error on makeClientSocket("
			<< host << ":" << port << "): " << strerror(errno) << endl;
			sock = -1;
		} else {

			CONN_CACHE[hashKey] = sock;

			putSockMutex(host, port);
		}
	} else {

		sock = it->second;
	}
#else
	sock = makeClientSocket(host, port);
#endif

	return sock;
}

int TCPProxy::makeClientSocket(const string& host, const uint& port) {

	struct sockaddr_in dest;
	memset(&dest, 0, sizeof(struct sockaddr_in)); /*zero the struct*/
	dest.sin_family = PF_INET; /*storing the server info in sockaddr_in structure*/
	dest.sin_port = htons(port);

	struct hostent * hinfo = gethostbyname(host.c_str());
	if (hinfo == NULL) {
		cerr << "TCPProxy::makeClientSocket(): ";
		herror(host.c_str());
		return -1;
	}

	memcpy(&dest.sin_addr, hinfo->h_addr, sizeof(dest.sin_addr));

	int to_sock = socket(PF_INET, SOCK_STREAM, 0); //try change here.................................................

	if (to_sock < 0) {

		cerr << "TCPProxy::makeClientSocket(): error on ::socket(...): "
				<< strerror(errno) << endl;
		return -1;
	}

	int ret_con = connect(to_sock, (struct sockaddr *) &dest, sizeof(sockaddr));

	if (ret_con < 0) {

		cerr << "TCPProxy::makeClientSocket(): error on ::connect(...): "
				<< strerror(errno) << endl;
		return -1;
	}

	return to_sock;
}

#ifdef BIG_MSG
int TCPProxy::sendTo(int sock, const void* sendbuf, int sendcount) {

	BdSendBase *pbsb = new BdSendToServer((char*) sendbuf);
	int sentSize = pbsb->bsend(sock);
	delete pbsb;
	pbsb = NULL;

	//prompt errors
	if (sentSize < sendcount) {

		//todo: bug prone
		/*cerr << "TCPProxy::sendTo(): error on BdSendToServer::bsend(...): "
		 << strerror(errno) << endl;*/
	}

	return sentSize;
}
#endif

#ifdef SML_MSG
int TCPProxy::sendTo(int sock, const void* sendbuf, int sendcount) {

	int sentSize = ::send(sock, sendbuf, sendcount, 0);

	//prompt errors
	if (sentSize < sendcount) {

		cerr << "TCPProxy::sendTo(): error on BdSendToServer::bsend(...): "
		<< strerror(errno) << endl;
	}

	return sentSize;
}
#endif

#ifdef BIG_MSG
int TCPProxy::recvFrom(int sock, void* recvbuf) {

	string result;
	int recvcount = loopedrecv(sock, result);

	memcpy(recvbuf, result.c_str(), result.size());

	//prompt errors
	if (recvcount < 0) {

		cerr << "TCPProxy::recvFrom(): error on loopedrecv(...): "
		<< strerror(errno) << endl;
	}

	return recvcount;
}
#endif

#ifdef SML_MSG
int TCPProxy::recvFrom(int sock, void* recvbuf) {

	char buf[Env::BUF_SIZE];
	memset(buf, '\0', sizeof(buf));

	int recvcount = ::recv(sock, buf, sizeof(buf), 0);

	memcpy(recvbuf, buf, strlen(buf));

	//prompt errors
	if (recvcount < 0) {

		cerr << "TCPProxy::recvFrom(): error on ::recv(...): "
		<< strerror(errno) << endl;
	}

	memset(buf, '\0', sizeof(buf));

	return recvcount;
}
#endif

int TCPProxy::loopedrecv(int sock, string &srecv) {

	return IPProtoProxy::loopedrecv(sock, NULL, srecv);
}

TCPStub::TCPStub() {

}

TCPStub::~TCPStub() {
}

bool TCPStub::recvsend(ProtoAddr addr, const void *recvbuf) {

	//get response to be sent to client
	string recvstr((char*) recvbuf);

#ifdef SCCB
	HTWorker htw(addr, this);
#else
	HTWorker htw;
#endif

	string result = htw.run(recvstr.c_str());

#ifdef SCCB
	return true;
#else
	const char *sendbuf = result.data();
	int sendcount = result.size();

	//send response to client over server sock fd
	int sentsize = sendBack(addr, sendbuf, sendcount);
	bool sent_bool = sentsize == sendcount;

	return sent_bool;
#endif
}

#ifdef BIG_MSG
int TCPStub::sendBack(ProtoAddr addr, const void* sendbuf, int sendcount) const {

	//send response to client over server sock fd
	BdSendBase *pbsb = new BdSendToClient((char*) sendbuf);
	int sentsize = pbsb->bsend(addr.fd);
	delete pbsb;
	pbsb = NULL;

	//prompt errors
	if (sentsize < sendcount) {

		//todo: bug prone
		/*	cerr << "TCPStub::sendBack():  error on BdSendToClient::bsend(...): "
		 << strerror(errno) << endl;*/
	}

	return sentsize;
}
#endif

#ifdef SML_MSG
int TCPStub::sendBack(ProtoAddr addr, const void* sendbuf, int sendcount) const {

	//send response to client over server sock fd
	int sentsize = ::send(addr.fd, sendbuf, sendcount, 0);

	//prompt errors
	if (sentsize < sendcount) {

		cerr << "TCPStub::sendBack():  error on BdSendToClient::bsend(...): "
		<< strerror(errno) << endl;
	}

	return sentsize;
}
#endif

