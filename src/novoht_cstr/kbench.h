#ifndef KBENCH_H
#define KBENCH_H
#include "kbench.h"
#include <time.h>

int main(int argc, char *argv[]);
void testInsert();
void testGet(NoVoHT &map);
void testRemove(NoVoHT &map);
void testRemove2(NoVoHT &map);
double diffclock(clock_t clock1, clock_t clock2);

#endif
