#include <stddef.h>
#include <stdlib.h>
#include <new>
#include <string>
#include <locale>
#include <stdio.h>
#include <iostream>
#include "../../inc/novoht.h"

NoVoHT::NoVoHT(){
   kvpairs = new kvpair*[1000];
   size = 1000;
   numEl = 0;
   magicNumber = 1000;
   resizeNum = -1;
   resizing=false;
   map_lock=false;
   write_lock=false;
   resizing = false;
   oldpairs = NULL;
}

/*
NoVoHT::NoVoHT(int s){
   kvpairs = new kvpair*[s];
   size = s;
   numEl=0;
   file = NULL;
}

NoVoHT::NoVoHT(char * f){
   kvpairs = new kvpair*[1000];
   size = 1000;
   numEl=0;
   file = f;
   readFile();
}
*/
NoVoHT::NoVoHT(string f,int s, int m){
   kvpairs = new kvpair*[s];
   for (int x = 0;x<s;x++){
      kvpairs[x] = NULL;
   }
   resizing=false;
   map_lock=false;
   write_lock=false;
   magicNumber = m;
   nRem = 0;
   resizeNum = 0;
   size = s;
   numEl=0;
   filename=f;
   dbfile = fopen(f.c_str(), "r+");
   if (!dbfile) dbfile = fopen(f.c_str(), "w+");
   readFile();
   oldpairs = NULL;
}
NoVoHT::NoVoHT(string f,int s, int m, float r){
   kvpairs = new kvpair*[s];
   for (int x = 0;x<s;x++){
      kvpairs[x] = NULL;
   }
   resizing=false;
   map_lock=false;
   write_lock=false;
   magicNumber = m;
   nRem = 0;
   resizeNum = r;
   size = s;
   numEl=0;
   filename=f;
   dbfile = fopen(f.c_str(), "r+");
   if (!dbfile) dbfile = fopen(f.c_str(), "w+");
   readFile();
   oldpairs = NULL;
}
/*
NoVoHT::NoVoHT(char * f, NoVoHT *map){
   kvpairs = new kvpair*[1000];
   size = 1000;
   numEl=0;
   file = f;
   readFile();
}*/
NoVoHT::~NoVoHT(){
   if (dbfile){
      writeFile();
      fclose(dbfile);
   }
   for (int i = 0; i < size; i++){
      fsu(kvpairs[i]);
   }
   delete [] kvpairs;
}

//0 success, -1 no insert, -2 no write
int NoVoHT::put(string k, string v){
   //while(resizing || map_lock){ /* Wait till done */}
   while(map_lock){ }
   map_lock=true;
   if (numEl >= size*resizeNum) {
      if (resizeNum !=0){
         resize(size*2);
      }
   }
   int slot;
   slot = hash(k)%size;
   kvpair *cur = kvpairs[slot];
   kvpair *add = new kvpair;
   add->key = k;
   add->val = v;
   add->next = NULL;
   if (cur == NULL){
      kvpairs[slot] = add;
      numEl++;
      map_lock=false;
      return write(add);
   }
   while (cur->next != NULL){
      if (k.compare(cur->key) == 0) {
         cur->val = v;
         mark(cur->pos);
         delete add;
         map_lock=false;
         return write(cur);
      }
      cur = cur->next;
   }
   if (k.compare(cur->key) == 0) {
      cur->val=v;
      mark(cur->pos);
      delete add;
      map_lock=false;
      return write(cur);
   }
   cur->next = add;
   numEl++;
   map_lock=false;
   return write(add);
}

string* NoVoHT::get(string k){
   while(map_lock){ /* Wait till done */}
   int loc = hash(k)%size;
   kvpair *cur = kvpairs[loc];
   while (cur != NULL && !k.empty()){
      if (k.compare(cur->key) == 0) return &(cur->val);
      cur = cur->next;
   }
   return NULL;
}

