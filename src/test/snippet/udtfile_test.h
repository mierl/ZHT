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
 * udtfile_test.h
 *
 *  Created on: Sep 24, 2012
 *      Author: tony, xiaobingo
 */

#ifndef UDTFILE_TEST_H_
#define UDTFILE_TEST_H_

#include <string>
using namespace std;

class FileClient {
public:
	FileClient();
	FileClient(const string& fileClient, const string& host, const string& port,
			const string& remoteFilename, const string& localFileName);

	/*FileClient(const char* const fileClient, const char* const host,
			const char* const port, const char* const remoteFilename,
			const char* const localFileName);*/

	virtual ~FileClient();

	int getFileFromZHT();

private:
	int getFileFromZHTInternal(const int& argc, const char** argv);

private:
	string _fileClient;
	string _host;
	string _port;
	string _remoteFilename;
	string _localFileName;
};

#endif /* UDTFILE_TEST_H_ */
