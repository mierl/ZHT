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
 * ZHTClient.h
 *
 *  Created on: Sep 16, 2012
 *      Author: tony, xiaobingo
 */

#ifndef ZHTCLIENT_H_
#define ZHTCLIENT_H_

#include <stdint.h>
#include <map>
#include <string>
using namespace std;

#include "../zht_util.h"
#include "../lru_cache.h"

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

/*
 *
 */
class ZHTClient {

public:
	typedef map<uint64_t, HostEntity> MAP;
	typedef pair<uint64_t, HostEntity> PAIR;
	typedef HostEntity VALUE;

public:
	ZHTClient();

	/*todo: dynMemFile, at this point, it's neighbor file*/
	ZHTClient(const string& dynMemFile, const bool& tcp);
	virtual ~ZHTClient();

	/*todo: dynMemFile, at this point, it's neighbor file*/
	int init(const string& dynMemFile, const bool& tcp);
	int init2(const string& dynMemFile, const bool& tcp);
	int lookup(const string& pair, string& result);
	int remove(const string& pair);
	int insert(const string& pair);
	int tearDown();
	int tearDown2();

public:
	static string sendPkg(const string& host, const uint& port,
			const string& msg);
	static string sendPkg(const string& host, const uint& port,
			const string& msg, string& result);

private:
	const string getHostByKey(const string& msg, HostEntity& he);
	const HostEntity* getHostByKey2(const string& msg);

	const string getDestZHTByKey(const HostEntity& coordinator,
			const string& msg, string& destZHT);
	uint64_t getRingPosByKey(const uint64_t& hashKey);

	int getSockByKey2(const string& msg, const bool& tcp);

	int commonOp(const string& opcode, const string& pair, string& result);
	string commonOpInternal(const string& opcode, const HostEntity* phe,
			const string& pair, string& result);

	int readDynamicMembership(string dynMemFile);

private:
	static int getSockCached(const HostEntity* phe, const bool& tcp);
	static int getSockCached(const string& host, const uint& port,
			const bool& tcp);

	static void parseStatusAndResult(const int& sock, string& sstatus,
			string& result);
	static string sendPkgInternal(const string& host, const uint& port,
			const string& msg, string& result);

private:
	static int UDP_SOCKET;
	static int CACHE_SIZE;
	static LRUCache<string, int> CONN_CACHE;

private:
	bool TCP;
	MAP _dynMemMap; //map to store dynamic membership
	string _dynMemFile; //dynamic membership file

	vector<struct HostEntity> _MemList;
};

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* ZHTCLIENT_H_ */
