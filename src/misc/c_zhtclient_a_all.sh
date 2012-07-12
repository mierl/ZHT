gcc -g -c meta.pb.cc novoht.cxx net_util.cpp zht_util.cpp cpp_zhtclient.cpp c_zhtclient.cpp

ar -rcv libczhtclient.a meta.pb.o novoht.o net_util.o zht_util.o cpp_zhtclient.o c_zhtclient.o
rm -rf *.o

gcc -g c_zhtclient_main.c -o c_zhtclient_main -L. -lczhtclient -lstdc++ -lprotobuf

./c_zhtclient_main neighbor zht.cfg TCP

#cp libczhtclient.a ../c-zhtclient-test/libczhtclient.a




