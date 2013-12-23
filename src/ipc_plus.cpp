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
 * ipc_plus.cpp
 *
 *  Created on: Jul 5, 2013
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include "ipc_plus.h"
/// only Sun needs this...
//#define _XOPEN_VERSION 5
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <errno.h>

namespace IPC {

Msg::Msg() {
	msgbuf.mtype = 0;
	mSize = 0;
	qid = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0777);
	if (qid < 0) {
		perror("msgget failed");
	}
}

Msg::Msg(const unsigned int key, const bool createIfNotExist) {
	msgbuf.mtype = 0;
	mSize = 0;
	int f = 0777;
	if (createIfNotExist) {
		f |= IPC_CREAT;
	}
	qid = msgget(key, f);
	if (qid < 0) {
		perror("msgget failed");
	}
}

bool Msg::destroy() {
	struct msqid_ds buf;
	return msgctl(qid, IPC_RMID, &buf) > -1;
}

bool Msg::Snd(const void *data, const size_t size, const int mType) {
	if (mType <= 0) {
		return false;
	}
	mSize = size;
	if (mSize > IPC_MAX_MSG_SZ) {
		return false;
	}
	memcpy(msgbuf.mdata, data, mSize);
	msgbuf.mtype = mType;
	return Snd();
}

bool Msg::Snd() {
	int r;
	do {
		r = msgsnd(qid, &msgbuf, mSize, 0);
	} while (r < 0 && errno == EINTR);
	if (r < 0) {
		perror("msgsnd failed");
		return false;
	}
	return true;
}

bool Msg::Rcv(const int mType, const int mFlag) {
	int r;
	do {
		r = msgrcv(qid, &msgbuf, IPC_MAX_MSG_SZ, mType, mFlag);
	} while (r < 0 && errno == EINTR);
	if (r < 0) {
		perror("msgrcv failed");
		return false;
	} else {
		mSize = (unsigned int) r;
	}
	return true;
}

unsigned int Msg::numMsg() {
	struct msqid_ds buf;
	if (msgctl(qid, IPC_STAT, &buf) < 0) {
		buf.msg_qnum = 0;
	}
	return buf.msg_qnum;
}

bool MsgServer::recv(void *mData, size_t &mSize) {

	while (recvReq(mData, mSize, 0)) {
		if (req->msgbuf.mtype == 1) {
			TwoWayMsg::xmitRep(&nextClient, sizeof(nextClient), 2);
			nextClient++;
			// This is very primitive... and error prone!
			if (nextClient > 32000) {
				nextClient = 3;
			}
		} else {
			return true;
		}
	}
	return false;
}

bool MsgClient::xmit(const void *data, const size_t size) {
	if (!myAddress) {
		if (!xmitReq("R", 1, 1)) {
			return false;
		}
		size_t dummy;
		// Note that this may be the reply to the registration request of another process.
		// This does not matter as in that case the other process will obtain the reply to
		// our registration request.
		if (!recvRep(&myAddress, dummy, 2)) {
			return false;
		}
	}
	return xmitReq(data, size, myAddress);
}
} // namespace IPC
