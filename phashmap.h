#ifndef PHASHMAP_H
#define PHASHMAP_H
#include "phashmap.h"
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

class phashmap{
   int size;
   kvpair** kvpairs;
   int numEl;
   string file;
   int nRem;
   void resize(int ns);
   int write(kvpair *);
   //void writeFile();
   void readFile();
   int mark(fpos_t);
   int magicNumber;
   float resizeNum;
   public:
        phashmap();
        //phashmap(int);
        //phashmap(char *);
        phashmap(string, int, int);
        phashmap(string, int, int, float);
        //phashmap(char *, phashmap*);
        ~phashmap();
        int writeFile();
        int put(string,  string);
        string* get(string);
        int remove(string);
        int getSize() {return numEl;}
        int getCap() {return size;}
};

unsigned long long hash (string k);

void fsu(kvpair *);
#endif
