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
 * ZHTManager.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: tony, xiaobingo
 */

#include <stdint.h>
#include <stdlib.h>
using namespace std;

#include "ZHTManager.h"
#include "../zht_util.h"
#include "../meta.pb.h"
#include "Util.h"
#include "Const-impl.h"

#include "ConfigHandler.h"
#include "ZHTClient.h"
#include "NeighborManager.h"

#include <iostream>
#include <vector>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

const string ZHTManager::NEIGHBOR_MANAGER_NAME = "ZHTManager_NeighborManager";

ZHTManager::ZHTManager(
		const string& neighborManagerName = NEIGHBOR_MANAGER_NAME) :
		_NeighborManager(neighborManagerName) {
}

ZHTManager::~ZHTManager() {

}

string ZHTManager::addAllZHTOnNode() {

	ConfigHandler::IT kvi;
	ConfigHandler::MAP* map = &ConfigHandler::NodeParameters;

	vector<ConfigEntry> ipports;

	for (kvi = map->begin(); kvi != map->end(); kvi++) {

		ipports.push_back(kvi->second);

		/*		ConfigEntry ce = kvi->second;
		 string start = ce.name().substr(0, 1);

		 if (start.compare("$") == 0) { //ip:port, start with $

		 ce.name(ce.name().substr(1)); //remove "$"
		 ipports.push_back(ce);
		 }*/
	}

	return addAllZHTOnNodeInternal(&ipports[0], ipports.size());

}

string ZHTManager::addAllZHTOnNodeInternal(
		const ConfigEntry* const configEntries, const int& length) {

	double start = getTime_msec();

	string sstaus = Const::ASC_REC_NTHDOZM;

	for (int i = 0; i < length; i++) {

		sstaus = addZHTInternal(configEntries[i].name(),
				atoi(configEntries[i].value().c_str()));
	}

	double end = getTime_msec();

	cout << "migration, cost " << end - start << " ms" << endl;

	return sstaus; //todo...
}

int ZHTManager::addZHT(const uint& port) {

	return 0;
}

int ZHTManager::addZHT(const string& localIp, const uint& port) {

	return 0;
}

/*send message to get lower and upper source zht as neighbor, ZSC_OPC_GET_ASNEIGHBOR*/
string ZHTManager::getSourceZHTAsNeighbor(const Neighbor& sourceZHT,
		Neighbor& asNeighbor) {

	Package pkg;
	pkg.set_opcode(Const::ZSC_OPC_GET_ASNGHB);

	string result;
	string sstatus = ZHTClient::sendPkg(sourceZHT.host(), sourceZHT.port(),
			pkg.SerializeAsString(), result);

	Package pkg2;
	pkg2.ParseFromString(result);

	asNeighbor.assign(pkg2.asneighbor());

	return sstatus;
}

/*class MigEntries {

 public:
 uint novohtInst = 0;
 uint64_t lowerMigRingPos = 0;
 uint64_t upperMigRingPos = 0;

 uint lowerNovoInst = 0;
 uint upperNovoInst = 0;

 string targetHostPort;
 string opcode;
 string wifecode;

 };*/

/* todo: how to support only one zht???
 * todo: how to support lower == upper??
 * in case: there is only one zht,
 * begin to migrate novoht from lower neighbor...,
 * just pick lower(=uppper) neighbor*/
