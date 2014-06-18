/*
 * Copyright 2010-2020 DatasysLab@iit.edu(http://datasys.cs.iit.edu/index.html)
 *      Director: Ioan Raicu(iraicu@cs.iit.edu)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is part of ZHT library(http://datasys.cs.iit.edu/projects/ZHT/index.html).
 *      Tonglin Li(tli13@hawk.iit.edu) with nickname Tony,
 *      Xiaobing Zhou(xzhou40@hawk.iit.edu) with nickname Xiaobingo,
 *      Ke Wang(kwang22@hawk.iit.edu) with nickname KWang,
 *      Dongfang Zhao(dzhao8@@hawk.iit.edu) with nickname DZhao,
 *      Ioan Raicu(iraicu@cs.iit.edu).
 *
 * ConfEntry.cpp
 *
 *  Created on: Aug 7, 2012
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include "ConfEntry.h"

#include "Const-impl.h"

#include <stdio.h>
#include <string.h>

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

ConfEntry::ConfEntry() {

}

ConfEntry::ConfEntry(const string& sconfigEntry) {

	assign(sconfigEntry);
}

ConfEntry::ConfEntry(const string& name, const string& value) :
		_name(name), _value(value) {

}

ConfEntry::~ConfEntry() {

}

string ConfEntry::name() const {

	return _name;
}

void ConfEntry::name(const string& name) {

	_name = name;
}

string ConfEntry::value() const {

	return _value;
}

void ConfEntry::value(const string& value) {

	_value = value;
}

string ConfEntry::operator()() const {

	return toString();
}

string ConfEntry::toString() const {

	char buf[50];
	memset(buf, 0, sizeof(buf));
	int n = sprintf(buf, getFormat().c_str(), _name.c_str(), _value.c_str());

	string result(buf, 0, n);

	return result;
}

ConfEntry& ConfEntry::assign(string sconfigEntry) {

	const char* delimiter = ",";

	string remains = Const::trim(sconfigEntry);

	size_t found = remains.find(delimiter);

	if (found != string::npos) {

		name(Const::trim(remains.substr(0, int(found))));
		value(Const::trim(remains.substr(int(found) + 1)));
	}

	return *this;
}

string ConfEntry::getFormat() {

	return "%s,%s";
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
