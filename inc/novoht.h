#ifndef PHASHMAP_H
#define PHASHMAP_H
#include "novoht.h"
#include <string>
#include <stdio.h>
using namespace std;

struct kvpair{
   struct kvpair * next;
   string key;
   string val;
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
   string filename;
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
        NoVoHT(string, int, int);
        NoVoHT(string, int, int, float);
        //NoVoHT(char *, NoVoHT*);
        ~NoVoHT();
        int writeFile();
        int put(string,  string);
        string* get(string);
        int remove(string);
        int getSize() {return numEl;}
        int getCap() {return size;}
};

unsigned long long hash (string k);

void fsu(kvpair *);

char *readTabString(FILE*, char*);
#endif
