#include "c_zhtclient.h"
#include "cpp_zhtclient.h"
#include <string.h>

ZHTClient zhtClient;
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

int c_zht_init(const char *memberConfig, const char *zhtConfig, bool tcp) {

	string zhtStr(zhtConfig);
	string memberStr(memberConfig);

	if (zhtClient.initialize(zhtStr, memberStr, tcp) != 0) {
		printf("Crap! ZHTClient initialization failed, program exits.");

		return -1;
	}

	return 0;
}

int c_zht_insert(const char *pair) {

	string str(pair);

	return zhtClient.insert(str);
}

int c_zht_insert2(const char *key, const char *value) {

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

	return zhtClient.insert(package.SerializeAsString());

}

int c_zht_lookup(const char *pair, char *result) {

	string pkg(pair);
	string returnStr;

	int ret = zhtClient.lookup(pkg, returnStr);

	char *chars = new char[returnStr.size() + 1];
	strcpy(chars, returnStr.c_str());

	result = chars;

	return ret;
}

int c_zht_lookup2(const char *key, char *result, size_t *n) {

	string keyStr(key);
	string resultStr;

	if (keyStr.empty()) //empty key not allowed.
		return -1;

	Package package;
	package.set_virtualpath(keyStr); //as key
	package.set_isdir(true);
	package.set_replicano(5);
	package.set_operation(1); //1 for look up, 2 for remove, 3 for insert

	int ret = zhtClient.lookup(package.SerializeAsString(), resultStr);

	Package package2;
	package2.ParseFromString(resultStr);
	string strRealfullpath = package2.realfullpath();
	strncpy(result, strRealfullpath.c_str(), strRealfullpath.size());
	*n = strRealfullpath.size();

	return ret;
}

int c_zht_remove(const char *pair) {

	string str(pair);

	return zhtClient.remove(str);
}

int c_zht_remove2(const char *key) {

	string keyStr(key);

	if (keyStr.empty()) //empty key not allowed.
		return -1;

	Package package;
	package.set_virtualpath(keyStr);
	package.set_operation(2); //1 for look up, 2 for remove, 3 for insert

	return zhtClient.remove(package.SerializeAsString());
}

int c_zht_teardown() {

	return zhtClient.tearDownTCP();
}

