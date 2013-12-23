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
 * cpp_zhtclient_test.cpp
 *
 *  Created on: Aug 7, 2012
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include "include/cpp_zhtclient.h"

#include  <getopt.h>
#include  <stdlib.h>
#include   <stdio.h>
#include   <string>
#include   <exception>
using namespace std;

void test_insert();
void test_lookup();
void test_remove();
void test_append();

void printUsage(char *argv_0);

ZHTClient zc;

void test_all() {

	printf("starting test_insert...\n");
	test_insert();

	printf("starting test_lookup...\n");
	test_lookup();

	printf("starting test_remove...\n");
	test_remove();

	printf("starting test_append...\n");
	test_append();
}

void test_lookup_shared(string key) {

	string result;
	int rc = zc.lookup(key, result);

	if (rc == 0)
		printf("LOOKUP OK, rc(%d), value={%s}\n", rc, result.c_str());
	else
		printf("LOOKUP ERR, rc(%d), value={%s}\n", rc, result.c_str());
}

void test_all_other() {

	string key = "hello";
	string val = "1.zht";

	zc.insert(key, val);
	test_lookup_shared(key);

	val = "2.zht";
	zc.append(key, val);
	test_lookup_shared(key);

	val = "3.zht";
	zc.insert(key, val);
	test_lookup_shared(key);

	val = "4.zht";
	zc.append(key, val);
	test_lookup_shared(key);
}

int main(int argc, char **argv) {

	extern char *optarg;

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

			zc.init(zhtConf, neighborConf);

			test_all_other();

			test_all();

			zc.teardown();

		} else {

			if (!helpPrinted)
				printUsage(argv[0]);
		}
	} catch (exception& e) {

		fprintf(stderr, "%s, exception caught:\n\t%s", "ZHTServer::main",
				e.what());
	}

}

void printUsage(char *argv_0) {

	fprintf(stdout, "Usage:\n%s %s\n", argv_0,
			"-z zht.conf -n neighbor.conf [-h(help)]");
}

void test_insert() {

	string key = "goodman";
	string val =
			"[1],The Only Thing Necessary for the Triumph of Evil is that Good Men Do Nothing";

	int rc = zc.insert(key, val);

	if (rc == 0)
		printf("INSERT OK, rc(%d)\n", rc);
	else
		printf("INSERT ERR, rc(%d)\n", rc);
}

void test_lookup() {

	test_insert();

	string key = "goodman";

	string result;
	int rc = zc.lookup(key, result);

	if (rc == 0)
		printf("LOOKUP OK, rc(%d), value={%s}\n", rc, result.c_str());
	else
		printf("LOOKUP ERR, rc(%d), value={%s}\n", rc, result.c_str());
}

void test_remove() {

	test_insert();

	string key = "goodman";

	int rc = zc.remove(key);

	if (rc == 0)
		printf("REMOVE OK, rc(%d)\n", rc);
	else
		printf("REMOVE ERR, rc(%d)\n", rc);
}

void test_append() {

	test_insert();

	string key = "goodman";
	string val2 =
			"[2],The Only Thing Necessary for the Triumph of Evil is that Good Men Do Nothing";

	int rc = zc.append(key, val2);

	if (rc == 0)
		printf("APPEND OK, rc(%d)\n", rc);
	else
		printf("APPEND ERR, rc(%d)\n", rc);

	string result;
	rc = zc.lookup(key, result);

	if (rc == 0)
		printf("LOOKUP OK, rc(%d), value={%s}\n", rc, result.c_str()); //todo: consider ": delimited"
	else
		printf("LOOKUP ERR, rc(%d), value={%s}\n", rc, result.c_str()); //todo: consider ": delimited"
}
