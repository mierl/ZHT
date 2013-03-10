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
 * const_ref.h
 *
 *  Created on: Sep 14, 2012
 *      Author: tony, xiaobingo
 */

#ifndef CONST_REF_H_
#define CONST_REF_H_

class Sender {

public:
	Sender();
	explicit Sender(int fd);
	~Sender();

	int fd() const;
	void fd(const int& fd);
private:
	int _fd;
};
/*
 *
 */
class ConstRef {
public:
	ConstRef();
	virtual ~ConstRef();

	int uuid() const;
	void uuid(const int& uuid);

	Sender& sender(); //fine
//	Sender& sender() const; //error: invalid initialization of reference of type ‘Sender&’ from expression of type ‘const Sender’
//	const Sender& sender() const; //fine
	void sender(const Sender& sender);

private:
	Sender _sender;
	int _uuid;
};
#endif /* CONST_REF_H_ */
