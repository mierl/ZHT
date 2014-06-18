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
 * Util.h
 *
 *  Created on: Feb 14, 2013
 *      Author: Tony
 *      Contributor: Tony, KWang, DZhao
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>
#include <string>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

class TimeUtil {
public:
	TimeUtil();
	virtual ~TimeUtil();

	static double getTime_usec();

	static double getTime_msec();

	static double getTime_sec();
};

/*
 *
 */
class HashUtil {
public:
	HashUtil();
	virtual ~HashUtil();

	static uint64_t genHash(const char *pc);
	static uint64_t genHash(const string& base);
	static string genBase(const string& host, const int& port);
	static string randomString(int lenbase);

public:
	static const int LEN_BASE;
	static const uint64_t ULL_MAX;
};

/*
 *
 */
class RingUtil {
public:
	RingUtil();
	virtual ~RingUtil();

public:
	static void unwrap(uint64_t& begin, uint64_t& end);

public:
	static const uint64_t RING_BASE;
	static const uint64_t TOKEN_MAX;
};

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* UTIL_H_ */
