/*
 * Copyright 2010-2020 DatasysLab@iit.edu(http://datasys.cs.iit.edu/index.html)
 *      Director: Ioan Raicu(iraicu@cs.iit.edu)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is part of ZHT library(http://datasys.cs.iit.edu/projects/ZHT/index.html).
 *      Tonglin Li(tli13@hawk.iit.edu) with nickname Tony,
 *      Xiaobing Zhou(xzhou40@hawk.iit.edu) with nickname Xiaobingo,
 *      Ke Wang(kwang22@hawk.iit.edu) with nickname KWang,
 *      Dongfang Zhao(dzhao8@@hawk.iit.edu) with nickname DZhao,
 *      Ioan Raicu(iraicu@cs.iit.edu).
 *
 * mpi_broker.cpp
 *
 *  Created on: Jul 7, 2013
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include <errno.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <string>
#include <getopt.h>

#include "mq_proxy_stub.h"
#include "mpi_proxy_stub.h"

#include "ConfHandler.h"

using namespace std;

using namespace iit::datasys::zht::dm;

MQStub mqstub;
MPIProxy mpiproxy;

void sig_handler(int signo) {

	if (signo == SIGHUP || signo == SIGINT || signo == SIGQUIT
			|| signo == SIGABRT || signo == SIGKILL || signo == SIGTERM
			|| signo == SIGSTOP || signo == SIGTSTP) {

//		printf("sig_handler called\n");
		mqstub.teardown();
	}
}

void init_sig_handler() {

	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_handler;
	sa.sa_flags = 0; // or SA_RESTART

	//SIGHUP | SIGINT | SIGQUIT | SIGABRT | SIGKILL | SIGTERM | SIGSTOP | SIGTSTP
	sigemptyset(&sa.sa_mask);
	sigaction(SIGHUP, &sa, 0);
	sigaction(SIGINT, &sa, 0);
	sigaction(SIGQUIT, &sa, 0);
	sigaction(SIGABRT, &sa, 0);
	sigaction(SIGKILL, &sa, 0);
	sigaction(SIGTERM, &sa, 0);
	sigaction(SIGSTOP, &sa, 0);
	sigaction(SIGTSTP, &sa, 0);
}

void init_me(const string &zhtConf, const string &neighborConf) {

	/*init signal handler*/
	init_sig_handler();

	/*init config*/
	ConfHandler::initConf(zhtConf, neighborConf);
}

void printUsage(char *argv_0);

void serve(int argc, char **argv) {

	size_t msz;
	char req[IPC_MAX_MSG_SZ];
	char ans[IPC_MAX_MSG_SZ];
	memset(req, 0, sizeof(req));
	memset(ans, 0, sizeof(ans));

	mpiproxy.init(argc, argv);

	for (;;) {/* never quits! */

		if (!mqstub.recv(req, msz)) {

			perror("MQStub::recv()");
			exit(1);
		}

		string reqstr(req, msz);

		if (!mpiproxy.sendrecv(reqstr.c_str(), reqstr.size(), ans, msz)) {

			perror("MPIProxy::sendrecv()");
			exit(1);
		}

		if (!mqstub.send(ans, msz)) {

			perror("MQStub::send()");
			exit(1);
		}

		memset(req, 0, sizeof(req));
		memset(ans, 0, sizeof(ans));
	}
}

int main(int argc, char **argv) {

	int printHelp = 0;
	string zhtConf = "";
	string neighborConf = "";

	int c;
	while ((c = getopt(argc, argv, "z:n:h")) != -1) {
		switch (c) {
		case 'z':
			zhtConf = string(optarg);
			break;
		case 'n':
			neighborConf = string(optarg);
			break;
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

		if (!zhtConf.empty() && !neighborConf.empty()) {

			init_me(zhtConf, neighborConf);

			serve(argc, argv);

		} else {

			if (!helpPrinted)
				printUsage(argv[0]);
		}

	} catch (exception& e) {

		fprintf(stderr, "%s, exception caught:\n\t%s", "zht-mpibroker::main",
				e.what());
	}

	return 0;
}

void printUsage(char *argv_0) {

	fprintf(stdout, "Usage:\n%s %s\n", argv_0,
			"-z zht.conf -n neighbor.conf [-h(help)]");
}
