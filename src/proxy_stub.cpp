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
 * proxy_stub.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include "proxy_stub.h"

#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>

ProtoAddr::ProtoAddr() :
		fd(-1), sender(NULL) {
}

ProtoAddr::ProtoAddr(const ProtoAddr& addr) {

	fd = addr.fd;
	sender = calloc(1, sizeof(sockaddr));
	memcpy(sender, &addr.sender, sizeof(sockaddr));
}

ProtoAddr::~ProtoAddr() {

	free(sender);
}

ProtoProxy::ProtoProxy() {
}

ProtoProxy::~ProtoProxy() {
}

bool ProtoProxy::init(int argc, char **argv) {

	return false;
}

bool ProtoProxy::send(const void *sendbuf, const size_t sendcount) {

	return false;
}

bool ProtoProxy::recv(void *recvbuf, size_t &recvcount) {

	return false;
}

bool ProtoProxy::sendrecv(const void *sendbuf, const size_t sendcount,
		void *recvbuf, size_t &recvcount) {

	return false;
}

bool ProtoProxy::teardown() {

	return false;
}

ProtoStub::ProtoStub() {
}

ProtoStub::~ProtoStub() {
}

bool ProtoStub::init(int argc, char **argv) {

	return false;
}

bool ProtoStub::send(const void *sendbuf, const size_t sendcount) {

	return false;
}

bool ProtoStub::recv(void *recvbuf, size_t &recvcount) {

	return false;
}

bool ProtoStub::recvsend(ProtoAddr addr, const void *recvbuf) {

	return false;
}

bool ProtoStub::teardown() {

	return false;
}

int ProtoStub::sendBack(ProtoAddr addr, const void* sendbuf,
		int sendcount) const {

	return -1;
}
