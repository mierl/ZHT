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
 * cpp_zhtclient_threaded_test.cpp
 *
 *  Created on: Apr 24, 2013
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

/*

 /*
 This file is to let you know how to run the testcases(cpp_zhtclient_threaded_test.cpp)
 for ZHT API C++ binding with multi-thread support.

 1. make

 2. too see usage, run ./cpp_zhtclient_threaded_test or ./cpp_zhtclient_threaded_test -h

 Usage:
 See also the method <printUsage>.


 for example in real tests:
 a) to run insert/remove/lookup/append/comp_swap/state_change_callback concurrently, each run by 10 threads
 ./cpp_zhtclient_threaded_test -t 10 -z zht.conf -n neighbor.conf -i -r -l -a -c -s

 b) to run insert/remove/lookup/append/comp_swap concurrently, each run by 10 threads
 ./cpp_zhtclient_threaded_test -t 10 -z zht.conf -n neighbor.conf -i -r -l -a -c

 c) to run insert/remove/lookup/append concurrently, each run by 10 threads
 ./cpp_zhtclient_threaded_test -t 10 -z zht.conf -n neighbor.conf -i -r -l -a

 d) to run insert/remove/lookup concurrently, each run by 10 threads
 ./cpp_zhtclient_threaded_test -t 10 -z zht.conf -n neighbor.conf -i -r -l

 e) to run insert/remove concurrently, each run by 10 threads
 ./cpp_zhtclient_threaded_test -t 10 -z zht.conf -n neighbor.conf -i -r

 f) to run insert concurrently, each run by 10 threads
 ./cpp_zhtclient_threaded_test -t 10 -z zht.conf -n neighbor.conf -i

 3. at least one of -i -r -l -a -c -s MUST be specified.

 * */

#include <pthread.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "cpp_zhtclient.h"

#include "Util.h"

#include <getopt.h>

using namespace iit::datasys::zht::dm;

/*concurrent threads to invoke a specified ZHT API*/
int CONCUR_DEGREE = -1;

/*size of buffer to store lookup result*/
const int LOOKUP_SIZE = 1024 * 2; //size of buffer to store lookup result, larger enough than TOTAL_SIZE

char *zhtConf = NULL;
char *neighborConf = NULL;
ZHTClient zhtclient;

const char *key = "cpp_zhtclient_threaded_test";

void printUsage(char *argv_0);

void *insert_threaded(void *arg) {

//	const char *key = "insert_threaded";

	ZHTClient *zhtclient = (ZHTClient*) arg;

	char buf[100];
	sprintf(buf, "%lu", pthread_self());

	int rc = zhtclient->insert(key, buf);

	fprintf(stdout,
			"[%lu] ZHTClient::insert, {key}:{value}=> {%s}:{%s}, rc(%d)\n",
			pthread_self(), key, buf, rc);

}

void* test_insert(void *arg) {

	pthread_t threads[CONCUR_DEGREE];

	double start = TimeUtil::getTime_msec();

	int i;
	for (i = 0; i < CONCUR_DEGREE; i++) {

		pthread_create(&threads[i], NULL, insert_threaded, (void*) &zhtclient);
	}

	for (i = 0; i < CONCUR_DEGREE; i++) {

		pthread_join(threads[i], NULL);
	}

	double end = TimeUtil::getTime_msec();

	char buf[200];
	sprintf(buf, "test_insert(%d thread(s), each running %d insert(s)), %f(ms)",
			CONCUR_DEGREE, 1, end - start);
	fprintf(stdout, "%s\n", buf);

}

void *remove_threaded(void *arg) {

//	const char *key = "remove_threaded";

	ZHTClient *zhtclient = (ZHTClient*) arg;

	int rc = zhtclient->remove(key);

	fprintf(stdout, "[%lu] ZHTClient::remove, {key}=> {%s}, rc(%d)\n",
			pthread_self(), key, rc);
}

void* test_remove(void *arg) {

	pthread_t threads[CONCUR_DEGREE];

	double start = TimeUtil::getTime_msec();

	int i;
	for (i = 0; i < CONCUR_DEGREE; i++) {

		pthread_create(&threads[i], NULL, remove_threaded, (void*) &zhtclient);
	}

	for (i = 0; i < CONCUR_DEGREE; i++) {

		pthread_join(threads[i], NULL);
	}

	double end = TimeUtil::getTime_msec();

	char buf[200];
	sprintf(buf, "test_remove(%d thread(s), each running %d insert(s)), %f(ms)",
			CONCUR_DEGREE, 1, end - start);
	fprintf(stdout, "%s\n", buf);
}

