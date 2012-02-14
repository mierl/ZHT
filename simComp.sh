#This script is designed to compile c++ code wrote with Kyoto Cabinet
# Created by Tony 
sourceFile=$1
fileName=$(echo $sourceFile | awk -F '.' '{print $1}')



g++ -g -Xlinker -zmuldefs -I/home/tony/Installed/built/include $sourceFile -L/home/tony/Installed/built/lib -lz -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc d3_tcp.cpp d3_udp.cpp novoht.cxx -o $fileName



#rm *.o
