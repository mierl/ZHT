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
 * ZHTClient.cpp
 *
 *  Created on: Sep 16, 2012
 *      Author: tony, xiaobingo
 */

#include "ZHTClient.h"

#include "Const-impl.h"
#include "Util.h"
#include "../meta.pb.h"
#include "../net_util.h"
#include "Address.h"
#include "ConfigHandler.h"

#include <sys/types.h>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

int ZHTClient::UDP_SOCKET = -1;
int ZHTClient::CACHE_SIZE = 1024;
LRUCache<string, int> ZHTClient::CONN_CACHE = LRUCache<string, int>(
		ZHTClient::CACHE_SIZE);

ZHTClient::ZHTClient() {
}

/*todo: dynMemFile, at this point, it's neighbor file*/
ZHTClient::ZHTClient(const string& dynMemFile, const bool& tcp) {

	init(dynMemFile, tcp);
}

ZHTClient::~ZHTClient() {
}

/*todo: dynMemFile, at this point, is neighbor file*/
int ZHTClient::init(const string& dynMemFile, const bool& tcp) {

	int rcode = 0;

	TCP = tcp;

	try {

		_MemList = getMembership(dynMemFile);
	} catch (exception& e) {

		fprintf(stderr, "%s, exception caught:\n\t%s",
				"ZHTClient::init(const string& dynMemFile, const bool& tcp)",
				e.what());

		rcode = -1;
	}

	return rcode;
}

int ZHTClient::init2(const string& dynMemFile, const bool& tcp) {

	TCP = tcp;
	_dynMemFile = dynMemFile;

	return readDynamicMembership(dynMemFile);
}

int ZHTClient::commonOp(const string& opcode, const string& pair,
		string& result) {

	if (opcode != Const::ZSC_OPC_LOOKUP && opcode != Const::ZSC_OPC_REMOVE
			&& opcode != Const::ZSC_OPC_INSERT)
		return Const::toInt(Const::ZSC_REC_UOPC);

	string sstatus = commonOpInternal(opcode, NULL, pair, result);

	while (sstatus == Const::ZSC_REC_SECDTRY) {

		Package pkg;
		pkg.ParseFromString(result);

		usleep(ConfigHandler::ZC_MIGSLP_TIME);

		/*retry host:port*/
		if (pkg.targetzht().empty()) { //no target zht to second-try

			result = Const::StringEmpty;
			sstatus = commonOpInternal(opcode, NULL, pair, result);

		} else {

			HostEntity he = getHostEntity(Address::getHost(pkg.targetzht()),
					Address::getPort(pkg.targetzht()));
			result = Const::StringEmpty;

			Package fwdpkg;
			fwdpkg.ParseFromString(pair);
			fwdpkg.set_isfwdmsg(true);
			sstatus = commonOpInternal(opcode, &he, fwdpkg.SerializeAsString(),
					result);
		}

	}

	int status = Const::ZSI_REC_CLTFAIL;
	if (!sstatus.empty())
		status = Const::toInt(sstatus);

	return status;
}

int ZHTClient::lookup(const string& pair, string& result) {

	return commonOp(Const::ZSC_OPC_LOOKUP, pair, result);
}

int ZHTClient::remove(const string& pair) {

	string result;
	return commonOp(Const::ZSC_OPC_REMOVE, pair, result);
}

int ZHTClient::insert(const string& pair) {

	string result;
	return commonOp(Const::ZSC_OPC_INSERT, pair, result);
}

string ZHTClient::commonOpInternal(const string& opcode, const HostEntity* phe,
		const string& pair, string& result) {

	Package pkg;
	pkg.ParseFromString(pair);
	pkg.set_opcode(opcode); // 1 for look up, 2 for remove, 3 for insert
	pkg.set_replicano(3); //5: original, 3 not original

	if (pkg.virtualpath().empty())
		return Const::ZSC_REC_EMPTYKEY; //-1, empty key not allowed.

	if (pkg.realfullpath().empty())
		pkg.set_realfullpath(" "); //coup, to fix ridiculous bug of protobuf!

	string msg = pkg.SerializeAsString();

	HostEntity he;
	if (phe == NULL) {

		getHostByKey(msg, he);
		if (!he.host.empty())
			phe = &he;
	}

	if (phe == NULL)
		return Const::ZSC_REC_CLTFAIL; //-2

	int sock = getSockCached(phe, TCP);
	reuseSock(sock);

	/*...send...*/
	int ssize = generalSendTo(phe->host.data(), phe->port, sock, msg.c_str(),
			msg.size(), TCP);

	/*...parse status and result, returned...*/
	string sstatus;
	if (ssize == msg.length()) { //this only work for TCP. UDP need to make a new one so accept returns from server.

			/*...parse status, returned...*/
		parseStatusAndResult(sock, sstatus, result);
	}

	return sstatus;
}

