/*
 * Copyright (C) 2010-2015
 * Datasys Lab@Illinois Inititute of Technology
 *
 * This file is part of ZHT library, as a c wrapper of ZHT commmon interfaces, such as initialize, insert,
 * lookup, remove and teardown.
 *
 * Contributor: Tony, Xiaobingo, Corentin Debains
 */

#ifndef C_ZHTCLIENTSTD_H_
#define C_ZHTCLIENTSTD_H_

#ifdef __cplusplus
# define ZHT_CPP(x) x
#else
# define ZHT_CPP(x)
#endif

#include <stddef.h>

typedef void* ZHTClient_c;

ZHT_CPP(extern "C" {)

	/* wrapp C++ ZHTClient::initialize.
	 * return code: 0 if succeeded, or -1 if failed.
	 * */
	int c_zht_init_std(ZHTClient_c * zhtClient, const char *memberConfig,
			const char *zhtConfig, bool tcp);

	/* wrapp C++ ZHTClient::insert.
	 * PAIR is expected to be a serialization string with protocol-buffer-c-binding representation.
	 * return code: 0 if succeeded, or -1 if empty key, or -2 if failed, -98 if unrecognized operation.
	 * */
	int c_zht_insert_std(ZHTClient_c zhtClient, const char *pair);

	/* wrapp C++ ZHTClient::insert.
	 * KEY: empty key not allowed, if empty, return -1, means failed.
	 * VALUE: empty value ignored.
	 * return code: 0 if succeeded, or -1 if empty key, or , -98 if unrecognized operation.
	 * */
	int c_zht_insert2_std(ZHTClient_c zhtClient, const char *key,
			const char *value);

	/* wrapp C++ ZHTClient::lookup.
	 * PAIR is expected to be a serialization string with protocol-buffer-c-binding representation.
	 * RESULT: lookup result
	 * N: actual number of characters read.
	 * return code: 0 if succeeded, or -1 if empty key, or , -98 if unrecognized operation.
	 * */
	int c_zht_lookup_std(ZHTClient_c zhtClient, const char *pair, char *result,
			size_t *n);

	/* wrapp C++ ZHTClient::lookup.
	 * KEY: empty key not allowed, if empty, return -1, means failed.
	 * RESULT: lookup result
	 * N: actual number of characters read.
	 * return code: 0 if succeeded, or -1 if empty key, or , -98 if unrecognized operation.
	 * */
	int c_zht_lookup2_std(ZHTClient_c zhtClient, const char *key, char *result,
			size_t *n);

	/* wrapp C++ ZHTClient::remove.
	 * PAIR is expected to be a serialization string with protocol-buffer-c-binding representation.
	 * return code: 0 if succeeded, or -1 if empty key, or , -98 if unrecognized operation.
	 * */
	int c_zht_remove_std(ZHTClient_c zhtClient, const char *pair);

	/* wrapp C++ ZHTClient::remove.
	 * KEY: empty key not allowed, if empty, return -1, means failed.
	 * return code: 0 if succeeded, or -1 if empty key, or , -98 if unrecognized operation.
	 * */
	int c_zht_remove2_std(ZHTClient_c zhtClient, const char *key);

	/* wrapp C++ ZHTClient::teardown.
	 * return code: 0 if succeeded, or -1 if failed.
	 * */
	int c_zht_teardown_std(ZHTClient_c zhtClient);

ZHT_CPP	(})

#endif
