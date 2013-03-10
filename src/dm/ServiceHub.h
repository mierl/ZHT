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
 * ServiceHub.h
 *
 *  Created on: Sep 10, 2012
 *      Author: tony, xiaobingo
 */

#ifndef SERVICEHUB_H_
#define SERVICEHUB_H_

#include "../meta.pb.h"

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>

using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

/*
 *
 */
class ServiceHub {
public:
	ServiceHub();
	ServiceHub(const char *buf, const string& sourceZHT);
	ServiceHub(const int& fd, const char *buf, const sockaddr& sender,
			const int& protocol, const string& sourceZHT);
	virtual ~ServiceHub();

	string run();

	void preapreMigrage(const Package& pkg);

	string doingMigSource(const Package& pkg, Package& rpkg);
	void doingMigTarget(const Package& pkg);

	/*change state of this ZHTNode, such as Address, Workload*/
	void changeStateOfThisZHT(const Package& pkg);

	/*update this zht to neighbor manager*/
	void updateThisZHTAsNeighbor();

private:
	string getMigSourceReturnPkg(const string& rcode, Package& rpkg);

	void beforeMigSource(const Package& pkg);
	void afterMigSource(const Package& pkg);

private:
	int _fd;
	const char* _buf;
	sockaddr _sender;
	int _protocol;
	string _sourceZHT;
};

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* SERVICEHUB_H_ */
