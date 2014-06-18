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
 * udp_proxy_stub.cpp
 *
 *  Created on: Jun 21, 2013
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include "udp_proxy_stub.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>

#include "Util.h"
#include "ZHTUtil.h"
#include "HTWorker.h"

#include "Env.h"
#include "bigdata_transfer.h"

#include "lock_guard.h"

using namespace iit::datasys::zht::dm;

/*
 UDPProxy::SMAP UDPProxy::SOCK_CACHE = UDPProxy::SMAP();
 UDPProxy::AMAP UDPProxy::ADDR_CACHE = UDPProxy::AMAP();
 */

bool UDPProxy::INIT_AC_MUTEX = false;
pthread_mutex_t UDPProxy::AC_MUTEX;

UDPProxy::UDPProxy() :
		SOCK_CACHE(), ADDR_CACHE() {

	init_AC_MUTEX();
}

UDPProxy::~UDPProxy() {
}

bool UDPProxy::sendrecv(const void *sendbuf, const size_t sendcount,
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
	int sentSize = sendTo(sock, he.host, he.port, (char*) sendbuf, sendcount);
	int sent_bool = sentSize == sendcount;

	/*receive response from server over client sock fd*/ //todo: loopedReceive for zht_lookup
	recvcount = recvFrom(sock, recvbuf);
	int recv_bool = recvcount >= 0;

	/*combine flags as value to be returned*/
	return sent_bool && recv_bool;
}

#ifdef BIG_MSG
int UDPProxy::sendTo(int sock, const string &host, uint port,
		const void* sendbuf, int sendcount) {

	struct sockaddr_in dest = getAddrCached(host, port);

	BdSendBase *pbsb = new BdSendToServer((char*) sendbuf);
	int sentSize = pbsb->bsend(sock, &dest);
	delete pbsb;
	pbsb = NULL;

	//prompt errors
	if (sentSize < sendcount) {

		//todo: bug prone
		/*cerr << "UDPProxy::sendTo(): error on BdSendToServer::bsend(...): "
		 << strerror(errno) << endl;*/
	}

	return sentSize;
}
#endif

#ifdef SML_MSG
int UDPProxy::sendTo(int sock, const string &host, uint port,
		const void* sendbuf, int sendcount) {

	struct sockaddr_in dest = getAddrCached(host, port);

	int sentSize = sendto(sock, sendbuf, sendcount, 0, (struct sockaddr*) &dest,
			sizeof(struct sockaddr));

	//prompt errors
	if (sentSize < sendcount) {

		cerr << "UDPProxy::sendTo(): error on BdSendToServer::bsend(...): "
		<< strerror(errno) << endl;
	}

	return sentSize;
}
#endif

#ifdef BIG_MSG
int UDPProxy::recvFrom(int sock, void* recvbuf) {

	string result;
	int recvcount = loopedrecv(sock, result);

	memcpy(recvbuf, result.c_str(), result.size());

	/*prompt errors*/
	if (recvcount < 0) {

		cerr << "UDPProxy::recvFrom(): error on loopedrecv(...): "
		<< strerror(errno) << endl;
	}

	return recvcount;
}
#endif

#ifdef SML_MSG
int UDPProxy::recvFrom(int sock, void* recvbuf) {

	char buf[Env::BUF_SIZE];
	memset(buf, '\0', sizeof(buf));

	struct sockaddr_in recvAddr;
	socklen_t addr_len = sizeof(struct sockaddr);
	int recvcount = ::recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&recvAddr, &addr_len);

	memcpy(recvbuf, buf, strlen(buf));

	/*prompt errors*/
	if (recvcount < 0) {

		cerr << "UDPProxy::recvFrom(): error on loopedrecv(...): "
		<< strerror(errno) << endl;
	}

	return recvcount;
}
#endif

int UDPProxy::loopedrecv(int sock, string &srecv) {

	struct sockaddr_in recvAddr;

	return IPProtoProxy::loopedrecv(sock, &recvAddr, srecv);
}

bool UDPProxy::teardown() {

	bool result = true;

	SIT it;
	for (it = SOCK_CACHE.begin(); it != SOCK_CACHE.end(); it++) {

		int rc = close(it->second);

		result &= rc == 0;
	}

	SOCK_CACHE.clear();
	ADDR_CACHE.clear();

	result &= IPProtoProxy::teardown();

	return result;
}

