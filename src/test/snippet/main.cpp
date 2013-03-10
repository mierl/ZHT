/*
 * Copyright (C) 2010-2020 DatasysLab@iit.edu(http://datasys.cs.iit.edu/index.html).
 *      Director: Ioan Raicu(iraicu@cs.iit.edu)
 *
 * This file is part of ZHT library(http://datasys.cs.iit.edu/projects/ZHT/index.html).
 *      Ioan Raicu(iraicu@cs.iit.edu),
 *      Tonglin Li(tli13@hawk.iit.edu) with nickname tony,
 *      Xiaobing Zhou(xzhou40@hawk.iit.edu) with nickname xiaobingo.
 *
 * The ZHT Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The ZHT Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the ZHT Library; if not, write to the
 * Data-Intensive Distributed Systems Laboratory, 10 W. 31st Street,
 * Stuart Building, Room 003B, Chicago, IL 60616 USA.
 *
 * main.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: tony, xiaobingo
 */

#include <vector>
#include <set>

#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <typeinfo>
#include <map>
#include <string.h>
#include <sstream>
#include <sys/types.h>
#include <stdint.h>

using namespace std;

void foo(const string& arg) {

	cout << arg << endl;
}

void coo_foo() {

	const char *pch = "hello";

	string str("zht");

	foo(pch);
	foo(str);
	foo(string("fuck"));

	string other;
	other = string("damn");

	foo(other);
}

void test_vector() {

	vector<int> myvector(3, 100);
	vector<int>::iterator it;

	it = myvector.begin();
	it = myvector.insert(it, 200);

	myvector.insert(it, 2, 300);

	for (it = myvector.begin(); it < myvector.end(); it++)
		cout << " " << *it;

	// "it" no longer valid, get a new one:
	it = myvector.begin();

	vector<int> anothervector(2, 400);
	myvector.insert(it + 2, anothervector.begin(), anothervector.end());

	int myarray[] = { 501, 502, 503 };
	myvector.insert(myvector.begin(), myarray, myarray + 3);

	cout << "myvector contains:";
	for (it = myvector.begin(); it < myvector.end(); it++)
		cout << " " << *it;
	cout << endl;

}

uint64_t ULL_MAX = (uint64_t) -1;

void strtok_test(const string& membership) {

	const char* delimiter = "\t";
	char* token = strtok((char*) membership.c_str(), delimiter);

	while (token != NULL) {

		cout << token << endl;
		token = strtok(NULL, delimiter);
	}
}

void createFullPath(const string& uuid) {

	//	system("mkdir -p ./tmp/a/b/c");

	string cmd = "mkdir -p ";
	cmd.append(uuid);
	system(cmd.c_str());
}

class Entry {
public:
	Entry() {
	}

	Entry(string key, string value) {

		_key = key;
		_value = value;
	}
	string _key;
	string _value;

	string toString() {
		return _key + " " + _value;
	}
};

typedef map<string, Entry> MAP;
typedef pair<string, Entry> PAIR;
typedef map<string, Entry>::iterator MIT;
typedef map<string, Entry>::reverse_iterator MRIT;

typedef set<int> SET;
typedef set<int>* PSET;
typedef set<int>::iterator ITR;

inline void prtMem(MAP* map) {

	for (MIT it = map->begin(); it != map->end(); it++) {

		fprintf(stdout, "%s ==> %s\n", it->first.c_str(),
				it->second.toString().c_str());
	}
}

string SplitFilename(const string& str) {
	size_t found;
	cout << "Splitting: " << str << endl;
	found = str.find_last_of("/\\");
	/*cout << " folder: " << str.substr(0,found) << endl;
	 cout << " file: " << str.substr(found+1) << endl;*/

	return str.substr(0, found);
}

bool fncomp(int lhs, int rhs) {
	return lhs < rhs;
}

struct classcomp {
	bool operator()(const int& lhs, const int& rhs) const {
		return lhs < rhs;
	}
};

int main() {

	map<char, int> mymap;
	map<char, int>::iterator it;

	mymap['a'] = 50;
	mymap['b'] = 100;
	mymap['c'] = 150;
	mymap['d'] = 200;

	it = mymap.find('b');
	mymap.erase(it);
	mymap.erase(mymap.find('d'));

	// print content:
	cout << "elements in mymap:" << endl;
	cout << "a => " << mymap.find('a')->second << endl;
	cout << "c => " << mymap.find('c')->second << endl;

	it = mymap.find('b');

	if (it == mymap.end())
		cout << "NULL" << endl;
	else
		cout << "NOT NULL" << endl;

//	cout << it->first << endl;
	cout << "b => " << mymap.find('b')->second << endl;

	return 0;

}
