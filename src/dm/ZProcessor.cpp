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
 * ZProcessor.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: tony, xiaobingo
 */

#include "ZProcessor.h"
#include "Const-impl.h"

#include <sys/socket.h>
#include <stdio.h>

using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

ZProcessor::ZProcessor() {

}

ZProcessor::~ZProcessor() {

}

void ZProcessor::sendback(const int& fd, const char *buf, const size_t& count,
		const sockaddr& receiver, const int& protocol) {

	int i = -2;
	try {
		if (protocol == Const::PROTO_STREAM) {

			i = send(fd, buf, count, 0);
#if ILOG
			fprintf(stdout, "in sendback, send to sock(%d), %d bytes sent\n",
					fd, i);
#endif
		} else if (protocol == Const::PROTO_UGRADM) {

			i = sendto(fd, buf, count, 0, (struct sockaddr*) &receiver,
					sizeof(struct sockaddr));
#if ILOG
			fprintf(stdout, "in sendback, send to sock(%d), %d bytes sent\n",
					fd, i);
#endif
		} else {
		}
	} catch (exception& e) {

		fprintf(stderr, "%s, exception caught:\n\t%s",
				"ZProcessor::sendback(const int& fd, const char *buf, const size_t& count, const sockaddr& receiver, const int& protocol)",
				e.what());
	}

}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */

