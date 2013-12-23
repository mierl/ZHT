/*
 * Copyright 2013-2020
 *      Advisor: Zhiling Lan(lan@iit.edu), Ioan Raicu(iraicu@cs.iit.edu)
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
 * This file is part of iit::cs550::finalproj(Diskon: Distributed tasK executiON framework)
 *      Xiaobing Zhou(xzhou40@hawk.iit.edu) with nickname Xiaobingo,
 *      Hao Chen(hchen71@hawk.iit.edu) with nickname Hchen,
 *		Iman Sadooghi(isadoogh@iit.edu) with nickname Iman,
 *		Ke Wang(kwang22@hawk.iit.edu) with nickname KWang.
 *
 * TSafeQueue-impl.h
 *
 *  Created on: Nov 20, 2013
 *      Author: Xiaobingo, Hchen
 *      Contributor: Iman, KWang
 */

#ifndef TSAFEQUEUE_IMPL_H_
#define TSAFEQUEUE_IMPL_H_

#include "TSafeQueue.h"
#include "lock_guard.h"

namespace iit {
namespace cs550 {
namespace finalproj {

template<typename T>
TSafeQueue<T>::TSafeQueue() :
		_mutex() {
	init();
}

template<typename T>
TSafeQueue<T>::~TSafeQueue() {

	pthread_mutex_destroy(&_mutex);
}

template<typename T>
void TSafeQueue<T>::init() {

	pthread_mutex_init(&_mutex, NULL);
}

template<typename T>
size_t TSafeQueue<T>::size() {

	LockGuard lock(&_mutex);
	//unique_lock < mutex > lck(_mutex);
	return _queue.size();
}

template<typename T>
bool TSafeQueue<T>::push(const T &element) {

	LockGuard lock(&_mutex);
	//unique_lock < mutex > lck(_mutex);
	_queue.push(element);

	return true;
}

template<typename T>
bool TSafeQueue<T>::pop(T &element) {

	LockGuard lock(&_mutex);
	//unique_lock < mutex > lck(_mutex);

	bool result = false;
	if (!_queue.empty()) {

		element = _queue.front();
		_queue.pop();
		result = true;
	}

	return result;
}

template<typename T>
void TSafeQueue<T>::pop_n(int n) {

	LockGuard lock(&_mutex);
	//unique_lock < mutex > lck(_mutex);

	for (int i = 0; i < n; i++) {
		if (!_queue.empty())
			_queue.pop();
	}
}

template<typename T>
void TSafeQueue<T>::pop_all() {

	LockGuard lock(&_mutex);
	//std::unique_lock < std::mutex > lck(_mutex);

	while (!_queue.empty())
		_queue.pop();
}

#ifdef BQUEUE
template<typename T>
TSafeQueue<T>::TSafeQueue() :
_count(0), _queue(5600000) {
}

template<typename T>
TSafeQueue<T>::TSafeQueue(int reservation) :
_count(0), _queue(reservation) {
}

template<typename T>
TSafeQueue<T>::~TSafeQueue() {
}

template<typename T>
bool TSafeQueue<T>::empty() {

	return _count == 0;
}

template<typename T>
size_t TSafeQueue<T>::size() {

	return _count;
}

template<typename T>
bool TSafeQueue<T>::push(const T &element) {

	bool result;

	while (!(result = _queue.push(element)));
	++_count;

	return result;
}

template<typename T>
bool TSafeQueue<T>::pop(T &element) {

	bool result;

	while (!(result = _queue.pop(element)));
	--_count

	return result;
}

template<typename T>
bool TSafeQueue<T>::pop_n(uint n) {

	bool result = true;

	int min = min(n, _count)

	T ele;
	for (int i = 0; i < min; ++i) {

		result &= pop(ele);
	}

	return result;
}

template<typename T>
bool TSafeQueue<T>::pop_all() {

	bool result;
	T ele;

	for (int i = 0; i < _count; ++i) {

		result &= pop(ele);
	}
}
#endif

}
/* namespace finalproj */
} /* namespace cs550 */
} /* namespace iit */
#endif /* TSAFEQUEUE_IMPL_H_ */
