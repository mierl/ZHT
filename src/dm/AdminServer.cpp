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
 * AdminServer.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: tony, xiaobingo
 */

#include <stdlib.h>
#include "Const-impl.h"
#include "AdminServer.h"
#include <stdio.h>
#include <string.h>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

AdminServer::AdminServer() :
		_ZHTManager(ZHTManager::NEIGHBOR_MANAGER_NAME) {

}

AdminServer::~AdminServer() {

}

void AdminServer::process(const int& fd, const char *buf,
		const sockaddr& sender, const int& protocol) {

	string localIp; //todo: parse from buf
	int port; //todo: parse from buf

	string rcode = Const::ASC_REC_UNPR;

	string sbuf(buf);
	string sopcode = sbuf.substr(0, 3); //the first three chars means operation code, like 001, 002, 003 and so on.
	int opcode = atoi(sopcode.c_str());

	if (opcode == Const::ASI_OPC_AZ_ALL) {
		rcode = _ZHTManager.addAllZHTOnNode();
	} else if (opcode == Const::ASI_OPC_AZ_PORT) { //not yet supported
		_ZHTManager.addZHT(port);
		rcode = Const::ASC_REC_SUCC;
	} else if (opcode == Const::ASI_OPC_AZ_IPPORT) { //not yet supported
		_ZHTManager.addZHT(localIp, port);
		rcode = Const::ASC_REC_SUCC;
	} else if (opcode == Const::ASI_OPC_RZ_ALL) {
		rcode = _ZHTManager.removeAllZHTOnNode();
	} else if (opcode == Const::ASI_OPC_RZ_PORT) { //not yet supported
		_ZHTManager.removeZHT(port);
		rcode = Const::ASC_REC_SUCC;
	} else if (opcode == Const::ASI_OPC_RZ_IPPORT) { //not yet supported
		_ZHTManager.removeZHT(localIp, port);
		rcode = Const::ASC_REC_SUCC;
	} else {
		rcode = Const::ASC_REC_UOPC;
	}

	sendback(fd, rcode.c_str(), rcode.length(), sender, protocol);
}

void AdminServer::initMyself() {

	initNeighborManager();
}

void AdminServer::initNeighborManager() {

	_ZHTManager.initNeighborManager();
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
using namespace std;

#include "Const-impl.h"
#include "ConfigHandler.h"
#include "EpollServer.h"
using namespace iit::datasys::zht::dm;

void printUsage(char *argv_0);

int main(int argc, char **argv) {

	extern char *optarg;

	int printHelp = 0;
	string host = Const::LOCAL_HOST;
	string port = "40000";
	string nodeConfig = Const::StringEmpty;
	string neighbor = Const::StringEmpty;
	string zhtConfig = Const::StringEmpty;

	int c;
	while ((c = getopt(argc, argv, "t:p:d:n:z:h")) != -1) {
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
		case 'h':
			printHelp = 1;
			break;
		default:
			fprintf(stderr, "Illegal argument \"%c\"\n", c);
			printUsage(argv[0]);
			exit(1);
		}
	}

	if (printHelp)
		printUsage(argv[0]);

	try {
		if (!nodeConfig.empty() && !neighbor.empty() && !zhtConfig.empty()) {

			ConfigHandler::setNodeParameters(nodeConfig);
			ConfigHandler::setNeighborSeeds(neighbor);
			ConfigHandler::setZHTParameters(zhtConfig);

			AdminServer adminServer;
			adminServer.initMyself();

			char buf[100];
			memset(buf, 0, sizeof(buf));
			int n = sprintf(buf, "Admin server <%s:%s> started...\n",
					host.c_str(), port.c_str());

			cout << buf << endl;

			/*fprintf(stdout, "Admin server <%s:%s> started...\n", host.c_str(),
			 port.c_str());
			 */

			EpollServer es(&adminServer);
			es.serve(port.c_str());

		} else {
			if (!printHelp)
				printUsage(argv[0]);
		}
	} catch (exception& e) {

		fprintf(stderr, "%s, exception caught:\n\t%s", "AdminServer::main",
				e.what());
	}

}

void printUsage(char *argv_0) {

	fprintf(stdout, "Usage:\n%s %s\n", argv_0,
			"[-t localhost] [-p 40000] -d node.cfg -n neighbor -z zht.cfg [-h(help)]");
}
