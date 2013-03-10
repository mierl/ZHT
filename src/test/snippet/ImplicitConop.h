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
 * ImplicitConop.h
 *
 *  Created on: Aug 13, 2012
 *      Author: tony, xiaobingo
 */

#ifndef IMPLICITCONOP_H_
#define IMPLICITCONOP_H_

#include <stdio.h>
using namespace std;

struct GMan {
	int a, b;

	/* Side-note: these could be combined:
	 GMan():a(),b(){}
	 GMan(int _a):a(_a),b(){}
	 GMan(int _a, int _b):a(_a),b(_b){}
	 */
	GMan(int _a = 0, int _b = 0) :
			a(_a), b(_b) {
	} // into this

	// first implement the mutating operator
	GMan& operator+=(const GMan& _b) {
		// the use of 'this' to access members
		// is generally seen as noise
		a += _b.a;
		b += _b.b;

		return *this;
	}

	void print() {
		fprintf(stdout, "%d, %d\n", a, b);
	}
};

GMan operator+(GMan _a, const GMan& _b) {
	_a += _b; // code re-use
	return _a;
}

#endif /* IMPLICITCONOP_H_ */
