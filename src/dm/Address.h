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
 * Address.h
 *
 *  Created on: Jul 25, 2012
 *      Author: tony, xiaobingo
 */

#ifndef ADDRESS_H_
#define ADDRESS_H_

#include <string>
#include <stdint.h>
#include <sys/types.h>

using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

class Address {
public:
	Address();
	explicit Address(string sAddress);
	Address(const string& host, const uint& port);
	Address(const uint64_t& uuid, const string& host, const uint& port);
	virtual ~Address();

	uint64_t uuid() const;
	void uuid(const uint64_t& uuid);
	string uuidstr() const;
	void uuidstr(const string& suuid);

	uint64_t ouuid() const;
	void ouuid(const uint64_t& ouuid);
	string ouuidstr() const;
	void ouuidstr(const string& souuid);

	string host() const;
	void host(const string& host);

	uint port() const;
	void port(const uint& port);

	const string toPath() const;
	const string toString() const;
	const string toHostPort() const;
	void assign(string saddress);
	void assign(const Address& address);

public:
	static string getFormat();
	static string getHost(const string& hostPort);
	static uint getPort(const string& hostPort);
	static string genHostPort(const string& host, const uint& port);
	static string genHostPort(const string& host, const string& port);
	static string genAddressString(const uint64_t& uuid, const string& host,
			const uint& port);

private:
	static string genHostPortInternal(const string& host, const uint& port);
	static string genAddressStringInternal(const uint64_t& uuid,
			const string& host, const uint& port, const uint64_t& ouuid);

private:
	uint64_t _uuid; //the hashkey from hash space, as zht's unique id
	uint64_t _ouuid; //old hashkey, two version of uuid, in case track the change of Address
	string _host;
	uint _port;
};

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* ADDRESS_H_ */
