#include   <stdbool.h>
#include   <stdlib.h>
#include   <stdio.h>

#include   <string.h>
#include "c_zhtclient.h"

const int LOOKUP_SIZE = 65535;

void test_large_keyvalue();
void test_common_usecase();

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

	test_commone_usecase();

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
