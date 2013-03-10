#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <new>
#include "novoht.h"
#include "kbench.h"
#include "time.h"
#define SIZE 1000000
#define NUMEL 10000000
using namespace std;
int size;

int main(int argc, char *argv[]){
   if (argc > 1)
      size = atoi(argv[1]);
   else size = SIZE;
   NoVoHT *map = new NoVoHT("hash.table", size, -1);
   map->put("kevin","hello");
   map->put("kevin","overwrite");
   printf("The value is %s", (map->get("kevin"))->c_str());
   //testInsert();
   delete map;
   return 0;
}

void testInsert(){
   printf("Testing live data...    \nTesting insertion...      ");
   fflush(stdout);
   FILE *paths;
   paths = fopen("test.data", "r") ;
   if (paths == NULL){
      printf("File open failure\n");
      return;
   }
   int n =NUMEL;
   clock_t a=clock();
   NoVoHT *map = new NoVoHT("hash.table", size, 10000, 2/3);
   std::string *list = new string[n];
   char f[300];
   int count = 0;
   list[count] = "hello";
   while (fscanf(paths, "%s", f) != EOF){
      list[count].assign(f);
      count++;
   }
   clock_t begin=clock();
   while (count > 0){
      char t[300];
      sprintf(t, "%d",NUMEL-count+1);
      string z(t);
      map->put(list[count-1],z);
      n--;
      count--;
   }
   fclose(paths);
   printf("PASSED\n");
   clock_t end=clock();
   testGet(*map);
   printf("Insertion Time: %f\n", diffclock(end, begin));
   printf("Alloc Time: %f\n", diffclock(begin,a));
   delete [] list;
   delete map;
}

void testGet(NoVoHT &map){
   clock_t begin=clock();
   FILE *oth;
   printf("Testing retrieval...      ");
   fflush(stdout);
   oth = fopen("test.data", "r");
   if (oth == NULL){
      printf("File open failure\n");
      return;
   }
   int n =NUMEL;
   char f[300];
   while (fscanf(oth, "%s", f) != EOF){
      //printf("Checking %s...    ", f);
      if (atoi(map.get(f)->c_str()) != n){
         printf("FAILED\n");
         //printf("%s,%s,%d\n",f,map.get(f), n);
         return;
      }
      else{
         //printf("Found\n");
      }
      n--;
   }
   if (map.get("kevin") != NULL) printf("FAILED\n");
   else printf("PASSED\n");
   //delete [] f;
   fclose(oth);
   //printf("PASSED\n");
   clock_t end=clock();
   testRemove2(map);
   printf("Retrieval time: %f\n", diffclock(end, begin));
}

void testRemove(NoVoHT &map){
   clock_t beg=clock();
   FILE *oth;
   printf("Testing removal...        ");
   fflush(stdout);
   oth = fopen("test.data", "r");
   if (oth == NULL){
      printf("File open failure\n");
      return;
   }
   int n =NUMEL;
   char f[300];
   int start = map.getSize();
   while (fscanf(oth, "%s", f) != EOF){
      //printf("Removing %s...    ", f);
      //map.remove(f);
      if (n%1==0) {map.remove(f); /*printf(".");fflush(stdout);*/}
      if (atoi(map.get(f)->c_str()) != n){
         printf("FAILED\n");
         //printf("%s,%d,%d\n",f,map.get(f), n);
         return;
      }
      else{
         //printf("Removed\n");
      }
      n--;
   }
   fclose(oth);
   printf("PASSED\n");
   clock_t end=clock();
   printf("Inserted: %d\nRemoved: %d\n", start, start-map.getSize());
   printf("Removal Time: %f\n", diffclock(end,beg));
}
void testRemove2(NoVoHT &map){
   FILE *oth;
   printf("Testing removal...        ");
   fflush(stdout);
   oth = fopen("test.data", "r");
   //oth = fopen("/home/kevin/Library.pla","r");
   if (oth == NULL){
      printf("File open failure\n");
      return;
   }
   int n =NUMEL;
   char ** list = (char **) calloc(map.getSize(), sizeof(char*));
   char f[300];
   int count = 0;
   while (fscanf(oth, "%s", f) != EOF){
      list[count] = (char *)calloc(300, sizeof(char));
      strcpy(list[count],f);
      count++;
   }
   clock_t beg=clock();
   //for (int x =count-1; x>=0 ; x--){
   for (int x = 0; x < count; x++){
      //map.remove(list[count-x]);
      //free(list[count-x]);
      string t (list[x]);
      map.remove(t);
      free(list[x]);
   }
   //for(int i = count-1; i>=0; i--){
   //   free(list[i]);
   //}
   free(list);
   fclose(oth);
   printf("PASSED\n");
   clock_t end=clock();
   printf("Inserted: %d\nRemoved: %d\n",count, count-map.getSize());
   printf("Removal Time: %f\n", diffclock(end,beg));
}

double diffclock(clock_t clock1, clock_t clock2){
	double clock_ts=clock1-clock2;
	double diffms=(clock_ts*1000)/CLOCKS_PER_SEC;
	return diffms;
}
