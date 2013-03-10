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
 * Neighbor.h
 *
 *  Created on: Jul 25, 2012
 *      Author: tony, xiaobingo
 */

#ifndef NEIGHBOR_H_
#define NEIGHBOR_H_

#include <string>
using namespace std;

#include "Address.h"
#include "Workload.h"

namespace dm = iit::datasys::zht::dm;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

class Neighbor {
public:
	Neighbor();
	Neighbor(const string& sneighbor);
	virtual ~Neighbor();

	const string toString() const;
	const string toHostPort() const;
	string operator()() const;
	Neighbor operator()(const string& sneighbor);
	void assign(const string& sneighbor);

	Address& getAddress();
	void setAddress(const Address& address);
	void setAddress(const string& saddress);
	string getAddressString() const;

	Workload& getWorkload();
	void setWorkload(const Workload& sworkload);
	void setWorkload(const string& sworkload);
	string getWorkloadString() const;

	uint64_t uuid() const;
	string uuidstr() const;
	void uuidstr(const string& suuid);

	uint64_t ouuid() const;
	string ouuidstr() const;
	void ouuidstr(const string& souuid);

	string host() const;
	uint port() const;

public:
	static string getFormat();

private:
	Address _Address;
	Workload _Workload;
	uint64_t _ouuid; //old uuid, two version of uuid, in case track the change of Address of a neighbor

};

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* NEIGHBOR_H_ */
