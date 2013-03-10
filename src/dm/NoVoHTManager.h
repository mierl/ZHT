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
 * NoVoHTManager.h
 *
 *  Created on: Sep 10, 2012
 *      Author: tony, xiaobingo
 */

#ifndef NOVOHTMANAGER_H_
#define NOVOHTMANAGER_H_

#include "../novoht.h"

#include <set>
#include <map>
#include <stdint.h>
#include <sys/types.h>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

/*
 *
 */
class NoVoHTManager {

public:
	/*
	 typedef map<uint64_t, BingoMap<string, string> > MAP;
	 typedef pair<uint64_t, BingoMap<string, string> > PAIR;
	 typedef BingoMap<string, string> NOVOHT;
	 */

	typedef NoVoHT NOVOHT;
	typedef NOVOHT* PNOVOHT;

	typedef map<uint64_t, NOVOHT*> MAP;
	typedef pair<uint64_t, NOVOHT*> PAIR;
	typedef map<uint64_t, NOVOHT*>::iterator MIT;
	typedef map<uint64_t, NOVOHT*>::reverse_iterator MRIT;

	typedef set<uint64_t> SET;
	typedef SET* PSET;
	typedef set<uint64_t>::iterator SIT;

public:
	NoVoHTManager();
	virtual ~NoVoHTManager();

public:
	static void backupHTFilesBeingMigrated(const uint& migrateInst,
			const bool& uprange);

	static void initMyself(const uint64_t& begin, const uint64_t& end,
			const uint& coHostNoVoHTInstances);

	static void initMyself(PSET range, const string& hostPort);

	static void resetNoVoRange(const uint& migrateInst, const bool& uprange);

	static PNOVOHT getNoVoHT(const string& key);
	static PNOVOHT getNoVoHT(const uint64_t& hashKey);
	static void createFullPath(const string& uuid);

	static PSET getRangeBeingMigrated();

	static void clearNoVoRangeBeingMigrated();
	static PSET getRangeBeingMigrated(const uint& migrateInst,
			const bool& uprange);
	static uint64_t getNewUuidForZHTAfterMig(const uint& migrateInst,
			const bool& uprange);

	static string toFileName(const uint64_t& fileNameId);
	static string toBackupFileName(const uint64_t& fileNameId);
	static int flushDbfile(const uint64_t& uuid);

private:
	static void backupHTFilesBeingMigratedInternal(const uint64_t& fileNameId);

private:
	static MAP HTMAP; //novoht file map
	static PSET RANGE_BEING_MIGRATED;
};

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* NOVOHTMANAGER_H_ */
