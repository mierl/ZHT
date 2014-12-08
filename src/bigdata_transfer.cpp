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
 * bigdata_transfer.cpp
 *
 *  Created on: Jun 25, 2013
 *      Author: Xiaobingo
 *      Contributor: Tony, KWang, DZhao
 */

#include "bigdata_transfer.h"

#include "Env.h"
#include "ZHTUtil.h"

#include <stdio.h>

Blob::Blob(const string& blob) {

	assign(blob);
}

Blob::Blob(const uint64_t& uuid, const size_t& seqNum, const size_t& total,
		const string& value) :
		_uuid(uuid), _seqNum(seqNum), _total(total), _value(value), _size(
				value.size()) {
}

Blob::~Blob() {
}

uint64_t Blob::uuid() const {

	return _uuid;
}

void Blob::uuid(const uint64_t& uuid) {

	_uuid = uuid;
}

size_t Blob::seqNum() const {

	return _seqNum;
}

void Blob::seqNum(const size_t& seqNum) {

	_seqNum = seqNum;
}

size_t Blob::total() const {

	return _total;
}

void Blob::total(const size_t& total) {

	_total = total;
}

string Blob::value() const {

	return _value;
}

void Blob::value(const string& value) {

	_value = value;
}

size_t Blob::size() const {

	return _size;
}

void Blob::size(const size_t& size) {

	_size = size;
}

string Blob::toString() const {

	string result;

	result.append(format(getUuidFormat(), uuid()));

	result.append(format(getCountFormat(), seqNum()));
	result.append(format(getCountFormat(), total()));

	result.append(format(getSizeFormat(), size()));

	result.append(value());

	return result;
}

Blob& Blob::assign(const string& blob) {
	//add header, total length is 38 bytes.
	size_t sc = 0;

	uuid(strtoull(blob.substr(sc, getUuidLen()).c_str(), NULL, 10));
	sc += getUuidLen();

	seqNum(strtoul(blob.substr(sc, getCountLen()).c_str(), NULL, 10));
	sc += getCountLen();

	total(strtoul(blob.substr(sc, getCountLen()).c_str(), NULL, 10));
	sc += getCountLen();

	size(strtoul(blob.substr(sc, getSizeLen()).c_str(), NULL, 10));
	sc += getSizeLen();

	value(blob.substr(sc));

	return *this;
}

uint Blob::getHeaderLen() {

	return getUuidLen() + getCountLen() * 2 + getSizeLen();
}

uint Blob::getUuidLen() {

	return IdHelper::ID_LEN;
}

uint Blob::getCountLen() {

	return 7;
}

uint Blob::getSizeLen() {

	return 4;
}

string Blob::getUuidFormat() const {

	return "%020llu"; //20 chars, like 92233720368547758081, unique message id
}

string Blob::getCountFormat() const {

	return "%07lu"; //7 chars, like 1000000, max msg is 1000000*1K = 1G
}

string Blob::getSizeFormat() const {

	return "%04lu"; //4 chars, like 1024, each msg is 1K
}

string Blob::format(const string& format, const size_t& value) const {

	char buf[50];
	memset(buf, 0, sizeof(buf));
	int n = sprintf(buf, format.c_str(), value);

	return string(buf, 0, n);
}

BdSendBase::BdSendBase(const char *msg) :
		_msg(msg), BLOB_SIZE(Env::BUF_SIZE - Blob::getHeaderLen()) {

	split();
}

BdSendBase::BdSendBase(const string& msg) :
		_msg(msg), BLOB_SIZE(Env::BUF_SIZE - Blob::getHeaderLen()) {

	split();
}

int BdSendBase::bsend(int sock) const {

	bsend(sock, NULL);
}

int BdSendBase::bsend(int sock, void *senderAddr) const {

	int count = 0;
	socklen_t addr_len = sizeof(struct sockaddr);

	int i = 0;
	for (VI it = _blobs.begin(); it != _blobs.end(); it++) {

		string bstr = it->toString();

		if (senderAddr == NULL)
			count += send(sock, bstr.c_str(), bstr.size(), 0);
		else
			count += sendto(sock, bstr.c_str(), bstr.size(), 0,
					(struct sockaddr *) senderAddr, sizeof(struct sockaddr));

		/*remove the header, since the caller probably wants knowing real-sent-size*/
		count -= it->getHeaderLen();

		bool terminate = ++i == _blobs.size() ? true : false;

		/*wait for ack of this blob*/
		recAck(sock, it->uuid(), terminate);
	}

	return count;
}

BdSendBase::~BdSendBase() {
}

void BdSendBase::split() {

	uint64_t uuid = IdHelper::genId();

	size_t size = _msg.size();

	size_t fold = size / BLOB_SIZE;
	size_t mod = size % BLOB_SIZE;

	size_t total = 0;
	if (mod != 0)
		total = fold + 1;
	else
		total = fold;

	int seqNum = 0;
	for (size_t i = 0; i < fold; i++) {

		string value = _msg.substr(i * BLOB_SIZE, BLOB_SIZE);

		Blob blob(uuid, ++seqNum, total, value);

		_blobs.push_back(blob);
	}

	if (mod != 0) {

		string value = _msg.substr(fold * BLOB_SIZE, mod);

		Blob blob(uuid, ++seqNum, total, value);

		_blobs.push_back(blob);
	}
}

BdSendToServer::BdSendToServer(const char *msg) :
		BdSendBase(msg) {
}

