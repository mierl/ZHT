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
 * vector_trans.cpp
 *
 *  Created on: Aug 8, 2012
 *      Author: tony, xiaobingo
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cstring>

using namespace std;

void testSimpleTrans();
void testComplicatedTrans();

int main() {

	testSimpleTrans();

//	void testComplicatedTrans();
}

char *sconvert(const std::string & s) {

	char *pc = new char[s.size() + 1];
	std::strcpy(pc, s.c_str());
	return pc;
}

const char *cconvert(const std::string & s) {

	return s.c_str();
}

void testSimpleTrans() {

	std::vector<std::string> vs;
	vs.push_back("std::string");
	vs.push_back("std::vector<std::string>");
	vs.push_back("char*");
	vs.push_back("std::vector<char*>");
	std::vector<char*> vc;

	std::transform(vs.begin(), vs.end(), std::back_inserter(vc), sconvert);

	for (size_t i = 0; i < vs.size(); i++)
		std::cout << vs[i] << std::endl;

	cout << "###############################" << endl;
	for (size_t i = 0; i < vc.size(); i++)
		std::cout << vc[i] << std::endl;

	for (size_t i = 0; i < vc.size(); i++)
		delete[] vc[i];
}

void testComplicatedTrans() {

	std::vector<std::string> vs;
	vs.push_back("std::string");
	vs.push_back("std::vector<std::string>");
	vs.push_back("char*");
	vs.push_back("std::vector<char*>");
	std::vector<const char*> vc;

	std::transform(vs.begin(), vs.end(), std::back_inserter(vc), cconvert);

	for (size_t i = 0; i < vs.size(); i++)
		std::cout << vs[i] << std::endl;

	cout << "###############################" << endl;
	for (size_t i = 0; i < vc.size(); i++)
		std::cout << vc[i] << std::endl;

	vs[0] = "hello";
	cout << "###############################" << endl;
	for (size_t i = 0; i < vs.size(); i++)
		std::cout << vs[i] << std::endl;

	cout << "###############################" << endl;
	for (size_t i = 0; i < vc.size(); i++)
		std::cout << vc[i] << std::endl;

	vc[0] = "zht";
	cout << "###############################" << endl;
	for (size_t i = 0; i < vs.size(); i++)
		std::cout << vs[i] << std::endl;

	cout << "###############################" << endl;
	for (size_t i = 0; i < vc.size(); i++)
		std::cout << vc[i] << std::endl;

	vs[0] = "dummy";
	cout << "###############################" << endl;
	for (size_t i = 0; i < vs.size(); i++)
		std::cout << vs[i] << std::endl;

	cout << "###############################" << endl;
	for (size_t i = 0; i < vc.size(); i++)
		std::cout << vc[i] << std::endl;

	/*
	 for (size_t i = 0; i < vc.size(); i++)
	 delete[] vc[i];*/

}
