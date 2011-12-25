=============================================
This latest version adopts our own persistent hash table instead of Kyotocabinet.



=============================================
Software requirement
---------------------------------------------
Kyotokabinet 1.2.60(required only for old version, hash.cpp)
Google protocol buffers 2.3.0


=============================================
Compile options
---------------------------------------------
Compile your code in following way:
g++ -g -Xlinker -zmuldefs -I/where-you-installed/include $sourceFile -o $fileName -L/where-you-installed/lib -lkyotocabinet -lz -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc d3_tcp.cpp d3_udp.cpp



=============================================
Serialized data object class
---------------------------------------------
By using Google protocol buffers we can serialize a complex data object into a plain string so to be transferfed esily. The data object is pre-defined as Package and it must be followed. In later version we will make the data object be flexable.

IMPORTANT NOTES:
1. You must follow the class Package, later I'll make it allow you use your structure.


=============================================
Establish a ZHT
---------------------------------------------
Simply execute ./hash <port> <memberListFile> to start a instance of ZHT server node.






Class ZHTClient allows you to create a client object which featured with insert/lookup/remove access to a established ZHT network. 

Before accessing the ZHT, you have to initialize ZHTClient by calling ZHTClient::initialize(string configFilePath, string memberListFilePath).



=============================================
Configure file
---------------------------------------------
Configure file must be STRICTLY organized as following format, now it only have two options:

REPLICATION_TYPE=0
NUM_REPLICAS=1

REPLICATION_TYPE defines where is replication executed, 0 for server side, 1 for client side. By default use server side, we don't recommend client side.
NUM_REPLICAS specify the number of replicas that you want to set, 0 means no replica. For most of applications 3 is adequate.	 



=============================================
Member List File
---------------------------------------------
Member list file should be organized in following way:

hostname1 port1
hostname2 port2
...


