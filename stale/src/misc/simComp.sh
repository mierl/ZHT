#This script is designed to compile c++ code wrote with Kyoto Cabinet
# Created by Tony 
sourceFile=$1
fileName=$(echo $sourceFile | awk -F '.' '{print $1}')


#export USER_INCLUDE=/home/tony/Installed/built/include:/usr/loca/include
export USER_INCLUDE=/usr/loca/include


#export USER_LIB=$USER_LIB:/home/tony/Installed/built/lib:/usr/loca/lib
#export CPLUS_INCLUDE_PATH=/home/tony/Installed/built/include
export USER_LIB=/usr/loca/lib

g++ -g -Xlinker -zmuldefs -I$USER_INCLUDE $sourceFile -L$USER_LIB -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc novoht.cxx net_util.cpp zht_util.cpp cpp_zhtclient.cpp -o $fileName

#g++ -g -Xlinker -zmuldefs  $sourceFile  -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc novoht.cxx net_util.cpp zht_util.cpp cpp_zhtclient.cpp -o $fileName

#lru_cache.cpp 
# -zmuldefs -lz

#rm *.o
