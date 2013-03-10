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
 * vector_array.cpp
 *
 *  Created on: Aug 8, 2012
 *      Author: tony, xiaobingo
 */

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include <vector>
using namespace std;

void cfunction(char **ccArray, const int& length) {

	int size = sizeof(ccArray);
	cout << "in cfunction: " << size << endl;

	for (int i = 0; i < length; i++) {
		printf("%u, %s\n", i, ccArray[i]);
		ccArray[i] = (char*)"dummy";
	}

	for (int i = 0; i < length; i++) {
		printf("%u, %s\n", i, ccArray[i]);
	}
}

template<typename T, size_t N>
void foo(const T (&t)[N]) {
	std::cout << "array ref" << std::endl;
}

string randomString(const int& len) {

	string s(len, ' ');

	static const char alphanum[] = "0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	return s;
}

void buildVector(vector<char*> &cArray, const int& size) {

	cArray.reserve(size);
//	cArray.resize(size, "");

	for (int i = 0; i < size; i++) {

		string str = randomString(15);
		char *pc = new char[str.size() + 1];
		strcpy(pc, str.c_str());

		cArray.push_back(pc);
	}

}

int main(int argc, char **argv) {

	vector<char*> cArray;

	buildVector(cArray, 2);

	cout << "in main: " << cArray.size() << endl;

	//	cfunction(cArray);

	cfunction(&cArray[0], cArray.size());

	cout << "in main..." << endl;
	for (int i = 0; i < cArray.size(); i++) {
		printf("%u, %s\n", i, cArray[i]);
	}

	/*
	 for (int i = 0; i < cArray.size(); i++)
	 delete[] cArray[i];
	 */

}

