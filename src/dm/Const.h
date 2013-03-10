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
 * Const.h
 *
 *  Created on: Jul 23, 2012
 *      Author: tony, xiaobingo
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include "Neighbor.h"

#include <map>
#include <string>
#include <stdint.h>
using namespace std;

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

class Const {
public:
	typedef map<uint64_t, Neighbor> MAP;
	typedef pair<uint64_t, Neighbor> PAIR;
	typedef map<uint64_t, Neighbor>::iterator MIT;
	typedef map<uint64_t, Neighbor>::reverse_iterator MRIT;

public:
	Const();
	virtual ~Const();

public:
	template<class TYPE> static int toInt(const TYPE& ele);
	template<class TYPE> static uint64_t toUInt64(const TYPE& ele);
	template<class TYPE> static string toString(const TYPE& ele);
	template<class TYPE1, class TYPE2> static string concat(const TYPE1& ele1,
			const TYPE2& ele2);
	template<class TYPE1, class TYPE2> static string concat(const TYPE1& ele1,
			const string& delimiter, const TYPE2& ele2);

	static string trim(const string& value);

	static void prtMem(MAP* map);
	static void prtNb(const Neighbor& neighbor);

public:
	static const string StringEmpty;

	static const string LOCAL_HOST;

	static const string DIR_DELIM;

	static const string CFG_DELIMITERS; //delimiters used in config file

	static const int ADDR_UUIDZERO; //used to remember if the address is changed.

	/*
	 * PROTO_: protocol
	 */
	static const int PROTO_STREAM;
	static const int PROTO_UGRADM;

	/*
	 * NC_: node config
	 */
	static const string NC_ZHT_CAPACITY;
	static const string NC_FILECLIENT_PATH;
	static const string NC_FILESERVER_PATH;
	static const string NC_FILESERVER_PORT;

	/*
	 * ZC_: ZHT config
	 */
	static const string ZC_MAX_ZHT;
	static const string ZC_NUM_REPLICAS;
	static const string ZC_REPLICATION_TYPE;
	static const string ZC_HTDATA_PATH;
	static const string ZC_MIGSLP_TIME;

	/*
	 * ASC_: admin server(service) chars
	 * ASI_: admin server(service) integers
	 * OPC_: operation code
	 * REC_: return code
	 * AZ_: add ZHT
	 * RZ_: remove ZHT
	 * UNPR_: unprocessed
	 * UOPC_: unrecognized operation code
	 * SUCC_: succeeded
	 */
	static const string ASC_OPC_AZ_ALL; //add all ZHT
	static const string ASC_OPC_AZ_PORT; //add ZHT by port
	static const string ASC_OPC_AZ_IPPORT; //add ZHT by ip and port
	static const string ASC_OPC_RZ_ALL; //remove all ZHT
	static const string ASC_OPC_RZ_PORT; //remove ZHT by port
	static const string ASC_OPC_RZ_IPPORT; //remove ZHT by ip and port
	static const string ASC_REC_NTHDOZM; //nothing to do in zht manager
	static const string ASC_REC_UOPC; //unrecognized operation code
	static const string ASC_REC_UNPR; //unprocessed
	static const string ASC_REC_SUCC; //succeeded

	static const int ASI_OPC_AZ_ALL; //add all ZHT
	static const int ASI_OPC_AZ_PORT; //add ZHT by port
	static const int ASI_OPC_AZ_IPPORT; //add ZHT by ip and port
	static const int ASI_OPC_RZ_ALL; //remove all ZHT
	static const int ASI_OPC_RZ_PORT; //remove ZHT by port
	static const int ASI_OPC_RZ_IPPORT; //remove ZHT by ip and port
	static const int ASI_REC_NTHDOZM; //nothing to do in zht manager
	static const int ASI_REC_UOPC; //unrecognized operation code
	static const int ASI_REC_UNPR; //unprocessed
	static const int ASI_REC_SUCC; //succeeded

