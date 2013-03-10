PROJECT=HashTable
CC=g++
CFLAGS=-c -Wall
LDFLAGS=-I
SOURCE=hashmap.cxx main.cxx
OBJECTS=novoht.o main.o
HEADERS=novoht.h
BINS=kbench tonybench fbench pbench

all: $(BINS)

pbench: novoht.o pbench.cpp
	$(CC) novoht.o pbench.cpp -o pbench -lpthread

fbench: novoht.o fbench.cxx
	$(CC) -fpermissive novoht.o fbench.cxx -o fbench

gpbbench: novoht.o gpbbench.cxx
	$(CC) novoht.o meta.pb.cc gpbbench.cxx -o gpbbench `pkg-config --libs --cflags protobuf` -fpermissive

kbench: novoht.o kbench.cxx
	$(CC) novoht.o kbench.cxx -o kbench

tonybench: novoht.o test_hash.cpp
	$(CC) novoht.o test_hash.cpp -o tonybench

libnovoht: novoht.o
	g++ $(CFLAGS) -fPIC -o novoht.obj novoht.cxx
	g++ -shared -o libnovoht.so novoht.obj
	rm novoht.obj
	rm -f libnovoht.a
	ar -cvq libnovoht.a novoht.o

novoht.o: novoht.cxx
	$(CC) $(CFLAGS) novoht.cxx

clean:
	rm *.o $(BINS)

rebuild: clean

test_fbench: fbench
	./fbench 1000000 fbench.data

test_pbench: pbench
	./pbench 4096 pbench.data
