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
 * ZProcessor.h
 *
 *  Created on: Aug 9, 2012
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#ifndef ZPROCESSOR_H_
#define ZPROCESSOR_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <stddef.h>

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

/*
 *
 */
class ZProcessor {
public:
	ZProcessor();
	virtual ~ZProcessor();

	virtual void process(const int& fd, const char * const buf,
			sockaddr sender) = 0;

	virtual void sendback(const int& fd, const char *buf, const size_t& count,
			sockaddr receiver, const int& protocol);
};

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* ZPROCESSOR_H_ */
