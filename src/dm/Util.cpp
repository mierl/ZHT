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
 * Util.cpp
 *
 *  Created on: Feb 14, 2013
 *      Author: tony, xiaobingo
 */

#include "Util.h"

#include <stdlib.h>
#include <stdint.h>
#include <sstream>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

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
