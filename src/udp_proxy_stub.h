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
 * udp_proxy_stub.h
 *
 *  Created on: Jun 21, 2013
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#ifndef UDP_PROXY_STUB_H_
#define UDP_PROXY_STUB_H_

#include "ip_proxy_stub.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <pthread.h>
using namespace std;

/*
 *
 */
class UDPProxy: public IPProtoProxy {
public:
	typedef map<string, int> SMAP;
	typedef SMAP::iterator SIT;
	typedef map<string, sockaddr_in> AMAP;
	typedef AMAP::iterator AIT;

public:
	UDPProxy();
	virtual ~UDPProxy();

	virtual bool sendrecv(const void *sendbuf, const size_t sendcount,
			void *recvbuf, size_t &recvcount);
	virtual bool teardown();

protected:
	virtual int getSockCached(const string& host, const uint& port);
	virtual int makeClientSocket(const string& host, const uint& port);
	virtual int recvFrom(int sock, void* recvbuf);
	virtual int loopedrecv(int sock, string &srecv);

	virtual sockaddr_in getAddrCached(const string& host, const uint& port);
	virtual sockaddr_in makeClientAddr(const string& host, const uint& port);

private:
	int sendTo(int sock, const string &host, uint port, const void* sendbuf,
			int sendcount);

private:
	static void init_AC_MUTEX();

private:
	static bool INIT_AC_MUTEX;
	static pthread_mutex_t AC_MUTEX; //mutex for address cache

private:
	//static SMAP SOCK_CACHE;
	//static AMAP ADDR_CACHE;
	SMAP SOCK_CACHE;
	AMAP ADDR_CACHE;
};

class UDPStub: public IPProtoStub {
public:
	UDPStub();
	virtual ~UDPStub();

	virtual bool recvsend(ProtoAddr addr, const void *recvbuf);

public:
	virtual int sendBack(ProtoAddr addr, const void* sendbuf,
			int sendcount) const;

};

#endif /* UDP_PROXY_STUB_H_ */
