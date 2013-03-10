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
 * HTWorker.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: tony, xiaobingo
 */

#include "HTWorker.h"

#include "ConfigHandler.h"
#include "Address.h"
#include "FileClient.h"
#include "NeighborManager.h"

#include "ZHTNode.h"

#include "FileClient.h"

#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include "Const-impl.h"
#include "NoVoHTManager.h"
#include "Status.h"

#include "Status.h"
#include "Util.h"

#include <stdint.h>
#include <string.h>
#include <iostream>

using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

string HTWorker::TARGET_ZHT;

HTWorker::HTWorker() {
}

HTWorker::HTWorker(const string& sourceZHT) :
		_sourceZHT(sourceZHT) {
}

HTWorker::~HTWorker() {
}

string HTWorker::getTargetZHT() {

	return TARGET_ZHT;
}

void HTWorker::clearTargetZHT() {

	TARGET_ZHT = Const::StringEmpty;
}

void HTWorker::setTargetZHT(const string& targetZHT) {

	TARGET_ZHT = targetZHT;
}

string HTWorker::getGlobalMembership(const Package &pkg) {

	string rcode = Const::ZSC_REC_UNPR;

	string globalMembership = NeighborManager::getGlobalMembershipAsString();

	if (globalMembership.empty())
		rcode = Const::ZSC_REC_SRVFAIL; //-3
	else
		rcode = Const::concat(Const::ZSC_REC_SUCC, globalMembership);

	return rcode;
}

string HTWorker::updateGlobalMembership(const Package &pkg) {

	string rcode = Const::ZSC_REC_SUCC;

	string oldMembership = NeighborManager::getGlobalMembershipAsString();
	try {

		NeighborManager::updateGlobalMembership(pkg.gmembership());

	} catch (exception& e) {

		NeighborManager::updateGlobalMembership(oldMembership);

		rcode = Const::ZSC_REC_SRVFAIL; //-3

		fprintf(stderr, "%s, exception caught:\n\t%s",
				"HTWorker::updateGlobalMembership(const Package &pkg)",
				e.what());
	}

	return rcode;
}

string HTWorker::getZHTAsNeighbor(const Package &pkg) {

	Neighbor neighbor;
	neighbor.getAddress().assign(ZHTNode::getAddress());
	neighbor.getWorkload().assign(ZHTNode::getWorkload());

	Package pkg2;
	pkg2.set_asneighbor(neighbor.toString());

	Const::prtNb(neighbor);

	return Const::concat(Const::ZSC_REC_SUCC, pkg2.SerializeAsString());
}

string HTWorker::run(const char *buf, const ssize_t& count) {

	return runInternal(buf, count);
}

string HTWorker::runInternal(const char *buf, const ssize_t& count) {

	string rcode = Const::ZSC_REC_UNPR;
	Package pkg;
	pkg.ParseFromArray(buf, count);

	if (pkg.opcode() == Const::ZSC_OPC_LOOKUP) {

		rcode = lookup(pkg);

	} else if (pkg.opcode() == Const::ZSC_OPC_REMOVE) {

		rcode = remove(pkg);

	} else if (pkg.opcode() == Const::ZSC_OPC_INSERT) {

		rcode = insert(pkg);

	} else if (pkg.opcode() == Const::ZSC_OPC_GET_DESTZHT) { //get destination zht to service client request

		rcode = getDestZHT(pkg);

	} else if (pkg.opcode() == Const::ZSC_OPC_GET_GMEM) { //get global membership from zht

		rcode = getGlobalMembership(pkg);

	} else if (pkg.opcode() == Const::ZSC_OPC_BRD_GMEM) { //update global membership to zht

		BroadcastStatus::value(BSEnum::BS_DOING);
		rcode = updateGlobalMembership(pkg);

	} else if (pkg.opcode() == Const::ZSC_OPC_BRDDN_GMEM) { //update global membership to zht, done

		BroadcastStatus::value(BSEnum::BS_DONE);

		rcode = Const::ZSC_REC_UOPC;

	} else if (pkg.opcode() == Const::ZSC_OPC_GET_ASNGHB) { //get information of ZHTNode as neighbor

		rcode = getZHTAsNeighbor(pkg);

	} else {

		rcode = Const::ZSC_REC_UOPC;
	}

	return rcode;
}

string HTWorker::lookup(const Package &pkg) {

	string rcode = Const::ZSC_REC_UNPR; //-99

	if (MigrateStatus::value() == MSEnum::MS_DOING) {

		rcode = getSecondTryPkg(pkg);

		return rcode;
	}

	if (BroadcastStatus::value() == BSEnum::BS_DOING) {

		rcode = Const::ZSC_REC_SECDTRY;

		return rcode;
	}

	if (pkg.virtualpath().empty()) {

		rcode = Const::ZSC_REC_EMPTYKEY; //-1

	} else {

		rcode = hb_lookup(pkg);
	}

	return rcode;
}

