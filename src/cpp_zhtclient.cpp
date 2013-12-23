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
 * cpp_zhtclient.cpp
 *
 *  Created on: Sep 16, 2012
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include "cpp_zhtclient.h"

#include  <stdlib.h>
#include <string.h>

#include "zpack.pb.h"
#include "ConfHandler.h"
#include "Env.h"
#include "StrTokenizer.h"

using namespace iit::datasys::zht::dm;

ZHTClient::ZHTClient() :
		_proxy(0), _msg_maxsize(0) {

}

ZHTClient::ZHTClient(const string& zhtConf, const string& neighborConf) {

	init(zhtConf, neighborConf);
}

ZHTClient::~ZHTClient() {

	if (_proxy != NULL) {

		delete _proxy;
		_proxy = NULL;
	}
}

int ZHTClient::init(const string& zhtConf, const string& neighborConf) {

	ConfHandler::initConf(zhtConf, neighborConf);

	_msg_maxsize = Env::get_msg_maxsize();

	_proxy = ProxyStubFactory::createProxy();

	if (_proxy == 0)
		return -1;
	else
		return 0;
}

int ZHTClient::init(const char *zhtConf, const char *neighborConf) {

	string szhtconf(zhtConf);
	string sneighborconf(neighborConf);

	int rc = init(szhtconf, sneighborconf);

	return rc;
}

int ZHTClient::commonOp(const string &opcode, const string &key,
		const string &val, const string &val2, string &result, int lease) {

	if (opcode != Const::ZSC_OPC_LOOKUP && opcode != Const::ZSC_OPC_REMOVE
			&& opcode != Const::ZSC_OPC_INSERT
			&& opcode != Const::ZSC_OPC_APPEND
			&& opcode != Const::ZSC_OPC_CMPSWP
			&& opcode != Const::ZSC_OPC_STCHGCB)
		return Const::toInt(Const::ZSC_REC_UOPC);

	string sstatus = commonOpInternal(opcode, key, val, val2, result, lease);

	int status = Const::ZSI_REC_CLTFAIL;
	if (!sstatus.empty())
		status = Const::toInt(sstatus);

	return status;
}

int ZHTClient::lookup(const string &key, string &result) {

	string val;
	string val2;
	int rc = commonOp(Const::ZSC_OPC_LOOKUP, key, val, val2, result, 1);

	result = extract_value(result);

	return rc;
}

int ZHTClient::lookup(const char *key, char *result) {

	string skey(key);
	string sresult;

	int rc = lookup(skey, sresult);

	strncpy(result, sresult.c_str(), sresult.size() + 1);

	return rc;
}

int ZHTClient::remove(const string &key) {

	string val;
	string val2;
	string result;
	int rc = commonOp(Const::ZSC_OPC_REMOVE, key, val, val2, result, 1);

	return rc;
}

int ZHTClient::remove(const char *key) {

	string skey(key);

	int rc = remove(skey);

	return rc;
}

int ZHTClient::insert(const string &key, const string &val) {

	string val2;
	string result;
	int rc = commonOp(Const::ZSC_OPC_INSERT, key, val, val2, result, 1);

	return rc;
}

int ZHTClient::insert(const char *key, const char *val) {

	string skey(key);
	string sval(val);

	int rc = insert(skey, sval);

	return rc;
}

int ZHTClient::append(const string &key, const string &val) {

	string val2;
	string result;
	int rc = commonOp(Const::ZSC_OPC_APPEND, key, val, val2, result, 1);

	return rc;
}

int ZHTClient::append(const char *key, const char *val) {

	string skey(key);
	string sval(val);

	int rc = append(skey, sval);

	return rc;
}

string ZHTClient::extract_value(const string &returnStr) {

	string val;

	StrTokenizer strtok(returnStr, ":");
	/*
	 * hello,zht:hello,ZHT ==> zht:ZHT
	 * */

	if (strtok.has_more_tokens()) {

		while (strtok.has_more_tokens()) {

			ZPack zpack;
			zpack.ParseFromString(strtok.next_token());

			if (zpack.valnull())
				val.append("");
			else
				val.append(zpack.val());

			val.append(":");
		}

		size_t found = val.find_last_of(":");
		val = val.substr(0, found);

	} else {

		ZPack zpack;
		zpack.ParseFromString(returnStr);

		if (zpack.valnull())
			val = "";
		else
			val = zpack.val();
	}

	return val;
}

int ZHTClient::compare_swap(const string &key, const string &seen_val,
		const string &new_val, string &result) {

	int rc = commonOp(Const::ZSC_OPC_CMPSWP, key, seen_val, new_val, result, 1);

	result = extract_value(result);

	return rc;
}

int ZHTClient::compare_swap(const char *key, const char *seen_val,
		const char *new_val, char *result) {

	string skey(key);
	string sseen_val(seen_val);
	string snew_val(new_val);
	string sresult;

	int rc = compare_swap(skey, sseen_val, snew_val, sresult);

	strncpy(result, sresult.c_str(), sresult.size() + 1);

	return rc;
}

int ZHTClient::state_change_callback(const string &key,
		const string &expected_val, int lease) {

	string val2;
	string result;

	int rc = commonOp(Const::ZSC_OPC_STCHGCB, key, expected_val, val2, result,
			lease);

	return rc;
}

int ZHTClient::state_change_callback(const char *key, const char *expeded_val,
		int lease) {

	string skey(key);
	string sexpeded_val(expeded_val);

	int rc = state_change_callback(skey, sexpeded_val, lease);

	return rc;
}

string ZHTClient::commonOpInternal(const string &opcode, const string &key,
		const string &val, const string &val2, string &result, int lease) {

	ZPack zpack;
	zpack.set_opcode(opcode); //"001": lookup, "002": remove, "003": insert, "004": append, "005", compare_swap
	zpack.set_replicanum(3);

	if (key.empty())
		return Const::ZSC_REC_EMPTYKEY; //-1, empty key not allowed.
	else
		zpack.set_key(key);

	if (val.empty()) {

		zpack.set_val("^"); //coup, to fix ridiculous bug of protobuf! //to debug
		zpack.set_valnull(true);
	} else {

		zpack.set_val(val);
		zpack.set_valnull(false);
	}

	if (val2.empty()) {

		zpack.set_newval("?"); //coup, to fix ridiculous bug of protobuf! //to debug
		zpack.set_newvalnull(true);
	} else {

		zpack.set_newval(val2);
		zpack.set_newvalnull(false);
	}

	zpack.set_lease(Const::toString(lease));

	string msg = zpack.SerializeAsString();

	/*ZPack tmp;
	 tmp.ParseFromString(msg);
	 printf("{%s}:{%s,%s}\n", tmp.key().c_str(), tmp.val().c_str(),
	 tmp.newval().c_str());*/

	char *buf = (char*) calloc(_msg_maxsize, sizeof(char));
	size_t msz = _msg_maxsize;

	/*send to and receive from*/
	_proxy->sendrecv(msg.c_str(), msg.size(), buf, msz);

	/*...parse status and result*/
	string sstatus;

	string srecv(buf);

	if (srecv.empty()) {

		sstatus = Const::ZSC_REC_SRVEXP;
	} else {

		result = srecv.substr(3); //the left, if any, is lookup result or second-try zpack
		sstatus = srecv.substr(0, 3); //status returned, the first three chars, like 001, -98...
	}

	free(buf);
	return sstatus;
}

int ZHTClient::teardown() {

	if (_proxy->teardown())
		return 0;
	else
		return -1;
}
