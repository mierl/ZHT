/*
 * net_util.h
 *
 *  Created on: Feb 15, 2012
 *      Author: tony
 */

#ifndef NET_UTIL_H_
#define NET_UTIL_H_

#include "stddef.h"
#include <sys/types.h>

#include <vector>
#include <map>
#include <set>
#include <queue>
#include "meta.pb.h"
using namespace std;

int makeSvrSocket(int port, bool tcp);
int svr_accept(int sock, bool tcp);
int makeClientSocket(const char* host, int port, bool tcp);

int generalSendTo(const char* host, int port, int to_sock, const char* buf,
		int bufsize, bool tcp);

ssize_t sendDataBySize(const int& to_sock, const char *byteBuf,
		const ssize_t& bytesToSend);

ssize_t sendWholeData(const int& to_sock, const char *byteBuf,
		const size_t& bytesToSend);

int generalSendBack(int to_sock, const char* buf, int bufsize,
		struct sockaddr_in sendbackAddr, int flag, bool tcp);

int generalReceive(int sock, void* buf, int bufsize,
		struct sockaddr_in & recvAddr, int flag, bool tcp);

int loopedReceive(int sock, string& srecv, struct sockaddr_in & recvAddr,
		int flag, bool tcp);

int generalSendTCP(int to_sock, const char* buf, int bufsize);
int generalReveiveTCP(int sock, void *buf, size_t bufsize, int flags);

ssize_t recvDataSize(const int& from_sock, size_t& bytesToRead);

ssize_t recvDataBySize(const int& from_sock, char *byteBuf, size_t bytesToRead);

ssize_t recvWholeData(const int& from_sock, char *byteBuf,
		const size_t& bytesToRead);

//int serverReceive(int sock, void *buffer, size_t size, int flags, bool tcp);

int udpSendTo(int toSock, const char* host, int port, const char* buf,
		int bufsize);
int udpRecvFrom(int sock, void* buf, int bufsize, struct sockaddr_in & recvAddr,
		int flag);
int udpSendBack(int sock, const char* buf, int bufsize,
		struct sockaddr_in sendbackAddr, int flag);
int reuseSock(int sock);
int setRecvTimeout(int sockfd, unsigned int sec, unsigned int usec);

/*Big data is splitted into many blobs so as to be transfered over the network*/
class Blob {
public:
	Blob(const string& blob);
	Blob(const uint64_t& uuid, const size_t& seqNum, const size_t& total,
			const string& value);

	virtual ~Blob();

public:
	uint64_t uuid() const;
	void uuid(const uint64_t& uuid);

	size_t seqNum() const;
	void seqNum(const size_t& seqNum);

	size_t total() const;
	void total(const size_t& total);

	string value() const;
	void value(const string& value);

	size_t size() const;
	void size(const size_t& size);

	string toString() const;
	Blob& assign(const string& blob);

	static uint getHeaderLen();

public:
	static uint getUuidLen();
	static uint getCountLen();
	static uint getSizeLen();

	string getUuidFormat() const;
	string getCountFormat() const;
	string getSizeFormat() const;

	string format(const string& format, const size_t& value) const;

private:
	uint64_t _uuid; //62 chars,
	size_t _seqNum; //7 chars, like 1000000, max msg is 1000000*1K = 1G
	size_t _total; //7 chars, like 1000000, max msg is 1000000*1K = 1G
	size_t _size; //4 chars, like 1024
	string _value; //
};

/*BdSendBase: big data send base class*/
class BdSendBase {
public:
	typedef vector<Blob> VEC;
	typedef vector<Blob>::const_iterator VI;
	typedef vector<Blob>::reverse_iterator VRI;

public:
	BdSendBase(const string& msg);
	virtual ~BdSendBase();

	int bsend(int sock, bool tcp) const;
	int bsend(const char* host, int port, int sock, bool tcp) const;

protected:
	virtual int recAck(int sock, const uint64_t& ackid,
			const bool& terminate) const = 0;

private:
	void split();

private:
	VEC _blobs;
	string _msg;

private:
	uint BLOB_SIZE;
};

/*BdSendToClient: big data send to client*/
class BdSendToClient: public BdSendBase {
public:
	BdSendToClient(const string& msg);
	~BdSendToClient();

protected:
	int recAck(int sock, const uint64_t& ackid, const bool& terminate) const;
};

/*BdSendToServer: big data send to server*/
class BdSendToServer: public BdSendBase {
public:
	BdSendToServer(const string& msg);
	~BdSendToServer();

protected:
	int recAck(int sock, const uint64_t& ackid, const bool& terminate) const;
};

/*BdRecvBase: big data receive base class*/
class BdRecvBase {

public:
	class classcomp {
	public:
		bool operator()(const Blob& lhs, const Blob& rhs) const;
	};

public:
	typedef set<Blob, classcomp> SET;
	typedef set<Blob, classcomp>::iterator SI;
	typedef set<Blob, classcomp>::reverse_iterator SRI;
	typedef map<uint64_t, SET> MAP;
	typedef map<uint64_t, SET>::iterator MI;
	typedef map<uint64_t, SET>::reverse_iterator MRI;

public:
	BdRecvBase();
	virtual ~BdRecvBase();

	string getBdStr(int sock, const char * const buf, size_t count,
			bool& ready);

protected:
	virtual int sendAck(int sock, const uint64_t& ackid) const = 0;

private:
	string combineBlobs(const uint64_t& uuid);
	void deleteBlobs(const uint64_t& uuid);

private:
	MAP _BdMap;
};

/*BdRecvFromClient: big data receive from client*/
class BdRecvFromClient: public BdRecvBase {
protected:
	int sendAck(int sock, const uint64_t& ackid) const;
};

/*BdRecvFromServer: big data receive from server*/
class BdRecvFromServer: public BdRecvBase {
protected:
	int sendAck(int sock, const uint64_t& ackid) const;
};

template<class ID>
class AckQueue {
public:
	typedef queue<Package> QU;
	typedef map<ID, QU> MAP;
	typedef typename map<ID, QU>::iterator MI;
	typedef typename map<ID, QU>::const_iterator MCI;
	typedef typename map<ID, QU>::reverse_iterator MRI;

public:
	AckQueue();
	~AckQueue();

	bool empty(const ID& id) const;
	size_t size(const ID& id) const;
	const Package& front(const ID& id) const;
	const Package& back(const ID& id) const;

	void push(const ID& id, Package ack);
	void pop(const ID& id);

	void rmQueue(const ID& id);

private:
	MAP _QMap;

};

void enqueueAck(const Package& ack);
#endif /* NET_UTIL_H_ */

