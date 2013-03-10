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
 * strlensizeof.cpp
 *
 *  Created on: Aug 22, 2012
 *      Author: tony, xiaobingo
 */

#include <string.h>
#include <stdio.h>
#include <iostream>
using namespace std;

void foo(const string& arg) {

	cout << arg << endl;
}

void usage() {

	char str2[8];
	strncpy(str2, "Sanjeev", 7);
	char *str1 = str2;
	printf("%lu %lu\n", strlen(str1), sizeof(str1));
	printf("%lu %lu\n", strlen(str2), sizeof(str2));
}

void usage2() {

	const char *str1 = "Sanjeev";
	char str2[] = "Sanjeev";
	printf("%lu %lu\n", strlen(str1), sizeof(str1));
	printf("%lu %lu\n", strlen(str2), sizeof(str2));

}

void usage3() {

	char buff[10];

	cout << sizeof(buff) << endl;
	cout << strlen(buff) << endl;

	memset(buff, '1', sizeof(buff));
	cout << strlen(buff) << endl;

	cout << "--------str-------------" << endl;
	string str(buff);
	cout << str.size() << endl;
	cout << str.length() << endl;

	cout << "--------c_str-------------" << endl;
	cout << sizeof(str.c_str()) << endl;
	cout << strlen(str.c_str()) << endl;

	cout << "---------data------------" << endl;
	cout << strlen(str.data()) << endl;
	cout << strlen(str.data()) << endl;

}

int main(int argc, char **argv) {

//	usage();

	usage2();
}
