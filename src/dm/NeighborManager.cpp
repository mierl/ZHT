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
 * NeighborManager.cpp
 *
 *  Created on: Jul 26, 2012
 *      Author: tony, xiaobingo
 */

#include "NeighborManager.h"
#include  "ConfigHandler.h"
#include "../zht_util.h"
#include "Util.h"
#include "Const-impl.h"
#include "Neighbor.h"

#include "ZHTClient.h"
#include "../meta.pb.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

NeighborManager::MAP* NeighborManager::_NbMap = new MAP();

NeighborManager::NeighborManager() {

}

NeighborManager::NeighborManager(const string& id) {

}

NeighborManager::~NeighborManager() {

}

/*ZHTNode{%s_%u}_NeighborManager: format with 192.168.1.10, 50000*/
string NeighborManager::genNeighborManagerName(
		const string& neighborManagerFormat, const string& host,
		const uint& port) {

	char buf[500];
	memset(buf, 0, 500);
	int n = sprintf(buf, neighborManagerFormat.c_str(), host.c_str(), port); //192.168.1.10, 50000

	string sNeighborManagerName(buf, 0, n);

	return sNeighborManagerName;
}

void NeighborManager::initNeighborManager(NeighborManager& neighborManager) {

	neighborManager.erazeAllNeighbors();

	ConfigHandler::IT kvi;
	ConfigHandler::MAP* map = &ConfigHandler::NeighborSeeds;

	int size = map->size();

	uint64_t gap = HashUtil::ULL_MAX / (uint64_t) size; //equally partition the hash space

	int i = 0;
	for (kvi = map->begin(); kvi != map->end(); kvi++) {

		/*kv.name: ip(host);
		 *kv.value: port*/
		ConfigEntry kv = kvi->second;

		HostEntity he = getHostEntity(kv.name(), atoi(kv.value().c_str()));
		uint64_t uuid = HashUtil::genHash(HashUtil::genBase(he.host, he.port));

		Address address(kv.name(), atoi(kv.value().c_str()));

		if (i == 0)
			address.uuid(RingUtil::RING_BASE);
		else
			address.uuid(RingUtil::RING_BASE + i * gap);

		Workload workload(ConfigHandler::ZC_MAX_ZHT / size,
				ConfigHandler::NC_ZHT_CAPACITY); //ZC_MAX_ZHT should be N * size

		Neighbor neighbor;
		neighbor.setAddress(address.toString());
		neighbor.setWorkload(workload.toString());

		neighborManager.addNeighbor(neighbor);

		i++;
	}
}

bool NeighborManager::isNeighborSeed(const string& host, const string& port) {

	bool result = false;

	ConfigHandler::IT kvi;
	ConfigHandler::MAP* map = &ConfigHandler::NeighborSeeds;

	for (kvi = map->begin(); kvi != map->end(); kvi++) {

		ConfigEntry kv = kvi->second; //name:host; value:port

		if (kv.name() == host
				&& atoi(kv.value().c_str()) == atoi(port.c_str())) { //compare host:port

			result = true;
			break;
		}
	}

	return result;
}

uint64_t NeighborManager::genNeighborUuid(const string& host,
		const string& port) {

	return genNeighborUuid(host, atoi(port.c_str()));
}

uint64_t NeighborManager::genNeighborUuid(const string& host,
		const uint& port) {

	HostEntity he = getHostEntity(host, port);
	uint64_t uuid = HashUtil::genHash(HashUtil::genBase(he.host, he.port));

	return uuid;
}

const Neighbor NeighborManager::getNeighborByUuid(const uint64_t& uuid) {

	Neighbor result;

	MIT it = _NbMap->find(uuid);

	if (it != _NbMap->end())
		return it->second;

	return result;
}

string NeighborManager::getGlobalMembershipAsString() {

	string result = getGlobalMembershipAsString(_NbMap);

	return result;
}

string NeighborManager::getGlobalMembershipAsString(MAP* const neighborMap) {

	stringstream ss;

	for (MIT it = neighborMap->begin(); it != neighborMap->end(); it++) {

		ss << it->second.toString() << "\t";
	}

	Const::prtMem(neighborMap);

	return ss.str();
}

void NeighborManager::updateGlobalMembership(const string& gMembership) {

	const char* delimiter = "\t";

	string remains = gMembership;

	while (!remains.empty()) {

		size_t found = remains.find(delimiter);

		if (found != string::npos) {

			string token = Const::trim(remains.substr(0, int(found)));
			updateNeighbor(Neighbor(token));
			remains = remains.substr(int(found) + 1);
		}
	}
}

