/*
 #include "../inc/c_zhtclient.h"
 #include "../inc/c_zhtclientStd.h"
 */

#include "c_zhtclient.h"
#include "c_zhtclientStd.h"

ZHTClient_c zhtClient;

int c_zht_init(const char *memberConfig, const char *zhtConfig, bool tcp) {

	return c_zht_init_std(&zhtClient, memberConfig, zhtConfig, tcp);
}

int c_zht_insert(const char *pair) {

	return c_zht_insert_std(zhtClient, pair);
}

int c_zht_insert2(const char *key, const char *value) {

	return c_zht_insert2_std(zhtClient, key, value);
}

int c_zht_lookup(const char *pair, char *result, size_t *n) {

	return c_zht_lookup_std(zhtClient, pair, result, n);
}

int c_zht_lookup2(const char *key, char *result, size_t *n) {

	return c_zht_lookup2_std(zhtClient, key, result, n);
}

int c_zht_remove(const char *pair) {

	return c_zht_remove_std(zhtClient, pair);
}

int c_zht_remove2(const char *key) {

	return c_zht_remove2_std(zhtClient, key);
}

int c_zht_teardown() {

	return c_zht_teardown_std(zhtClient);
}

