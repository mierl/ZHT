/*
 * baseChecker.cpp
 *
 *  Created on: Mar 30, 2011
 *      Author: tony
 */

#include <kchashdb.h>
#include <kcprotodb.h>
#include <string>
#include <cstring>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <list>
using namespace std;
using namespace kyotocabinet;

list<string> readFile(string fileName) {
	ifstream inFile(fileName.c_str(), ios::in); //key list file

	string str;
	list < string > keyList;

	inFile >> str;
	while (!inFile.eof()) {
		keyList.push_back(str);
		//	cout <<str<<endl;
		inFile >> str;
	}

	inFile.close();
	return keyList;
}

int main(int argc, char* argv[]) {

	list < string > keyList = readFile(string(argv[1]));
	list < string >::iterator it;
	HashDB db;
	cout<<"Begin to open db..."<<endl;
	if (!db.open(argv[2], HashDB::HashDB::OREADER)) {
		cout << "Faild to database." << endl;
		cerr << "open error: " << db.error().name() << endl;
	}
	cout << "Database opened." << endl;

	int errorNum = 0;
	int size = keyList.size();
	int i=0;
	for (it=keyList.begin(); it != keyList.end(); it++) {
		//	if (db.get(keys[i])==) errorNum++;
		//cout<< db.get(keys[i]) <<endl;
		//value = db.get(keys[9000]);

		cout <<i<<endl;
		if (!db.get(*it)) {
			cout << "found no match." << endl;
			errorNum++;
		}
		i++;
	}
	cout << "found " << size - errorNum << " elements out of " << size << endl;

	if (!db.close()) {
		cerr << "close error: " << db.error().name() << endl;
	}
	return 0;
}
