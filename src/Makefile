
TARGETS = zht_c_binding zht_benchmark zht_server
CC = gcc
CCFLAGS = -g
LIBFLAGS = -lstdc++ -lrt -lpthread -lm -lc -lprotobuf


server: zht_server
all:	$(TARGETS)
	


zht_c_binding: c_zhtclient_main.o c_zhtclient.o c_zhtclientStd.o cpp_zhtclient.o meta.pb-c.o lru_cache.o meta.pb.o net_util.o novoht.o zht_util.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS) -lprotobuf-c


zht_benchmark: benchmark_client.o cpp_zhtclient.o meta.pb-c.o lru_cache.o meta.pb.o net_util.o novoht.o zht_util.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)



zht_server: server_general.o lru_cache.o meta.pb.o net_util.o novoht.o zht_util.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)


%.o:	%.cpp
	$(CC) $(CCFLAGS) -c $< $^ $(LIBFLAGS)

	

c_zhtclient_main.o:		c_zhtclient_main.c
c_zhtclient.o:			c_zhtclient.cpp
c_zhtclientStd.o:		c_zhtclientStd.cpp

benchmark_client.o:		benchmark_client.cpp
cpp_zhtclient.o:		cpp_zhtclient.cpp

server_general.o:		server_general.cpp

meta.pb-c.o:			meta.pb-c.c
lru_cache.o:			lru_cache.cpp	
meta.pb.o:				meta.pb.cc
net_util.o:				net_util.cpp
novoht.o:				novoht.cpp
zht_util.o:				zht_util.cpp


.PHONY:	clean
	

clean:
	rm *.o
	rm $(TARGETS)
	rm *.gch*
	rm *.bin*
