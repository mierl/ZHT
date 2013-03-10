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
 * ConfigHandler.cpp
 *
 *  Created on: Aug 7, 2012
 *      Author: tony, xiaobingo
 */

#include "ConfigHandler.h"
#include "Const-impl.h"
#include "ConfigEntry.h"

#include <stdlib.h>
#include <string>
#include <string.h>
#include <stdio.h>

#include <fstream>
#include <iostream>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

ConfigHandler::MAP ConfigHandler::NeighborSeeds = MAP();
ConfigHandler::MAP ConfigHandler::ZHTParameters = MAP();
ConfigHandler::MAP ConfigHandler::NodeParameters = MAP();

string ConfigHandler::CFG_ZHT = "zht.cfg";
string ConfigHandler::CFG_NODE = "node.cfg";
string ConfigHandler::CFG_NEIGHBOR = "neighbor";

uint ConfigHandler::ZC_MAX_ZHT = 0;
uint ConfigHandler::ZC_NUM_REPLICAS = 0;
uint ConfigHandler::ZC_REPLICATION_TYPE = 0;

uint ConfigHandler::NC_ZHT_CAPACITY = 0;
string ConfigHandler::NC_FILECLIENT_PATH = "./fileclient";
string ConfigHandler::NC_FILESERVER_PATH = "./file_server.exe";
uint ConfigHandler::NC_FILESERVER_PORT = 9000;

string ConfigHandler::ZC_HTDATA_PATH = ""; //todo: empty string not allowed.

uint ConfigHandler::ZC_MIGSLP_TIME = 1000000; //micro second

ConfigHandler::ConfigHandler() {

}

ConfigHandler::~ConfigHandler() {

}

void ConfigHandler::setNeighborSeeds(const string& neighborCfg) {

	setParametersInternal(neighborCfg, NeighborSeeds);
}

void ConfigHandler::setZHTParameters(const string& zhtConfig) {

	setParametersInternal(zhtConfig, ZHTParameters);

	pickZHTParameters();
}

void ConfigHandler::setNodeParameters(const string& nodeConfig) {

	setParametersInternal(nodeConfig, NodeParameters);

//	pickNodeParameters();
}

void ConfigHandler::setParametersInternal(string configFile, MAP& configMap) {

	ifstream ifs(configFile.c_str(), ifstream::in);

	const char *delimiter = Const::CFG_DELIMITERS.c_str();

	string line;
	while (getline(ifs, line)) {

		string remains = Const::toString(line);
		if (remains.empty())
			continue;

		size_t found = remains.find(delimiter);

		if (found != string::npos) {

			string one = Const::toString(remains.substr(0, int(found)));
			string two = Const::toString(remains.substr(int(found) + 1));

			if (one.empty() || two.empty())
				continue;

			if (one.substr(0, 1) == "#") //starts with #, means comment
				continue;

			string name = one;
			string value = two;

			if (!name.empty() && !value.empty()) {

				ConfigEntry ce(name, value);
				configMap.insert(PAIR(ce.toString(), ce)); //todo: use hash code to reduce size of key/value pair.
			}
		}
	}

	ifs.close();
}

void ConfigHandler::pickNodeParameters() {

	ConfigHandler::IT kvi;
	ConfigHandler::MAP* map = &ConfigHandler::NodeParameters;

	for (kvi = map->begin(); kvi != map->end(); kvi++) {

		ConfigEntry kv = kvi->second;

		if (kv.name() == Const::NC_ZHT_CAPACITY) {

			NC_ZHT_CAPACITY = atoi(kv.value().c_str());
		} else if (kv.name() == Const::NC_FILECLIENT_PATH) {

			NC_FILECLIENT_PATH = kv.value();
		} else if (kv.name() == Const::NC_FILESERVER_PATH) {

			NC_FILESERVER_PATH = kv.value();
		} else if (kv.name() == Const::NC_FILESERVER_PORT) {

			NC_FILESERVER_PORT = atoi(kv.value().c_str());
		} else {
		}
	}
}

void ConfigHandler::pickZHTParameters() {

	ConfigHandler::IT kvi;
	ConfigHandler::MAP* map = &ConfigHandler::ZHTParameters;

	for (kvi = map->begin(); kvi != map->end(); kvi++) {

		ConfigEntry kv = kvi->second;

		if (kv.name() == Const::ZC_MAX_ZHT) {

			ZC_MAX_ZHT = atoi(kv.value().c_str());
		} else if (kv.name() == Const::ZC_NUM_REPLICAS) {

			ZC_NUM_REPLICAS = atoi(kv.value().c_str());
		} else if (kv.name() == Const::ZC_REPLICATION_TYPE) {

			ZC_REPLICATION_TYPE = atoi(kv.value().c_str());
		} else if (kv.name() == Const::NC_ZHT_CAPACITY) {

			NC_ZHT_CAPACITY = atoi(kv.value().c_str());
		} else if (kv.name() == Const::NC_FILECLIENT_PATH) {

			NC_FILECLIENT_PATH = kv.value();
		} else if (kv.name() == Const::NC_FILESERVER_PATH) {

			NC_FILESERVER_PATH = kv.value();
		} else if (kv.name() == Const::NC_FILESERVER_PORT) {

			NC_FILESERVER_PORT = atoi(kv.value().c_str());
		} else if (kv.name() == Const::ZC_HTDATA_PATH) {

			ZC_HTDATA_PATH = kv.value();
		} else if (kv.name() == Const::ZC_MIGSLP_TIME) {

			ZC_MIGSLP_TIME = atoi(kv.value().c_str());
		} else {

		}
	}
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
