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
 * Env.cpp
 *
 *  Created on: Jun 25, 2013
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include "Env.h"
#include "ConfHandler.h"

using namespace iit::datasys::zht::dm;

const uint Env::BUF_SIZE = 512 + 38;
const int Env::MSG_DEFAULTSIZE = 1024 * 1024 * 2; //2M
const int Env::SCCB_POLL_DEFAULT_INTERVAL = 1; //1 ms;

int Env::NUM_REPLICAS = 0;
int Env::REPLICATION_TYPE = 0; //1 for Client-side replication

Env::Env() {
}

Env::~Env() {
}

int Env::get_msg_maxsize() {

	string val = ConfHandler::get_zhtconf_parameter(Const::MSG_MAXSIZE);

	return val.empty() ? MSG_DEFAULTSIZE : atoi(val.c_str());
}

int Env::get_sccb_poll_interval() {

	string val = ConfHandler::get_zhtconf_parameter(Const::SCCB_POLL_INTERVAL);

	return val.empty() ? SCCB_POLL_DEFAULT_INTERVAL : atoi(val.c_str());
}
