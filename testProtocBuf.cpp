/*
 * testProtocBuf.cpp
 *
 *  Created on: Feb 2, 2012
 *      Author: tony
 */
#include <list>
#include <vector>
#include <string>
#include <iostream>
#include "zht_util.h"

int main() {

	int lenString = 15;
	Package package, package_tmp;
	package.set_virtualpath(randomString(lenString)); //as key
	package.set_isdir(true);
	package.set_replicano(5); //orginal--Note: never let it be nagative!!!
	package.set_operation(3); // 3 for insert, 1 for look up, 2 for remove
	package.set_realfullpath(
			"Some-Real-longer-longer-and-longer-Paths--------");
	package.add_listitem("item-----1");
	package.add_listitem("item-----2");
	package.add_listitem("item-----3");
	package.add_listitem("item-----4");
	package.add_listitem("item-----5");
	string str = package.SerializeAsString();

	long i = 0;
	long n = 10 * 1E6;

	double start = 0;
	double end = 0;

	start = getTime_msec();
	for (i = 0; i < n; i++) {

		string s = package.SerializeAsString();
	}
	end = getTime_msec();

	cout << "Finished " << n << " serializations  cost "
			<< end - start << " ms." << endl;


	start = getTime_msec();
		for (i = 0; i < n; i++) {
			package_tmp.ParseFromString(str);

		}
		end = getTime_msec();

		cout << "Finished " << n << " deserializations cost "
				<< end - start << " ms." << endl;

	return 0;
}

