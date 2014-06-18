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
 * mpi_proxy_stub.h
 *
 *  Created on: Jun 21, 2013
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#ifndef MPI_PROXY_STUB_H_
#define MPI_PROXY_STUB_H_

#include "proxy_stub.h"

/*
 *
 */
class MPIProxy: public ProtoProxy {

public:
	MPIProxy();
	virtual ~MPIProxy();

	virtual bool init(int argc, char **argv);
	virtual bool sendrecv(const void *sendbuf, const size_t sendcount,
			void *recvbuf, size_t &recvcount);

private:
	virtual int get_mpi_dest(const void *sendbuf, const size_t sendcount);

private:
	int size;
	int rank;
};

class MPIStub: public ProtoStub {

public:
	MPIStub();
	virtual ~MPIStub();

	virtual bool init(int argc, char **argv);
	virtual bool recvsend(ProtoAddr addr, const void *recvbuf);

private:
	int size;
	int rank;
};
#endif /* MPI_PROXY_STUB_H_ */