int UDPProxy::getSockCached(const string& host, const uint& port) {

	int sock = 0;

#ifdef SOCKET_CACHE
	string hashKey = HashUtil::genBase(host, port);

	SIT it = SOCK_CACHE.find(hashKey);

	if (it == SOCK_CACHE.end()) {

		LockGuard lock(&CC_MUTEX);

		sock = makeClientSocket(host, port);

		if (sock <= 0) {

			cerr << "UDPProxy::getSockCached(): error on makeClientSocket("
			<< host << ":" << port << "): " << strerror(errno) << endl;
			sock = -1;
		} else {

			SOCK_CACHE[hashKey] = sock;

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

sockaddr_in UDPProxy::getAddrCached(const string& host, const uint& port) {

	sockaddr_in result;

#ifdef SOCKET_CACHE
	string hashKey = HashUtil::genBase(host, port);

	AIT it = ADDR_CACHE.find(hashKey);

	if (it == ADDR_CACHE.end()) {

		LockGuard lock(&AC_MUTEX);

		result = makeClientAddr(host, port);

		ADDR_CACHE[hashKey] = result;

	} else {

		result = ADDR_CACHE[hashKey];
	}
#else
	result = makeClientAddr(host, port);
#endif

	return result;
}

int UDPProxy::makeClientSocket(const string& host, const uint& port) {

	int to_sock = 0;

	to_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (to_sock < 0) {

		fprintf(stderr,
				"UDPProxy::makeClientSocket(): error on creating the socket:%d\n",
				to_sock);
		fprintf(stderr, "%s\n", strerror(errno));
		close(to_sock);

		return -1;
	}

	return to_sock;
}

sockaddr_in UDPProxy::makeClientAddr(const string& host, const uint& port) {

	struct sockaddr_in dest;
	memset(&dest, 0, sizeof(struct sockaddr_in)); /*zero the struct*/
	dest.sin_family = PF_INET;
	dest.sin_port = htons(port);

	struct hostent *hinfo = gethostbyname(host.c_str());
	if (hinfo == NULL) {
		cerr << "UDPProxy::makeClientAddr(): ";
		herror(host.c_str());
		return dest;
	}

	memcpy(&dest.sin_addr, hinfo->h_addr, sizeof(dest.sin_addr));

	return dest;
}

void UDPProxy::init_AC_MUTEX() {

	if (!INIT_AC_MUTEX) {

		pthread_mutex_init(&AC_MUTEX, NULL);
		INIT_AC_MUTEX = true;
	}
}

UDPStub::UDPStub() {

}

UDPStub::~UDPStub() {
}

bool UDPStub::recvsend(ProtoAddr addr, const void *recvbuf) {

	/*get response to be sent to client*/
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

	/*send response to client over server sock fd*/
	int sentsize = sendBack(addr, sendbuf, sendcount);
	bool sent_bool = sentsize == sendcount;

	return sent_bool;

#endif
}

#ifdef BIG_MSG
int UDPStub::sendBack(ProtoAddr addr, const void* sendbuf, int sendcount) const {

//send response to client over server sock fd
	BdSendBase *pbsb = new BdSendToClient((char*) sendbuf);
	int sentsize = pbsb->bsend(addr.fd, addr.sender);
	delete pbsb;
	pbsb = NULL;

//prompt errors
	if (sentsize < sendcount) {

		//todo: bug prone
		/*cerr << "UDPStub::sendBack():  error on BdSendToClient::bsend(...): "
		 << strerror(errno) << endl;*/
	}

	return sentsize;
}
#endif

#ifdef SML_MSG
int UDPStub::sendBack(ProtoAddr addr, const void* sendbuf, int sendcount) const {

//send response to client over server sock fd
	int sentsize = ::sendto(addr.fd, sendbuf, sendcount, 0,
			(struct sockaddr*)addr.sender, sizeof(struct sockaddr));

//prompt errors
	if (sentsize < sendcount) {

		cerr << "UDPStub::sendBack():  error on BdSendToClient::bsend(...): "
		<< strerror(errno) << endl;
	}

	return sentsize;
}
#endif
