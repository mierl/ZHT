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
 * set_test.cpp
 *
 *  Created on: Sep 3, 2012
 *      Author: tony, xiaobingo
 */

#include "set_test.h"
#include <set>
#include <iostream>
#include <stdlib.h>

using namespace std;

SetTest::SetTest() {

}

SetTest::~SetTest() {
}

uint64_t SetTest::findInSet(const uint64_t& uuid) const {

	set<uint64_t> myset;
	myset.insert(1);
	myset.insert(2);
	myset.insert(3);
	myset.insert(4);

	uint64_t token = 0;

	if (myset.count(uuid) > 0) {

		set<uint64_t>::iterator it = myset.find(uuid);
		it++;

		if (it == myset.end()) {

			it = myset.begin();
			token = *it;

		} else {

			token = *(it);
		}
	}

	return token;
}

void foo() {

	set<int> myset;
	set<int>::iterator it;

	// set some initial values:
	for (int i = 1; i <= 5; i++)
		myset.insert(i * 10); // set: 10 20 30 40 50

	it = myset.find(60);
	cout << *it << endl;

	it = myset.find(70);
	cout << *it << endl;

	it = myset.find(6);
	cout << *it << endl;

	it = myset.find(10);
	cout << *it << endl;
}

int main(int argc, char** argv) {

	SetTest st;

	cout << st.findInSet(1) << endl;
	cout << st.findInSet(2) << endl;
	cout << st.findInSet(3) << endl;
	cout << st.findInSet(4) << endl;
	cout << st.findInSet(5) << endl;
}

