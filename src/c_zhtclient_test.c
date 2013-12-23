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
 * c_zhtclient_test.c
 *
 *  Created on: Aug 7, 2012
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include   <stdbool.h>
#include   <stdlib.h>
#include   <stdio.h>
#include <getopt.h>

#include   <string.h>
#include "c_zhtclient.h"
#include "meta.pb-c.h"

const int LOOKUP_SIZE = 1000 * 100 * 2; //size of buffer to store lookup result, larger enough than TOTAL_SIZE
//const int LOOKUP_SIZE = 16 * 3; //size of buffer to store lookup result, larger enough than TOTAL_SIZE

const int TOTAL_SIZE = 1000 * 100 * 1; //total size of a message to be transfered
//const int TOTAL_SIZE = 16 * 2; //total size of a message to be transfered

void test_large_keyvalue();
void test_large_keyvalue_blankspace();
void test_large_keyvalue_emptystring();
void test_large_keyvalue_reuse();

void test_common_usecase();
void test_common_usecase_blankspace();
void test_common_usecase_emptystring();
void test_common_usecase_reuse();

void test_pass_package();
void test_pass_package_blankspace();
void test_pass_package_emptystring();
void test_pass_package_reuse();

void test_simple_largevalue();

void printUsage(char *argv_0);

void test_all() {

	printf("%s\n", "//////////////////test_common_usecase");

	test_common_usecase();

	printf("%s\n", "//////////////////test_simple_largevalue");

	test_simple_largevalue();

	printf("%s\n", "//////////////////test_large_keyvalue");

	test_large_keyvalue();

//	test_pass_package();
}

int main(int argc, char **argv) {

	extern char *optarg;

	int printHelp = 0;
	int numOfOps = -1;
	char *zhtConf = NULL;
	char *neighborConf = NULL;

	int c;
	while ((c = getopt(argc, argv, "z:n:h")) != -1) {
		switch (c) {
		case 'z':
			zhtConf = optarg;
			break;
		case 'n':
			neighborConf = optarg;
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

	if (zhtConf != NULL && neighborConf != NULL ) {

		c_zht_init(zhtConf, neighborConf);

		test_all();

		c_zht_teardown();

	} else {

		if (!helpPrinted)
			printUsage(argv[0]);
	}
}

void printUsage(char *argv_0) {

	fprintf(stdout, "Usage:\n%s %s\n", argv_0,
			"-z zht.conf -n neighbor.conf [-h(help)]");
}

void test_simple_largevalue() {

	const char *key = "test_simple_largevalue";
//	const char *value = "xiaobingo";

	char *value2 = calloc(TOTAL_SIZE, sizeof(char));
	memset(value2, '1', TOTAL_SIZE - 1);

	/*
	 * test c_zht_insert
	 * */
	int iret = c_zht_insert(key, value2); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_insert, return code: %d\n", iret);

	/*
	 * test c_zht_lookup
	 * */
	size_t ln;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL ) {

		int lret = c_zht_lookup(key, result); //1 for look up, 2 for remove, 3 for insert, 4 for append

		ln = strlen(result);

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);
		/*	fprintf(stdout, "c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
		 key, result);*/
	}

	free(result);

	/*
	 * test c_zht_append
	 * */
	int aret = c_zht_append(key, "value appended"); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_append, return code: %d\n", aret);

	/*
	 * test c_zht_lookup again
	 * */
	result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL ) {

		int lret = c_zht_lookup(key, result); //1 for look up, 2 for remove, 3 for insert, 4 for append

		ln = strlen(result);

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);
		/*	fprintf(stdout, "c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
		 key, result);*/
	}

	free(result);

	/*
	 * test c_zht_remove
	 * */
	int rret = c_zht_remove(key); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_remove, return code: %d\n", rret);

	free(value2);
}

void test_common_usecase() {

	test_common_usecase_blankspace();

	fprintf(stdout, "%s\n", "--------------------------");

	test_common_usecase_emptystring();
}

void test_common_usecase_blankspace() {

	const char *key = "test_common_usecase_blankspace";
	const char *value = " ";
	const char *value2 = "zht";

	test_common_usecase_reuse(key, value, value2);
}

void test_common_usecase_emptystring() {

	const char *key = "test_common_usecase_emptystring";
	const char *value = "";
	const char *value2 = "ZHT";

	test_common_usecase_reuse(key, value, value2);
}

void test_common_usecase_reuse(const char * const key, const char * const value,
		const char * const value2) {

	/*
	 * test c_zht_insert
	 * */
	int iret = c_zht_insert(key, value); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_insert, return code: %d\n", iret);

	/*
	 * test c_zht_lookup
	 * */
	size_t ln;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL ) {

		int lret = c_zht_lookup(key, result); //1 for look up, 2 for remove, 3 for insert, 4 for append

		ln = strlen(result);

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);
		fprintf(stdout, "c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
				key, result);
	}

	free(result);

	/*
	 * test c_zht_append
	 * */
	int aret = c_zht_append(key, value2); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_append, return code: %d\n", aret);

	/*
	 * test c_zht_lookup again
	 * */
	ln = 0;
	result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL ) {

		int lret = c_zht_lookup(key, result); //1 for look up, 2 for remove, 3 for insert, 4 for append

		ln = strlen(result);

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);
		fprintf(stdout, "c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
				key, result);
	}

	free(result);

	/*
	 * test c_zht_remove
	 * */
	int rret = c_zht_remove(key); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_remove, return code: %d\n", rret);
}

void test_large_keyvalue() {

	test_large_keyvalue_blankspace();

	fprintf(stdout, "%s\n", "--------------------------");

	test_large_keyvalue_emptystring();
}