string HTWorker::hb_lookup(const Package &pkg) {

	string rcode = Const::ZSC_REC_UNPR;

	string key = pkg.virtualpath();

	NoVoHTManager::PNOVOHT pn = NoVoHTManager::getNoVoHT(key);

	string *result = pn->get(key);

	if (result == NULL) {

		cout << "lookup find nothing." << endl;
		rcode = Const::ZSC_REC_NONEXISTKEY;

	} else {

		string retStr((*result));
		rcode = Const::concat(Const::ZSC_REC_SUCC, retStr);
	}

	return rcode;
}

string HTWorker::remove(const Package &pkg) {

	string rcode = Const::ZSC_REC_UNPR; //-99

	if (MigrateStatus::value() == MSEnum::MS_DOING) {

		rcode = getSecondTryPkg(pkg);

		return rcode;
	}

	if (BroadcastStatus::value() == BSEnum::BS_DOING) {

		rcode = Const::ZSC_REC_SECDTRY;

		return rcode;
	}

	if (pkg.virtualpath().empty()) {

		rcode = Const::ZSC_REC_EMPTYKEY; //-1

	} else {

		rcode = hb_remove(pkg);
	}

	return rcode;
}

string HTWorker::hb_remove(const Package &pkg) {

	string rcode = Const::ZSC_REC_UNPR;

	string key = pkg.virtualpath();

	NoVoHTManager::PNOVOHT pn = NoVoHTManager::getNoVoHT(key);

	int ret = pn->remove(key); // return 0 means correct.

	if (ret != 0) {

		cerr << "DB Error: fail to remove :ret= " << ret << endl;
		rcode = Const::ZSC_REC_NONEXISTKEY; //-92

	} else {

		rcode = Const::ZSC_REC_SUCC; //0, succeed.
	}

	return rcode;
}

string HTWorker::insert(const Package &pkg) {

	string rcode = Const::ZSC_REC_UNPR; //-99

	if (MigrateStatus::value() == MSEnum::MS_DOING) {

		rcode = getSecondTryPkg(pkg);

		return rcode;
	}

	if (BroadcastStatus::value() == BSEnum::BS_DOING) {

		rcode = Const::ZSC_REC_SECDTRY;

		return rcode;
	}

	if (pkg.virtualpath().empty()) {

		rcode = Const::ZSC_REC_EMPTYKEY; //-1

	} else {

		rcode = hb_insert(pkg);
	}

	return rcode;
}

string HTWorker::hb_insert(const Package &pkg) {

	string rcode = Const::ZSC_REC_UNPR;

	string key = pkg.virtualpath();
	string value = pkg.SerializeAsString();

	NoVoHTManager::PNOVOHT pn = NoVoHTManager::getNoVoHT(key);

	int ret = pn->put(key, value);

	if (ret != 0) {

		cerr << "insert error: ret = " << ret << endl;
		rcode = Const::ZSC_REC_NONEXISTKEY; //-92

	} else {

		rcode = Const::ZSC_REC_SUCC; //0, succeed.
	}

	return rcode;
}

string HTWorker::getDestZHT(const Package &pkg) {

	string rcode;

	string destZHT = getDestZHTInternal(pkg);

	Package pkg2;

	if (destZHT.empty()) {

		rcode = Const::concat(Const::ZSC_REC_NODESTZHT,
				pkg2.SerializeAsString());

	} else {

		pkg2.set_targetzht(destZHT);
		rcode = Const::concat(Const::ZSC_REC_SUCC, pkg2.SerializeAsString());
	}

	return rcode;
}

string HTWorker::getDestZHTInternal(const Package &pkg) {

	return NeighborManager::getDestZHTByHashKey(
			HashUtil::genHash(pkg.virtualpath()));
}

string HTWorker::getSecondTryPkg(const Package &pkg) {

	string result;

	if (pkg.isfwdmsg()) { //message is forwared from source to target zht

		result = Const::ZSC_REC_SECDTRY;

	} else {

		if (shouldSecondTryToTargetZHT(pkg)) { //some pairs should be forwarded to target zht

			Package pkg2;
			pkg2.set_targetzht(TARGET_ZHT);

			result = Const::concat(Const::ZSC_REC_SECDTRY,
					pkg2.SerializeAsString());

		} else { //the other pairs should come to source zht

			result = Const::ZSC_REC_SECDTRY;
		}
	}

	return result;
}

bool HTWorker::shouldSecondTryToTargetZHT(const Package &pkg) {

	typedef set<uint64_t>* PSET;
	typedef set<uint64_t>::iterator IT;
	typedef set<uint64_t>::reverse_iterator RIT;

	PSET range = NoVoHTManager::getRangeBeingMigrated();

	IT begin = range->begin(); //lowest value
	RIT end = range->rbegin(); //highest value
	uint64_t hashKey = HashUtil::genHash(pkg.virtualpath());

	if (range->size() == 0) {

		return false;

	} else if (range->size() == 1) {

		if (hashKey < (*begin))
			return false;
		else
			return true;

	} else {

		if ((*begin) <= hashKey && hashKey < (*end))
			return true;
		else
			return false;
	}
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
