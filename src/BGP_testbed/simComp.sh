#This script is designed to compile c++ code wrote with Kyoto Cabinet
# Created by Tony 
sourceFile=$1
log=$2
fileName=$(echo $sourceFile | awk -F '.' '{print $1}')
# Old compiling 
#g++ -g -Xlinker -zmuldefs -I/home/tony/Installed/built/include $sourceFile -L/home/tony/Installed/built/lib -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc d3_tcp.cpp d3_udp.cpp novoht.cxx net_util.cpp -o $fileName


#export USER_INCLUDE=/home/tonglin/Installed/built/include
#export USER_LIB=/home/tonglin/Installed/built/lib

if [[ $log == "l" ]]; then
g++ -g -Xlinker -zmuldefs -I$USER_INCLUDE $sourceFile -L$USER_LIB -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc novoht.cxx net_util.cpp zht_util.cpp cpp_zhtclient.cpp -o $fileName  -DILOG=1

else
g++ -g -Xlinker -zmuldefs -I$USER_INCLUDE $sourceFile -L$USER_LIB -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc novoht.cxx net_util.cpp zht_util.cpp cpp_zhtclient.cpp -o $fileName

fi
#g++ -g -Xlinker -zmuldefs -I/home/tonglin/Installed/built/include $sourceFile -L/home/tonglin/Installed/built/lib -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc novoht.cxx net_util.cpp zht_util.cpp cpp_zhtclient.cpp -o $fileName


#g++ -g  -Xlinker -zmuldefs  $sourceFile  -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc novoht.cxx net_util.cpp zht_util.cpp cpp_zhtclient.cpp -o $fileName

#rm *.o
