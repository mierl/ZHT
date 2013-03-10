
TARGETS = libzht.a zht_server.exe admin_server.exe admin_console.exe file_server.exe
CC = gcc
CCFLAGS = -g
LIBFLAGS = -L. -lzht -ludt -lstdc++ -lprotobuf -lpthread

all:	$(TARGETS)

libzht.a: Address.o ConfigEntry.o ConfigHandler.o Const.o EpollServer.o ffsnet.o FileClient.o FilePorter.o HTWorker.o \
MigrateManager.o MutexCondition.o Neighbor.o NeighborManager.o NoVoHTManager.o ServiceHub.o Status.o Util.o \
Workload.o ZHTClient.o ZHTManager.o ZProcessor.o novoht.o net_util.o zht_util.o meta.pb.o
	ar -rcv $@ *.o 
	
		
zht_server.exe: ZHTNode.o 
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)
	
admin_server.exe: AdminServer.o 
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

admin_console.exe: AdminConsole.o 
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

file_server.exe: ffsnetd.o 
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)
	
			
%.o: %.c
	$(CC) $(CCFLAGS) -c $< $^ $(LIBFLAGS)


ZHTNode.o:			ZHTNode.cpp
AdminServer.o:		AdminServer.cpp
AdminConsole.o:		AdminConsole.cpp
ffsnetd.o:			ffsnetd.cpp	

Address.o:			Address.cpp
ConfigEntry.o:		ConfigEntry.cpp
ConfigHandler.o:	ConfigHandler.cpp
Const.o:			Const.cpp
EpollServer.o:		EpollServer.cpp
ffsnet.o:			ffsnet.cpp
FileClient.o:		FileClient.cpp
FilePorter.o:		FilePorter.cpp
HTWorker.o:			HTWorker.cpp
MigrateManager.o:	MigrateManager.cpp
MutexCondition.o:	MutexCondition.cpp
Neighbor.o:			Neighbor.cpp
NeighborManager.o:	NeighborManager.cpp
NoVoHTManager.o:	NoVoHTManager.cpp
ServiceHub.o:		ServiceHub.cpp
Status.o:			Status.cpp
Util.o:				Util.cpp
Workload.o:			Workload.cpp
ZHTClient.o:		ZHTClient.cpp
ZHTManager.o:		ZHTManager.cpp
ZProcessor.o:		ZProcessor.cpp

novoht.o:			novoht.cpp
net_util.o:			net_util.cpp
zht_util.o:			zht_util.cpp
meta.pb.o:			meta.pb.cc


.PHONY:	clean

clean:
	rm *.a
	rm *.o	
	rm *.exe
	rm *.gch*
	rm *.bin*