	/*
	 * ZSC_: ZHT server(service) chars
	 * ZSI_: ZHT server(service) integers
	 * OPC_: operation code
	 * REC_: return code
	 * UNPR_: unprocessed
	 * UOPC_: unrecognized operation code
	 * SUCC_: succeeded
	 */
	static const string ZSC_OPC_LOOKUP; //lookup item
	static const string ZSC_OPC_REMOVE; //remove item
	static const string ZSC_OPC_INSERT; //insert item
	static const string ZSC_OPC_BRDDN_GMEM; //broadcast global membership done
	static const string ZSC_OPC_OPR_CANCEL; //cancle an operation
	static const string ZSC_OPC_GET_ASNGHB; //get information of ZHTNode as a neighbor
	static const string ZSC_OPC_BRD_GMEM; //broadcast global membership to zht, triggered by broadcast
	static const string ZSC_OPC_GET_GMEM; //get global membership from zht
	static const string ZSC_OPC_GET_DESTZHT; //get destination zht to service client request
	static const string ZSC_OPC_PULLFILE; //pull file from source zht server
	static const string ZSC_OPC_MIGDONETGT; //inform target zht that migrage is done
	static const string ZSC_OPC_MIGDONESRC; //inform source zht that migrage is done
	static const string ZSC_OPC_MIGTARGET; //migrate novoht in target ZHT
	static const string ZSC_OPC_MIGSOURCE; //migrate novoht in source ZHT
	static const string ZSC_REC_EMPTYKEY; //empty key
	static const string ZSC_REC_CLTFAIL; //operation failed in client-side
	static const string ZSC_REC_SRVFAIL; //operation failed in server-side
	static const string ZSC_REC_SRVEXP; //operation failed
	static const string ZSC_REC_NONEXISTKEY; //non existent key
	static const string ZSC_REC_NODESTZHT; //no destination for a key
	static const string ZSC_REC_NONEEDMIG; //no need to migrate from
	static const string ZSC_REC_FLPUSHFAIL; //fail to push file by source zht
	static const string ZSC_REC_FLPULLFAIL; //fail to pull file by target zht
	static const string ZSC_REC_SECDTRY; //second try client request
	static const string ZSC_REC_UOPC; //unrecognized operation code
	static const string ZSC_REC_UNPR; //unprocessed
	static const string ZSC_REC_SUCC; //succeeded

	static const int ZSI_OPC_LOOKUP; //lookup item
	static const int ZSI_OPC_REMOVE; //remove item
	static const int ZSI_OPC_INSERT; //insert item
	static const int ZSI_OPC_BRDDN_GMEM; //broadcast global membership done
	static const int ZSI_OPC_OPR_CANCEL; //cancel an operation
	static const int ZSI_OPC_GET_ASNGHB; //get information of ZHTNode as a neighbor
	static const int ZSI_OPC_BRD_GMEM; //broadcast global membership to zht, triggered by broadcast
	static const int ZSI_OPC_GET_GMEM; //get global membership from zht
	static const int ZSI_OPC_GET_DESTZHT; //get destination zht to service client request
	static const int ZSI_OPC_PULLFILE; //pull file from source zht server
	static const int ZSI_OPC_MIGDONETGT; //inform target zht that migrage is done
	static const int ZSI_OPC_MIGDONESRC; //inform source zht that migrage is done
	static const int ZSI_OPC_MIGTARGET; //migrate novoht in target ZHT
	static const int ZSI_OPC_MIGSOURCE; //migrate novoht in source ZHT
	static const int ZSI_REC_EMPTYKEY; //empty key
	static const int ZSI_REC_CLTFAIL; //operation failed in client-side
	static const int ZSI_REC_SRVFAIL; //operation failed in server-side
	static const int ZSI_REC_SRVEXP; //server excpetion
	static const int ZSI_REC_NONEXISTKEY; //non existent key
	static const int ZSI_REC_NODESTZHT; //no destination for a key
	static const int ZSI_REC_NONEEDMIG; //no need to migrate from
	static const int ZSI_REC_FLPUSHFAIL; //fail to push file by source zht
	static const int ZSI_REC_FLPULLFAIL; //fail to pull file by target zht
	static const int ZSI_REC_SECDTRY; //second try client request
	static const int ZSI_REC_UOPC; //unrecognized operation code
	static const int ZSI_REC_UNPR; //unprocessed
	static const int ZSI_REC_SUCC; //succeeded

};


} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
#endif /* CONSTANTS_H_ */
