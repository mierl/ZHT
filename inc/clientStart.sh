#!/bin/sh

numClient=$1
numOps=$2
protoc=$3

result=`echo "result.$numClient.$protoc"`
echo $result
i=1
while [ $i -le $numClient ]
do
        ./client_general $numOps neighbor zht.cfg $protoc >> $result &
        i=`expr $i + 1`
done
