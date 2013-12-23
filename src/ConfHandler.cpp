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
 * ConfHandler.cpp
 *
 *  Created on: Aug 7, 2012
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include "ConfHandler.h"
#include "ConfEntry.h"
#include "StrTokenizer.h"

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

bool ConfHandler::BEEN_INIT = false;

ConfHandler::VEC ConfHandler::NeighborVector = VEC();
ConfHandler::MAP ConfHandler::NeighborSeeds = MAP();
ConfHandler::MAP ConfHandler::ZHTParameters = MAP();
ConfHandler::MAP ConfHandler::NodeParameters = MAP();

string ConfHandler::CONF_ZHT = "zht.conf";
string ConfHandler::CONF_NODE = "node.conf";
string ConfHandler::CONF_NEIGHBOR = "neighbor.conf";
string ConfHandler::NOVOHT_FILE = "";

uint ConfHandler::ZC_MAX_ZHT = 0;
uint ConfHandler::ZC_NUM_REPLICAS = 0;
uint ConfHandler::ZC_REPLICATION_TYPE = 0;

uint ConfHandler::NC_ZHT_CAPACITY = 0;
string ConfHandler::NC_FILECLIENT_PATH = "./fileclient";
string ConfHandler::NC_FILESERVER_PATH = "./file_server.exe";
uint ConfHandler::NC_FILESERVER_PORT = 9000;

string ConfHandler::ZC_HTDATA_PATH = ""; //todo: empty string not allowed.
uint ConfHandler::ZC_MIGSLP_TIME = 1000000; //micro second

ConfHandler::ConfHandler() {

}

ConfHandler::~ConfHandler() {

}

string ConfHandler::getProtocolFromConf() {

	return get_zhtconf_parameter(Const::PROTO_NAME);
}

string ConfHandler::getPortFromConf() {

	return get_zhtconf_parameter(Const::PROTO_PORT);
}

string ConfHandler::get_zhtconf_parameter(const string &paraname) {

	string result;
	ConfHandler::MAP *zpmap = &ConfHandler::ZHTParameters;

	ConfHandler::MIT it;

	for (it = zpmap->begin(); it != zpmap->end(); it++) {

		ConfEntry ce;
		ce.assign(it->first);

		if (ce.name() == paraname) {

			result = ce.value();

			break;
		}
	}

	return result;
}

void ConfHandler::initConf(string zhtConf, string neighborConf) {

	if (!BEEN_INIT) {

		ConfHandler::CONF_ZHT = zhtConf; //zht.conf
		ConfHandler::CONF_NEIGHBOR = neighborConf; //neighbor.conf

		ConfHandler::setZHTParameters(zhtConf);
		ConfHandler::setNeighborSeeds(neighborConf);

		BEEN_INIT = true;
	}
}

void ConfHandler::setNeighborSeeds(const string& neighborCfg) {

	setParametersInternal(neighborCfg, NeighborSeeds);

	setNeighborVector(NeighborVector);
}

void ConfHandler::setZHTParameters(const string& zhtConfig) {

	setParametersInternal(zhtConfig, ZHTParameters);

	pickZHTParameters();
}

void ConfHandler::setNodeParameters(const string& nodeConfig) {

	setParametersInternal(nodeConfig, NodeParameters);

//	pickNodeParameters();
}

void ConfHandler::setParametersInternal(string configFile, MAP& configMap) {

	ifstream ifs(configFile.c_str(), ifstream::in);

	const char *delimiter = Const::CONF_DELIMITERS.c_str();

	string line;
	while (getline(ifs, line)) {

		string remains = line;

		if (remains.empty())
			continue;

		if (remains.substr(0, 1) == "#") //starts with #, means comment
			continue;

		StrTokenizer strtok(remains);

		string one;
		string two;

		if (strtok.has_more_tokens())
			one = strtok.next_token();

		if (strtok.has_more_tokens())
			two = strtok.next_token();

		if (one.empty())
			continue;

		ConfEntry ce(one, two);
		configMap.insert(PAIR(ce.toString(), ce)); //todo: use hash code to reduce size of key/value pair.
	}

	ifs.close();
}

void ConfHandler::setNeighborVector(VEC &neighborVector) {

	ConfHandler::MIT kvi;
	ConfHandler::MAP* map = &ConfHandler::NeighborSeeds;

	for (kvi = map->begin(); kvi != map->end(); kvi++) {

		neighborVector.push_back(kvi->second);
	}
}

void ConfHandler::pickNodeParameters() {

	ConfHandler::MIT kvi;
	ConfHandler::MAP* map = &ConfHandler::NodeParameters;

	for (kvi = map->begin(); kvi != map->end(); kvi++) {

		ConfEntry kv = kvi->second;

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

void ConfHandler::pickZHTParameters() {

	ConfHandler::MIT kvi;
	ConfHandler::MAP* map = &ConfHandler::ZHTParameters;

	for (kvi = map->begin(); kvi != map->end(); kvi++) {

		ConfEntry kv = kvi->second;

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
