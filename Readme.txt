=============================================

=============================================
Software requirement
---------------------------------------------
Google protocol buffers 2.3.0 or newer


=============================================
Compile options
---------------------------------------------
To compile type:
* make

Note for Compilation: This compile the server in the bin folder. It also compiles the static library zht in the lib folder.

=============================================
How to use
---------------------------------------------
ZHT is designed to manage file system meta data, if you want to use it storing general purpose data, please make sure it will follow the structure of class Package. Now only plain string is supported inside of Package.

Establish a ZHT
---------------------------------------------
Note: the $protocol could only be "TCP" or "UDP"

Server:
Simply execute ./server_general $port $memberListFile $configFile $protocol to start a instance of ZHT server node.
How to stop servers: execute ./haltServer $memberListFile $configFile on any node.
Client:
For runing the example, run
./client_general $numOps $memberListFile $configFile $protocol
This will insert, lookup and remove $numOps records in ZHT servers.

Class ZHTClient allows you to create a client object which featured with insert/lookup/remove access to a established ZHT network. 
Before accessing the ZHT, you have to initialize ZHTClient by calling ZHTClient::initialize(string configFilePath, string memberListFilePath).
There is a comprehensive example in client.cpp, please check following functions:
int benchmarkInsert(...)
int benchmarkLookup(...)
int benchmarkRemove(...)



=============================================
Configure file
---------------------------------------------
Configure file must be STRICTLY organized as following format, now it only have two options:

REPLICATION_TYPE=0
NUM_REPLICAS=0

NUM_REPLICAS specify the number of replicas that you want to set, 0 means no replica. For most of applications 3 is adequate.(Now replica has some problem, we’re fixing it, so please set both of them to be 0 for now.)	 



=============================================
Member List File
---------------------------------------------
Member list file should be organized in following way:
hostname1 port1
hostname2 port2
...



=============================================
IMPORTANT NOTES:
1. The class Package must be followed.
2. Make sure the serialized package is smaller than 64KB.




