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
 * ServiceHub.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: tony, xiaobingo
 */

#include "ServiceHub.h"

#include "Const-impl.h"
#include "../zht_util.h"
#include <sys/types.h>

#include "HTWorker.h"

#include "MigrateManager.h"
#include "NoVoHTManager.h"

#include "Status.h"

#include "ZHTNode.h"
#include "NeighborManager.h"

#include "Const-impl.h"

#include <stdio.h>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

ServiceHub::ServiceHub() {
}

ServiceHub::ServiceHub(const char *buf, const string& sourceZHT) :
		_buf(buf), _sourceZHT(sourceZHT) {
}

ServiceHub::ServiceHub(const int& fd, const char *buf, const sockaddr& sender,
		const int& protocol, const string& sourceZHT) :
		_fd(fd), _buf(buf), _sender(sender), _protocol(protocol), _sourceZHT(
				sourceZHT) {
}

ServiceHub::~ServiceHub() {
}

string ServiceHub::run() {

	string rcode = Const::ZSC_REC_UNPR;

	Package pkg;
	pkg.ParseFromArray(_buf, Env::MAX_MSG_SIZE);

	string opcode = pkg.opcode();

	if (opcode == Const::ZSC_OPC_MIGSOURCE) {

		beforeMigSource(pkg); //before

		Package rpkg; //return pkg;
		rcode = doingMigSource(pkg, rpkg);

		rcode = getMigSourceReturnPkg(rcode, rpkg);

	} else if (opcode == Const::ZSC_OPC_MIGTARGET) {

		MigrateStatus::value(MSEnum::MS_DOING);

		doingMigTarget(pkg);

		rcode = Const::ZSC_REC_SUCC;

	} else if (opcode == Const::ZSC_OPC_MIGDONESRC) {

		MigrateStatus::value(MSEnum::MS_DONE);

		afterMigSource(pkg); //after

		rcode = Const::ZSC_REC_SUCC;

	} else if (opcode == Const::ZSC_OPC_MIGDONETGT) {

		MigrateStatus::value(MSEnum::MS_DONE);

		rcode = Const::ZSC_REC_SUCC;

	} else {

		HTWorker htw = HTWorker(_sourceZHT);
		rcode = htw.run(_buf, Env::MAX_MSG_SIZE);
	}

	return rcode;
}

void ServiceHub::preapreMigrage(const Package& pkg) {

	MigrateStatus::value(MSEnum::MS_DOING);

	/*	NoVoHTManager::backupHTFilesBeingMigrated(pkg.migrateinst(), pkg.uprange()); //todo: live migration */
}

string ServiceHub::doingMigSource(const Package& pkg, Package& rpkg) {

	string sstatus;

	sstatus = MigrateManager::migrateNovoHT(_sourceZHT, pkg, rpkg);

	if (sstatus == Const::ZSC_REC_SUCC) { //how to reverse????

		changeStateOfThisZHT(pkg);
		updateThisZHTAsNeighbor();
	}

	return sstatus;
}

void ServiceHub::updateThisZHTAsNeighbor() {

	Neighbor neighbor;
	neighbor.getAddress().assign(ZHTNode::getAddress());
	neighbor.getWorkload().assign(ZHTNode::getWorkload());

	Const::prtNb(neighbor);

	NeighborManager::updateNeighbor(neighbor);
}

void ServiceHub::changeStateOfThisZHT(const Package& pkg) {

	uint64_t uuid = NoVoHTManager::getNewUuidForZHTAfterMig(pkg.migrateinst(),
			pkg.uprange());
	uint novohtInst = ZHTNode::getWorkload().coHostNoVoHTInstances()
			- pkg.migrateinst();

	ZHTNode::getAddress().ouuid(ZHTNode::getAddress().uuid()); //remember old uuid, todo: bugs here, ouuid unupdated.
	ZHTNode::getAddress().uuid(uuid);
	ZHTNode::getWorkload().coHostNoVoHTInstances(novohtInst);

#if ILOG
	fprintf(stdout, "ZHTNode is: %s:%s\n\n",
			ZHTNode::getAddress().toString().c_str(),
			ZHTNode::getWorkload().toString().c_str());
#endif
}

string ServiceHub::getMigSourceReturnPkg(const string& rcode, Package& rpkg) {

	return Const::concat(rcode, rpkg.SerializeAsString());
}

void ServiceHub::beforeMigSource(const Package& pkg) {

	MigrateStatus::value(MSEnum::MS_DOING);

	HTWorker::setTargetZHT(pkg.targetzht()); //set target zht
}

void ServiceHub::afterMigSource(const Package& pkg) {

	HTWorker::clearTargetZHT(); //clear target zht

	NoVoHTManager::clearNoVoRangeBeingMigrated();

	if (pkg.wifecode() == Const::ZSC_OPC_OPR_CANCEL) //cancel migrate, don't do hereafter-operations
		return;

	NoVoHTManager::resetNoVoRange(pkg.migrateinst(), pkg.uprange()); //todo: when to do it?
}

void ServiceHub::doingMigTarget(const Package& pkg) {

	typedef set<uint64_t> SET;
	SET range;

	const uint64_t* it;
	for (it = pkg.migringpos().begin(); it != pkg.migringpos().end(); it++) {

		range.insert(*it);
	}

	Neighbor targetAsNeighbor;
	targetAsNeighbor.assign(pkg.asneighbor());

	ZHTNode::getAddress().assign(targetAsNeighbor.getAddress());
	ZHTNode::getWorkload().assign(targetAsNeighbor.getWorkload());

	NoVoHTManager::initMyself(&range, pkg.targetzht());
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
