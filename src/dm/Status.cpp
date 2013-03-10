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
 * Status.cpp
 *
 *  Created on: Feb 14, 2013
 *      Author: tony, xiaobingo
 */

#include "Status.h"

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

const int BSEnum::BS_DOING = 0X90;
const int BSEnum::BS_DONE = 0X45;

int BroadcastStatus::BS_STATUS = BSEnum::BS_DONE;

BroadcastStatus::BroadcastStatus() {

}

BroadcastStatus::~BroadcastStatus() {
}

int BroadcastStatus::value() {

	return BS_STATUS;

}

void BroadcastStatus::value(const int& status) {

	BS_STATUS = status;
}

const int MSEnum::MS_DOING = 0X40;
const int MSEnum::MS_DONE = 0X80;

int MigrateStatus::MS_STATUS = MSEnum::MS_DONE;

MutexCondition MigrateStatus::MC = MutexCondition();

MigrateStatus::MigrateStatus() {
}

MigrateStatus::~MigrateStatus() {
}

int MigrateStatus::value() {

	return MS_STATUS;
}

void MigrateStatus::value(const int& status) {

	MS_STATUS = status;
}

const int PSEnum::PS_DOING = 0X10;
const int PSEnum::PS_DONE = 0X20;

int PorterStatus::PS_STATUS = PSEnum::PS_DONE;

MutexCondition PorterStatus::MC = MutexCondition();

PorterStatus::PorterStatus() {
}

PorterStatus::~PorterStatus() {
}

int PorterStatus::value() {

//	MC.wait();
//	value = PS_STATUS;
//	MC.signal();

	return PS_STATUS;
}

void PorterStatus::value(const int& status) {

	MC.lock();
	PS_STATUS = status;
	MC.unLock();
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
