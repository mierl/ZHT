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
 * ZHTManager.h
 *
 *  Created on: Jul 23, 2012
 *      Author: tony, xiaobingo
 */

#ifndef ZHTMANAGER_H_
#define ZHTMANAGER_H_

#include "NeighborManager.h"
#include "../meta.pb.h"

#include <map>
#include <sys/types.h>
#include <string>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

class ZHTManager {
public:
	typedef map<uint64_t, Neighbor> MAP;
	typedef pair<uint64_t, Neighbor> PAIR;
	typedef map<uint64_t, Neighbor>::iterator MIT;
	typedef map<uint64_t, Neighbor>::reverse_iterator MRIT;

public:
	explicit ZHTManager(const string& neighborManagerName);
	virtual ~ZHTManager();

	string addAllZHTOnNode();
	int addZHT(const uint& port);
	int addZHT(const string& localIp, const uint& port);

	int removeAllZHTOnNode();
	int removeZHT(const uint& port);
	int removeZHT(const string& localIp, const uint& port);

	void initNeighborManager();

private:
	string addAllZHTOnNodeInternal(const ConfigEntry* const configEntries,
			const int& length);
	string addZHTInternal(const string& localIp, const uint& port);

	/* NEIGHBORS: lower and upper neighbor to migrate from
	 * TARGETZHT: target ZHT to to migrate to
	 * MIGRINGPOS: ring position for target ZHT after migration
	 * MIGRATEINST: novoht instances being migrated
	 * LN(LOWER NEIGHBOR): from lower neighbor if true, from upper neighbor if false
	 * */
	string migrateFromNeighbor(Neighbor* const neighbors,
			const string& targetHostPort, uint64_t& migRingPos,
			uint& migrateInst, const bool& ln, Package& rpkg);

	string migrateToNeighbor(const string& targetHostPort,
			const Package& lowerRPkg, const Package& upperRPkg,
			const Neighbor& targetAsNeighbor);

	bool isSameNeighbor(Neighbor* const neighbors);

	string getSourceZHTAsNeighbor(const Neighbor& sourceZHT,
			Neighbor& asNeighbor);
	/*
	 * prepair incremental changes(of global membership) to broadcast
	 * LOCALIP:
	 * PORT:
	 * NEIGHBORS:
	 * TARGETUUID: the uuid of this newly added zht
	 * MIGCOUNT: novoht instances being migrated
	 * NMAP: neighbor map to store incremental changes of global membership, due to this zht addition/removal
	 * */
	string getGlobalmembershipChanges(const uint64_t& targetUuid,
			const string& targetHostPort, Neighbor* const neighbors,
			const uint& novohtInst, MAP* const nmap);

	Neighbor getTargetZHTAsNeighbor(const uint64_t& targetUuid,
			const string& targetHostPort, const uint& novohtInst);

	string notifyMigDone(Neighbor* const neighbors, const uint& lowerNovoInst,
			const uint& upperNovoInst, const string& targetHostPort);

	string notifyMigCancle(Neighbor* const neighbors,
			const string& targetHostPort);

	string notifyMigDoneInternal(const string& hostPort, const uint& novoInst,
			const string& opcode, const bool& ln);
	string notifyMigDoneInternal(const string& hostPort, const uint& novoInst,
			const string& opcode, const string& wifecode, const bool& ln);

public:
	static const string NEIGHBOR_MANAGER_NAME;

private:
	NeighborManager _NeighborManager;

};

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* ZHTMANAGER_H_ */
