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
 * Status.h
 *
 *  Created on: Feb 14, 2013
 *      Author: tony, xiaobingo
 */

#ifndef STATUS_H_
#define STATUS_H_

#include  "MutexCondition.h"

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

class BSEnum {
public:
	static const int BS_DOING;
	static const int BS_DONE;
};

/*
 *
 */
class BroadcastStatus {
public:
	BroadcastStatus();
	virtual ~BroadcastStatus();

public:
	static int value();
	static void value(const int& status);

private:
	static int BS_STATUS;
};

class MSEnum {
public:
	static const int MS_DOING;
	static const int MS_DONE;
};
/*
 *
 */
class MigrateStatus {
public:
	MigrateStatus();
	virtual ~MigrateStatus();

public:
	static int value();
	static void value(const int& status);

private:
	static int MS_STATUS;
	static MutexCondition MC;
};

class PSEnum {
public:
	static const int PS_DOING;
	static const int PS_DONE;
};

/*
 *
 */
class PorterStatus {
public:
	PorterStatus();
	virtual ~PorterStatus();

public:
	static int value();
	static void value(const int& status);

private:
	static int PS_STATUS;
	static MutexCondition MC;
};
} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* STATUS_H_ */
