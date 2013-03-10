/*
 * Copyright (C) 2010-2020 DatasysLab@iit.edu(http://datasys.cs.iit.edu/index.html).
 *      Director: Ioan Raicu(iraicu@cs.iit.edu)
 *
 * This file is part of ZHT library(http://datasys.cs.iit.edu/projects/ZHT/index.html).
 *      Ioan Raicu(iraicu@cs.iit.edu),
 *      Tonglin Li(tli13@hawk.iit.edu) with nickname tony,
 *      Xiaobing Zhou(xzhou40@hawk.iit.edu) with nickname xiaobingo.
 *
 * The ZHT Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The ZHT Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the ZHT Library; if not, write to the
 * Data-Intensive Distributed Systems Laboratory, 10 W. 31st Street,
 * Stuart Building, Room 003B, Chicago, IL 60616 USA.
 *
 * Util.h
 *
 *  Created on: Feb 14, 2013
 *      Author: tony, xiaobingo
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
