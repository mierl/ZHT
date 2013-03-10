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
 * file_ftruncate.cpp
 *
 *  Created on: Aug 11, 2012
 *      Author: tony, xiaobingo
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

#define string_len 1000

int main(int argc, char **argv) {

	char *mega_string;
	int file_descriptor;
	int ret;
	char fn[] = "write.file";
	struct stat st;

	if ((mega_string = (char*) malloc(string_len)) == NULL)
		perror("malloc() error");
	else if ((file_descriptor = creat(fn, S_IWUSR)) < 0)
		perror("creat() error");
	else {

		memset(mega_string, '0', string_len);

		if ((ret = write(file_descriptor, mega_string, string_len)) == -1)
			perror("write() error");
		else {
			printf("write() wrote %d bytes\n", ret);
			fstat(file_descriptor, &st);
			printf("the file has %ld bytes\n", (long) st.st_size);
			if (ftruncate(file_descriptor, 1) != 0)
				perror("ftruncate() error");
			else {
				fstat(file_descriptor, &st);
				printf("the file has %ld bytes\n", (long) st.st_size);
			}
		}
		close(file_descriptor);
		unlink(fn);
	}
	free(mega_string);
}


