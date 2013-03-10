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
 * Neighbor.cpp
 *
 *  Created on: Jul 25, 2012
 *      Author: tony, xiaobingo
 */

#include "Neighbor.h"

#include "Address.h"
#include "Workload.h"

#include "../zht_util.h"
#include "Util.h"
#include <stddef.h>
#include "Const-impl.h"

#include <sstream>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

Neighbor::Neighbor() :
		_Address(), _Workload(), _ouuid(0) {
}

Neighbor::Neighbor(const string& sneighbor) :
		_ouuid(0) {

	assign(sneighbor);
}

Neighbor::~Neighbor() {

}

const string Neighbor::toString() const {

	char buf[200];
	memset(buf, 0, sizeof(buf));
	int n = sprintf(buf, getFormat().c_str(), _Address.toString().c_str(),
			_Workload.toString().c_str()); //38769960, 192.168.1.10, 50000 : 10, 20

	string result(buf, 0, n);

	return result;
}

const string Neighbor::toHostPort() const {

	return _Address.toHostPort();
}

string Neighbor::operator()() const {

	return toString();
}

Neighbor Neighbor::operator()(const string& sneighbor) {

	Neighbor neighbor(sneighbor);

	return neighbor;
}

string Neighbor::getFormat() {

	return "%s:%s";
}

void Neighbor::assign(const string& sneighbor) {

	string delimiter = ":";

	string remains = Const::trim(sneighbor);

	size_t found = remains.find(delimiter);

	string saddress;
	string sworkdload;
	if (found != string::npos) {

		saddress = Const::trim(remains.substr(0, int(found)));
		sworkdload = Const::trim(remains.substr(int(found) + 1));
	}

	_Address.assign(saddress);
	_Workload.assign(sworkdload);
}
void Neighbor::setAddress(const Address& address) {

	_Address.assign(address.toString());
}

void Neighbor::setAddress(const string& saddress) {

	_Address.assign(saddress);
}

void Neighbor::setWorkload(const Workload& workload) {

	_Workload.assign(workload.toString());
}

void Neighbor::setWorkload(const string& sworkload) {

	_Workload.assign(sworkload);
}

Address& Neighbor::getAddress() {

	return _Address;
}

Workload& Neighbor::getWorkload() {

	return _Workload;
}

string Neighbor::getAddressString() const {

	return _Address.toString();
}

string Neighbor::getWorkloadString() const {

	return _Workload.toString();
}

uint64_t Neighbor::uuid() const {

	return _Address.uuid();
}

string Neighbor::uuidstr() const {

	return _Address.uuidstr();
}

void Neighbor::uuidstr(const string& suuid) {

	_Address.uuid(strtoull(suuid.c_str(), NULL, 10));
}

uint64_t Neighbor::ouuid() const {

	return _Address.ouuid();
}

string Neighbor::ouuidstr() const {

	return _Address.ouuidstr();
}

void Neighbor::ouuidstr(const string& souuid) {

	_Address.ouuidstr(souuid);
}

string Neighbor::host() const {

	return _Address.host();
}

uint Neighbor::port() const {

	return _Address.port();
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
