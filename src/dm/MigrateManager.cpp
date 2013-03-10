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
 * MigrateManager.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: tony, xiaobingo
 */

#include "MigrateManager.h"

#include "NoVoHTManager.h"
#include "FilePorter.h"

#include "Const-impl.h"

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

MigrateManager::MigrateManager() {
}

MigrateManager::~MigrateManager() {
}

string MigrateManager::migrateNovoHT(const string& sourceZHT,
		const Package& pkg, Package& rpkg) {

	string sstatus;

	typedef set<uint64_t>* PSET;
	typedef set<uint64_t>::iterator ITR;

	PSET range = NoVoHTManager::getRangeBeingMigrated(pkg.migrateinst(),
			pkg.uprange());

	ITR it;
	for (it = range->begin(); it != range->end(); it++) {

		rpkg.add_migringpos(*it);

		NoVoHTManager::flushDbfile(*it);

		sstatus = FilePorter::transferNovoHT(Const::toString(*it), sourceZHT,
				pkg.targetzht());

	}

	return sstatus;
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