string ZHTManager::addZHTInternal(const string& localIp, const uint& port) {

	string sstatus = Const::ASC_REC_UNPR;

	HostEntity he = getHostEntity(localIp, port);

	/*update global membership to local*/
	sstatus = _NeighborManager.pullGlobalMembership();

	/*calc ring pos to add zht between*/
	Neighbor* neighbors = new Neighbor[2];
	_NeighborManager.calcRingPosToAddZHT(neighbors);

	uint novohtInst = 0;
	uint64_t lowerMigRingPos = 0;
	uint64_t upperMigRingPos = 0;

	uint lowerNovoInst = 0;
	uint upperNovoInst = 0;

	Package lowerRPkg;
	Package upperRPkg;

	/*get target zht to migrate to*/
	string targetHostPort = Address::genHostPort(localIp, port);

	/* begin to migrate novoht from lower neighbor...*/
	sstatus = migrateFromNeighbor(neighbors, targetHostPort, lowerMigRingPos,
			novohtInst, true, lowerRPkg);

	if (sstatus == Const::ZSC_REC_NONEEDMIG)
		return sstatus;

	lowerNovoInst = novohtInst;
	if (sstatus != Const::ZSC_REC_SUCC) { //fail to migrate from lower neighbor

		notifyMigDoneInternal(neighbors[0].toHostPort(), 0,
				Const::ZSC_OPC_MIGDONESRC, Const::ZSC_OPC_OPR_CANCEL, true);

		return sstatus;
	}

	/*begin to migrate novoht from uppper neighbor...*/
	sstatus = migrateFromNeighbor(neighbors, targetHostPort, upperMigRingPos,
			novohtInst, false, upperRPkg);

	if (sstatus == Const::ZSC_REC_NONEEDMIG)
		return sstatus;

	upperNovoInst = novohtInst - lowerNovoInst;
	if (sstatus != Const::ZSC_REC_SUCC) { //fail to migrate from upper neighbor

		notifyMigDoneInternal(neighbors[0].toHostPort(), 0,
				Const::ZSC_OPC_MIGDONESRC, Const::ZSC_OPC_OPR_CANCEL, true);

		notifyMigDoneInternal(neighbors[1].toHostPort(), 0,
				Const::ZSC_OPC_MIGDONESRC, Const::ZSC_OPC_OPR_CANCEL, false);

		return sstatus;
	}

	uint64_t targetUuid =
			lowerMigRingPos < upperMigRingPos ?
					lowerMigRingPos : upperMigRingPos;

	/*begin to migrate novoht to target neighbor...*/
	Neighbor targetAsNeighbor = getTargetZHTAsNeighbor(targetUuid,
			targetHostPort, novohtInst);
	sstatus = migrateToNeighbor(targetHostPort, lowerRPkg, upperRPkg,
			targetAsNeighbor);
	if (sstatus != Const::ZSC_REC_SUCC) { //fail to migrate to target neighbor

		notifyMigCancle(neighbors, targetHostPort);

		return sstatus;
	}

	/*get global membership incremental changes*/
	MAP* nmap = new MAP();
	sstatus = getGlobalmembershipChanges(targetUuid, targetHostPort, neighbors,
			novohtInst, nmap);
	string gmembershipChange = NeighborManager::getGlobalMembershipAsString(
			nmap);

	/*locally update global membership changes*/
	_NeighborManager.updateGlobalMembership(gmembershipChange);

	/*print global membership*/
	_NeighborManager.printMembership();

	/*broadcast global membership changes*/
	sstatus = _NeighborManager.broadcastGlobalMembership(gmembershipChange);

	/*notify source and target zht of migration being done*/
	sstatus = notifyMigDone(neighbors, lowerNovoInst, upperNovoInst,
			targetHostPort);

	delete nmap;
	delete[] neighbors;

	return sstatus;
}

string ZHTManager::notifyMigCancle(Neighbor* const neighbors,
		const string& targetHostPort) {

	string sstatus;

	sstatus = notifyMigDoneInternal(neighbors[0].toHostPort(), 0,
			Const::ZSC_OPC_MIGDONESRC, Const::ZSC_OPC_OPR_CANCEL, true);

	sstatus = notifyMigDoneInternal(neighbors[1].toHostPort(), 0,
			Const::ZSC_OPC_MIGDONESRC, Const::ZSC_OPC_OPR_CANCEL, false);

	sstatus = notifyMigDoneInternal(targetHostPort, 0,
			Const::ZSC_OPC_MIGDONETGT, Const::ZSC_OPC_OPR_CANCEL, false);

	return sstatus;
}

string ZHTManager::notifyMigDone(Neighbor* const neighbors,
		const uint& lowerNovoInst, const uint& upperNovoInst,
		const string& targetHostPort) {

	string sstatus;

	/*send message to lower source zht, saying migrate is done*/
	sstatus = notifyMigDoneInternal(neighbors[0].toHostPort(), lowerNovoInst,
			Const::ZSC_OPC_MIGDONESRC, true);

	/*send message to upper source zht, saying migrate is done*/
	sstatus = notifyMigDoneInternal(neighbors[1].toHostPort(), upperNovoInst,
			Const::ZSC_OPC_MIGDONESRC, false);

	/*send message to target source zht, saying migrate is done*/
	sstatus = notifyMigDoneInternal(targetHostPort,
			lowerNovoInst + upperNovoInst, Const::ZSC_OPC_MIGDONETGT, false);

	return sstatus;
}

inline string ZHTManager::notifyMigDoneInternal(const string& hostPort,
		const uint& novoInst, const string& opcode, const bool& ln) {

	return notifyMigDoneInternal(hostPort, novoInst, opcode, opcode, ln);
}

inline string ZHTManager::notifyMigDoneInternal(const string& hostPort,
		const uint& novoInst, const string& opcode, const string& wifecode,
		const bool& ln) {

	Package pkg;
	pkg.set_opcode(opcode);
	pkg.set_wifecode(wifecode);
	pkg.set_migrateinst(novoInst);
	pkg.set_uprange(ln);

	return ZHTClient::sendPkg(Address::getHost(hostPort),
			Address::getPort(hostPort), pkg.SerializeAsString());
}

