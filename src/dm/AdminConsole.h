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
 * AdminConsole.h
 *
 *  Created on: Jul 18, 2012
 *      Author: tony, xiaobingo
 */

#ifndef ADMINCONSOLE_H_
#define ADMINCONSOLE_H_

#include "../zht_util.h"

#include <sys/types.h>
#include <string>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

class AdminConsole {
public:
	AdminConsole(const string& host, const uint& port);
	virtual ~AdminConsole();

	int addPhysicalNode() const;
	int addZHT(const uint& port) const;
	int addZHT(const string& localIp, const uint& port) const;

	int removePhysicalNode() const;
	int removeZHT(const uint& port) const;
	int removeZHT(const string& localIp, const uint& port) const;

private:
	int getSocket() const;
	int runOnOpcode(const string& opcode) const;

private:
	static const bool TCP;
	static const uint MSG_SIZE;

private:
	const string _host;
	const uint _port;
	HostEntity DEST;
};

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* ADMINCONSOLE_H_ */
