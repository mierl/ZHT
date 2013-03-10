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
 * ConfigHandler.h
 *
 *  Created on: Aug 7, 2012
 *      Author: tony, xiaobingo
 */

#ifndef CONFIGHANDLER_H_
#define CONFIGHANDLER_H_

#include "ConfigEntry.h"

#include <map>
#include <string>
#include <sys/types.h>

using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

/*
 * todo: monitor node.cfg
 */
class ConfigHandler {
public:
	typedef map<string, ConfigEntry> MAP;
	typedef pair<string, ConfigEntry> PAIR;
	typedef map<string, ConfigEntry>::iterator IT;
	typedef map<string, ConfigEntry>::reverse_iterator RIT;

public:
	ConfigHandler();
	virtual ~ConfigHandler();

	static void setNeighborSeeds(const string& neighborCfg);
	static void setZHTParameters(const string& zhtConfig);
	static void setNodeParameters(const string& nodeConfig);

private:
	static void pickNodeParameters();
	static void pickZHTParameters();

	static void setParametersInternal(string configFile, MAP& configMap);

public:
	static MAP NeighborSeeds;
	static MAP ZHTParameters;
	static MAP NodeParameters;

public:
	static string CFG_ZHT;
	static string CFG_NODE;
	static string CFG_NEIGHBOR;

public:
	static string NC_FILECLIENT_PATH;
	static string NC_FILESERVER_PATH;
	static uint NC_FILESERVER_PORT;

	static uint ZC_MAX_ZHT;
	static uint ZC_NUM_REPLICAS;
	static uint ZC_REPLICATION_TYPE;
	static uint NC_ZHT_CAPACITY;
	static string ZC_HTDATA_PATH;
	static uint ZC_MIGSLP_TIME;
};

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* CONFIGHANDLER_H_ */
