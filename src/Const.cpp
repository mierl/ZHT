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
 * Const.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include "Const-impl.h"

namespace iit {
namespace datasys {
namespace zht {
namespace dm {

const string Const::StringEmpty = "";

const string Const::LOCAL_HOST = "localhost";

const string Const::DIR_DELIM = "/";

const string Const::CONF_DELIMITERS = " ";

const int Const::ADDR_UUIDZERO = 0;

const int Const::PROTO_STREAM = 1;
const int Const::PROTO_UGRADM = 2;

const string Const::NC_ZHT_CAPACITY = "ZHT_CAPACITY";
const string Const::NC_FILECLIENT_PATH = "FILECLIENT_PATH";
const string Const::NC_FILESERVER_PATH = "FILESERVER_PATH";
const string Const::NC_FILESERVER_PORT = "FILESERVER_PORT";

const string Const::ZC_MAX_ZHT = "MAX_ZHT";
const string Const::ZC_NUM_REPLICAS = "NUM_REPLICAS";
const string Const::ZC_REPLICATION_TYPE = "REPLICATION_TYPE";
const string Const::ZC_HTDATA_PATH = "HTDATA_PATH";
const string Const::ZC_MIGSLP_TIME = "MIGSLP_TIME";
const string Const::ZC_PROTOCOL = "PROTOCOL";

const string Const::PROTO_NAME = "PROTOCOL";
const string Const::PROTO_PORT = "PORT";
const string Const::PROTO_VAL_TCP = "TCP";
const string Const::PROTO_VAL_UDP = "UDP";
const string Const::PROTO_VAL_UDT = "UDT";
const string Const::PROTO_VAL_MPI = "MPI";

const string Const::MSG_MAXSIZE = "MSG_MAXSIZE";

const string Const::SCCB_POLL_INTERVAL = "SCCB_POLL_INTERVAL";

const string Const::INSTANT_SWAP = "INSTANT_SWAP";

const string Const::ASC_OPC_AZ_ALL = "101";
const string Const::ASC_OPC_AZ_PORT = "102";
const string Const::ASC_OPC_AZ_IPPORT = "103";
const string Const::ASC_OPC_RZ_ALL = "104";
const string Const::ASC_OPC_RZ_PORT = "105";
const string Const::ASC_OPC_RZ_IPPORT = "106";
const string Const::ASC_REC_NTHDOZM = "197";
const string Const::ASC_REC_UOPC = "198";
const string Const::ASC_REC_UNPR = "199";
const string Const::ASC_REC_SUCC = "100";

const int Const::ASI_OPC_AZ_ALL = 101;
const int Const::ASI_OPC_AZ_PORT = 102;
const int Const::ASI_OPC_AZ_IPPORT = 103;
const int Const::ASI_OPC_RZ_ALL = 104;
const int Const::ASI_OPC_RZ_PORT = 105;
const int Const::ASI_OPC_RZ_IPPORT = 106;
const int Const::ASI_REC_NTHDOZM = 197;
const int Const::ASI_REC_UOPC = 198;
const int Const::ASI_REC_UNPR = 199;
const int Const::ASI_REC_SUCC = 100;

const string Const::ZSC_OPC_LOOKUP = "001";
const string Const::ZSC_OPC_REMOVE = "002";
const string Const::ZSC_OPC_INSERT = "003";
const string Const::ZSC_OPC_APPEND = "004";
const string Const::ZSC_OPC_CMPSWP = "005";
const string Const::ZSC_OPC_STCHGCB = "006";
const string Const::ZSC_OPC_BRDDN_GMEM = "087";
const string Const::ZSC_OPC_OPR_CANCEL = "088";
const string Const::ZSC_OPC_GET_ASNGHB = "089";
const string Const::ZSC_OPC_BRD_GMEM = "090";
const string Const::ZSC_OPC_GET_GMEM = "091";
const string Const::ZSC_OPC_GET_DESTZHT = "092";
const string Const::ZSC_OPC_PULLFILE = "093";
const string Const::ZSC_OPC_MIGDONETGT = "094";
const string Const::ZSC_OPC_MIGDONESRC = "095";
const string Const::ZSC_OPC_MIGTARGET = "096";
const string Const::ZSC_OPC_MIGSOURCE = "097";
const string Const::ZSC_REC_EMPTYKEY = "-01";
const string Const::ZSC_REC_CLTFAIL = "-02";
const string Const::ZSC_REC_SRVFAIL = "-03";
const string Const::ZSC_REC_SRVEXP = "-04";
const string Const::ZSC_REC_SCCBPOLLTRY = "-91";
const string Const::ZSC_REC_NONEXISTKEY = "-92";
const string Const::ZSC_REC_NODESTZHT = "-93";
const string Const::ZSC_REC_NONEEDMIG = "-94";
const string Const::ZSC_REC_FLPUSHFAIL = "-95";
const string Const::ZSC_REC_FLPULLFAIL = "-96";
const string Const::ZSC_REC_SECDTRY = "-97";
const string Const::ZSC_REC_UOPC = "-98";
const string Const::ZSC_REC_UNPR = "-99";
const string Const::ZSC_REC_SUCC = "000";

const int Const::ZSI_OPC_LOOKUP = 1;
const int Const::ZSI_OPC_REMOVE = 2;
const int Const::ZSI_OPC_INSERT = 3;
const int Const::ZSI_OPC_APPEND = 4;
const int Const::ZSI_OPC_CMPSWP = 5;
const int Const::ZSI_OPC_BRDDN_GMEM = 87;
const int Const::ZSI_OPC_OPR_CANCEL = 88;
const int Const::ZSI_OPC_GET_ASNGHB = 89;
const int Const::ZSI_OPC_BRD_GMEM = 90;
const int Const::ZSI_OPC_GET_GMEM = 91;
const int Const::ZSI_OPC_GET_DESTZHT = 92;
const int Const::ZSI_OPC_PULLFILE = 93;
const int Const::ZSI_OPC_MIGDONETGT = 94;
const int Const::ZSI_OPC_MIGDONESRC = 95;
const int Const::ZSI_OPC_MIGTARGET = 96;
const int Const::ZSI_OPC_MIGSOURCE = 97;
const int Const::ZSI_REC_EMPTYKEY = -1;
const int Const::ZSI_REC_CLTFAIL = -2;
const int Const::ZSI_REC_SRVFAIL = -3;
const int Const::ZSI_REC_SRVEXP = -4;
const int Const::ZSI_REC_NONEXISTKEY = -92;
const int Const::ZSI_REC_NODESTZHT = -93;
const int Const::ZSI_REC_NONEEDMIG = -94;
const int Const::ZSI_REC_FLPUSHFAIL = -95;
const int Const::ZSI_REC_FLPULLFAIL = -96;
const int Const::ZSI_REC_SECDTRY = -97;
const int Const::ZSI_REC_UOPC = -98;
const int Const::ZSI_REC_UNPR = -99;
const int Const::ZSI_REC_SUCC = 0;

Const::Const() {
}

Const::~Const() {
}

string Const::trim(const string& value) {

	string str = value;
	stringstream trimmer;
	trimmer << str;
	trimmer >> str;

	return str;
}

} /* namespace dm */
} /* namespace zht */
} /* namespace datasys */
} /* namespace iit */
