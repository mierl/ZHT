#include <iostream>
#include "time.h"
#include <string>
#include <unistd.h>
#include "novoht.h"
#include <cstdlib>
#include <cstddef>
#include <sys/time.h>//
#include <sys/stat.h>
#include <cstring>
#define KEY_LEN 32
#define VAL_LEN 128
using namespace std;
struct timeval tp;
double diffclock(clock_t clock1, clock_t clock2){
   double clock_ts=clock1-clock2;
   double diffms=(clock_ts*1000)/CLOCKS_PER_SEC;
   return diffms;
}
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
double testInsert(NoVoHT &map, string keys[], string vals[], int l){
   int fails =0;
   cerr << "0\% Complete\r";
   cerr.flush();
   //clock_t a=clock();
   double a = getTime_usec();
   int t;
   for (t = 0; t<l; t++){
      fails -= map.put(keys[t].c_str(), vals[t].c_str());
      if ((t+1)%1000 == 0)
         cerr << (long)t*100/l << "\% Complete\r";
   }
   double b = getTime_usec();
   //clock_t b=clock();
   cerr << "100\% Complete with " << fails << " not inserted" << endl << endl;
   //return diffclock(b,a);
   return (b-a);
   ;
}
double testGet(NoVoHT &map, string keys[], string vals[], int l){
   int fails = 0;
   cerr << "0\% Complete\r";
   cerr.flush();
   double a = getTime_usec();
   for (int t=0; t<l; t++){
      char* s = map.get(keys[t].c_str());
      if (!s) fails++;
      else if (strcmp(vals[t].c_str(), s) != 0)fails++;
      if ((t+1)%1000 == 0)cerr << (long)t*100/l << "\% Complete\r";
   }
   double b = getTime_usec();
   cerr << "100\% Complete with " << fails << " not found" << endl;
   return b-a;
}
double testRemove(NoVoHT &map, string keys[], int l){
   int fails = 0;
   cerr << "0\% Complete\r";
   cerr.flush();
   double a = getTime_usec();
   for (int t=0; t<l; t++){
      fails -= map.remove(keys[t].c_str());
      if ((t+1)%1000 == 0)cerr << (long)t*100/l << "\% Complete\r";
   }
   double b = getTime_usec();
   cerr << "100\% Complete with " << fails << " not found" << endl;
   return b-a;
}
int main(int argc, char *argv[]){
   cerr << "\nInitializing key-value pairs for testing\n";
   cerr << "0\%\r";
   cerr.flush();
   int size = atoi(argv[1]);
   string* keys = new string[size];
   string* vals = new string[size];
   for (int t=0; t<size; t++){
      keys[t] = randomString(KEY_LEN);
      vals[t] = randomString(VAL_LEN);
      if(t%1000 == 0)cerr << (long)t*100/size << "\%\r";
   }
   cerr << "Done\n" << endl;

   //NoVoHT map ("fbench.data", 1000000000, 10000);
   char c[40];
   struct stat fstate;
   sprintf(c, "cat /proc/%d/status | grep VmPeak", (int)getpid());
   system(c);
   const char* fn = "";
   if(argc > 2) fn = argv[2];
   //NoVoHT map (fn, size, 10000, .7);
   NoVoHT map (fn, size, -1);

   //NoVoHT map ("", 10000000, -1);
   //NoVoHT map ("", 1000000, 10000, .7);
   //NoVoHT map ("", 1000000, -1);
   //NoVoHT map ("/dev/shm/fbench.data", 1000000, -1);

   double ins, ret, rem;
   cerr << "Testing Insertion: Inserting " << size << " elements" << endl;
   ins = testInsert(map, keys,vals,size);

   cerr << "Testing Retrieval: Retrieving " << size << " elements" << endl;
   ret = testGet(map,keys,vals,size);
   cerr << endl;

   cerr << "Testing Removal: Removing " << size << " elements" << endl;
   rem = testRemove(map,keys,size);

   cout << size << " k-v pairs " << KEY_LEN << "-" << VAL_LEN << endl;
   cout << "\nInsertion done in " << ins << " microseconds" << endl;
   cout << "Retrieval done in " << ret << " microseconds" << endl;
   cout << "Removal done in " << rem << " microseconds" << endl;
   system(c);
   stat(fn, &fstate);
   cout << "Filesize in use " << fstate.st_size << endl << endl;
   delete [] keys;
   delete [] vals;
   return 0;
}

