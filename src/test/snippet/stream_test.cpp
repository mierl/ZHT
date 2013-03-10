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
 * stream_test.cpp
 *
 *  Created on: Sep 21, 2012
 *      Author: tony, xiaobingo
 */

#include "stream_test.h"

StreamTest::StreamTest() {
}

StreamTest::~StreamTest() {
}

#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
using namespace std;

int main(int argc, char** argv) {

	string result =
			"12345671, 192.168.1.1, 5001 : 101, 1\t12345672, 192.168.1.2, 5002 : 102, 2\t12345673, 192.168.1.3, 5003 : 103, 3\t";
	stringstream ss(result);

	int i = 0;

	/*	while (!ss.eof()) {

	 string value;

	 ss >> value;

	 i++;

	 cout << value << endl << endl;

	 }*/

	const char* delimiter = "\t";
	char* token = strtok((char*) result.c_str(), delimiter);

	while (token != NULL) {
		i++;
		cout << token << endl;
		token = strtok(NULL, delimiter);
	}

	cout << "------------------------" << endl;
	cout << i << endl;

	return 0;
}