void *lookup_threaded(void *arg) {

//	const char *key = "lookup_threaded";

	ZHTClient *zhtclient = (ZHTClient*) arg;

	size_t ln;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL) {

		int rc = zhtclient->lookup(key, result); //1 for look up, 2 for remove, 3 for insert, 4 for append

		ln = strlen(result);

		/*		fprintf(stdout, "[%d] c_zht_lookup, return code(length): %d(%lu)\n",
		 seq_num, lret, ln);*/

		fprintf(stdout,
				"[%lu] ZHTClient::lookup, return {key}:{value}=> {%s}:{%s}, rc(%d)\n",
				pthread_self(), key, result, rc);
	}

	free(result);
}

void* test_lookup(void *arg) {

	pthread_t threads[CONCUR_DEGREE];

	double start = TimeUtil::getTime_msec();

	int i;
	for (i = 0; i < CONCUR_DEGREE; i++) {

		pthread_create(&threads[i], NULL, lookup_threaded, (void*) &zhtclient);
	}

	for (i = 0; i < CONCUR_DEGREE; i++) {

		pthread_join(threads[i], NULL);
	}

	double end = TimeUtil::getTime_msec();

	char buf[200];
	sprintf(buf, "test_lookup(%d thread(s), each running %d insert(s)), %f(ms)",
			CONCUR_DEGREE, 1, end - start);
	fprintf(stdout, "%s\n", buf);
}

void *append_threaded(void *arg) {

//	const char *key = "append_threaded";

	ZHTClient *zhtclient = (ZHTClient*) arg;

	char buf[100];
	sprintf(buf, "%lu", pthread_self());

	int rc = zhtclient->append(key, buf);

	fprintf(stdout,
			"[%lu] ZHTClient::append, {key}:{value}=> {%s}:{%s}, rc(%d)\n",
			pthread_self(), key, buf, rc);
}

void* test_append(void *arg) {

	pthread_t threads[CONCUR_DEGREE];

	double start = TimeUtil::getTime_msec();

	int i;
	for (i = 0; i < CONCUR_DEGREE; i++) {

		pthread_create(&threads[i], NULL, append_threaded, (void*) &zhtclient);
	}

	for (i = 0; i < CONCUR_DEGREE; i++) {

		pthread_join(threads[i], NULL);
	}

	double end = TimeUtil::getTime_msec();

	char buf[200];
	sprintf(buf, "test_append(%d thread(s), each running %d insert(s)), %f(ms)",
			CONCUR_DEGREE, 1, end - start);
	fprintf(stdout, "%s\n", buf);
}

void *comp_swap_threaded(void *arg) {

//	const char *key = "comp_swap_threaded";

	ZHTClient *zhtclient = (ZHTClient*) arg;

	char buf[100];
	sprintf(buf, "%lu", pthread_self());

	char buf2[100];
	sprintf(buf2, "%lu", pthread_self() - 100);

	char *buf3 = (char*) calloc(100, sizeof(char));

	int rc = zhtclient->compare_swap(key, buf, buf2, buf3);

	fprintf(stdout,
			"[%lu] ZHTClient::compare_swap, {key}:{seen_value}:{new_value}=> {%s}:{%s}:{%s}, rc(%d)\n",
			pthread_self(), key, buf, buf2, rc);

	free(buf3);
}

void* test_comp_swap(void *arg) {

	pthread_t threads[CONCUR_DEGREE];

	double start = TimeUtil::getTime_msec();

	int i;
	for (i = 0; i < CONCUR_DEGREE; i++) {

		pthread_create(&threads[i], NULL, comp_swap_threaded,
				(void*) &zhtclient);
	}

	for (i = 0; i < CONCUR_DEGREE; i++) {
		pthread_join(threads[i], NULL);
	}

	double end = TimeUtil::getTime_msec();

	char buf[200];
	sprintf(buf,
			"test_comp_swap(%d thread(s), each running %d insert(s)), %f(ms)",
			CONCUR_DEGREE, 1, end - start);
	fprintf(stdout, "%s\n", buf);
}

void* state_change_callback_threaded(void *arg) {

//	const char *key = "state_change_callback_threaded";

	ZHTClient *zhtclient = (ZHTClient*) arg;

	char *buf = (char*) calloc(100, sizeof(char));

	//sprintf(buf, "%lu", pthread_self()); //try this line to unblock state_change_callback
	sprintf(buf, "%lu", pthread_self() + 1); //try this line to block state_change_callback

	int rc = zhtclient->insert(key, buf); //first, insert the key/value pair

	sprintf(buf, "%lu", pthread_self());
	rc = zhtclient->state_change_callback(key, buf, 500);

	fprintf(stdout,
			"[%lu] ZHTClient::state_change_callback, {key}:{value}=> {%s}:{%s}, rc(%d)\n",
			pthread_self(), key, buf, rc);

	free(buf);
}

