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
 * Workload.h
 *
 *  Created on: Jul 26, 2012
 *      Author: tony, xiaobingo
 */

#ifndef WORKLOAD_H_
#define WORKLOAD_H_

#include <sys/types.h>
#include <stdint.h>
#include <string>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

class Workload {
public:
	Workload(const uint& noVoHTInstances = 0, const uint& capcacity = 0);
	virtual ~Workload();

	uint coHostCores() const;
	void coHostCores(const uint& cores);

	uint coHostMemory() const;
	void coHostMemory(const uint & memory);

	uint64_t coHostRawDiskSize() const;
	void coHostRawDiskSize(const uint64_t& raw);

	uint64_t coHostUsedDiskSize() const;
	void coHostUsedDiskSize(const uint64_t& used);

	uint coHostNoVoHTInstances() const;
	void coHostNoVoHTInstances(const uint & instances);

	uint coHostZHTCapacity() const;
	void coHostZHTCapacity(const uint & capcacity);

	const string toString() const;
	void assign(string sworkload);
	void assign(const Workload& workload);

public:
	static string getFormat();

private:
	uint _coHostCores;
	uint _coHostMemory;
	uint64_t _coHostRawDiskSize;
	uint64_t _coHostUsedDiskSize;
	uint _coHostNoVoHTInstances;
	uint _coHostZHTCapacity;
};

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* WORKLOAD_H_ */
