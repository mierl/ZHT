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
 * NoVoHTManager.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: tony, xiaobingo
 */

#include "NoVoHTManager.h"

#include "NeighborManager.h"

#include "Const-impl.h"

#include "ConfigHandler.h"

#include "Util.h"

#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <sstream>

using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

NoVoHTManager::MAP NoVoHTManager::HTMAP = MAP();

NoVoHTManager::PSET NoVoHTManager::RANGE_BEING_MIGRATED = new SET();

NoVoHTManager::NoVoHTManager() {
}

NoVoHTManager::~NoVoHTManager() {
}

void NoVoHTManager::backupHTFilesBeingMigrated(const uint& migrateInst,
		const bool& uprange) {

	MIT it;
	MRIT rit;
	rit = HTMAP.rbegin();

	if (!uprange) { //migrate upper range of source zht

		int i = migrateInst;
		for (it = HTMAP.begin(); it != HTMAP.end(); it++) {

			if (i > 0) {

				backupHTFilesBeingMigratedInternal(it->first);
				i--;
			}
		}
	} else {

		int i = migrateInst;
		for (rit = HTMAP.rbegin(); rit != HTMAP.rend(); rit++) {

			if (i > 0) {

				backupHTFilesBeingMigratedInternal(rit->first);
				i--;
			}
		}
	}
}

void NoVoHTManager::backupHTFilesBeingMigratedInternal(
		const uint64_t& fileNameId) {

	ifstream source(toFileName(fileNameId).c_str(), ios::binary);

	ofstream dest(toBackupFileName(fileNameId).c_str(), ios::binary);

	dest << source.rdbuf();

	source.close();
	dest.close();
}

/*
 * add the fileNameId to the set, based on begin-zht-token, end-zht-token and the number of NoVoHTInstance
 * in current zht.
 * */
void NoVoHTManager::initMyself(const uint64_t& begin, const uint64_t& end,
		const uint& coHostNoVoHTInstances) {

	HTMAP.clear();

	/*
	 * begein, end, [coHostNoVoHTInstances], fileId-1,fileId-2, ...
	 *
	 * 10, 20, [3], 10, 13, 16
	 * 10, 30, [3], 10, 16, 22
	 * 10, 30, [5], 10, 14, 18, 22, 26,
	 */

	uint64_t gap = (end - begin) / (uint64_t) coHostNoVoHTInstances;

	for (uint i = 0; i < coHostNoVoHTInstances; i++) {

		uint64_t uuid = begin + i * gap;

		Neighbor neighbor = NeighborManager::getZHTAsNeighbor(uuid);

		if (neighbor.host().empty())
			continue;

		string dir = Const::concat(ConfigHandler::ZC_HTDATA_PATH,
				neighbor.toHostPort());

		createFullPath(dir);

		HTMAP.insert(
				PAIR(uuid,
						new NOVOHT(Const::concat(dir, Const::DIR_DELIM, uuid),
								100000, 10000, 0.7)));

	}

}

void NoVoHTManager::initMyself(PSET range, const string& hostPort) {

	for (SIT it = range->begin(); it != range->end(); it++) {

		uint64_t uuid = *it;
		string dir = Const::concat(ConfigHandler::ZC_HTDATA_PATH, hostPort);

		createFullPath(dir);

		HTMAP.insert(
				PAIR(uuid,
						new NOVOHT(Const::concat(dir, Const::DIR_DELIM, uuid),
								100000, 10000, 0.7)));

	}
}

void NoVoHTManager::createFullPath(const string& uuid) {

	//	system("mkdir -p ./tmp/a/b/c");

	string cmd = "mkdir -p ";
	cmd.append(uuid);
	system(cmd.c_str());
}

/*
 * reset range of novoht so as to make the range still continuous, after migration
 * */
void NoVoHTManager::resetNoVoRange(const uint& migrateInst,
		const bool& uprange) {

	MIT it;
	MRIT rit;

	if (!uprange) {

		int i = migrateInst;
		for (it = HTMAP.begin(); it != HTMAP.end(); it++) {

			if (i > 0) {

				HTMAP.erase(it->first);
				i--;
			}
		}
	} else {

		int i = migrateInst;
		for (rit = HTMAP.rbegin(); rit != HTMAP.rend(); rit++) {

			if (i > 0) {

				HTMAP.erase(rit->first);
				i--;
			}
		}
	}
}

NoVoHTManager::PSET NoVoHTManager::getRangeBeingMigrated() {

	return RANGE_BEING_MIGRATED;
}

void NoVoHTManager::clearNoVoRangeBeingMigrated() {

	RANGE_BEING_MIGRATED->clear();

}

NoVoHTManager::PSET NoVoHTManager::getRangeBeingMigrated(
		const uint& migrateInst, const bool& uprange) {

	clearNoVoRangeBeingMigrated();

	MIT it;
	MRIT rit;

	if (!uprange) {

		int i = migrateInst;
		for (it = HTMAP.begin(); it != HTMAP.end(); it++) {

			if (i > 0) {

				RANGE_BEING_MIGRATED->insert(it->first);
				i--;
			}
		}
	} else {

		int i = migrateInst;
		for (rit = HTMAP.rbegin(); rit != HTMAP.rend(); rit++) {

			if (i > 0) {

				RANGE_BEING_MIGRATED->insert(rit->first);
				i--;
			}
		}
	}

	return RANGE_BEING_MIGRATED;
}

uint64_t NoVoHTManager::getNewUuidForZHTAfterMig(const uint& migrateInst,
		const bool& uprange) {

	uint64_t uuid = 0;

	MIT it;
	MRIT rit;

	if (!uprange) {

		int i = migrateInst;
		for (it = HTMAP.begin(); it != HTMAP.end(); it++) {

			if (i > 0) {
				i--;
				continue;
			}

			uuid = it->first;

		}
	} else {

		int i = migrateInst;
		for (rit = HTMAP.rbegin(); rit != HTMAP.rend(); rit++) {

			if (i > 0) {
				i--;
				continue;
			}

			uuid = rit->first;
		}
	}

	return uuid;
}

string NoVoHTManager::toFileName(const uint64_t& fileNameId) {

	stringstream ss;
	ss << fileNameId;

	return ss.str();
}

/*fileNameId => fileNameId.bak, which is newly copied file.*/
string NoVoHTManager::toBackupFileName(const uint64_t& fileNameId) {

	stringstream ss;
	ss << fileNameId;
	ss << ".bak";

	return ss.str();
}

NoVoHTManager::PNOVOHT NoVoHTManager::getNoVoHT(const string& key) {

	return getNoVoHT(HashUtil::genHash(key.c_str()));
}

NoVoHTManager::PNOVOHT NoVoHTManager::getNoVoHT(const uint64_t& hashKey) {

	PNOVOHT result;

	MRIT rit = HTMAP.rbegin();

	if (rit->first <= hashKey) {

		return rit->second;

	} else {

		for (; rit != HTMAP.rend(); rit++) {

			if (hashKey < rit->first)
				continue;

			result = rit->second;
			break;
		}
	}

	return result;
}

int NoVoHTManager::flushDbfile(const uint64_t& uuid) {

	PNOVOHT pnovoht = getNoVoHT(uuid);

	while (pnovoht->isRewriting())
		usleep(1000);

	return pnovoht->flushDbfile();
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
