#This script is designed to compile c++ code wrote with Kyoto Cabinet
# Created by Tony 
sourceFile=$1
fileName=$(echo $sourceFile | awk -F '.' '{print $1}')
# Old compiling 
#g++ -g -Xlinker -zmuldefs -I/home/tony/Installed/built/include $sourceFile -L/home/tony/Installed/built/lib -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc d3_tcp.cpp d3_udp.cpp novoht.cxx net_util.cpp -o $fileName




#g++ -g -Xlinker -zmuldefs -I/home/tony/Installed/built/include $sourceFile -L/home/tony/Installed/built/lib -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc novoht.cxx net_util.cpp zht_util.cpp -o $fileName

echo $USER_LIB
echo $USER_INCLUDE

g++ -g -Xlinker -zmuldefs -I$USER_INCLUDE $sourceFile -L$USER_LIB -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc novoht.cxx net_util.cpp zht_util.cpp -o $fileName

#lru_cache.cpp 
# -zmuldefs -lz

#rm *.o