BdSendToServer::BdSendToServer(const string& msg) :
		BdSendBase(msg) {
}

BdSendToServer::~BdSendToServer() {
}

int BdSendToServer::recAck(int sock, const uint64_t& ackid,
		const bool& terminate) const {

	/*wait for ack of this blob*/
	/*char buf[Env::MAX_MSG_SIZE];
	 memset(buf, 0, sizeof(buf));
	 int count = recv(sock, buf, sizeof(buf), 0);

	 return count;*/

	return 0;
}

BdSendToClient::BdSendToClient(const char *msg) :
		BdSendBase(msg) {
}

BdSendToClient::BdSendToClient(const string& msg) :
		BdSendBase(msg) {
}

BdSendToClient::~BdSendToClient() {

}

int BdSendToClient::recAck(int sock, const uint64_t& ackid,
		const bool& terminate) const {

	/*wait for ack of this blob*/
	/*	while (ACK_QUEUE.empty(ackid)) {

	 int i = 0;
	 }

	 ACK_QUEUE.pop(ackid);

	 if (terminate)
	 ACK_QUEUE.rmQueue(ackid);*/

	return 0;
}

string BdRecvBase::getBdStr(int sock, const char * const buf, size_t count,
		bool& ready) {

	string result;

	/*allocate new mem and copy the buf into mem*/
	/*	char *bbuf = (char*) calloc(str.size() + 1, sizeof(char));
	 strncpy(bbuf, str.c_str(), str.size());
	 Blob blob(bbuf);*/

	Blob blob(string(buf, 0, count));

	if (blob.total() == 1) { //only one blob

		ready = true;
		result = blob.value();

	} else {

		MI it = _BdMap.find(blob.uuid());

		if (it == _BdMap.end()) { //first time to receive a blob of big data

			SET set;
			_BdMap[blob.uuid()] = set; //init a set to store blobs of big data
			_BdMap[blob.uuid()].insert(blob); //put the blob into the set

		} else {

			_BdMap[blob.uuid()].insert(blob); //put the blob into the set

			size_t size = _BdMap[blob.uuid()].size();

			if (size == blob.total()) { //receive all blobs of big data

				ready = true;
				result = combineBlobs(blob.uuid());

				deleteBlobs(blob.uuid());
			}
		}
	}

	sendAck(sock, blob.uuid());

	return result;
}

bool BdRecvBase::classcomp::operator()(const Blob& lhs, const Blob& rhs) const {

	return lhs.seqNum() < rhs.seqNum();
}

BdRecvBase::BdRecvBase() {
}

BdRecvBase::~BdRecvBase() {
}

string BdRecvBase::combineBlobs(const uint64_t& uuid) {

	string result;

	SET *ps = &_BdMap[uuid];

	for (SI it = ps->begin(); it != ps->end(); it++) {

		result.append(it->value());
	}

	return result;
}

void BdRecvBase::deleteBlobs(const uint64_t& uuid) {

	_BdMap.erase(_BdMap.find(uuid));
}

int BdRecvFromClient::sendAck(int sock, const uint64_t& ackid) const {

	/*send ack of this blob to sender*/
	/*	ZPack zpack;
	 zpack.set_opcode("ack");
	 zpack.set_rcode("000");

	 string ack = zpack.SerializeAsString();

	 return send(sock, ack.c_str(), ack.size(), 0);*/

	return 0;
}

int BdRecvFromServer::sendAck(int sock, const uint64_t& ackid) const {

	/*send ack of this blob to sender*/
	/*ZPack zpack;
	 zpack.set_opcode("ack");
	 zpack.set_rcode("000");
	 zpack.set_ackid(ackid);

	 BdSendBase *pbsb = new BdSendToServer(zpack.SerializeAsString());
	 int count = pbsb->bsend(sock, true);

	 delete pbsb;
	 pbsb = NULL;

	 return count;*/

	return 0;
}

template<class ID>
AckQueue<ID>::AckQueue() {
}

template<class ID>
AckQueue<ID>::~AckQueue() {
}

template<class ID>
bool AckQueue<ID>::empty(const ID& id) const {

	MCI it = _QMap.find(id);

	if (it == _QMap.end()) {

		return true;

	} else {

		return it->second.empty();
	}
}

template<class ID>
size_t AckQueue<ID>::size(const ID& id) const {

	MCI it = _QMap.find(id);

	if (it == _QMap.end()) {

		return 0;

	} else {

		return it->second.size();
	}
}

template<class ID>
const ZPack& AckQueue<ID>::front(const ID& id) const {

	MCI it = _QMap.find(id);

	return it->second.front();
}

template<class ID>
const ZPack& AckQueue<ID>::back(const ID& id) const {

	MCI it = _QMap.find(id);

	return it->second.back();
}

template<class ID>
void AckQueue<ID>::push(const ID& id, ZPack ack) {

	MI it = _QMap.find(id);

	if (it == _QMap.end()) {

		QU queue;
		_QMap[id] = queue;
		_QMap[id].push(ack);

	} else {

		it->second.push(ack);
	}
}

template<class ID>
void AckQueue<ID>::pop(const ID& id) {

	MI it = _QMap.find(id);

	if (it == _QMap.end()) {

		return;

	} else {

		it->second.pop();
	}
}

template<class ID>
void AckQueue<ID>::rmQueue(const ID& id) {

	_QMap.erase(_QMap.find(id));
}
