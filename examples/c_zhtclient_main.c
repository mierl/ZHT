#include   <stdbool.h>
#include   <stdlib.h>
#include   <stdio.h>

#include   <string.h>
#include "c_zhtclient.h"
#include "meta.pb-c.h"

const int LOOKUP_SIZE = 1024 * 10000 * 7; //size of buffer to store lookup result, larger enough than TOTAL_SIZE
//const int LOOKUP_SIZE = 16 * 3; //size of buffer to store lookup result, larger enough than TOTAL_SIZE

const int TOTAL_SIZE = 1024 * 10000 * 6; //total size of a message to be transfered
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

	test_large_keyvalue();

//	test_common_usecase();

//	test_pass_package();

//	test_simple_largevalue();

	c_zht_teardown();

	return 0;
}

void test_simple_largevalue() {

	const char *key = "keyofLargeValue";
//	const char *value = "xiaobingo";

	char *value2 = calloc(TOTAL_SIZE, sizeof(char));
	memset(value2, '1', TOTAL_SIZE - 1);

	/*
	 * test c_zht_insert
	 * */
	int iret = c_zht_insert2(key, value2); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_insert, return code: %d\n", iret);

	/*
	 * test c_zht_lookup
	 * */
	size_t ln;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL ) {

		int lret = c_zht_lookup2(key, result, &ln); //1 for look up, 2 for remove, 3 for insert, 4 for append

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);
		/*	fprintf(stdout, "c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
		 key, result);*/
	}

	free(result);

	/*
	 * test c_zht_append
	 * */
	int aret = c_zht_append2(key, "value appended"); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_append, return code: %d\n", aret);

	/*
	 * test c_zht_remove
	 * */
	int rret = c_zht_remove2(key); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_remove, return code: %d\n", rret);

	free(value2);
}

void test_common_usecase() {

	test_common_usecase_blankspace();

	fprintf(stdout, "%s\n", "--------------------------");

	test_common_usecase_emptystring();
}

void test_common_usecase_blankspace() {

	const char *key = "hello";
	const char *value = " ";
	const char *value2 = "zht";

	test_common_usecase_reuse(key, value, value2);
}

void test_common_usecase_emptystring() {

	const char *key = "hello";
	const char *value = "";
	const char *value2 = "ZHT";

	test_common_usecase_reuse(key, value, value2);
}

void test_common_usecase_reuse(const char * const key, const char * const value,
		const char * const value2) {

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

	if (result != NULL ) {

		int lret = c_zht_lookup2(key, result, &ln); //1 for look up, 2 for remove, 3 for insert, 4 for append

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);
		fprintf(stdout, "c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
				key, result);
	}

	free(result);

	/*
	 * test c_zht_append
	 * */
	int aret = c_zht_append2(key, value2); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_append, return code: %d\n", aret);

	/*
	 * test c_zht_lookup again
	 * */
	ln = 0;
	result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL ) {

		int lret = c_zht_lookup2(key, result, &ln); //1 for look up, 2 for remove, 3 for insert, 4 for append

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);
		fprintf(stdout, "c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
				key, result);
	}

	free(result);

	/*
	 * test c_zht_remove
	 * */
	int rret = c_zht_remove2(key); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_remove, return code: %d\n", rret);
}

void test_large_keyvalue() {

	test_large_keyvalue_blankspace();

	fprintf(stdout, "%s\n", "--------------------------");

	test_large_keyvalue_emptystring();
}

void test_large_keyvalue_blankspace() {

	const char *key = "keyofLargeValue";
	const char *value = " ";

	char *value2 = calloc(TOTAL_SIZE, sizeof(char));
	memset(value2, '1', TOTAL_SIZE - 1);

	test_large_keyvalue_reuse(key, value, value2);

	free(value2);

}

void test_large_keyvalue_emptystring() {

	const char *key = "keyofLargeValue";
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
	int iret = c_zht_insert2(key, value); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_insert, return code: %d\n", iret);

	/*
	 * test c_zht_lookup
	 * */
	size_t ln;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL ) {

		int lret = c_zht_lookup2(key, result, &ln); //1 for look up, 2 for remove, 3 for insert, 4 for append

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);
		/*fprintf(stdout, "c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
		 key, result);*/
	}

	free(result);

	/*
	 * test c_zht_append
	 * */
	int aret = c_zht_append2(key, value2); //1 for look up, 2 for remove, 3 for insert, 4 for append
	fprintf(stdout, "c_zht_append, return code: %d\n", aret);

	/*
	 * test c_zht_lookup again
	 * */
	ln = 0;
	result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL ) {

		int lret = c_zht_lookup2(key, result, &ln); //1 for look up, 2 for remove, 3 for insert, 4 for append

		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);
		/*fprintf(stdout, "c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
		 key, result);*/
	}

	free(result);

	/*
	 * test c_zht_remove
	 */
	int rret = c_zht_remove2(key); //1 for look up, 2 for remove, 3 for insert, 4 for append
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

