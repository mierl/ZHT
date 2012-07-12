# Author:  Corentin Debains
# Email:   cdebains@iit.edu
#

CXX=g++
CC=gcc
CPPFLAGS=-Xlinker -zmuldefs
CFLAGS=-Llib -L/usr/local/lib -Iinc
LFLAGS=-lzht
LFLAGS+=-lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc

PROTOBUF_HOME=/usr/local/include/google/protobuf #your Google Protobuf location here :) (Default is:/usr/local/include/google/protobuf)

#SOURCES=$(wildcard src/common/*.cpp)
#OBJECTS=$(SOURCES:.cpp=.o)
OBJECTS=obj/meta.pb.o obj/net_util.o obj/novoht.o obj/zht_util.o obj/lru_cache.o

CFLAGS+=-I$(PROTOBUF_HOME)

.PHONY: clean examples clients


all: library
	make bin/server_zht
	make examples

library: lib/libzht.a
	#Note that the header files (.h) are required to be included by a client that would alread have the static library libzht
	#Uncomment the next two copy if you removed the zht_util enormous dependencies or the zht_util dependency in cpp_zhtclient. For now, client have to import all the include folder when compiling.
	#cp inc/cpp_zhtclient.h lib/cpp_zhtclient.h
	#cp inc/c_zhtclient.h lib/c_zhtclient.h

bin/server_zht:
	$(CXX) $(CFLAGS) src/server_general.cpp -o bin/server_zht $(LFLAGS)

examples:
	$(CXX) $(CPPFLAGS) $(CFLAGS) examples/benchmark_client.cpp -o examples/benchmark_client $(LFLAGS)
	$(CC) $(CFLAGS) examples/c_zhtclient_main.c -o examples/c_zhtclient_main $(LFLAGS)
	$(CXX) $(CPPFLAGS) $(CFLAGS) examples/testProtocBuf.cpp -o examples/testProtocBuf $(LFLAGS)

lib/libzht.a: $(OBJECTS) clients
	ar rus lib/libzht.a obj/*.o 

obj:
	mkdir -p obj

clients: src/cpp_zhtclient.cpp src/c_zhtclient.cpp
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c src/cpp_zhtclient.cpp -o obj/cpp_zhtclient.o
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c src/c_zhtclient.cpp -o obj/c_zhtclient.o

obj/%.o: src/common/%.cpp obj
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c src/common/$*.cpp -o obj/$*.o

gProto: src/misc/meta.proto
	rm src/common/*.pb.cpp inc/*.pb.h
	protoc -I=src/misc/ --cpp_out=src/common src/misc/meta.proto
	mv src/common/*.h inc/
	rename 's/\.cc/.cpp/' src/common/*.cc
	
clean:
	rm -rf obj

mrproper: clean
	rm -f lib/*.a
	rm -f lib/*.h
	rm -f bin/server_zht
	rm examples/benchmark_client
	rm examples/c_zhtclient_main
	rm examples/testProtocBuf
