#include   <stdbool.h>
#include   <stdlib.h>
#include   <stdio.h>

#include   <string.h>
#include "c_zhtclient.h"
#include "meta.pb-c.h"

const int LOOKUP_SIZE = 65535;

void test_large_keyvalue();
void test_common_usecase();
void test_pass_package();

int main(int argc, char **argv) {

	if (argc < 4) {

		printf("Usage: %s", "###.exe neighbor zht.cfg TCP\n");

		return -1;
	}

	bool useTCP = false;
	char *tcpFlag = argv[3];

	if (!strcmp("TCP", tcpFlag)) {
		useTCP = true;
	} else {
		useTCP = false;
	}

	c_zht_init(argv[1], argv[2], useTCP); //neighbor zht.cfg TCP

//	test_large_keyvalue();

//	test_common_usecase();

	test_pass_package();

	c_zht_teardown();

	return 0;
}

void test_common_usecase() {

	const char *key = "hello";
//	const char *key = "nonexistent_key";
	const char *value = "zht";

	/*
	 * test c_zht_insert
	 * */
	int iret = c_zht_insert2(key, value); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_insert, return code: %d\n", iret);

	/*
	 * test c_zht_lookup
	 * */
	size_t ln;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL) {

		int lret = c_zht_lookup2(key, result, &ln); //1 for look up, 2 for remove, 3 for insert, 4 for append

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);
		fprintf(stdout, "c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
				key, result);
	}

	free(result);

	/*

	 * test c_zht_remove
	 *
	 int rret = c_zht_remove2(key); //1 for look up, 2 for remove, 3 for insert, 4 for append
	 fprintf(stdout, "c_zht_remove, return code: %d\n", rret);
	 */

	/*
	 * test c_zht_append
	 * */
	int aret = c_zht_append2(key, value); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_append, return code: %d\n", aret);

	/*
	 * test c_zht_lookup again
	 * */
	ln = 0;
	result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL) {

		int lret = c_zht_lookup2(key, result, &ln); //1 for look up, 2 for remove, 3 for insert, 4 for append

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);
		fprintf(stdout, "c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
				key, result);
	}

	free(result);

}

void test_large_keyvalue() {

	const char *key = "keyofLargeValue";
	char value[10240] = { '\0' };
	memset(value, '1', sizeof(value) - 1);

	/*
	 * test c_zht_insert
	 * */
	int iret = c_zht_insert2(key, value); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_insert, return code: %d\n", iret);

	/*
	 * test c_zht_lookup
	 * */
	size_t ln;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL) {

		int lret = c_zht_lookup2(key, result, &ln); //1 for look up, 2 for remove, 3 for insert, 4 for append

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);
		fprintf(stdout, "c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
				key, result);
	}

	free(result);

	/*
	 * test c_zht_remove
	 *
	 int rret = c_zht_remove2(key); //1 for look up, 2 for remove, 3 for insert, 4 for append
	 fprintf(stdout, "c_zht_remove, return code: %d\n", rret);*/

	/*
	 * test c_zht_append
	 * */
	int aret = c_zht_append2(key, value); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_append, return code: %d\n", aret);

	/*
	 * test c_zht_lookup again
	 * */
	ln = 0;
	result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL) {

		int lret = c_zht_lookup2(key, result, &ln); //1 for look up, 2 for remove, 3 for insert, 4 for append

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);
		fprintf(stdout, "c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
				key, result);
	}

	free(result);
}

void test_pass_package() {

	char *key = "hello";
	char *value = "zht";

	Package package = PACKAGE__INIT; // Package
	package.virtualpath = key;
	package.realfullpath = value;
	package.has_isdir = true;
	package.isdir = false;
	package.has_operation = true;
	package.operation = 3; //1 for look up, 2 for remove, 3 for insert, 4 for append

	char *buf; // Buffer to store serialized data
	unsigned len; // Length of serialized data

	len = package__get_packed_size(&package);
	buf = (char*) calloc(len, sizeof(char));
	package__pack(&package, buf);

	/*
	 * test c_zht_insert
	 * */
	int iret = c_zht_insert(buf);
	fprintf(stdout, "c_zht_insert, return code: %d\n", iret);

	/*
	 * test c_zht_lookup
	 * */
	memset(buf, 0, len);
	package.operation = 1; //1 for look up, 2 for remove, 3 for insert, 4 for append
	package.realfullpath = "";
	package__pack(&package, buf);

	size_t ln;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL) {

		int lret = c_zht_lookup(buf, result, &ln);
		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);

		if (lret == 0 && ln > 0) {

			Package *lPackage;
			char *lBuf = (char*) calloc(ln, sizeof(char));

			strncpy(lBuf, result, ln);
			lPackage = package__unpack(NULL, ln, lBuf);

			if (lPackage == NULL) {

				fprintf(stdout, "error unpacking lookup result\n");

			} else {

				fprintf(stdout,
						"c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
						lPackage->virtualpath, lPackage->realfullpath);

			}

			free(lBuf);
			package__free_unpacked(lPackage, NULL);
		}
	}

	free(result);

	/*
	 * test c_zht_remove
	 *
	 memset(buf, 0, len);
	 package.operation = 2; //1 for look up, 2 for remove, 3 for insert, 4 for append
	 package__pack(&package, buf);

	 int rret = c_zht_remove(buf);
	 fprintf(stdout, "c_zht_remove, return code: %d\n", rret);*/

	/*
	 * test c_zht_append
	 * */
	memset(buf, 0, len);
	package.operation = 4; //1 for look up, 2 for remove, 3 for insert, 4 for append
	package.realfullpath = value;
	len = package__get_packed_size(&package);
	buf = (char*) calloc(len, sizeof(char));
	package__pack(&package, buf);

	int aret = c_zht_append(buf);
	fprintf(stdout, "c_zht_append, return code: %d\n", aret);

	/*
	 * test c_zht_lookup again
	 * */
	memset(buf, 0, len);
	package.operation = 1; //1 for look up, 2 for remove, 3 for insert, 4 for append
	package.realfullpath = "";
	package__pack(&package, buf);

	ln = 0;
	result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL) {

		int lret = c_zht_lookup(buf, result, &ln);
		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);

		if (lret == 0 && ln > 0) {

			Package *lPackage;
			char *lBuf = (char*) calloc(ln, sizeof(char));

			strncpy(lBuf, result, ln);
			lPackage = package__unpack(NULL, ln, lBuf);

			if (lPackage == NULL) {

				fprintf(stdout, "error unpacking lookup result\n");

			} else {

				fprintf(stdout,
						"c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
						lPackage->virtualpath, lPackage->realfullpath);

			}

			free(lBuf);
			package__free_unpacked(lPackage, NULL);
		}
	}

	free(result);

	free(buf); // Free the allocated serialized buffer
}