void test_large_keyvalue_blankspace() {

	const char *key = "test_large_keyvalue_blankspace";
	const char *value = " ";

	char *value2 = calloc(TOTAL_SIZE, sizeof(char));
	memset(value2, '1', TOTAL_SIZE - 1);

	test_large_keyvalue_reuse(key, value, value2);

	free(value2);

}

void test_large_keyvalue_emptystring() {

	const char *key = "test_large_keyvalue_emptystring";
	const char *value = "";

	char *value2 = calloc(TOTAL_SIZE, sizeof(char));
	memset(value2, '2', TOTAL_SIZE - 1);

	test_large_keyvalue_reuse(key, value, value2);

	free(value2);
}

void test_large_keyvalue_reuse(const char * const key, const char * const value,
		const char * const value2) {

	/*
	 * test c_zht_insert
	 * */
	int iret = c_zht_insert(key, value); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_insert, return code: %d\n", iret);

	/*
	 * test c_zht_lookup
	 * */
	size_t ln;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL ) {

		int lret = c_zht_lookup(key, result); //1 for look up, 2 for remove, 3 for insert, 4 for append

		ln = strlen(result);

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);
		/*fprintf(stdout, "c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
		 key, result);*/
	}

	free(result);

	/*
	 * test c_zht_append
	 * */
	int aret = c_zht_append(key, value2); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_append, return code: %d\n", aret);

	/*
	 * test c_zht_lookup again
	 * */
	ln = 0;
	result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL ) {

		int lret = c_zht_lookup(key, result); //1 for look up, 2 for remove, 3 for insert, 4 for append

		ln = strlen(result);

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);
		/*fprintf(stdout, "c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
		 key, result);*/
	}

	free(result);

	/*
	 * test c_zht_remove
	 */
	int rret = c_zht_remove(key); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_remove, return code: %d\n", rret);

}

void test_pass_package() {

	test_pass_package_blankspace();

	fprintf(stdout, "%s\n", "--------------------------");

	test_pass_package_emptystring();
}

void test_pass_package_blankspace() {

	const char *key = "hello";
	const char *value = " ";
	const char *value2 = "zht";

	test_pass_package_reuse(key, value, value2);
}

void test_pass_package_emptystring() {

	const char *key = "hello";
	const char *value = "";
	const char *value2 = "ZHT";

	test_pass_package_reuse(key, value, value2);
}

void test_pass_package_reuse(const char *key, const char *value,
		const char *value2) {

	char *buf; //buffer to store serialized data
	unsigned len; //length of serialized data

	Package package = PACKAGE__INIT; // Package
	package.virtualpath = (char*) key;
	if (strcmp(value, "") != 0) //tricky: bypass protocol-buf's bug
		package.realfullpath = (char*) value;
	package.has_isdir = true;
	package.isdir = false;

	len = package__get_packed_size(&package);
	buf = (char*) calloc(len, sizeof(char));
	package__pack(&package, buf);

	/*
	 * test c_zht_insert
	 * */
	int iret = c_zht_insert(key, buf);
	fprintf(stdout, "c_zht_insert, return code: %d\n", iret);
	free(buf);

	/*
	 * test c_zht_lookup
	 * */
	size_t ln = 0;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL ) {

		int lret = c_zht_lookup(key, result);
		ln = strlen(result);

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);

		if (lret == 0 && ln > 0) {

			char *tokens, *pch;
			tokens = strtok_r(result, ":", &pch);
			while (tokens != NULL ) {

				Package *lpkg = package__unpack(NULL, LOOKUP_SIZE, result);

				if (lpkg == NULL ) {

					fprintf(stdout, "error unpacking lookup result\n");
				} else {

					fprintf(stdout,
							"c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
							lpkg->virtualpath, lpkg->realfullpath);

				}

				package__free_unpacked(lpkg, NULL );

				tokens = strtok_r(NULL, ":", &pch);
			}
		}
	}
	free(result);

	/*
	 * test c_zht_append
	 * */
	Package package2 = PACKAGE__INIT;
	package2.virtualpath = (char*) key;
	if (strcmp(value2, "") != 0) //tricky: bypass protocol-buf's bug
		package2.realfullpath = (char*) value2;
	package2.has_isdir = true;
	package2.isdir = false;

	len = package__get_packed_size(&package2);
	buf = (char*) calloc(len, sizeof(char));
	package__pack(&package2, buf);

	int aret = c_zht_append(key, buf);
	fprintf(stdout, "c_zht_append, return code: %d\n", aret);
	free(buf);

	/*
	 * test c_zht_lookup again
	 * */
	ln = 0;
	result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL ) {

		int lret = c_zht_lookup(key, result); //Here's bug
		ln = strlen(result);

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);

		if (lret == 0 && ln > 0) {

			char *tokens, *pch;
			tokens = strtok_r(result, ":", &pch);
			while (tokens != NULL ) {

				Package *lpkg = package__unpack(NULL, LOOKUP_SIZE, result);

				if (lpkg == NULL ) {

					fprintf(stdout, "error unpacking lookup result\n");

				} else {

					fprintf(stdout,
							"c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
							lpkg->virtualpath, lpkg->realfullpath);

				}

				package__free_unpacked(lpkg, NULL );

				tokens = strtok_r(NULL, ":", &pch);
			}
		}
	}
	free(result);

	/*
	 * test c_zht_remove
	 * */
	int rret = c_zht_remove(key);
	fprintf(stdout, "c_zht_remove, return code: %d\n", rret);
}
