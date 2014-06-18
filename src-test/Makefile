TARGETS = zht_ctest zht_cpptest zht_ben zhtserver libzht.a c_zhtclient_threaded_test cpp_zhtclient_threaded_test c_zhtclient_lanl_threaded
CC = gcc
CCFLAGS = -g -I${USER_INCLUDE} -L${USER_LIB} -DPF_INET -DBIG_MSG -DSOCKET_CACHE -DSCCB -DTSQUEUE
#CCFLAGS = -g -I${USER_INCLUDE} -L${USER_LIB} -DTHREADED_SERVE -DPF_INET -DBIG_MSG -DSOCKET_CACHE -DSCCB -DTSQUEUE 
MPIFLAGS = -g -I${USER_INCLUDE} -L${USER_LIB} -DMPI_INET
LIBFLAGS = -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotobuf-c
MPILIBFLAGS = -lprotobuf -lpthread
COMMON = common/

all:	$(TARGETS)

c_zhtclient_lanl_threaded: c_zhtclient_lanl_threaded.o c_zhtclient.o c_zhtclientStd.o lock_guard.o cpp_zhtclient.o meta.pb-c.o lru_cache.o meta.pb.o zpack.pb.o novoht.o bigdata_transfer.o\
Const.o ConfHandler.o ConfEntry.o \
ProxyStubFactory.o proxy_stub.o ip_proxy_stub.o mq_proxy_stub.o ipc_plus.o tcp_proxy_stub.o udp_proxy_stub.o \
ZHTUtil.o Env.o Util.o \
HTWorker.o StrTokenizer.o TSafeQueue.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)


c_zhtclient_threaded_test: c_zhtclient_threaded_test.o c_zhtclient.o c_zhtclientStd.o lock_guard.o cpp_zhtclient.o meta.pb-c.o lru_cache.o meta.pb.o zpack.pb.o novoht.o bigdata_transfer.o\
Const.o ConfHandler.o ConfEntry.o \
ProxyStubFactory.o proxy_stub.o ip_proxy_stub.o mq_proxy_stub.o ipc_plus.o tcp_proxy_stub.o udp_proxy_stub.o \
ZHTUtil.o Env.o Util.o \
HTWorker.o StrTokenizer.o TSafeQueue.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)


cpp_zhtclient_threaded_test: cpp_zhtclient_threaded_test.o lock_guard.o cpp_zhtclient.o meta.pb-c.o lru_cache.o meta.pb.o zpack.pb.o novoht.o bigdata_transfer.o\
Const.o ConfHandler.o ConfEntry.o \
ProxyStubFactory.o proxy_stub.o ip_proxy_stub.o mq_proxy_stub.o ipc_plus.o tcp_proxy_stub.o udp_proxy_stub.o \
ZHTUtil.o Env.o Util.o \
HTWorker.o StrTokenizer.o TSafeQueue.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)



zht_ctest: c_zhtclient_test.o c_zhtclient.o c_zhtclientStd.o lock_guard.o cpp_zhtclient.o meta.pb-c.o lru_cache.o meta.pb.o zpack.pb.o novoht.o bigdata_transfer.o\
Const.o ConfHandler.o ConfEntry.o \
ProxyStubFactory.o proxy_stub.o ip_proxy_stub.o mq_proxy_stub.o ipc_plus.o tcp_proxy_stub.o udp_proxy_stub.o \
ZHTUtil.o Env.o Util.o \
HTWorker.o StrTokenizer.o TSafeQueue.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

zht_cpptest: cpp_zhtclient_test.o lock_guard.o cpp_zhtclient.o meta.pb-c.o lru_cache.o meta.pb.o zpack.pb.o novoht.o bigdata_transfer.o\
Const.o ConfHandler.o ConfEntry.o \
ProxyStubFactory.o proxy_stub.o ip_proxy_stub.o mq_proxy_stub.o ipc_plus.o tcp_proxy_stub.o udp_proxy_stub.o \
ZHTUtil.o Env.o Util.o \
HTWorker.o StrTokenizer.o TSafeQueue.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)
	

zht_ben: benchmark_client.o lock_guard.o cpp_zhtclient.o meta.pb-c.o lru_cache.o meta.pb.o zpack.pb.o novoht.o bigdata_transfer.o\
Const.o ConfHandler.o ConfEntry.o \
ProxyStubFactory.o proxy_stub.o ip_proxy_stub.o mq_proxy_stub.o ipc_plus.o tcp_proxy_stub.o udp_proxy_stub.o \
ZHTUtil.o Env.o Util.o \
HTWorker.o StrTokenizer.o TSafeQueue.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)


zhtserver: ZHTServer.o lock_guard.o meta.pb-c.o lru_cache.o meta.pb.o zpack.pb.o novoht.o bigdata_transfer.o\
Const.o ConfHandler.o ConfEntry.o \
ProxyStubFactory.o proxy_stub.o ip_proxy_stub.o mq_proxy_stub.o ipc_plus.o tcp_proxy_stub.o udp_proxy_stub.o \
ZHTUtil.o Env.o Util.o StrTokenizer.o\
EpollServer.o ZProcessor.o ip_server.o HTWorker.o StrTokenizer.o TSafeQueue.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)


libzht.a: 
	ar r libzht.a *.o	

			
%.o: %.c
	$(CC) $(CCFLAGS) -c $^ $(LIBFLAGS)
	
%.o: %.cpp
	$(CC) $(CCFLAGS) -c $^ $(LIBFLAGS)
	
%.o: %.cc
	$(CC) $(CCFLAGS) -c $^ $(LIBFLAGS)


.PHONY:	clean

clean:	
	rm *.o
	rm $(TARGETS)	
	rm zht-mpibroker
	rm zht-mpiserver	
	
mpi:
	mpicxx mpi_broker.cpp proxy_stub.cpp mq_proxy_stub.cpp ipc_plus.cpp mpi_proxy_stub.cpp ConfHandler.cpp ConfEntry.cpp StrTokenizer.cpp Util.cpp Env.cpp HTWorker.cpp Const.cpp novoht.cpp meta.pb.cc zpack.pb.cc lock_guard.cpp $(MPIFLAGS) $(MPILIBFLAGS) -o zht-mpibroker
		
	mpicxx ZHTServer.cpp mpi_server.cpp ProxyStubFactory.cpp proxy_stub.cpp mpi_proxy_stub.cpp Util.cpp Env.cpp mq_proxy_stub.cpp ipc_plus.cpp ConfHandler.cpp ConfEntry.cpp StrTokenizer.cpp HTWorker.cpp Const.cpp novoht.cpp meta.pb.cc zpack.pb.cc lock_guard.cpp $(MPIFLAGS) $(MPILIBFLAGS) -o zht-mpiserver

	
