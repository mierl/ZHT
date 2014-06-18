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
 * c_zhtclient.cpp
 *
 *  Created on: Aug 7, 2012
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include "c_zhtclient.h"
#include "c_zhtclientStd.h"
#include <pthread.h>

ZHTClient_c zhtClient_c;

int c_zht_init(const char *zhtConfig, const char *neighborConf) {

	return c_zht_init_std(&zhtClient_c, zhtConfig, neighborConf);
}

int c_zht_lookup(const char *key, char *result) {

	return c_zht_lookup_std(zhtClient_c, key, result);
}

int c_zht_remove(const char *key) {

	return c_zht_remove_std(zhtClient_c, key);
}

int c_zht_insert(const char *key, const char *value) {

	return c_zht_insert_std(zhtClient_c, key, value);
}

int c_zht_append(const char *key, const char *value) {

	return c_zht_append_std(zhtClient_c, key, value);
}

int c_zht_compare_swap(const char *key, const
char *seen_value, const char *new_value, char *value_queried) {

	return c_zht_compare_swap_std(zhtClient_c, key, seen_value, new_value,
			value_queried);
}

int c_state_change_callback(const char *key, const char *expected_val,
		int lease) {

	return c_state_change_callback_std(zhtClient_c, key, expected_val, lease);
}

int c_zht_teardown() {

	return c_zht_teardown_std(zhtClient_c);
}

