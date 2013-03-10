#include <stdio.h>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "novoht.h"
#include <sys/time.h>
#include <sys/shm.h>
#define KEY_LEN 32
#define VAL_LEN 128
using namespace std;
struct timeval tp;

double getTime_usec() {
   gettimeofday(&tp, NULL);
   return static_cast<double>(tp.tv_sec) * 1E6+ static_cast<double>(tp.tv_usec);
}
string randomString(int len) {
   string s(len, ' ');
   srand(/*getpid()*/ clock() + getTime_usec());
   static const char alphanum[] = "0123456789"
       "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
       "abcdefghijklmnopqrstuvwxyz";
   for (int i = 0; i < len; ++i) {
      s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
   }
   return s;
}
struct ipair{
   NoVoHT *map;
   string key;
   string val;
};

void* insert(void* theargs){
   struct ipair args = *((ipair*)theargs);
   NoVoHT *map = args.map;
      //(theargs.map)->put(theargs.key,theargs.val);
   int stat;
   if((stat=map->put(args.key,args.val)) == 0)
      cout << "Insert" << endl;
   else
      cerr << "InsertFailure with " << stat << args.key<< endl;
   pthread_exit(NULL);
}

void* get(void* theargs){
   struct ipair args = *((ipair*)theargs);
   NoVoHT *map = args.map;
   string* ret = map->get(args.key);
   if(ret){
      if(ret->compare(args.val) == 0){
         cout<< "Found" << endl;
         pthread_exit(NULL);
      }
      else{ cerr << "key doesn't match" << endl;}
   }
   else
        cerr << "Key not found" << args.key<< endl;
   pthread_exit(NULL);
}

void* remove(void* argv){
   struct ipair args = *((ipair*)argv);
   NoVoHT *map = args.map;
   int stat;
   if((stat=map->remove(args.key)) == 0)
      cout << "Removed" << endl;
   else
      cerr << "RemoveErr " << stat <<endl;
   pthread_exit(NULL);
}

int main(int argc, char** argv){
   int size = atoi(argv[1]);
   string* keys = new string[size];
   string* vals = new string[size];
   pthread_t threads[size];
   for (int t = 0; t<size; t++){
      keys[t] = randomString(KEY_LEN);
      vals[t] = randomString(VAL_LEN);
   }
   const char* fname = "";
   if (argc >2) fname = argv[2];
   NoVoHT *pmap = new NoVoHT(fname,size,-1);
   struct ipair args[size];
   //pmap = new NoVoHT(fname, size, -1);
   double pointA = getTime_usec();
   for (int i=0; i<size; i++){
      args[i].map = pmap;
      args[i].key = keys[i];
      args[i].val = vals[i];
      int rc;
      rc = pthread_create(&threads[i], NULL, insert, (void *)&args[i]);
      if (rc)
         cout << "Failed: " << rc << endl;
   }
   for (int z =0; z <size; z++){
      pthread_join(threads[z], NULL);
   }
   double pointB = getTime_usec();
   for (int i=0; i<size; i++){
      args[i].map = pmap;
      args[i].key = keys[i];
      args[i].val = vals[i];
      int rc;
      rc = pthread_create(&threads[i], NULL, get, (void *)&args[i]);
      if (rc)
         cout << "Failed: " << rc << endl;
   }
   for (int z =0; z <size; z++){
      pthread_join(threads[z], NULL);
   }
   double pointC = getTime_usec();
   for (int i=0; i<size; i++){
      args[i].map = pmap;
      args[i].key = keys[i];
      args[i].val = vals[i];
      int rc;
      rc = pthread_create(&threads[i], NULL, remove, (void *)&args[i]);
      if (rc)
         cout << "Failed: " << rc << endl;
   }
   for (int z =0; z <size; z++){
      pthread_join(threads[z], NULL);
   }
   double pointD = getTime_usec();
   cout << "Insert time: " << pointB-pointA << endl;
   cout << "Retrieve time: " << pointC-pointB << endl;
   cout << "Remove time: " << pointD-pointC << endl;
   delete [] keys;
   delete [] vals;
   delete pmap;
   return 0;
}