void* test_state_change_callback(void *arg) {

	pthread_t threads[CONCUR_DEGREE];

	double start = TimeUtil::getTime_msec();

	int i;
	for (i = 0; i < CONCUR_DEGREE; i++) {

		pthread_create(&threads[i], NULL, state_change_callback_threaded,
				(void*) &zhtclient);
	}

	for (i = 0; i < CONCUR_DEGREE; i++) {
		pthread_join(threads[i], NULL);
	}

	double end = TimeUtil::getTime_msec();

	char buf[200];
	sprintf(buf,
			"test_state_change_callback(%d thread(s), each running %d insert(s)), %f(ms)",
			CONCUR_DEGREE, 1, end - start);
	fprintf(stdout, "%s\n", buf);
}

int is_insert = 0;
int is_remove = 0;
int is_lookup = 0;
int is_append = 0;
int is_comp_swap = 0;
int is_state_change_callback = 0;

void test_dipatch() {

	int tc = 6;
	pthread_t threads[tc];
	int mask[6] = { 0 };

	if (is_insert) {

		/*test concurrent insert*/

		mask[0] = 1;
		pthread_create(&threads[0], NULL, test_insert, NULL);
	}

	if (is_remove) {

		/*test concurrent remove*/
		mask[1] = 1;
		pthread_create(&threads[1], NULL, test_remove, NULL);
	}

	if (is_lookup) {

		/*test concurrent lookup*/
		mask[2] = 1;
		pthread_create(&threads[2], NULL, test_lookup, NULL);
	}

	if (is_append) {

		/*test concurrent append*/
		mask[3] = 1;
		pthread_create(&threads[3], NULL, test_append, NULL);
	}

	if (is_comp_swap) {

		/*test concurrent com_swap*/
		mask[4] = 1;
		pthread_create(&threads[4], NULL, test_comp_swap, NULL);
	}

	if (is_state_change_callback) {

		/*test concurrent state_change_callback*/
		mask[5] = 1;
		pthread_create(&threads[5], NULL, test_state_change_callback, NULL);
	}

	int i;
	for (i = 0; i < tc; i++) {
		if (mask[i])
			pthread_join(threads[i], NULL);
	}
}

int main(int argc, char **argv) {

	extern char *optarg;

	double us = 0;
	int printHelp = 0;

	int c;
	while ((c = getopt(argc, argv, "t:z:n:irlacsh")) != -1) {
		switch (c) {
		case 't':
			CONCUR_DEGREE = atoi(optarg);
			break;
		case 'z':
			zhtConf = optarg;
			break;
		case 'n':
			neighborConf = optarg;
			break;
		case 'i':
			is_insert = 1;
			break;
		case 'r':
			is_remove = 1;
			break;
		case 'l':
			is_lookup = 1;
			break;
		case 'a':
			is_append = 1;
			break;
		case 'c':
			is_comp_swap = 1;
			break;
		case 's':
			is_state_change_callback = 1;
			break;
		case 'h':
			printHelp = 1;
			break;
		default:
			fprintf(stdout, "Illegal argument \"%c\"\n", c);
			printUsage(argv[0]);
			exit(1);
		}
	}

	if (printHelp) {
		printUsage(argv[0]);
		exit(1);
	}

	if (zhtConf != NULL && neighborConf != NULL && CONCUR_DEGREE > 0) {

		/*init...*/
		//c_zht_init(zhtConf, neighborConf); //zht.conf, neighbor.conf
		zhtclient.init(zhtConf, neighborConf);

		test_dipatch();

		if (!is_insert && !is_remove && !is_lookup && !is_append
				&& !is_comp_swap && !is_state_change_callback)
			printUsage(argv[0]);

		/*clear...*/
		//c_zht_teardown();
		zhtclient.teardown();
	} else {

		printUsage(argv[0]);
		exit(1);
	}

	return 0;
}

void printUsage(char *argv_0) {

	fprintf(stdout, "Usage:\n%s %s\n", argv_0,
			"{-t concurrent_threads -z zht.conf -n neighbor.conf {-i(insert) | -r(remove) | -l(lookup) | -a(append) | -c(commpare_and_swap) | -s(state_change_callback) } | [-h(help)] }");
}

