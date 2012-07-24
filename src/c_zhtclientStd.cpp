#include "../inc/c_zhtclientStd.h"
#include "../inc/cpp_zhtclient.h"
#include <string.h>

bool TCP = false;

/*
 * to be removed.
 */
void test_proto_buffer(const char *key, const char *value) {
	Package package;
	package.set_virtualpath(key); //as key
	package.set_isdir(true);
	package.set_replicano(5);
	package.set_operation(3); //1 for look up, 2 for remove, 3 for insert
	package.set_realfullpath(value);

	fprintf(stderr, "package virtualpath is: %s\n",
			package.virtualpath().c_str());

	const char * realfullpath = package.realfullpath().c_str();
	fprintf(stderr, "package realfullpath is: %s\n", realfullpath);

	string str = package.SerializeAsString();
	int len = strlen(str.c_str());

	Package package2;
	package2.ParseFromString(str);
	string str2 = package2.SerializeAsString();

	fprintf(stderr, "package2 virtualpath is: %s\n",
			package2.virtualpath().c_str());

	const char *realfullpath2 = package2.realfullpath().c_str();
	fprintf(stderr, "package2 realfullpath is: %s\n", realfullpath2);

}

int c_zht_init_std(ZHTClient_c * zhtClient, const char *memberConfig, const char *zhtConfig, bool tcp) {

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

int c_zht_insert2_std(ZHTClient_c zhtClient, const char *key, const char *value) {

	ZHTClient * zhtcppClient = (ZHTClient *) zhtClient;

	string keyStr(key);
	string valueStr(value);

	if (keyStr.empty()) //empty key not allowed.
		return -1;

	Package package;
	package.set_virtualpath(keyStr); //as key
	package.set_isdir(true);
	package.set_replicano(5);
	package.set_operation(3); //1 for look up, 2 for remove, 3 for insert
	if (!valueStr.empty())
		package.set_realfullpath(valueStr);

	return zhtcppClient->insert(package.SerializeAsString());

}

int c_zht_lookup_std(ZHTClient_c zhtClient, const char *pair, char *result) {

	ZHTClient * zhtcppClient = (ZHTClient *) zhtClient;

	string pkg(pair);
	string returnStr;

	int ret = zhtcppClient->lookup(pkg, returnStr);

	char *chars = new char[returnStr.size() + 1];
	strcpy(chars, returnStr.c_str());

	result = chars;

	return ret;
}

int c_zht_lookup2_std(ZHTClient_c zhtClient, const char *key, char *result, size_t *n) {

	ZHTClient * zhtcppClient = (ZHTClient *) zhtClient;

	string keyStr(key);
	string resultStr;

	if (keyStr.empty()) //empty key not allowed.
		return -1;

	Package package;
	package.set_virtualpath(keyStr); //as key
	package.set_isdir(true);
	package.set_replicano(5);
	package.set_operation(1); //1 for look up, 2 for remove, 3 for insert

	int ret = zhtcppClient->lookup(package.SerializeAsString(), resultStr);

	Package package2;
	package2.ParseFromString(resultStr);
	string strRealfullpath = package2.realfullpath();
	strncpy(result, strRealfullpath.c_str(), strlen(result));
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

	string keyStr(key);

	if (keyStr.empty()) //empty key not allowed.
		return -1;

	Package package;
	package.set_virtualpath(keyStr);
	package.set_operation(2); //1 for look up, 2 for remove, 3 for insert

	return zhtcppClient->remove(package.SerializeAsString());
}

int c_zht_teardown_std(ZHTClient_c zhtClient) {

	ZHTClient * zhtcppClient = (ZHTClient *) zhtClient;

	return zhtcppClient->tearDownTCP();
}

