/*
 *  * scriptRunner.cpp
 *   *
 *    *  Created on: Mar 28, 2011
 *     *      Author: tony
 *      */

#include<stdlib.h>
#include<string.h>
#include <iostream>
using namespace std;
/*This program is used to test function system*/

int main(int argc, char* argv[]) {
	char str[1024];
	strcpy(str, argv[1]);

	for (int i = 2; i < argc; i++) {
		strcat(str, " ");
		strcat(str, argv[i]);
	}
	system(str);
	return 0;
										}

