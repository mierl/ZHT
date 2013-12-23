#This script is designed to compile c++ code wrote with Kyoto Cabinet
# Created by Tony 
sourceFile=$1
fileName=$(echo $sourceFile | awk -F '.' '{print $1}')
#g++ -c -I. -I.. -Wall -fsigned-char -O2 $sourceFile
#LD_RUN_PATH=/lib:/usr/lib:/home/tony/lib:/usr/local/lib:.:.. g++ -I. -I.. -Wall -fsigned-char -O2 -o $fileName $fileName.o  -L. -L.. -lkyotocabinet -lstdc++ -lz -lrt -lpthread -lm -lc -lprotobuf -lprotoc metadata.pb.cc d3_tcp.cpp

#Below works for installing in /usr/;
#g++ -g -Xlinker -zmuldefs -I/usr/local/include $sourceFile -o $fileName -L/usr/local/lib -lkyotocabinet -lz -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc d3_tcp.cpp d3_udp.cpp


#  This works for general situation
g++ -g -Xlinker -zmuldefs -I/home/tony/Installed/built/include $sourceFile -o $fileName -L/home/tony/Installed/built/lib -lkyotocabinet -lz -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc d3_tcp.cpp d3_udp.cpp novoht.cxx



#rm *.o
