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
 * c_zhtclient_lanl_threaded.c
 *
 *  Created on: Apr 24, 2013
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

/*
 The following is sequence you need to run this test case(c_zhtclient_lanl_threaded.c).
 You can also only run step 3 to see what will happened.

 1. to make, run
 make

 2. to insert 1000 nodes, run
 ./c_zhtclient_lanl_threaded -n neighbor.conf -z zht.conf -i

 3. to concurrently allocate nodes by comp_and_swap, run
 ./c_zhtclient_lanl_threaded -n neighbor.conf -z zht.conf

 4. to to check nodes finally available, run
 ./c_zhtclient_lanl_threaded -n neighbor.conf -z zht.conf -l

 5. to clean, run
 make clean


 => I changed the startup arguments only for this test programs, e.g. ./c_zhtclient_lanl_threaded -n neighbor.conf -z zht.conf
 to see the usage, run
 ./c_zhtclient_lanl_threaded or
 ./c_zhtclient_lanl_threaded -h


 ###########special attention###########
 1. this testcase uses multi-threads to simulate concurrent requests of nodes allocation.

 2. copy the code and adjust it on your demand, but take care the critical section code.
 * */

#include   <stdbool.h>
#include   <stdlib.h>
#include   <stdio.h>
#include   <limits.h>

#include   <string.h>
#include "c_zhtclient.h"
#include "meta.pb-c.h"

#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <getopt.h>

/*concurrent threads to request c_zht_compare_and_swap*/
#define CONCUR_DEGREE 5

/*the number of nodes every thread requests*/
#define NODES_ALLOC  100

/*return code, means no more nodes available*/
#define RC_NO_NODES_AVAIL -1000

/*the number of nodes initially in the pool*/
char *NODES_POOL = "1000";

/*the key for nodes resource*/
char *key = "lanl_node_res";

/*size of buffer to store lookup result*/
const int LOOKUP_SIZE = 1024 * 2; //size of buffer to store lookup result, larger enough than TOTAL_SIZE

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int lookup_nodes() {

	size_t ln;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL ) {

		int lret = c_zht_lookup(key, result); //1 for look up, 2 for remove, 3 for insert, 4 for append
		ln = strlen(result);

		/*		fprintf(stdout, "[%d] c_zht_lookup, return code(length): %d(%lu)\n",
		 seq_num, lret, ln);*/

		fprintf(stdout,
				"[%lu] c_zht_lookup, return {key}:{value}=> {%s}:{%s}, rc(%d)\n",
				pthread_self(), key, result, lret);
	}

	int nodes = atoi(result);

	free(result);

	return nodes;
}

int compare_swap_internal(int nodes_avail, bool first_called,
		int *value_queried) {

	int nodes_left;

	if (first_called) {

		nodes_left = nodes_avail - NODES_ALLOC; //allocate 100 nodes
	} else {

		nodes_left = *value_queried - NODES_ALLOC; //allocate 100 nodes
	}

	/*todo: guard the case: nodes_left < 0, means no nodes available*/
	if (nodes_left < 0)
		return RC_NO_NODES_AVAIL; //for example, -1000 means no nodes available

	char p_nodes_avail[20] = { 0 };
	sprintf(p_nodes_avail, "%d", nodes_avail);

	char p_nodes_left[20] = { 0 };
	sprintf(p_nodes_left, "%d", nodes_left);

	char *p_nodes_queried = calloc(20, sizeof(char));

	int rc = c_zht_compare_swap(key, p_nodes_avail, p_nodes_left,
			p_nodes_queried);

	*value_queried = atoi(p_nodes_queried);

	fprintf(stdout,
			"[%lu] c_zht_compare_and_swap, {seen_value}:{new_value}:{value_queried} => {%s}:{%s}:{%s}, rc(%d)\n",
			pthread_self(), p_nodes_avail, p_nodes_left, p_nodes_queried, rc);

	free(p_nodes_queried);

	return rc;
}

void* compare_swap(void *arg) {

	pthread_mutex_lock(&mutex);

	int node_avail = lookup_nodes();

	int value_queried = 0;

	int rc = compare_swap_internal(node_avail, true, &value_queried);

	if (rc == RC_NO_NODES_AVAIL) {

//		return NULL; //todo: here,  you may need to return RC_NO_NODES_AVAIL to caller application
	} else {

		while (rc != 0) {

			rc = compare_swap_internal(node_avail, false, &value_queried);
		}
	}

	pthread_mutex_unlock(&mutex);

	printf("[%lu] %s\n", pthread_self(), "threaded compare_swap ends");
}

void compare_swap_final() {

	int nodes_avail = lookup_nodes();

	int nodes_pool = atoi(NODES_POOL);

	/*nodes_avail = (1000 - 5 * 100), since 5 threads request 100 nodes each once a time*/
	assert(nodes_avail == (nodes_pool - CONCUR_DEGREE * NODES_ALLOC));

	printf("[%lu] %s\n", pthread_self(), "compare_swap_final ends");
}

void insert_resource() {

	int rc = c_zht_insert(key, NODES_POOL);

	fprintf(stdout, "[%lu] init [%s] nodes available, rc(%d)\n", pthread_self(),
			NODES_POOL, rc);
}

void test_compare_swap() {

	insert_resource();

	int tc = CONCUR_DEGREE;
	pthread_t threads[tc];

	int i;
	for (i = 0; i < tc; i++) {

		int th_num = i + 1;
		pthread_create(&threads[i], NULL, compare_swap, (void*) &th_num);
	}

	for (i = 0; i < tc; i++) {
		pthread_join(threads[i], NULL );
	}

	compare_swap_final();

	printf("[%lu] %s\n", pthread_self(), "test_compare_swap ends");
}

void printUsage(char *argv_0);

int main(int argc, char **argv) {

	extern char *optarg;

	double us = 0;
	int printHelp = 0;

	char *zhtConf = NULL;
	char *neighborConf = NULL;
	int is_init = 0;
	int is_lookup = 0;

	int c;
	while ((c = getopt(argc, argv, "z:n:ilh")) != -1) {
		switch (c) {
		case 'z':
			zhtConf = optarg;
			break;
		case 'n':
			neighborConf = optarg;
			break;
		case 'i':
			is_init = 1;
			break;
		case 'l':
			is_lookup = 1;
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

	if (zhtConf != NULL && neighborConf != NULL ) {

		/*init...*/
		c_zht_init(zhtConf, neighborConf); //zht.conf, neighbor,conf

		if (is_init) {

			/*e.g. init node pool with 1000 nodes*/
			insert_resource();
		} else if (is_lookup) {

			/*lookup nodes available*/
			lookup_nodes();
		} else {

			/*test concurrent compare_and_swap*/
			test_compare_swap();
		}

		/*clear...*/
		c_zht_teardown();

	} else {

		printUsage(argv[0]);
		exit(1);
	}

	return 0;
}

void printUsage(char *argv_0) {

	fprintf(stdout, "Usage:\n%s %s\n", argv_0,
			"{-z zht.conf -n neighbor.conf {-i(init) | -l(lookup)} | -h(help)");
}
