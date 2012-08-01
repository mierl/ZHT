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

	int iret = c_zht_insert2(key, value);
	fprintf(stderr, "c_zht_insert, return code: %d\n", iret);

	size_t n;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));
	if (result != NULL) {
		int lret = c_zht_lookup2(key, result, &n);
		fprintf(stderr, "c_zht_lookup, return code: %d\n", lret);
		fprintf(stderr, "c_zht_lookup, return value: length(%lu), %s\n", n,
				result);
	}
	free(result);

	int rret = c_zht_remove2(key);
	fprintf(stderr, "c_zht_remove, return code: %d\n", rret);

}

void test_large_keyvalue() {
	const char *key = "keyofLargeValue";
	char value[10240] = { '\0' };
	memset(value, '1', sizeof(value) - 1);

	int iret = c_zht_insert2(key, value);
	fprintf(stderr, "c_zht_insert, return code: %d\n", iret);

	size_t n;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));
	if (result != NULL) {
		int lret = c_zht_lookup2(key, result, &n);
		fprintf(stderr, "c_zht_lookup, return code: %d\n", lret);
		fprintf(stderr, "c_zht_lookup, return value: length(%lu), %s\n", n,
				result);
	}
	free(result);

	int rret = c_zht_remove2(key);
	fprintf(stderr, "c_zht_remove, return code: %d\n", rret);
}

void test_pass_package() {

	char *key = "hello";
	char *value = "zht";

	Package package = PACKAGE__INIT; // Package
	package.virtualpath = key;
	package.realfullpath = value;
	package.has_isdir = true;
	package.isdir = true;
	package.has_operation = true;
	package.operation = 3; //1 for look up, 2 for remove, 3 for insert

	char *buf; // Buffer to store serialized data
	unsigned len; // Length of serialized data

	len = package__get_packed_size(&package);
	buf = (char*) calloc(len, sizeof(char));
	package__pack(&package, buf);

	/*
	 * test c_zht_insert
	 * */
	int iret = c_zht_insert(buf);
	fprintf(stderr, "c_zht_insert, return code: %d\n", iret);

	/*
	 * test c_zht_lookup
	 * */
	memset(buf, 0, len);
	package.operation = 1; //1 for look up, 2 for remove, 3 for insert
	package.realfullpath = "";
	package__pack(&package, buf);

	size_t ln;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));
	if (result != NULL) {
		int lret = c_zht_lookup(buf, result, &ln);
		fprintf(stderr, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);

		if (lret == 0 && ln > 0) {
			Package *lkPackage;
			char *lkBuf = (char*) calloc(ln, sizeof(char));

			strncpy(lkBuf, result, ln);
			lkPackage = package__unpack(NULL, ln, lkBuf);

			if (lkPackage == NULL) {
				fprintf(stderr, "error unpacking lookup result\n");
			} else {
				fprintf(stderr,
						"c_zht_lookup, return {key}:{value} ==>\n{%s}:{%s}\n",
						lkPackage->virtualpath, lkPackage->realfullpath);
			}

			free(lkBuf);
			package__free_unpacked(lkPackage, NULL);
		}
	}
	free(result);

	/*
	 * test c_zht_remove
	 * */
	memset(buf, 0, len);
	package.operation = 2; //1 for look up, 2 for remove, 3 for insert
	package__pack(&package, buf);

	int rret = c_zht_remove(buf);
	fprintf(stderr, "c_zht_remove, return code: %d\n", rret);

	free(buf); // Free the allocated serialized buffer
}
