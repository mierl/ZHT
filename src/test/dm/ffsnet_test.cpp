/**
 * File name: ffsnet_test.cpp
 *
 * Function: test ffs_recvfile() and ffs_sendfile()
 *
 * Author: dzhao8@hawk.iit.edu
 *
 * Update history:
 *		- 06/18/2012: initial development
 *
 * To compile this single file:
 *		g++ ffsnet_test.cpp -c -o ffsnet_test.o 
 * 		g++ ffsnet.o ffsnet_test.o -o ffsnet_test -I../src -L../src -ludt -lstdc++ -lpthread
 *		NOTE: -I../src (similarly to -L../src) means to include the directory of the udt library, i.e. libudt.so  
 */

#include <iostream>
#include <cstdlib>
#include <cstring>

#include "ffsnet.h"

using namespace std;

int 
main(int argc, char* argv[])
{
	if ((argc != 6) || (0 == atoi(argv[2]))) {
		cout << "usage: ffsnet_test server_ip server_port remote_filename local_filename [download?0:1]" << endl;
		return -1;
	}

	if (!atoi(argv[5])) /* 0-download, 1-upload */
		ffs_recvfile("udt", argv[1], argv[2], argv[3], argv[4]);
	else
		ffs_sendfile("udt", argv[1], argv[2], argv[4], argv[3]);
}
