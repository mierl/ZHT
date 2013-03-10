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
 * socket_hostname.cpp
 *
 *  Created on: Aug 19, 2012
 *      Author: tony, xiaobingo
 */

#include "socket_hostname.h"

SocketHostName::SocketHostName() {

}

SocketHostName::~SocketHostName() {
}

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

int main(int argc, char **argv) {

	struct addrinfo hints, *info, *p;
	int gai_result;

	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
		exit(1);
	}

	for (p = info; p != NULL; p = p->ai_next) {

		printf("ai_flags: %d\n", p->ai_flags);
		printf("ai_family: %d\n", p->ai_family);
		printf("ai_socktype: %d\n", p->ai_socktype);
		printf("ai_protocol: %d\n", p->ai_protocol);
		printf("ai_addrlen: %d\n", p->ai_addrlen);
		printf("hostname: %s\n", p->ai_canonname);

	}

	/*	int ai_flags;			 Input flags.
	 int ai_family;		 Protocol family for socket.
	 int ai_socktype;		 Socket type.
	 int ai_protocol;		 Protocol for socket.
	 socklen_t ai_addrlen;		 Length of socket address.
	 struct sockaddr *ai_addr;	 Socket address for socket.
	 char *ai_canonname;		 Canonical name for service location.
	 struct addrinfo *ai_next;	 Pointer to next in list.
	 */
	freeaddrinfo(info);

}

