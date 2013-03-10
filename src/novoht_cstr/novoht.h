#ifndef PHASHMAP_H
#define PHASHMAP_H
#include "novoht.h"
#include <stdio.h>
#include <cstring>
using namespace std;

struct kvpair{
   struct kvpair * next;
   char* key;
   char* val;
   //int val;
   fpos_t pos;
};

class NoVoHT{
   int size;
   kvpair** kvpairs;
   kvpair** oldpairs;
   bool resizing;
   bool map_lock;
   bool write_lock;
   int numEl;
   FILE * dbfile;
   char* filename;
   int nRem;
   void resize(int ns);
   int write(kvpair *);
   //void writeFile();
   void readFile();
   int mark(fpos_t);
   int magicNumber;
   float resizeNum;
   public:
        NoVoHT();
        //NoVoHT(int);
        //NoVoHT(char *);
        NoVoHT(char*, int, int);
        NoVoHT(char*, int, int, float);
        //NoVoHT(char *, NoVoHT*);
        ~NoVoHT();
        int writeFile();
        int put(const char*, const char*);
        char* get(const char*);
        int remove(char*);
        int getSize() {return numEl;}
        int getCap() {return size;}
};

unsigned long long hash (const char* k);

void fsu(kvpair *);

char *readTabString(FILE*, char*);
#endif
