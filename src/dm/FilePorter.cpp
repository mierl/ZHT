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
 * FilePorter.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: tony, xiaobingo
 */

#include "FilePorter.h"
#include "ZHTClient.h"

#include "Address.h"
#include "Const-impl.h"
#include "ConfigHandler.h"
#include "../meta.pb.h"

#include "FileClient.h"

#include <sstream>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

FilePorter::FilePorter() {
}

FilePorter::~FilePorter() {
}

string FilePorter::transferNovoHT(const string& fileName,
		const string& sourceZHT, const string& targetZHT) {

	string sdir = Const::concat(ConfigHandler::ZC_HTDATA_PATH, sourceZHT);
	string ddir = Const::concat(ConfigHandler::ZC_HTDATA_PATH, targetZHT);

	FileClient fc(Address::getHost(targetZHT),
			Const::toString(ConfigHandler::NC_FILESERVER_PORT),
			Const::concat(sdir, Const::DIR_DELIM, fileName),
			Const::concat(ddir, Const::DIR_DELIM, fileName));

	int status = fc.putFileToZHT();

	string sstatus;
	if (status != 0)
		sstatus = Const::ZSC_REC_FLPUSHFAIL;
	else
		sstatus = Const::ZSC_REC_SUCC;

	return sstatus;
}

string FilePorter::transferNovoHT2(const string& fileName,
		const string& sourceZHT, const string& targetZHT) {

	Package pkg;

	pkg.set_opcode(Const::ZSC_OPC_PULLFILE); //let target zht pull file from source zht
	pkg.set_rfname(fileName);
	pkg.set_lfname(fileName);
	pkg.set_sourcezht(sourceZHT);

	string sstatus = ZHTClient::sendPkg(Address::getHost(targetZHT),
			Address::getPort(targetZHT), pkg.SerializeAsString());

	if (sstatus == Const::ZSC_REC_CLTFAIL)
		sstatus = Const::ZSC_REC_FLPUSHFAIL;

	return sstatus;
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
