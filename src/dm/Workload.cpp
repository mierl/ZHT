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
 * Workload.cpp
 *
 *  Created on: Jul 26, 2012
 *      Author: tony, xiaobingo
 */

#include "Workload.h"
#include "Const-impl.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

Workload::Workload(const uint& noVoHTInstances, const uint& capcacity) :
		_coHostNoVoHTInstances(noVoHTInstances), _coHostZHTCapacity(capcacity) {
}

Workload::~Workload() {

}

uint Workload::coHostCores() const {

	return _coHostCores;
}

void Workload::coHostCores(const uint& cores) {

	_coHostCores = cores;
}

uint Workload::coHostMemory() const {

	return _coHostMemory;
}

void Workload::coHostMemory(const uint& memory) {

	_coHostMemory = memory;
}

uint64_t Workload::coHostRawDiskSize() const {

	return _coHostRawDiskSize;
}

void Workload::coHostRawDiskSize(const uint64_t& raw) {

	_coHostRawDiskSize = raw;
}

uint64_t Workload::coHostUsedDiskSize() const {

	return _coHostUsedDiskSize;
}

void Workload::coHostUsedDiskSize(const uint64_t& used) {

	_coHostUsedDiskSize = used;
}

uint Workload::coHostNoVoHTInstances() const {

	return _coHostNoVoHTInstances;
}

void Workload::coHostNoVoHTInstances(const uint& instances) {

	_coHostNoVoHTInstances = instances;
}

uint Workload::coHostZHTCapacity() const {

	return _coHostZHTCapacity;
}

void Workload::coHostZHTCapacity(const uint& capcacity) {

	_coHostZHTCapacity = capcacity;
}

string Workload::getFormat() {

	return "%u,%u";
}

const string Workload::toString() const {

	char buf[20];
	memset(buf, 0, sizeof(buf));
	int n = sprintf(buf, getFormat().c_str(), _coHostNoVoHTInstances,
			_coHostZHTCapacity); //10, 20

	string result(buf, 0, n);

	return result;
}

void Workload::assign(const Workload& workload) {

	assign(workload.toString());
}

void Workload::assign(string sworkload) {

	const char* delimiter = ",";

	string tokens = Const::trim(sworkload);

	size_t found = tokens.find(delimiter);

	if (found != string::npos) {
		_coHostNoVoHTInstances = atoi(
				Const::trim(tokens.substr(0, int(found))).c_str());
		_coHostZHTCapacity = atoi(
				Const::trim(tokens.substr(int(found) + 1)).c_str());
	}
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
