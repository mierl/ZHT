#include   <stdbool.h>
#include   <stdlib.h>
#include   <stdio.h>

#include   <string.h>
#include <c_zhtclientStd.h>

const int LOOKUP_SIZE = 65535;

int main(int argc, char **argv) {

	ZHTClient_c zhtClient;

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

	c_zht_init_std(&zhtClient, argv[1], argv[2], useTCP); //neighbor zht.cfg TCP

	const char *key = "hello";
	const char *value = "zht";

	const char *largeKey = "keyofLargeValue";
	char largeVal[10240] = { '\0' };
	memset(largeVal, '1', sizeof(largeVal) - 1);
	key = largeKey;
	value = largeVal;

	int iret = c_zht_insert2_std(zhtClient, key, value);
	fprintf(stderr, "c_zht_insert, return code: %d\n", iret);

	size_t n;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));
	if (result != NULL) {
		int lret = c_zht_lookup2_std(zhtClient, key, result, &n);
		fprintf(stderr, "c_zht_lookup, return code: %d\n", lret);
		fprintf(stderr, "c_zht_lookup, return value: length(%lu), %s\n", n,
				result);
	}
	free(result);

	int rret = c_zht_remove2_std(zhtClient, key);
	fprintf(stderr, "c_zht_remove, return code: %d\n", rret);

	c_zht_teardown_std(zhtClient);

	return 0;
}
