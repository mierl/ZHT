#!/bin/sh
project="FusionFS"
numNode=$1
runTime=$2
userName="tonglin"
protoc=$3
./old_n_to_1_startAll_general.sh $project $numNode $runTime $userName $protoc
