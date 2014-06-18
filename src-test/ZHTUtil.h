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
 * ZHTUtil.h
 *
 *  Created on: Jun 25, 2013
 *      Author: Tony
 *      Contributor: Xiaobingo, KWang, DZhao
 */

#ifndef ZHTUTIL_H_
#define ZHTUTIL_H_

#include <string>
#include <vector>
using namespace std;

#include <arpa/inet.h>

struct HostEntity {
	int sock;
	string host;
	int port;

	/*bool valid;
	 struct sockaddr_in si;
	 vector<unsigned long long> ringID;*/
};

/*
 *
 */
class ZHTUtil {
public:
	ZHTUtil();
	virtual ~ZHTUtil();

	HostEntity getHostEntityByKey(const string& msg);

private:
	HostEntity buildHostEntity(const string& host, const uint& port);
};

class IdHelper {
public:
	IdHelper();
	virtual ~IdHelper();

	static uint64_t genId();

public:
	static const uint ID_LEN;
};

#endif /* ZHTUTIL_H_ */
