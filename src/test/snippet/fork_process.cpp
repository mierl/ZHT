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
 * fork_process.cpp
 *
 *  Created on: Sep 16, 2012
 *      Author: tony, xiaobingo
 */

#include "fork_process.h"

ForkProcess::ForkProcess() {
}

ForkProcess::~ForkProcess() {
}

#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

void printUsage(char *argv_0);

int main_(int argc, char** argv) {

	pid_t fk;

	printf("\tbefore fork my pid = %lu\n", (unsigned long) getpid());

	fflush(stdout); /* This may keep the above print
	 statement from outputing twice. */

	fk = fork(); /* The OS kernel makes a copy of the current process here */

	printf("fork returned %lu and now my pid = %lu\n", (unsigned long) fk,
			(unsigned long) getpid());

	return 0;
}

int main(int argc, char** argv) {

	int printHelp = 0;
	string fileClient = "./fileclient"; //c
	string serverHost = "localhost"; //s
	string serverPort = "9000"; //p
	string remoteFileName = ""; //r
	string localFileName = ""; //l

	int c;
	while ((c = getopt(argc, argv, "c:s:p:r:l:h")) != -1) {
		switch (c) {
		case 'c':
			fileClient = string(optarg);
			break;
		case 's':
			serverHost = string(optarg);
			break;
		case 'p':
			serverPort = string(optarg);
			break;
		case 'r':
			remoteFileName = string(optarg);
			break;
		case 'l':
			localFileName = string(optarg);
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
		if (!remoteFileName.empty() && !localFileName.empty()) {

			fprintf(stdout, "%s %s %s %s %s\n", fileClient.c_str(),
					serverHost.c_str(), serverPort.c_str(),
					remoteFileName.c_str(), localFileName.c_str());

			char* args[6];

			args[0] = (char*) fileClient.c_str();
			args[1] = (char*) serverHost.c_str();
			args[2] = (char*) serverPort.c_str();
			args[3] = (char*) remoteFileName.c_str();
			args[4] = (char*) localFileName.c_str();
			args[5] = NULL;

			pid_t pid = vfork();
			if (pid == 0) {

				execv(args[0], args);
			}

		} else {
			if (!printHelp)
				printUsage(argv[0]);
		}
	} catch (exception& e) {
		fprintf(stderr, "exception caught:\n\t%s", e.what());
	}

	exit(1);
}

void printUsage(char *argv_0) {

	fprintf(stdout, "Usage:\n%s %s\n", argv_0,
			"[-c ./recvfile] [-s server_ip] [-p server_port] -r remote_filename -l local_filename [-h(help)]");
}