Neighbor NeighborManager::getUuidLowestNeighbor() {

	MIT it = _NbMap->begin();

	return getNeighborByUuid(it->first);
}

Neighbor NeighborManager::getUuidHighestNeighbor() {

	MRIT rit = _NbMap->rbegin();

	return getNeighborByUuid(rit->first);
}

bool NeighborManager::isUuidLowestNeighbor(const Neighbor& neighbor) {

	return getUuidLowestNeighbor().uuid() == neighbor.uuid();
}

bool NeighborManager::isUuidHighestNeighbor(const Neighbor& neighbor) {

	return getUuidHighestNeighbor().uuid() == neighbor.uuid();
}

void NeighborManager::updateNeighbor(const Neighbor& neighbor) {

	Const::prtMem(_NbMap);

	if (neighbor.ouuid() != Const::ADDR_UUIDZERO
			&& neighbor.ouuid() != neighbor.uuid()) { //means this neighbor changed

		MIT it = _NbMap->find(neighbor.ouuid());

		if (it != _NbMap->end())
			_NbMap->erase(it);
	}

	Const::prtMem(_NbMap);

	(*_NbMap)[neighbor.uuid()] = neighbor;

	Const::prtMem(_NbMap);
}

ConfigEntry NeighborManager::getRandomNeighborFromMap() {

	ConfigEntry result;

	int size = _NbMap->size();
	int mod = rand() % size;

	int i = 0;
	for (MIT it = _NbMap->begin(); it != _NbMap->end(); it++) {

		if (i == mod) {

			Neighbor neighbor = it->second;

			result = ConfigEntry(neighbor.host(),
					Const::toString(neighbor.port()));

			break;
		}

		i++;
	}

	return result;
}

ConfigEntry NeighborManager::getRandomNeighborFromSeeds() {

	ConfigEntry result;

	ConfigHandler::IT kvi;
	ConfigHandler::MAP* map = &ConfigHandler::NeighborSeeds;

	int i = 0;
	int size = map->size();
	int mod = rand() % size;

	for (kvi = map->begin(); kvi != map->end(); kvi++) {

		if (i == mod) {
			result = kvi->second;
			break;
		}

		i++;
	}

	return result;
}

/*todo: what if either _NbMap or NeighborSeeds is out of date?
 * what if the host:port from them is not working? */
string NeighborManager::pullGlobalMembership() {

	ConfigEntry ce;

	int size = _NbMap->size();

	if (size > 0)
		ce = getRandomNeighborFromMap();
	else
		ce = getRandomNeighborFromSeeds();

	return pullGlobalMembership(ce.name(), atoi(ce.value().c_str()));
}

void NeighborManager::calcRingPosToAddZHT(Neighbor* const neighbors) {

	typedef multimap<uint, Neighbor> MMAP;
	typedef multimap<uint, Neighbor>::iterator IT;
	typedef multimap<uint, Neighbor>::reverse_iterator RIT;
	typedef pair<uint, Neighbor> MPAIR;

	MMAP rmap;

	for (MIT it = _NbMap->begin(); it != _NbMap->end(); it++) {

		rmap.insert(
				MPAIR(it->second.getWorkload().coHostNoVoHTInstances(),
						it->second));
	}

	RIT rit = rmap.rbegin();

	/*in case the overloaded zht is uuid highest, must be upper neighbor to migrate from*/
	if (isUuidHighestNeighbor(rit->second)) {

		neighbors[1] = rit->second;

		rit++;
		neighbors[0] = rit->second;

	} else {

		neighbors[0] = rit->second;

		rit++;
		neighbors[1] = rit->second;
	}
}

string NeighborManager::pullGlobalMembership(const string& hostPort) {

	return pullGlobalMembership(Address::getHost(hostPort),
			Address::getPort(hostPort));
}

/*get global membership and workload from ZHTNode*/
string NeighborManager::pullGlobalMembership(const string& host,
		const uint& port) {

	string sstatus;

	try {
		string gMembership;

		Package pkg;
		pkg.set_opcode(Const::ZSC_OPC_GET_GMEM);
		sstatus = ZHTClient::sendPkg(host, port, pkg.SerializePartialAsString(),
				gMembership);

		flushGlobalMembership(gMembership);

	} catch (exception& e) {

		fprintf(stderr, "%s, exception caught:\n\t%s",
				"NeighborManager::pullGlobalMembership(const string& host, const uint& port)",
				e.what());
	}

	return sstatus;
}

