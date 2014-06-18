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
 * proxy_stub.h
 *
 *  Created on: Jun 26, 2013
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#ifndef PROXY_STUB_H_
#define PROXY_STUB_H_

#include <sys/types.h>

#include "protocol_shared.h"

class ProtoAddr {

public:
	ProtoAddr();
	ProtoAddr(const ProtoAddr& addr);
	virtual ~ProtoAddr();

	int fd;
	void *sender;
};

class ProtoProxy {

public:
	ProtoProxy();
	virtual ~ProtoProxy();

	virtual bool init(int argc, char **argv);

	virtual bool send(const void *sendbuf, const size_t sendcount);

	virtual bool recv(void *recvbuf, size_t &recvcount);

	virtual bool sendrecv(const void *sendbuf, const size_t sendcount,
			void *recvbuf, size_t &recvcount);

	virtual bool teardown();
};

class ProtoStub {

public:
	ProtoStub();
	virtual ~ProtoStub();

	virtual bool init(int argc, char **argv);

	virtual bool send(const void *sendbuf, const size_t sendcount);

	virtual bool recv(void *recvbuf, size_t &recvcount);

	virtual bool recvsend(ProtoAddr addr, const void *recvbuf);

	virtual bool teardown();

public:
	virtual int sendBack(ProtoAddr addr, const void* sendbuf,
			int sendcount) const;
};
#endif /* PROXY_STUB_H_ */
