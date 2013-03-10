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
 * return_byref.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: tony, xiaobingo
 */

#include <iostream>
using namespace std;

int i = 2;

const int& a() {
	return i;
}

class Fancy {
	int x;
public:
	void X(const int& _x) {
		x = _x;
	}
	int& X() {
		return x;
	}

	void print() {
		cout << "in class: " << x << endl;
	}
};

int main(int argc, char **argv) {

	Fancy fancy;

	fancy.X(10);
	fancy.print();

	fancy.X(20);
	fancy.print();

	int n = fancy.X();

	cout << "out class: " << n << endl;

	n = 20;

	cout << "out class: " << n << endl;

	fancy.print();
	cout << "out class: " << n << endl;
}