Neighbor ZHTManager::getTargetZHTAsNeighbor(const uint64_t& targetUuid,
		const string& targetHostPort, const uint& novohtInst) {

	Address address(targetUuid, Address::getHost(targetHostPort),
			Address::getPort(targetHostPort));
	Workload workload(novohtInst, ConfigHandler::NC_ZHT_CAPACITY);

	Neighbor neighbor;
	neighbor.setAddress(address);
	neighbor.setWorkload(workload);

	return neighbor;
}

string ZHTManager::getGlobalmembershipChanges(const uint64_t& targetUuid,
		const string& targetHostPort, Neighbor* const neighbors,
		const uint& novohtInst, MAP* const nmap) {

	Neighbor neighbor = getTargetZHTAsNeighbor(targetUuid, targetHostPort,
			novohtInst);

	(*nmap)[neighbor.uuid()] = neighbor;

	//send message to get lower source zht as neighbor
	Neighbor lowerNeighbor;
	string sstatus = getSourceZHTAsNeighbor(neighbors[0], lowerNeighbor);
	(*nmap)[lowerNeighbor.uuid()] = lowerNeighbor;

	//send message to get upper source zht as neighbor
	Neighbor upperNeighbor;
	sstatus = getSourceZHTAsNeighbor(neighbors[1], upperNeighbor);
	(*nmap)[upperNeighbor.uuid()] = upperNeighbor;

	return sstatus;
}

bool ZHTManager::isSameNeighbor(Neighbor* const neighbors) {

	return neighbors[0].uuid() == neighbors[1].uuid();
}

string ZHTManager::migrateToNeighbor(const string& targetHostPort,
		const Package& lowerRPkg, const Package& upperRPkg,
		const Neighbor& targetAsNeighbor) {

	Package pkg;
	pkg.set_opcode(Const::ZSC_OPC_MIGTARGET);

	const uint64_t* it;
	for (it = lowerRPkg.migringpos().begin();
			it != lowerRPkg.migringpos().end(); it++) {

		pkg.add_migringpos(*it);
	}
	for (it = upperRPkg.migringpos().begin();
			it != upperRPkg.migringpos().end(); it++) {

		pkg.add_migringpos(*it);
	}

	pkg.set_targetzht(targetHostPort);
	pkg.set_asneighbor(targetAsNeighbor.toString());

	string result;
	string sstatus = ZHTClient::sendPkg(Address::getHost(targetHostPort),
			Address::getPort(targetHostPort), pkg.SerializeAsString(), result);

	return sstatus;
}

bool shouldMigFromLowerNeighbor(Neighbor* const neighbors) {

	return false;
}

bool shouldMigFromUpperNeighbor(Neighbor* const neighbors) {

	return false;
}

string ZHTManager::migrateFromNeighbor(Neighbor* const neighbors,
		const string& targetHostPort, uint64_t& migRingPos, uint& migrateInst,
		const bool& ln, Package& rpkg) {

	Neighbor source;

	if (ln)
		source = neighbors[0];
	else
		source = neighbors[1];

	uint lower = neighbors[0].getWorkload().coHostNoVoHTInstances();
	uint upper = neighbors[1].getWorkload().coHostNoVoHTInstances();

	uint avg = (lower + upper) / 2;

	uint count = 0;
	if (ln) {

		count = lower - avg / 2; //todo: need more accurate

		if (count <= 0 || lower <= 1) //no need to move novoht from
			return Const::ZSC_REC_NONEEDMIG;

	} else {

		count = upper - avg / 2; //todo: need more accurate

		if (count <= 0 || upper <= 1) //no need to move novoht from
			return Const::ZSC_REC_NONEEDMIG;
	}

	migrateInst += count;

	Package pkg;
	pkg.set_opcode(Const::ZSC_OPC_MIGSOURCE);
	pkg.set_migrateinst(count);
	if (ln)
		pkg.set_uprange(true);
	else
		pkg.set_uprange(false);
	pkg.set_targetzht(targetHostPort);

	string result;
	string sstatus = ZHTClient::sendPkg(source.host(), source.port(),
			pkg.SerializeAsString(), result);

	rpkg.ParseFromString(result);
	migRingPos = (*(rpkg.migringpos().begin()));

	return sstatus;
}

int ZHTManager::removeAllZHTOnNode() {

	return 0;
}

int ZHTManager::removeZHT(const uint& port) {
	return 0;
}

int ZHTManager::removeZHT(const string& localIp, const uint& port) {
	return 0;
}

void ZHTManager::initNeighborManager() {

	NeighborManager::initNeighborManager(_NeighborManager);
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */

