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
 * Address.cpp
 *
 *  Created on: Jul 25, 2012
 *      Author: tony, xiaobingo
 */

#include "Address.h"

#include "Const-impl.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

Address::Address() :
		_uuid(0), _host(""), _port(0), _ouuid(Const::ADDR_UUIDZERO) {

}

Address::Address(string sAddress) :
		_uuid(0), _host(""), _port(0), _ouuid(Const::ADDR_UUIDZERO) {

	assign(sAddress);
}

Address::Address(const string& host, const uint& port) :
		_uuid(0), _host(host), _port(port), _ouuid(Const::ADDR_UUIDZERO) {
}

Address::Address(const uint64_t& uuid, const string& host, const uint& port) :
		_uuid(uuid), _host(host), _port(port), _ouuid(Const::ADDR_UUIDZERO) {

}

Address::~Address() {

}

uint64_t Address::uuid() const {

	return _uuid;
}

void Address::uuid(const uint64_t& uuid) {

	_uuid = uuid;
}

string Address::uuidstr() const {

	return Const::toString(uuid());
}

void Address::uuidstr(const string& suuid) {

	_uuid = strtoull(suuid.c_str(), NULL, 10);
}

uint64_t Address::ouuid() const {

	return _ouuid;
}

void Address::ouuid(const uint64_t& ouuid) {

	_ouuid = ouuid;
}

string Address::ouuidstr() const {

	return Const::toString(ouuid());
}

void Address::ouuidstr(const string& souuid) {

	_ouuid = strtoull(souuid.c_str(), NULL, 10);
}

string Address::host() const {

	return _host;
}

void Address::host(const string& host) {

	_host = host;
}

uint Address::port() const {

	return _port;
}

void Address::port(const uint& port) {

	_port = port;
}

string Address::getFormat() {

	return "%llu,%s,%u,%llu";
}

string Address::getHost(const string& hostPort) {

	string delimiter = ":";

	string remains = Const::trim(hostPort);

	size_t found = remains.find(delimiter);

	string host;
	if (found != string::npos) {

		host = Const::trim(remains.substr(0, int(found)));
	}

	return host;
}

uint Address::getPort(const string& hostPort) {

	string delimiter = ":";

	string remains = Const::trim(hostPort);

	size_t found = remains.find(delimiter);

	uint port = 0;
	if (found != string::npos) {
		port = atoi(Const::trim(remains.substr(int(found) + 1)).c_str());
	}

	return port;
}

string Address::genHostPort(const string& host, const uint& port) {

	return genHostPortInternal(host, port);
}

string Address::genHostPort(const string& host, const string& port) {

	return genHostPortInternal(host, Const::toInt(port));
}

string Address::genAddressString(const uint64_t& uuid, const string& host,
		const uint& port) {

	return genAddressStringInternal(uuid, host, port, 0);
}

string Address::genAddressStringInternal(const uint64_t& uuid,
		const string& host, const uint& port, const uint64_t& ouuid) {

	char buf[500];
	memset(buf, 0, sizeof(buf));
	int n = sprintf(buf, Address::getFormat().c_str(), uuid, host.c_str(), port,
			ouuid); //38769960, 192.168.1.10, 50000
	string sAddress(buf, 0, n);

	return sAddress;
}

string Address::genHostPortInternal(const string& host, const uint& port) {

	stringstream ss;
	ss << host;
	ss << ":";
	ss << port;

	return ss.str();
}

/*get a file path by ip and port, like 192.168.1.1:5000*/
const string Address::toPath() const {

	return toHostPort();
}

const string Address::toString() const {

	char buf[100];
	memset(buf, 0, sizeof(buf));

	int n = sprintf(buf, getFormat().c_str(), uuid(), host().c_str(), port(),
			ouuid()); //38769960, 192.168.1.10, 50000, 0

	string result(buf, 0, n);

	return result;
}

/*get a file path by ip and port, like 192.168.1.1:5000*/
const string Address::toHostPort() const {

	return genHostPortInternal(host(), port());
}

void Address::assign(const Address& address) {

	assign(address.toString());
}

void Address::assign(string saddress) {

	const char* delimiter = ",";

	string remains = Const::trim(saddress);

	for (int i = 1; i <= 4; i++) {

		size_t found = remains.find(delimiter);

		if (found != string::npos) {

			if (i == 1) {

				string token = Const::trim(remains.substr(0, int(found)));
				uuid(strtoull(token.c_str(), NULL, 10)); //38769960, strtoull
				remains = Const::trim(remains.substr(int(found) + 1));
			}

			if (i == 2) {

				string token = Const::trim(remains.substr(0, int(found)));
				host(token); //192.168.1.10
				remains = Const::trim(remains.substr(int(found) + 1));
			}

			if (i == 3) {

				string token = Const::trim(remains.substr(0, int(found)));
				port(atoi(token.c_str())); //50000, strtou
				remains = Const::trim(remains.substr(int(found)));
			}

			if (i == 4) {

				string token = Const::trim(remains.substr(int(found) + 1));
				ouuid(strtoull(token.c_str(), NULL, 10)); //38769960, strtoull
				break;
			}
		}
	}
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
