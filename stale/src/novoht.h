/*
 * Copyright 2012 Kevin Brandstatter
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
 */
#ifndef PHASHMAP_H
#define PHASHMAP_H
#include "novoht.h"
#include <string>
#include <semaphore.h>
#include <stdio.h>
using namespace std;

struct fpos_list {
   fpos_t pos;
   struct fpos_list * next;
};

void destroyFposList(fpos_list * list);


struct kvpair{
   struct kvpair * next;
   string key;
   string val;
   //int val;
   //fpos_t pos;
   struct fpos_list * positions;
   bool diff;
};

// Delete ?
void delete_kvpair(kvpair * redshirt);

struct writeJob{
   pthread_t wjob;
   char * fname;
   writeJob *next;
};

class NoVoHT;

template<class type>
class novoht_iterator {

protected:
	kvpair ** list;
	kvpair * previous;
	kvpair * current;
	int index;
	int idxplus;
	int length;
	NoVoHT *_NoVoHT;

protected:
	void nextInternal();

public:
	novoht_iterator(kvpair**, int, NoVoHT*);
	virtual ~novoht_iterator();

	bool hasNext();
	virtual type next() = 0;
	void remove();

};

template<class type>
novoht_iterator<type>::novoht_iterator(kvpair** l, int size,
		NoVoHT *container) {

	previous = NULL;
	_NoVoHT = container;
	list = l;
	index = 0;
	idxplus = 0;
	length = size;
	while (index < length && (current = list[index]) == NULL) {
		index++;
		if (index >= length)
			break;
	}
}

template<class type>
novoht_iterator<type>::~novoht_iterator() {
}

template<class type>
bool novoht_iterator<type>::hasNext() {

	bool result = false;

	if (current != NULL && current->next != NULL)
		result = true;
	else {
		int i = index;
		kvpair *cursor;

		while (i + 1 < length && (cursor = list[++i]) == NULL) {
		}

		if (index + 1 >= length)
			result = false;
		else
			result = true;
	}

	return result;
}

template<class type>
void novoht_iterator<type>::nextInternal() {

	previous = current;

	if (current != NULL && current->next != NULL)
		current = current->next;
	else {
		while (index + 1 < length && (current = list[++index]) == NULL) {
		}
	}
}

class key_iterator: public novoht_iterator<string> {
public:
	key_iterator(kvpair** l, int s, NoVoHT *container) :
			novoht_iterator<string>(l, s, container) {
	}

	virtual ~key_iterator() {
	}

	string next(void) {
		string key = current->key;
		nextInternal();

		return key;
	}
};

class val_iterator: public novoht_iterator<string> {
public:
	val_iterator(kvpair** l, int s, NoVoHT *container) :
			novoht_iterator<string>(l, s, container) {
	}

	virtual ~val_iterator() {
	}

	string next(void) {
		string value = current->val;
		nextInternal();

		return value;
	}

};

class pair_iterator: public novoht_iterator<kvpair> {
public:
	pair_iterator(kvpair** l, int s, NoVoHT *container) :
			novoht_iterator<kvpair>(l, s, container) {
	}

	virtual ~pair_iterator() {
	}

	kvpair next(void) {
//		kvpair kv = *current; //lead to bug
		kvpair kv;
		kv.key = string(current->key.c_str());
		kv.val = string(current->val.c_str());

		nextInternal();

		return kv;
	}
};

class NoVoHT{
   int size;
   kvpair** kvpairs;
   kvpair** oldpairs;
   bool resizing;
   //bool map_lock;
   sem_t map_lock;
   //bool write_lock;
   sem_t write_lock;
   bool rewriting;
   int numEl;
   FILE * dbfile;
   FILE * swapFile;
   int swapNo;
   string filename;
   int nRem;
   void resize(int ns);
   int write(kvpair *);
   //void writeFile();
   void readFile();
   // Fix mark
   int mark(struct fpos_list *);
   int magicNumber;
   float resizeNum;
   //writeJob* rewriteQueue;
   //// Fix Merge
   void merge();
   pthread_t writeThread;
   // Fix write
   void rewriteFile(void*);
   //Fix logrm
   int logrm(string, struct fpos_list *);
   int writeAppend(kvpair *, string);
public:
	NoVoHT();
	//NoVoHT(int);
	//NoVoHT(char *);
	NoVoHT(const string&);
	NoVoHT(const string&, const int&, const int&);
	NoVoHT(const string&, const int&, const int&, const float&);
	void initialize(const string&, const int&, const int&, const float&);
	//NoVoHT(char *, NoVoHT*);
	~NoVoHT();
	int writeFile();
	int put(string, string);
   int append(string,string);
	string* get(string);
	int remove(string);
	int getSize() const {
		return numEl;
	}
	int getCap() const {
		return size;
	}

	bool isRewriting() const;

	int flushDbfile();

	key_iterator keyIterator();
	val_iterator valIterator();
	pair_iterator pairIterator();
        bool isRewriting() {return rewriting; }
        void flushDBFile() { fflush(dbfile); }
        static void* rewriteCaller(void* map){
           ((NoVoHT*)map)->rewriteFile(NULL);
			  return NULL;}
};

unsigned long long hash(string k);

void fsu(kvpair *);

char *readTabString(FILE*, char*);

template<class type>
void novoht_iterator<type>::remove() {

	if (previous != NULL)
		_NoVoHT->remove(previous->key);
}

#endif
