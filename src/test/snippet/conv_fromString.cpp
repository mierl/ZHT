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
 * conv_fromString.cpp
 *
 *  Created on: Sep 2, 2012
 *      Author: tony, xiaobingo
 */

#include "conv_fromString.h"

conv_fromString::conv_fromString() {

}

conv_fromString::~conv_fromString() {
}

#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <typeinfo>
using namespace std;

template<class KTYPE>
KTYPE template_foo(string key) {

	if (typeid(int) == typeid(KTYPE)) {
		int i;
		sscanf(key.c_str(), "%d", &i);
		return i;
	} else if (typeid(string) == typeid(KTYPE)) {
		return key;
	} else {
		return key;
	}
}

int foo(string key) {

	int i;
	sscanf(key.c_str(), "%d", &i);
	return i;
}



int main(int argc, char **argv) {

	int i = 10;

//	cout << template_foo<int>("101") << endl;
//	cout << template_foo<string>("101") << endl;

//	coo_foo();
}