void NeighborManager::flushGlobalMembership(const string& gMembership) {

	Const::prtMem(_NbMap);

	erazeAllNeighbors();

	updateGlobalMembership(gMembership);

	Const::prtMem(_NbMap);
}

Neighbor NeighborManager::getNeighbor(const string& uuid) {

	Neighbor result;

	MIT it = _NbMap->find(Const::toUInt64(uuid));

	if (it != _NbMap->end())
		result = it->second;

	return result;
}

Neighbor NeighborManager::getNeighbor(const uint64_t& uuid) {

	return getNeighbor(Const::toString(uuid));
}

Neighbor NeighborManager::getNeighborByHostPort(const string& hostPort) {

	Neighbor result;

	for (MIT it = _NbMap->begin(); it != _NbMap->end(); it++) {

		Neighbor neighbor = it->second;

		if (neighbor.toHostPort() == hostPort) {
			result = neighbor;
			break;
		}
	}

	return result;
}

void NeighborManager::addNeighbor(const Neighbor& neighbor) {

	Const::prtMem(_NbMap);

	(*_NbMap)[neighbor.uuid()] = neighbor;

	Const::prtMem(_NbMap);
}

//todo: broadcast incremental changes of global membership
string NeighborManager::broadcastGlobalMembership(
		const string& gmembershipChanges) {

	Const::prtMem(_NbMap);

	string sstatus = Const::ZSC_REC_UNPR;

	/* broadcast the global membership */
	for (MIT it = _NbMap->begin(); it != _NbMap->end(); it++) {

		Neighbor neighbor = it->second;

		Package pkg;
		pkg.set_opcode(Const::ZSC_OPC_BRD_GMEM); //todo:
		pkg.set_gmembership(gmembershipChanges);

		sstatus = ZHTClient::sendPkg(neighbor.host(), neighbor.port(),
				pkg.SerializeAsString());
	}

	/* DONE the broadcast */
	for (MIT it = _NbMap->begin(); it != _NbMap->end(); it++) {

		Neighbor neighbor = it->second;

		Package pkg;
		pkg.set_opcode(Const::ZSC_OPC_BRDDN_GMEM); //todo:

		sstatus = ZHTClient::sendPkg(neighbor.host(), neighbor.port(),
				pkg.SerializeAsString());

	}

	Const::prtMem(_NbMap);

	return sstatus;
}

void NeighborManager::printMembership() {

	for (MIT it = _NbMap->begin(); it != _NbMap->end(); it++) {

		fprintf(stdout, "%s => %s\n", Const::toString(it->first).c_str(),
				it->second.toString().c_str());
	}
}

uint64_t NeighborManager::getStartRingToken(const string& uuid) {

	MIT it = _NbMap->begin();

	return it->first;
}

uint64_t NeighborManager::getEndRingToken(const uint64_t& uuid) {

	MIT it = _NbMap->end();

	return it->first;
}

uint64_t NeighborManager::getNextRingToken(const string& uuid) {

	return getNextRingToken(strtoul(uuid.c_str(), NULL, 10));
}

uint64_t NeighborManager::getNextRingToken(const uint64_t& uuid) {

	uint64_t token = 0;

	MIT it = _NbMap->find(uuid);
	it++;

	if (it == _NbMap->end()) {

		it = _NbMap->begin();
		token = it->first;

	} else {

		token = it->first;

	}

	return token;
}

string NeighborManager::getDestZHTByHashKey(const uint64_t& hashKey) {

	uint64_t destZHT = 0;

	Neighbor neighbor = getZHTAsNeighbor(hashKey);

	string result;
	if (neighbor.host().empty())
		result = string("");
	else
		result = Address::genHostPort(neighbor.host(), neighbor.port());

	return result;
}

Neighbor NeighborManager::getZHTAsNeighbor(const uint64_t& hashKey) {

	Neighbor result;

	MRIT rit = _NbMap->rbegin();

	if (rit->first <= hashKey) {

		return rit->second;

	} else {

		for (; rit != _NbMap->rend(); rit++) {

			if (hashKey < rit->first)
				continue;

			result = rit->second;
			break;
		}
	}

	return result;
}

void NeighborManager::erazeAllNeighbors() {

	_NbMap->clear();
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
