/*
 #include "../inc/c_zhtclientStd.h"
 #include "../inc/cpp_zhtclient.h"
 */

#include "c_zhtclientStd.h"
#include "cpp_zhtclient.h"
#include <string.h>

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
	package.set_operation(3); //1 for look up, 2 for remove, 3 for insert
	if (!sValue.empty())
		package.set_realfullpath(sValue);

	return zhtcppClient->insert(package.SerializeAsString());

}

int c_zht_lookup_std(ZHTClient_c zhtClient, const char *pair, char *result,
		size_t *n) {

	ZHTClient *zhtcppClient = (ZHTClient *) zhtClient;

	string sPair(pair);

	string resultStr;
	int ret = zhtcppClient->lookup(sPair, resultStr);

	strncpy(result, resultStr.c_str(), strlen(resultStr.c_str()));
	*n = resultStr.size();

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
	package.set_operation(1); //1 for look up, 2 for remove, 3 for insert

	string resultStr;
	int ret = zhtcppClient->lookup(package.SerializeAsString(), resultStr);

	Package package2;
	package2.ParseFromString(resultStr);
	string strRealfullpath = package2.realfullpath();

	strncpy(result, strRealfullpath.c_str(), strlen(strRealfullpath.c_str()));
	*n = strRealfullpath.size();

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
	package.set_operation(2); //1 for look up, 2 for remove, 3 for insert

	return zhtcppClient->remove(package.SerializeAsString());
}

int c_zht_teardown_std(ZHTClient_c zhtClient) {

	ZHTClient * zhtcppClient = (ZHTClient *) zhtClient;

	return zhtcppClient->tearDownTCP();
}

