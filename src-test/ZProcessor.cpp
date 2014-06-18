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
 * ZProcessor.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include "ZProcessor.h"
#include "Const-impl.h"

#include <sys/socket.h>
#include <stdio.h>

using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

ZProcessor::ZProcessor() {

}

ZProcessor::~ZProcessor() {

}

void ZProcessor::sendback(const int& fd, const char *buf, const size_t& count,
		sockaddr receiver, const int& protocol) {

	int i = -2;
	try {
		if (protocol == Const::PROTO_STREAM) {

			i = send(fd, buf, count, 0);
#if ILOG
			fprintf(stdout, "in sendback, send to sock(%d), %d bytes sent\n",
					fd, i);
#endif
		} else if (protocol == Const::PROTO_UGRADM) {

			i = sendto(fd, buf, count, 0, (struct sockaddr*) &receiver,
					sizeof(struct sockaddr));
#if ILOG
			fprintf(stdout, "in sendback, send to sock(%d), %d bytes sent\n",
					fd, i);
#endif
		} else {
		}
	} catch (exception& e) {

		fprintf(stderr, "%s, exception caught:\n\t%s",
				"ZProcessor::sendback(const int& fd, const char *buf, const size_t& count, const sockaddr& receiver, const int& protocol)",
				e.what());
	}

}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */

