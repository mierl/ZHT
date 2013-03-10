/*
 * test_hash.cpp
 *
 *  Created on: Dec 20, 2011
 *      Author: tony
 */



#include <string>
#include "novoht.h"
#include <stdlib.h>
#include <sys/time.h>
//#include <stdio.h>

//#include "zht_util.h"
using namespace std;


struct timeval tp;

double getTime_usec() {
	gettimeofday(&tp, NULL);
	return static_cast<double>(tp.tv_sec) * 1E6
			+ static_cast<double>(tp.tv_usec);
}
string randomString(int len) {
	string s(len, ' ');
	srand(getpid() + clock() + getTime_usec());
	static const char alphanum[] = "0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	return s;
}

int main(int argc, char * argv[]){

	int num = atoi(argv[1]);
	string fileName = "hashmap.data";
	NoVoHT map = NoVoHT(fileName, 100000, 1000, 0.7);
	int keyLen = 32;
	int valueLen = 96;
	for(int i=0; i<num; i++){
		string key = randomString(keyLen);
		string value = randomString(valueLen);
		map.put(key, value);
	}
	return 0;
}
