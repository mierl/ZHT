/*
 #include "../inc/c_zhtclientStd.h"
 #include "../inc/cpp_zhtclient.h"
 */

#include "c_zhtclientStd.h"
#include "cpp_zhtclient.h"
#include <string.h>
using namespace std;

bool TCP = false;

int c_zht_init_std(ZHTClient_c * zhtClient, const char *memberConfig,
		const char *zhtConfig, bool tcp) {

	ZHTClient * zhtcppClient = new ZHTClient();

	string zhtStr(zhtConfig);
	string memberStr(memberConfig);

	if (zhtcppClient->initialize(zhtStr, memberStr, tcp) != 0) {
		printf("Crap! ZHTClient initialization failed, program exits.");

		return -1;
	}

	*zhtClient = (ZHTClient_c) zhtcppClient;

	return 0;
}

int c_zht_lookup_std(ZHTClient_c zhtClient, const char *pair, char *result,
		size_t *n) {

	ZHTClient *zhtcppClient = (ZHTClient *) zhtClient;

	string sPair(pair);

	string resultStr;
	int ret = zhtcppClient->lookup(sPair, resultStr);

	/*
	 * hello,zht:hello,zht ==> hello,zht:zht
	 * */
	string store;
	char * pch, *sp;
	pch = strtok_r((char*) resultStr.c_str(), ":", &sp);
	Package package2;

	while (pch != NULL) {

		package2.ParseFromString(pch);
		string strRealfullpath = package2.realfullpath();

		store.append(strRealfullpath);
		store.append(":");

		pch = strtok_r(NULL, ":", &sp);
	}

	size_t found = store.find_last_of(":");
	store = store.substr(0, found);
	package2.set_realfullpath(store);

	store = package2.SerializeAsString();
	strncpy(result, store.c_str(), strlen(store.c_str()));
	*n = store.size();

	return ret;
}

int c_zht_lookup2_std(ZHTClient_c zhtClient, const char *key, char *result,
		size_t *n) {

	ZHTClient *zhtcppClient = (ZHTClient *) zhtClient;

	string sKey(key);

	Package package;
	package.set_virtualpath(sKey); //as key
	package.set_isdir(true);
	package.set_replicano(5);
	package.set_operation(1); //1 for look up, 2 for remove, 3 for insert, 4 append

	string resultStr;
	int ret = zhtcppClient->lookup(package.SerializeAsString(), resultStr);

	/*
	 * hello,zht:hello,zht ==> hello,zht:zht
	 * */
	string store;
	char * pch, *sp;
	pch = strtok_r((char*) resultStr.c_str(), ":", &sp);
	Package package2;

	while (pch != NULL) {

		package2.ParseFromString(pch);
		string strRealfullpath = package2.realfullpath();

		store.append(strRealfullpath);
		store.append(":");

		pch = strtok_r(NULL, ":", &sp);
	}

	size_t found = store.find_last_of(":");
	store = store.substr(0, found);

	strncpy(result, store.c_str(), strlen(store.c_str()));
	*n = store.size();

	return ret;
}

int c_zht_remove_std(ZHTClient_c zhtClient, const char *pair) {

	ZHTClient * zhtcppClient = (ZHTClient *) zhtClient;

	string str(pair);

	return zhtcppClient->remove(str);
}

int c_zht_remove2_std(ZHTClient_c zhtClient, const char *key) {

	ZHTClient * zhtcppClient = (ZHTClient *) zhtClient;

	string sKey(key);

	Package package;
	package.set_virtualpath(sKey);
	package.set_operation(2); //1 for look up, 2 for remove, 3 for insert, 4 append

	return zhtcppClient->remove(package.SerializeAsString());
}

int c_zht_insert_std(ZHTClient_c zhtClient, const char *pair) {

	ZHTClient * zhtcppClient = (ZHTClient *) zhtClient;

	string str(pair);

	return zhtcppClient->insert(str);
}

int c_zht_insert2_std(ZHTClient_c zhtClient, const char *key,
		const char *value) {

	ZHTClient * zhtcppClient = (ZHTClient *) zhtClient;

	string sKey(key);
	string sValue(value);

	Package package;
	package.set_virtualpath(sKey); //as key
	package.set_isdir(true);
	package.set_replicano(5);
	package.set_operation(3); //1 for look up, 2 for remove, 3 for insert, 4 append
	if (!sValue.empty())
		package.set_realfullpath(sValue);

	return zhtcppClient->insert(package.SerializeAsString());
}

int c_zht_append_std(ZHTClient_c zhtClient, const char *pair) {

	ZHTClient * zhtcppClient = (ZHTClient *) zhtClient;

	string str(pair);

	return zhtcppClient->append(str);
}

int c_zht_append2_std(ZHTClient_c zhtClient, const char *key,
		const char *value) {

	ZHTClient * zhtcppClient = (ZHTClient *) zhtClient;

	string sKey(key);
	string sValue(value);

	Package package;
	package.set_virtualpath(sKey); //as key
	package.set_isdir(true);
	package.set_replicano(5);
	package.set_operation(4); //1 for look up, 2 for remove, 3 for insert, 4 append
	if (!sValue.empty())
		package.set_realfullpath(sValue);

	return zhtcppClient->append(package.SerializeAsString());

}

int c_zht_teardown_std(ZHTClient_c zhtClient) {

	ZHTClient * zhtcppClient = (ZHTClient *) zhtClient;

	return zhtcppClient->tearDownTCP();
}

