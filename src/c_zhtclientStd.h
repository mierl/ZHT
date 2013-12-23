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
 * c_zhtclientStd.h
 *
 *  Created on: Aug 7, 2012
 *      Author: Corentin Debains
 *      Contributor: Xiaobingo, Tony, KWang, DZhao
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
	int c_zht_init_std(ZHTClient_c *zhtClient, const char *zhtConfig,
			const char *neighborConf);

	/* wrapp C++ ZHTClient::lookup.
	 * KEY: empty key not allowed, if empty, return -1, means failed.
	 * RESULT: lookup result
	 * return code: 0 if succeeded, or -1 if empty key, or , -98 if unrecognized operation.
	 * */
	int c_zht_lookup_std(ZHTClient_c zhtClient, const char *key, char *result);

	/* wrapp C++ ZHTClient::remove.
	 * KEY: empty key not allowed, if empty, return -1, means failed.
	 * return code: 0 if succeeded, or -1 if empty key, or , -98 if unrecognized operation.
	 * */
	int c_zht_remove_std(ZHTClient_c zhtClient, const char *key);

	/* wrapp C++ ZHTClient::insert.
	 * KEY: empty key not allowed, if empty, return -1, means failed.
	 * VALUE: empty value ignored.
	 * return code: 0 if succeeded, or -1 if empty key, or , -98 if unrecognized operation.
	 * */
	int c_zht_insert_std(ZHTClient_c zhtClient, const char *key,
			const char *value);

	/* wrapp C++ ZHTClient::append.
	 * KEY: empty key not allowed, if empty, return -1, means failed.
	 * VALUE: empty value ignored.
	 * return code: 0 if succeeded, or -1 if empty key, or , -98 if unrecognized operation.
	 * */
	int c_zht_append_std(ZHTClient_c zhtClient, const char *key,
			const char *value);

	/* wrapp C++ ZHTClient::compare_swapappend.
	 * Return 0(zero), if SEEN_VALUE equals to value lookuped by the key, and set the value to NEW_VALUE
	 * Return non-zero, if the above doesn’t meet, and VALUE_QUERIED returned
	 *
	 * KEY: empty key not allowed, if empty, return -1, means failed.
	 * SEEN_VALUE: value expected to be equal to that lookuped by the key
	 * NEW_VALUE: if equal, set value to NEW_VALUE
	 * VALUE_QUERIE: if equal or not equal, get new value queried
	 *
	 * return code: 0 if succeeded, or -1 if empty key, or -2 if failed, -98 if unrecognized operation.
	 * */
	int c_zht_compare_swap_std(ZHTClient_c zhtClient, const char *key,
			const char *seen_value, const char *new_value, char *value_queried);

	/* wrapp C++ ZHTClient::compare_swapappend.
	 * KEY: empty key not allowed, if empty, return -1, means failed.
	 * EXPECTED_VAL: the value expected to be equal to what is lookuped by the key, if equal, return 0, or keep polling in server-side.
	 * return code: 0 if succeeded, or -1 if empty key, or -2 if failed, -98 if unrecognized operation.
	 * */
	int c_state_change_callback_std(ZHTClient_c zhtClient, const char *key,
			const char *expected_val, int lease);

	/* wrapp C++ ZHTClient::teardown.
	 * return code: 0 if succeeded, or -1 if failed.
	 * */
	int c_zht_teardown_std(ZHTClient_c zhtClient);

	ZHT_CPP (})

#endif
