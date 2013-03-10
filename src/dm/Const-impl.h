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
 * Const-impl.h
 *
 *  Created on: Feb 13, 2013
 *      Author: tony, xiaobingo
 */

#ifndef CONST_IMPL_H_
#define CONST_IMPL_H_

#include "Const.h"

#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <iostream>

using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

template<class TYPE> int Const::toInt(const TYPE& ele) {

	return atoi(toString(ele).c_str());
}

template<class TYPE> uint64_t Const::toUInt64(const TYPE& ele) {

	return strtoull(toString(ele).c_str(), NULL, 10);
}

template<class TYPE> string Const::toString(const TYPE& ele) {

	stringstream ss;
	ss << ele;

	return ss.str();
}

template<class TYPE1, class TYPE2> string Const::concat(const TYPE1& ele1,
		const TYPE2& ele2) {

	stringstream ss;
	ss << ele1;
	ss << ele2;

	return ss.str();
}

template<class TYPE1, class TYPE2> string Const::concat(const TYPE1& ele1,
		const string& delimiter, const TYPE2& ele2) {

	stringstream ss;
	ss << ele1;
	ss << delimiter;
	ss << ele2;

	return ss.str();
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* CONST_IMPL_H_ */