//return 0 for success, -1 fail to remove, -2+ write failure
int NoVoHT::remove(string k){
   while(map_lock){ /* Wait till done */}
   int ret =0;
   int loc = hash(k)%size;
   kvpair *cur = kvpairs[loc];
   if (cur == NULL) return ret-1;       //not found
   if (k.compare(cur->key) ==0) {
      fpos_t toRem = kvpairs[loc]->pos;
      kvpairs[loc] = cur->next;
      numEl--;
      ret+=mark(toRem);
      delete cur;
      nRem++;
      if (nRem == magicNumber) {ret+=writeFile(); nRem = 0;} //write and save write success
      return ret;
   }
   while(cur != NULL){
      if (cur->next == NULL) return ret-1;
      else if (k.compare(cur->next->key)==0){
         kvpair *r = cur->next;
         cur->next = r->next;
         ret+=mark(r->pos);              //mark and sace status code
         delete r;
         numEl--;
         nRem++;
         if (nRem == magicNumber) {ret+=writeFile(); nRem = 0;}              //mark and sace status code
         return ret;
      }
      cur = cur->next;
   }
   return ret-1;        //not found
}

//return 0 if success -2 if failed
//write hashmap to file
int NoVoHT::writeFile(){
   while (write_lock){}
   if (!dbfile)return (filename.compare("") == 0 ? 0 : -2);
   write_lock=true;
   int ret =0;
   rewind(dbfile);
   ftruncate(fileno(dbfile), 0);
   for (int i=0; i<size;i++){
      kvpair *cur = kvpairs[i];
      while (cur != NULL){
         if(!cur->key.empty() && !cur->val.empty()){
                fgetpos(dbfile, &(cur->pos));
                fprintf(dbfile, "%s\t%s\t", cur->key.c_str(), cur->val.c_str());
         }
         cur = cur->next;
      }
   }
   write_lock=false;
   return ret;
}

//success 0 fail -2
//resize the hashmap's base size
void NoVoHT::resize(int ns){
   resizing = true;
   int olds = size;
   size = ns;
   oldpairs = kvpairs;
   kvpairs = new kvpair*[ns];
   for (int z=0; z<ns; z++) { kvpairs[z] = NULL;}
   numEl = 0;
   for (int i=0; i<olds;i++){
      kvpair *cur = oldpairs[i];
      while (cur != NULL){
         int pos = hash(cur->key)%size;
         kvpair * tmp = kvpairs[pos];
         kvpairs[pos] = cur;
         cur = cur->next;
         kvpairs[pos]->next = tmp;
      }
   }
   delete [] oldpairs;
   resizing = false;
}

//success 0 fail -2
//write kvpair to file
int NoVoHT::write(kvpair * p){
   while (write_lock){}
   if (!dbfile)return (filename.compare("") == 0 ? 0 : -2);
   write_lock=true;
   fseek(dbfile, 0, SEEK_END);
   fgetpos(dbfile, &(p->pos));
   fprintf(dbfile, "%s\t%s\t", p->key.c_str(), p->val.c_str());
   write_lock=false;
   return 0;
}

//success 0 fail -2
//mark line in file for deletion
int NoVoHT::mark(fpos_t position){
   while(write_lock){}
   if (!dbfile)return (filename.compare("") == 0 ? 0 : -2);
   write_lock=true;
   fsetpos(dbfile, &position);
   fputc((int) '~', dbfile);
   write_lock=false;
   return 0;
}
char *readTabString(FILE *file, char *buffer){
   int n =0;
   char t;
   while((t=fgetc(file)) != EOF && n < 300){
      if (t == '\t') {
         buffer[n] = '\0';
         return buffer;
      }
      buffer[n] = t;
      n++;
   }
   buffer[n] = '\0';
   return (n == 0 ? NULL : buffer);
}

void NoVoHT::readFile(){
   if(!dbfile) return;
   char s[300];
   char v[300];
   while(readTabString(dbfile, s) != NULL){
      string key(s);
      if (readTabString(dbfile, v) == NULL) break;
      string val(v);
      if (key[0] != '~'){
         put(key,val);
      }
   }
   writeFile();
}

unsigned long long hash(string k){ //FNV hash
#if SIZE_OF_LONG_LONG_INT==8
#define FNV_PRIME 14695981039346656037
#define FNV_OFFSET 1099511628211
#else //SIZE_OF_LONG_LONG_INT == 4
#define FNV_PRIME 16777619
#define FNV_OFFSET 2166136261
#endif
   unsigned long long x = FNV_PRIME;
   for (unsigned int y=0;y<k.length();y++){
      x = x ^ (k[y]);
      x = x * FNV_OFFSET;
   }
   return (x);
}

void fsu(kvpair* p){
   if(p != NULL){
      fsu(p->next);
      delete p;
   }
}
