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
 * ZHTNode.cpp
 *
 *  Created on: Aug 1, 2012
 *      Author: tony, xiaobingo
 */

#include "ZHTNode.h"
#include "NeighborManager.h"
#include "NoVoHTManager.h"
#include "Workload.h"

#include "Util.h"
#include "Const-impl.h"
#include "ConfigHandler.h"

#include <stdio.h>
#include <stdlib.h>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

Address* ZHTNode::_Address = new Address();
Workload* ZHTNode::_Workload = new Workload();

ZHTNode::ZHTNode(const uint64_t& uuid, const string& host, const uint& port) :
		_NeighborManager() {

	getAddress().assign(Address::genAddressString(uuid, host, port));

}

ZHTNode::~ZHTNode() {

}

void ZHTNode::process(const int& fd, const char *buf, const sockaddr& sender,
		const int& protocol) {

	ServiceHub sh(buf, getAddress().toHostPort());
	string rcode = sh.run();

	sendback(fd, rcode.c_str(), rcode.length(), sender, protocol);
}

Address& ZHTNode::getAddress() {

	return *_Address;
}

Workload& ZHTNode::getWorkload() {

	return *_Workload;
}

void ZHTNode::initMyself() {

	initNeighborManager();

	initMeAsNeighbor();

	initNoVoHTManager();
}

void ZHTNode::initMeAsNeighbor() {

	Neighbor neighbor = getMyselfAsNeighor();

	getAddress().assign(neighbor.getAddress());
	getWorkload().assign(neighbor.getWorkload());

}

void ZHTNode::initNeighborManager() {

	_NeighborManager.printMembership();

	NeighborManager::initNeighborManager(_NeighborManager);

	_NeighborManager.printMembership();
}

void ZHTNode::initNoVoHTManager() {

	int novoInst = getWorkload().coHostNoVoHTInstances();

	Neighbor me = getMyselfAsNeighor();

	if (me.host().empty() || me.port() == 0)
		return;

	getAddress().uuid(me.uuid());

	uint64_t begin = me.uuid();
	uint64_t end = _NeighborManager.getNextRingToken(begin);

	RingUtil::unwrap(begin, end);

	NoVoHTManager::initMyself(begin, end, novoInst);
}

Neighbor ZHTNode::getMyselfAsNeighor() {

	return _NeighborManager.getNeighborByHostPort(getAddress().toHostPort());
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */

/*
 * Hereafter are the code for start of AdminServer...
 *
 * Usage:
 * 	see also printUsage...
 */
#include <getopt.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
using namespace std;

#include "EpollServer.h"
using namespace iit::datasys::zht::dm;

void printUsage(char *argv_0);

int main(int argc, char **argv) {

	extern char *optarg;

	int printHelp = 0;
	int jaaSeed = 0;
	string host = Const::LOCAL_HOST;
	string port = "50000";
	string nodeConfig = Const::StringEmpty;
	string neighbor = Const::StringEmpty;
	string zhtConfig = Const::StringEmpty;

	int c;
	while ((c = getopt(argc, argv, "t:p:d:n:z:sh")) != -1) {
		switch (c) {
		case 't':
			host = string(optarg);
			break;
		case 'p':
			port = string(optarg);
			break;
		case 'd':
			nodeConfig = string(optarg);
			break;
		case 'n':
			neighbor = string(optarg);
			break;
		case 'z':
			zhtConfig = string(optarg);
			break;
		case 's':
			jaaSeed = 1;
			break;
		case 'h':
			printHelp = 1;
			break;
		default:
			fprintf(stderr, "Illegal argument \"%c\"\n", c);
			printUsage(argv[0]);
			exit(1);
		}
	}

	int helpPrinted = 0;
	if (printHelp) {
		printUsage(argv[0]);
		helpPrinted = 1;
	}

	try {
		if (!nodeConfig.empty() && !neighbor.empty() && !zhtConfig.empty()
				&& !port.empty()) {

			ConfigHandler::setNodeParameters(nodeConfig);
			ConfigHandler::setNeighborSeeds(neighbor);
			ConfigHandler::setZHTParameters(zhtConfig);

			ConfigHandler::CFG_NODE = nodeConfig; //node.cfg
			ConfigHandler::CFG_NEIGHBOR = neighbor; //neighbor
			ConfigHandler::CFG_ZHT = zhtConfig; //zht.cfg

			uint64_t uuid = NeighborManager::genNeighborUuid(host, port);
			ZHTNode zhtNode(uuid, host, atoi(port.c_str()));

			if (jaaSeed) { //START ZHTNode from neighbor(AKA seeds)

				if (NeighborManager::isNeighborSeed(host, port)) {

					zhtNode.initMyself();
				} else {

					fprintf(stderr,
							"{%s:%s} is not one of the neighbor seeds.\n",
							host.c_str(), port.c_str());
					exit(1);
				}
			}

			char buf[100];
			memset(buf, 0, sizeof(buf));
			int n = sprintf(buf, "ZHT server[%lu]- <%s:%s> started...\n",
					zhtNode.getAddress().uuid(), host.c_str(), port.c_str());

			cout << buf << endl;

			/*fprintf(stdout, "ZHT server[%lu]- <%s:%s> started...\n",
			 zhtNode.getAddress().uuid(), host.c_str(), port.c_str());*/

			EpollServer es(&zhtNode);
			es.serve(port.c_str());

		} else {
			if (!helpPrinted)
				printUsage(argv[0]);
		}
	} catch (exception& e) {

		fprintf(stderr, "%s, exception caught:\n\t%s", "ZHTNode::main",
				e.what());
	}

}

void printUsage(char *argv_0) {

	fprintf(stdout, "Usage:\n%s %s\n", argv_0,
			"[-t localhost] -p 50000 -d node.cfg -n neighbor -z zht.cfg [-s(join as a seed)] [-h(help)]");
}

