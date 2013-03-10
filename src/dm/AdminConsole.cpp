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
 * AdminConsole.cpp
 *
 *  Created on: Jul 18, 2012
 *      Author: tony, xiaobingo
 */

#include "AdminConsole.h"

#include <stdio.h>

#include "../net_util.h"
#include "../zht_util.h"
#include "Const-impl.h"

using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

const bool AdminConsole::TCP = true;

const uint AdminConsole::MSG_SIZE = 10;

AdminConsole::AdminConsole(const string& host, const uint& port) :
		_host(host), _port(port) {

	DEST = getHostEntity(host, _port);
}

AdminConsole::~AdminConsole() {

}

int AdminConsole::addPhysicalNode() const {

	int rc = runOnOpcode(Const::ASC_OPC_AZ_ALL);

	fprintf(stdout, "addPhysicalNode, rc(%d)\n", rc);

	return rc;
}

int AdminConsole::removePhysicalNode() const {

	int rc = runOnOpcode(Const::ASC_OPC_RZ_ALL);

	fprintf(stdout, "removePhysicalNode, rc(%d)\n", rc);

	return rc;
}

int AdminConsole::addZHT(const uint& port) const {

	//todo:
	return 0;
}

int AdminConsole::addZHT(const string& localIp, const uint& port) const {

	//todo:
	return 0;
}

int AdminConsole::removeZHT(const uint& port) const {

	//todo:
	return 0;
}

int AdminConsole::removeZHT(const string& localIp, const uint& port) const {

	//todo:
	return 0;
}

int AdminConsole::runOnOpcode(const string& opcode) const {

	string sstatus;
	int sock = getSocket();

	int ssize = generalSendTo(DEST.host.data(), DEST.port, sock, opcode.c_str(),
			opcode.size(), TCP);

	if (ssize == (int) opcode.length()) {

		char buff[MSG_SIZE];
		memset(buff, 0, sizeof(buff));
		sockaddr_in recvAddr;

		int rsize = -1;
		rsize = generalReceive(sock, (void*) buff, sizeof(buff), recvAddr, 0,
				TCP);

		if (rsize < 0) {
			fprintf(stderr, "receive error, got size[%d] bytes from sock[%d]",
					rsize, sock);
		} else {
			string srecv;
			srecv.assign(buff);
			sstatus = srecv.substr(0, 3); //the first three chars means status code, like 001, 002, -98, -99 and so on.
		}

	}

	int status = -1;
	if (!sstatus.empty())
		status = atoi(sstatus.c_str());

	return status;
}

int AdminConsole::getSocket() const {

	int sock = makeClientSocket(DEST.host.c_str(), DEST.port, TCP);

	if (sock <= 0) {
		fprintf(stderr, "failed to make console socket: {%s:%u}", _host.c_str(),
				_port);
		sock = -1;
	}

	reuseSock(sock);

	return sock;
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */

/*
 * Hereafter are the code for start of AdminConsole...
 *
 * Usage:
 *	see also printUsage...
 */
#include <getopt.h>
#include <stdio.h>
using namespace std;

using namespace iit::datasys::zht::dm;

void printUsage(char *argv_0);

int main(int argc, char **argv) {

	extern char *optarg;

	string host = Const::LOCAL_HOST;
	string port = "40000";
	int printHelp = 0;
	int addNode = 0;
	int removeNode = 0;

	int c;
	while ((c = getopt(argc, argv, "t:p:arh")) != -1) {
		switch (c) {
		case 't':
			host = string(optarg);
			break;
		case 'p':
			port = string(optarg);
			break;
		case 'a':
			addNode = 1;
			break;
		case 'r':
			removeNode = 1;
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
		if ((addNode && !removeNode) || (!addNode && removeNode)) {

			AdminConsole ac(host, atoi(port.c_str()));

			if (addNode)
				ac.addPhysicalNode();
			else if (removeNode)
				ac.removePhysicalNode();
			else {
				if (!printHelp)
					printUsage(argv[0]);

			}
		} else {
			if (!printHelp)
				printUsage(argv[0]);
		}
	} catch (exception& e) {

		fprintf(stderr, "%s, exception caught:\n\t%s", "AdminConsole::main",
				e.what());
	}

}

void printUsage(char *argv_0) {

	fprintf(stdout, "Usage:\n%s %s\n%s\n", argv_0,
			"[-t locahost] [-p 40000] {-a(add physical node) | -r(remove physical node)} [-h(help)].",
			"BWT: parallel use of -a and -r, not allowed.");
}

