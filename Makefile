# Author:  Corentin Debains
# Email:   cdebains@iit.edu
#


CXX=g++
CFLAGS=-Xlinker -zmuldefs -Llib -L/usr/local/lib -Iinc
LFLAGS=-lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc
LFLAGS+=-lzht

#SOURCES=$(wildcard src/common/*.cpp)
#OBJECTS=$(SOURCES:.cpp=.o)
OBJECTS=obj/meta.pb.o obj/net_util.o obj/novoht.o obj/zht_util.o

CFLAGS+=-I/usr/local/include/google/protobuf#your Google Protobuf location here :)

#.PHONY: clean examples


all: library
	make bin/server_zht

library: lib/libzht.a

bin/server_zht:
	$(CXX) $(CFLAGS) src/server_general.cpp -o bin/server_zht $(LFLAGS)

examples: lib/libzht.a
	$(CXX) $(CFLAGS) examples/benchmark.cpp -o examples/benchmark $(LFLAGS)

lib/libzht.a: $(OBJECTS) obj/client.o
	ar rus lib/libzht.a obj/*.o 

obj:
	mkdir -p obj

obj/client.o: src/client.cpp
	$(CXX) $(CFLAGS) -c src/client.cpp -o obj/client.o

obj/%.o: src/common/%.cpp obj
	$(CXX) $(CFLAGS) -c src/common/$*.cpp -o obj/$*.o

clean:
	rm -rf obj

mrproper: clean
	rm -f lib/*.a
	rm -f bin/zht_server