void test_pass_package_reuse(const char * const key, const char * const value,
		const char * const value2) {

	char *buf; // Buffer to store serialized data
	unsigned len; // Length of serialized data

	Package package = PACKAGE__INIT; // Package
	package.virtualpath = (char*) key;
	if (strcmp(value, "") != 0) //tricky: bypass protocol-buf's bug
		package.realfullpath = (char*) value;
	package.has_isdir = true;
	package.isdir = false;
	package.has_operation = true;
	package.operation = 3; //1 for look up, 2 for remove, 3 for insert, 4 for append

	len = package__get_packed_size(&package);
	buf = (char*) calloc(len, sizeof(char));
	package__pack(&package, buf);

	/*
	 * test c_zht_insert
	 * */
	int iret = c_zht_insert(buf);
	fprintf(stdout, "c_zht_insert, return code: %d\n", iret);
	free(buf);

	/*
	 * test c_zht_lookup
	 * */
	Package package2 = PACKAGE__INIT;
	package2.virtualpath = (char*) key;
	package2.operation = 1; //1 for look up, 2 for remove, 3 for insert, 4 for append

	len = package__get_packed_size(&package2);
	buf = (char*) calloc(len, sizeof(char));
	package__pack(&package2, buf);

	size_t ln;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL ) {

		int lret = c_zht_lookup(buf, result, &ln);
		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);

		if (lret == 0 && ln > 0) {

			Package * lPackage;
			char *lBuf = (char*) calloc(ln, sizeof(char));

			strncpy(lBuf, result, ln);
			lPackage = package__unpack(NULL, ln, lBuf);

			if (lPackage == NULL ) {

				fprintf(stdout, "error unpacking lookup result\n");

			} else {

				fprintf(stdout,
						"c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
						lPackage->virtualpath, lPackage->realfullpath);

			}

			free(lBuf);
			package__free_unpacked(lPackage, NULL );
		}
	}

	free(buf);
	free(result);

	/*
	 * test c_zht_append
	 * */
	Package package4 = PACKAGE__INIT;
	package4.virtualpath = (char*) key;
	if (strcmp(value2, "") != 0) //tricky: bypass protocol-buf's bug
		package4.realfullpath = (char*) value2;
	package4.has_isdir = true;
	package4.isdir = false;
	package4.has_operation = true;
	package4.operation = 4; //1 for look up, 2 for remove, 3 for insert, 4 for append

	len = package__get_packed_size(&package4);
	buf = (char*) calloc(len, sizeof(char));
	package__pack(&package4, buf);

	int aret = c_zht_append(buf);
	fprintf(stdout, "c_zht_append, return code: %d\n", aret);
	free(buf);

	/*
	 * test c_zht_lookup again
	 * */
	Package package5 = PACKAGE__INIT; // Package
	package5.virtualpath = (char*) key;
	package5.operation = 1; //1 for look up, 2 for remove, 3 for insert, 4 for append

	len = package__get_packed_size(&package5);
	buf = (char*) calloc(len, sizeof(char));
	package__pack(&package5, buf);

	ln = 0;
	result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL ) {

		int lret = c_zht_lookup(buf, result, &ln);
		fprintf(stdout, "c_zht_lookup, return code(length): %d(%lu)\n", lret,
				ln);

		if (lret == 0 && ln > 0) {

			Package * lPackage;
			char *lBuf = (char*) calloc(ln, sizeof(char));

			strncpy(lBuf, result, ln);
			lPackage = package__unpack(NULL, ln, lBuf);

			if (lPackage == NULL ) {

				fprintf(stdout, "error unpacking lookup result\n");

			} else {

				fprintf(stdout,
						"c_zht_lookup, return {key}:{value} => {%s}:{%s}\n",
						lPackage->virtualpath, lPackage->realfullpath);

			}

			free(lBuf);
			package__free_unpacked(lPackage, NULL );
		}
	}

	free(buf);
	free(result);

	/*
	 * test c_zht_remove
	 * */
	Package package3 = PACKAGE__INIT;
	package3.virtualpath = (char*) key;
	package3.operation = 2; //1 for look up, 2 for remove, 3 for insert, 4 for append

	len = package__get_packed_size(&package3);
	buf = (char*) calloc(len, sizeof(char));
	package__pack(&package3, buf);

	int rret = c_zht_remove(buf);
	fprintf(stdout, "c_zht_remove, return code: %d\n", rret);
	free(buf);
}
