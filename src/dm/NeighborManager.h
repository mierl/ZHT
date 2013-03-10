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
 * NeighborManager.h
 *
 *  Created on: Jul 26, 2012
 *      Author: tony, xiaobingo
 */

#ifndef NEIGHBORMANAGER_H_
#define NEIGHBORMANAGER_H_

#include "ConfigEntry.h"

#include "Neighbor.h"

#include <set>
#include <map>
#include <stdint.h>
#include <sys/types.h>
#include <string>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

/*
 *
 */
class NeighborManager { //todo: save dynamic membership to a file so as to be used in ZHTClient

public:
	typedef map<uint64_t, Neighbor> MAP;
	typedef pair<uint64_t, Neighbor> PAIR;
	typedef map<uint64_t, Neighbor>::iterator MIT;
	typedef map<uint64_t, Neighbor>::reverse_iterator MRIT;

	typedef set<uint64_t> SET;
	typedef set<uint64_t>::iterator SIT;
	typedef set<uint64_t>::reverse_iterator SRIT;

public:
	NeighborManager();
	explicit NeighborManager(const string& id);
	virtual ~NeighborManager();

public:
	string pullGlobalMembership();
	string pullGlobalMembership(const string& hostPort);
	string pullGlobalMembership(const string& host, const uint& port);
	void calcRingPosToAddZHT(Neighbor* const neighbors);

	Neighbor getNeighbor(const string& uuid);
	Neighbor getNeighbor(const uint64_t& uuid);
	Neighbor getNeighborByHostPort(const string& hostPort);
	void addNeighbor(const Neighbor& neighbor);

	string broadcastGlobalMembership(const string& gmembershipChanges);

	void printMembership();

private:

	ConfigEntry getRandomNeighborFromMap();
	ConfigEntry getRandomNeighborFromSeeds();

public:
	static string genNeighborManagerName(const string& neighborManagerFormat,
			const string& host, const uint& port);

	static void initNeighborManager(NeighborManager& neighborManager);

	static bool isNeighborSeed(const string& host, const string& port);
	static uint64_t genNeighborUuid(const string& host, const string& port);
	static uint64_t genNeighborUuid(const string& host, const uint& port);

	static uint64_t getStartRingToken(const string& uuid);
	static uint64_t getEndRingToken(const uint64_t& uuid);
	static uint64_t getNextRingToken(const string& uuid);
	static uint64_t getNextRingToken(const uint64_t& uuid);
	static string getDestZHTByHashKey(const uint64_t& hashKey);
	static Neighbor getZHTAsNeighbor(const uint64_t& hashKey);

	static const Neighbor getNeighborByUuid(const uint64_t& uuid);

	static string getGlobalMembershipAsString();
	static string getGlobalMembershipAsString(MAP* const neigborMap);
	static void updateGlobalMembership(const string& gMembership);
	static void updateNeighbor(const Neighbor& neighbor);
	static void flushGlobalMembership(const string& gMembership);

private:
	static void erazeAllNeighbors();

	static Neighbor getUuidLowestNeighbor();
	static Neighbor getUuidHighestNeighbor();

	static bool isUuidLowestNeighbor(const Neighbor& neighbor);
	static bool isUuidHighestNeighbor(const Neighbor& neighbor);

private:
	static MAP* _NbMap;
};

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* NEIGHBORMANAGER_H_ */