string ZHTClient::sendPkg(const string& host, const uint& port,
		const string& msg) {

	string result;

	return sendPkg(host, port, msg, result);
}

string ZHTClient::sendPkg(const string& host, const uint& port,
		const string& msg, string& result) {

	string sstatus = sendPkgInternal(host, port, msg, result);

	/*	int status = -1;
	 if (!sstatus.empty())
	 status = atoi(sstatus.c_str());*/

	return sstatus;
}

string ZHTClient::sendPkgInternal(const string& host, const uint& port,
		const string& msg, string& result) {

	int sock = getSockCached(host, port, true);

	if (sock <= 0) {

		fprintf(stderr, "failed to make console socket: {%s:%u}",
				Const::LOCAL_HOST.c_str(), port);
		sock = -1;
	}
	reuseSock(sock);

	int ssize = generalSendTo(host.data(), port, sock, msg.c_str(), msg.size(),
			true);

	/*...parse status and result, returned...*/
	string sstatus = Const::ZSC_REC_CLTFAIL; //-2
	if (ssize == (int) msg.length()) {

		/*...parse status, returned...*/
		parseStatusAndResult(sock, sstatus, result);
	}

	return sstatus;
}

void ZHTClient::parseStatusAndResult(const int& sock, string& sstatus,
		string& result) {

	/*...parse status, returned...*/
	int rsize = -1;
	sockaddr_in recvAddr;
	char buff[Env::MAX_MSG_SIZE]; //Env::MAX_MSG_SIZE
	memset(buff, 0, sizeof(buff));

	rsize = generalReceive(sock, (void*) buff, sizeof(buff), recvAddr, 0, true);

	if (rsize < 0) {

		fprintf(stderr, "receive error, got size[%d] bytes from sock[%d]",
				rsize, sock);
	} else {

		string srecv;
		srecv.assign(buff);

		if (srecv.empty()) {

			sstatus = Const::ZSC_REC_SRVEXP;
		} else {

			result = srecv.substr(3); //the left, if any, is lookup result or second-try pkg
			sstatus = srecv.substr(0, 3); //status returned, the first three chars, like 001, -98...
		}
	}
}

int ZHTClient::tearDown() {

	int rcode = 0;

	try {
		if (TCP == true) {

			int size = _MemList.size();

			for (int i = 0; i < size; i++) {

				struct HostEntity dest = _MemList.at(i);
				int sock = dest.sock;

				if (sock > 0) {
					close(sock);
				}
			}
		}
	} catch (exception& e) {

		fprintf(stderr, "%s, exception caught:\n\t%s", "ZHTClient::tearDown",
				e.what());
		rcode = -1;
	}

	return rcode;
}

int ZHTClient::tearDown2() {

	int rcode = 0;

	try {

		if (TCP == true) {

			MAP::iterator it;

			for (it = _dynMemMap.begin(); it != _dynMemMap.end(); it++) {

				int sock = it->second.sock;

				if (sock > 0)
					close(sock);
			}

		}
	} catch (exception& e) {

		fprintf(stderr, "%s, exception caught:\n\t%s",
				"ZHTClient::tearDown2() ", e.what());

		rcode = -1;
	}

	return rcode;
}

const string ZHTClient::getHostByKey(const string& msg, HostEntity& he) {

	Package pkg;
	pkg.ParseFromString(msg);

	int index = HashUtil::genHash(pkg.virtualpath()) % _MemList.size(); //todo: problem when shrink the size of ZHT network.

	string destZHT;
	string sstatus = getDestZHTByKey(_MemList.at(index), msg, destZHT);

	if (!destZHT.empty())
		he = getHostEntity(Address::getHost(destZHT),
				Address::getPort(destZHT));

	return sstatus;
}

const string ZHTClient::getDestZHTByKey(const HostEntity& coordinator,
		const string& msg, string& destZHT) {

	Package pkg;
	pkg.ParseFromString(msg);
	pkg.set_opcode(Const::ZSC_OPC_GET_DESTZHT);

	string result;
	string sstatus = sendPkgInternal(coordinator.host, coordinator.port,
			pkg.SerializeAsString(), result);

	Package pkg2;
	pkg2.ParseFromString(result);

	destZHT = pkg2.targetzht();

	return sstatus;
}

const HostEntity* ZHTClient::getHostByKey2(const string& msg) {

	HostEntity *phe = NULL;

	Package pkg;
	pkg.ParseFromString(msg);

	uint64_t ringPos = getRingPosByKey(HashUtil::genHash(pkg.virtualpath()));

	if (ringPos > 0) {

		phe = &_dynMemMap[ringPos];
	}

	return phe;
}

uint64_t ZHTClient::getRingPosByKey(const uint64_t& hashKey) {

	uint64_t pos = -1;

	MAP::iterator it;

	for (it = _dynMemMap.begin(); it != _dynMemMap.end(); it++) {

		if (hashKey > it->first) {

			pos = it->first;
			break;
		}
	}

	return pos;
}

int ZHTClient::getSockCached(const HostEntity* phe, const bool& tcp) {

	int sock = 0;

	if (phe == NULL)
		return sock;

	return getSockCached(phe->host, phe->port, tcp);

}

int ZHTClient::getSockCached(const string& host, const uint& port,
		const bool& tcp) {

	int sock = 0;

	string hashKey = HashUtil::genBase(host, port);

	if (tcp == true) {

		sock = CONN_CACHE.fetch(hashKey, tcp);

		if (sock <= 0) {

			sock = makeClientSocket(host.c_str(), port, tcp);

			if (sock <= 0) {

				cerr << "Client insert:making connection failed." << endl;
				sock = -1;
			} else {

				int tobeRemoved = -1;
				CONN_CACHE.insert(hashKey, sock, tobeRemoved);

				if (tobeRemoved != -1) {
					close(tobeRemoved);
				}
			}
		}
	} else {

		if (UDP_SOCKET <= 0) {
			sock = makeClientSocket(host.c_str(), port, tcp);
			UDP_SOCKET = sock;
		} else
			sock = UDP_SOCKET;
	}

	return sock;
}

int ZHTClient::getSockByKey2(const string& msg, const bool& tcp) {

	int sock = 0;
	const HostEntity* phe = getHostByKey2(msg);

	if (phe == NULL) {

		return sock;
	} else {

		string hashKey = HashUtil::genBase(phe->host, phe->port);

		if (tcp == true) {

			sock = CONN_CACHE.fetch(hashKey, tcp);

			if (sock <= 0) {

				sock = makeClientSocket(phe->host.c_str(), phe->port, tcp);

				if (sock <= 0) {

					cerr << "Client insert:making connection failed." << endl;
					sock = -1;
				} else {

					int tobeRemoved = -1;
					CONN_CACHE.insert(hashKey, sock, tobeRemoved);

					if (tobeRemoved != -1) {
						close(tobeRemoved);
					}
				}
			}
		} else {

			if (UDP_SOCKET <= 0) {
				sock = makeClientSocket(phe->host.c_str(), phe->port, tcp);
				UDP_SOCKET = sock;
			} else
				sock = UDP_SOCKET;
		}
	}

	return sock;
}

int ZHTClient::readDynamicMembership(string dynMemFile) {

	int rcode = 0;

	ifstream ifs;

	_dynMemMap.clear();

	try {

		ifs.open(dynMemFile.c_str(), ifstream::in);

		if (!ifs.is_open()) {

			fprintf(stderr, "Problem while opening config file {%s}\n.",
					dynMemFile.c_str());
		} else {

			string line;
			const char *delimiter = Const::CFG_DELIMITERS.c_str();

			while (getline(ifs, line)) {

				string uuid;
				string host;
				string port;
				char *token, *sp;

				token = strtok_r((char*) line.c_str(), delimiter, &sp);
				if (token != NULL)
					uuid = string(token);

				token = strtok_r(NULL, delimiter, &sp);
				if (token != NULL)
					host = string(token);

				token = strtok_r(NULL, delimiter, &sp);
				if (token != NULL)
					port = string(token);

				if (!uuid.empty() && !host.empty() && !port.empty()) {

					_dynMemMap.insert(
							PAIR(strtoul(uuid.c_str(), NULL, 10),
									::getHostEntity(host, atoi(port.c_str()))));
				}

			}
		}

		ifs.close();
	} catch (exception& e) {

		fprintf(stderr, "%s, exception caught:\n\t%s",
				"ZHTClient::readDynamicMembership(string dynMemFile)",
				e.what());

		rcode = -1;
		ifs.close();
		_dynMemMap.clear();
	}

	return rcode;
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
