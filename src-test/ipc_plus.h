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
 * ipc_plus.h
 *
 *  Created on: Jul 5, 2013
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#ifndef	H_include_IPC
#define	H_include_IPC
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/// Maximum message size.
#define IPC_MAX_MSG_SZ  102400
/// Lowest message priority.
#define IPC_MIN_MSG_PRI 100

/**
 * IPCplus
 *
 * These classes are simple wrappers around the System V IPC mechanism.
 */
namespace IPC {
/**
 * System V IPC shared memory.
 *
 * This class is intended to be a base class. See the test program for a sample derived class.
 */

/**
 * System V IPC semaphore set.
 */

/**
 * System V message queue.
 */
class Msg {
private:
	/**
	 * The Sys V IPC message queue id.
	 */
	int qid;
public:
	/**
	 * The current message.
	 */
	struct {
		long mtype;
		char mdata[IPC_MAX_MSG_SZ];
	} msgbuf;
	/**
	 * The current message size.
	 */
	unsigned int mSize;
	/**
	 * Default constructor, creates a private IPC message queue.
	 */
	Msg();
	/**
	 * Constructor, creates a non-private IPC message queue.
	 * \param key This is the "name" of the queue. By using the same value, different processes can
	 * obtain access to the same message queue. Obviously, some method must be deployed to both
	 * insure that this key is unique in the system's shared queue space as well as known to those
	 * processes that wish to share a queue. See ftok() for one possible method to determine such a
	 * key value.
	 * \param createIfNotExist Should be pretty evident.
	 */
	Msg(const unsigned int key, const bool createIfNotExist = true);
	/**
	 * Destructor.
	 */
	virtual ~Msg() {

//		destroy(); //zhouxb
	}
	;
	/**
	 * Send the current message.
	 *
	 * The caller is responsable for having set all the fields correctly.
	 * \return False if this didn't work.
	 */
	bool Snd();
	/**
	 * Simple send of a block of data.
	 *
	 * Places a message in the queue with the passed data.
	 * \param data The bytes to send.
	 * \param size The number of bytes to send.
	 * \param mType Optionally a priority or message type may be specified. This <b>must</b> be a
	 * positive integer.
	 * \return False if this didn't work.
	 */
	bool Snd(const void *data, const size_t size, const int mType = 1);
	/**
	 * Simple send of a C-type string.
	 *
	 * Places a message in the queue with mSize==strlen(string) + 1 and mdata==string.
	 * \param string The zero terminated string to send.
	 * \param mType Optionally a priority or message type may be specified. This <b>must</b> be a
	 * positive integer.
	 * \return False if this didn't work.
	 */
	bool Snd(const char *string, const int mType = 1) {
		return Snd(string, strlen(string) + 1, mType);
	}
	;
	/**
	 * Receive any message.
	 *
	 * Blocks until a message is available.
	 * \return False if this didn't work.
	 */
	bool Rcv(const int mType = 0, const int mFlag = 0);
	/**
	 * Receive a priority message.
	 *
	 * The messages in the queue may have different priorities. This method will
	 * return the message with a highest priority.
	 * Blocks until a message is available.
	 * \param mType Optionally a priority or message type may be specified. This <b>must</b> be a
	 * positive integer.
	 * \param mFlag See man msgrcv.
	 * \return False if this didn't work.
	 */
	bool RcvHigh() {
		return Rcv(-IPC_MIN_MSG_PRI);
	}
	;
	/**
	 * Receive a specific type of message.
	 *
	 * The messages in the queue may be qualified to be of different types. This
	 * method will return only messages with the indicated message type code.
	 * \param mType Must be a positive integer.
	 * \return False if this didn't work.
	 */
	bool RcvType(const int mType) {
		return Rcv(mType);
	}
	;
	/**
	 * Destroy the actual IPC queue.
	 *
	 * \return False if the destroy operation returned an error.
	 */
	virtual bool destroy();
	/**
	 * Return the number of messages in the queue.
	 *
	 * \return The number of queued messages.
	 */
	unsigned int numMsg();

};

/**
 * This class allows for two-way communication.
 *
 * Two message queues are used to avoid deadlocks. Co-operating processes should
 * use one queue to transmit "requests" and the other queue to receive "replies".
 */
class TwoWayMsg {
protected:
	/// The send and receive queues.
	Msg *req, *rep;
public:
	/**
	 * Constructs a double Sys V IPC queue.
	 *
	 * \param key0 Will be the transmit queue and <b>key0 + 1</b> will be the receive queue.
	 */
	TwoWayMsg(const unsigned int key0) {
		req = new Msg(key0);
		rep = new Msg(key0 + 1);
	}
	;
	/**
	 * Destructor. Does not delete the system queues themselves.
	 *
	 * Also see destroy().
	 */
	virtual ~TwoWayMsg() {

		/*delete req;
		 delete rep;*/ //zhouxb
	}
	;
	/**
	 * Destroys the system queues.
	 *
	 * After destroy () has been called the class becomes inoperative.
	 * \return False if either of the sub-destroys failed.
	 */
	virtual bool destroy() {

		bool r1 = false;
		if (req) {
			r1 = req->destroy();
		}
		bool r2 = false;
		if (rep) {
			r2 = rep->destroy();
		}
		// assure nobody will use these anymore

		delete req;
		req = NULL;

		delete rep;
		rep = NULL;

		return r1 && r2;
	}
	;
	/**
	 * Transmit a request.
	 *
	 * \param data The bytes to send.
	 * \param size The number of bytes to send.
	 * \param mType Optionally a priority or message type may be specified. This <b>must</b> be a
	 * positive integer.
	 * \return False if this didn't work.
	 */
	virtual bool xmitReq(const void *data, const size_t size, const int mType =
			1) {
		return req->Snd(data, size, mType);
	}
	;
	/**
	 * Transmit a reply.
	 *
	 * \param data The bytes to send.
	 * \param size The number of bytes to send.
	 * \param mType Optionally a priority or message type may be specified. This <b>must</b> be a
	 * positive integer.
	 * \return False if this didn't work.
	 */
	virtual bool xmitRep(const void *data, const size_t size, const int mType =
			1) {
		return rep->Snd(data, size, mType);
	}
	;
	/**
	 * Receive a request.
	 *
	 * \param mData Must be a pointer to a buffer of the correct size.
	 * \param mSize Will hold the number of bytes received.
	 * \param mType Must be a positive integer.
	 * \return False if this didn't work.
	 */
	virtual bool recvReq(void *mData, size_t &mSize, const int mType = 1) {
		if (req->Rcv(mType)) {
			memcpy(mData, req->msgbuf.mdata, mSize = req->mSize);
			return true;
		}
		return false;
	}
	;
	/**
	 * Receive a reply.
	 *
	 * \param mData Must be a pointer to a buffer of the correct size.
	 * \param mSize Will hold the number of bytes received.
	 * \param mType Must be a positive integer.
	 * \return False if this didn't work.
	 */
	virtual bool recvRep(void *mData, size_t &mSize, const int mType = 1) {
		if (rep->Rcv(mType)) {
			memcpy(mData, rep->msgbuf.mdata, mSize = rep->mSize);
			return true;
		}
		return false;
	}
	;
};

/**
 * This class uses TwoWayMsg for the server side of a single server/multiple client scenario.
 */
class MsgServer: public TwoWayMsg {
private:
	/// Used for the automatic client identification.
	int nextClient;
public:
	/**
	 * Constructor.
	 */
	MsgServer(const unsigned int key0) :
			TwoWayMsg(key0), nextClient(0x100) {
	}
	;
	/**
	 * Destructor.
	 */
	virtual ~MsgServer() {
//		destroy(); //zhouxb
	}
	;
	/**
	 * Transmit a reply to the current client.
	 *
	 * The current client is determined by the recv() method below.
	 * \param data The bytes to send.
	 * \param size The number of bytes to send.
	 * \return False if this didn't work.
	 */
	virtual bool xmit(const void *data, const size_t size) {
		return xmitRep(data, size, req->msgbuf.mtype);
	}
	;
	/**
	 * Receive a request.
	 *
	 * This methode automatically handles client registration in conjunction with the MsgClient
	 * class below. Clients register with the server to obtain a unique identification (address)
	 * which is handled transparantly by this method.
	 * The server MUST call xmit() before calling recv() again or the client will hang.
	 * \param mData Must be a pointer to a buffer of the correct size.
	 * \param mSize Will hold the number of bytes received.
	 * \return False if this didn't work.
	 */
	virtual bool recv(void *mData, size_t &mSize);
};

/**
 * This class uses TwoWayMsg for the client side of a single server/multiple client scenario.
 */
class MsgClient: public TwoWayMsg {
private:
	/**
	 * The address or client identification of this client.
	 *
	 * Initially set to zero and automatically assigned upon the first request performed.
	 */
	int myAddress;
public:
	/**
	 * Constructor.
	 */
	MsgClient(const unsigned int key0) :
			TwoWayMsg(key0), myAddress(0) {
	}
	;
	/**
	 * Destructor.
	 */
	virtual ~MsgClient() {
	}
	;
	/**
	 * Transmit a request.
	 * 
	 * The first time a client calls xmit, the registration with the server is performed.
	 * \param data The bytes to send.
	 * \param size The number of bytes to send.
	 * \return False if this didn't work.
	 */
	virtual bool xmit(const void *data, const size_t size);
	/**
	 * Receive a reply.
	 *
	 * \param mData Must be a pointer to a buffer of the correct size.
	 * \param mSize Will hold the number of bytes received.
	 * \return False if this didn't work.
	 */
	virtual bool recv(void *mData, size_t &mSize) {
		if (!myAddress) {
			return false;
		}
		return recvRep(mData, mSize, myAddress);
	}
	;
};
} // namespace IPC

#endif	/*H_include_IPC*/

