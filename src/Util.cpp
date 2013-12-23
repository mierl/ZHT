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
 * Util.cpp
 *
 *  Created on: Feb 14, 2013
 *      Author: Tony
 *      Contributor: Tony, KWang, DZhao
 */

#include "Util.h"

#include <sys/time.h>
#include <stdlib.h>
#include <stdint.h>
#include <sstream>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

TimeUtil::TimeUtil() {

}

TimeUtil::~TimeUtil() {

}

double TimeUtil::getTime_usec() {

	struct timeval tp;

	gettimeofday(&tp, NULL);
	return static_cast<double>(tp.tv_sec) * 1E6
			+ static_cast<double>(tp.tv_usec);
}

double TimeUtil::getTime_msec() {

	struct timeval tp;

	gettimeofday(&tp, NULL);
	return static_cast<double>(tp.tv_sec) * 1E3
			+ static_cast<double>(tp.tv_usec) / 1E3;

}

double TimeUtil::getTime_sec() {

	struct timeval tp;

	gettimeofday(&tp, NULL);
	return static_cast<double>(tp.tv_sec)
			+ static_cast<double>(tp.tv_usec) / 1E6;
}

const int HashUtil::LEN_BASE = 15;
const uint64_t HashUtil::ULL_MAX = (uint64_t) -1;

HashUtil::HashUtil() {

}

HashUtil::~HashUtil() {
}

uint64_t HashUtil::genHash(const char *pc) {

	uint64_t hash = 0;
	uint64_t c; //int c;

	while (c = (*pc++)) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}

	return hash;

}

uint64_t HashUtil::genHash(const string& base) {

	return genHash(base.c_str());
}

string HashUtil::genBase(const string& host, const int& port) {

	stringstream ss;
	ss << host;
	ss << ":";
	ss << port;

	return ss.str();
}

string HashUtil::randomString(int len) {

	string s(len, ' ');

	static const char alphanum[] = "0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	return s;
}

const uint64_t RingUtil::RING_BASE = 1;
const uint64_t RingUtil::TOKEN_MAX = HashUtil::ULL_MAX;

RingUtil::RingUtil() {

}

RingUtil::~RingUtil() {
}

void RingUtil::unwrap(uint64_t& begin, uint64_t& end) {

	uint64_t unend = end;

	begin > end && end == RING_BASE ? end = RingUtil::TOKEN_MAX : end = unend;
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
