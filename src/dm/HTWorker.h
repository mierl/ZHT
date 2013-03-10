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
 * HTWorker.h
 *
 *  Created on: Sep 10, 2012
 *      Author: tony, xiaobingo
 */

#ifndef HTWORKER_H_
#define HTWORKER_H_

#include "../meta.pb.h"

#include <string>
#include <sys/types.h>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

/*
 *
 */
class HTWorker {
public:
	HTWorker();
	explicit HTWorker(const string& sourceZHT);
	virtual ~HTWorker();

public:
	string run(const char *buf, const ssize_t& count);

public:
	static string getTargetZHT();
	static void clearTargetZHT();
	static void setTargetZHT(const string& targetZHT);

private:
	string runInternal(const char *buf, const ssize_t& count);

	string lookup(const Package &pkg);
	string remove(const Package &pkg);
	string insert(const Package &pkg);

	string hb_lookup(const Package &pkg);
	string hb_remove(const Package &pkg);
	string hb_insert(const Package &pkg);

//	string getFileFromZHT(const Package &pkg);
	string getDestZHT(const Package &pkg);
	string getDestZHTInternal(const Package &pkg);
	string getGlobalMembership(const Package &pkg);
	string updateGlobalMembership(const Package &pkg);
	string getZHTAsNeighbor(const Package &pkg);

	string getSecondTryPkg(const Package &pkg);
	bool shouldSecondTryToTargetZHT(const Package &pkg);

private:
	string _sourceZHT;
	static string TARGET_ZHT;
};
} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* HTWORKER_H_ */
