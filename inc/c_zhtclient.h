/*
 * Copyright (C) 2010-2015
 * Datasys Lab@Illinois Inititute of Technology
 *
 * This file is part of ZHT library, as a c wrapper of ZHT commmon interfaces, such as initialize, insert,
 * lookup, remove and teardown.
 *
 * Contributor: Tony, Xiaobingo
 */

#ifndef C_ZHTCLIENT_H_
#define C_ZHTCLIENT_H_

#ifdef __cplusplus
# define ZHT_CPP(x) x
#else
# define ZHT_CPP(x)
#endif

ZHT_CPP(extern "C" {)

	/* wrapp C++ ZHTClient::initialize.
	 * return code: 0 if succeeded, or -1 if failed.
	 * */
	int c_zht_init(const char *memberConfig, const char *zhtConfig, bool tcp);

	/* wrapp C++ ZHTClient::insert.
	 * PAIR is expected to be a serializationg string with protocol-buffer-c-binding representation.
	 * KEY: empty key not allowed, if empty, return -1, means failed.
	 * return code: 0 if succeeded, or -1 if failed.
	 * Untested, don't invoke it.
	 * */
	int c_zht_insert(const char *pair);

	/* wrapp C++ ZHTClient::insert.
	 * KEY: empty key not allowed, if empty, return -1, means failed.
	 * VALUE: empty value ignored.
	 * return code: 0 if succeeded, or -1 if failed.
	 * */
	int c_zht_insert2(const char *key, const char *value);

	/* wrapp C++ ZHTClient::lookup.
	 * PAIR is expected to be a serializationg string with protocol-buffer-c-binding representation.
	 * KEY: empty key not allowed, if empty, return -1, means failed.
	 * return code: actual number of bytes being read if succeeded, or -1 if failed.
	 * Untested, don't invoke it.
	 * */
	int c_zht_lookup(const char *pair, char *result);

	/* wrapp C++ ZHTClient::lookup.
	 * KEY: empty key not allowed, if empty, return -1, means failed.
	 * return code: actual number of bytes being read if succeeded, or -1 if failed.
	 * */
	int c_zht_lookup2(const char *key, char **result);

	/* wrapp C++ ZHTClient::remove.
	 * PAIR is expected to be a serializationg string with protocol-buffer-c-binding representation.
	 * KEY: empty key not allowed, if empty, return -1, means failed.
	 * return code: 0 if succeeded, or -1 if failed.
	 * Untested, don't invoke it.
	 * */
	int c_zht_remove(const char *pair);

	/* wrapp C++ ZHTClient::remove.
	 * KEY: empty key not allowed, if empty, return -1, means failed.
	 * return code: 0 if succeeded, or -1 if failed.
	 * */
	int c_zht_remove2(const char *key);

	/* wrapp C++ ZHTClient::teardown.
	 * return code: 0 if succeeded, or -1 if failed.
	 * */
	int c_zht_teardown();

ZHT_CPP	(})

#endif
