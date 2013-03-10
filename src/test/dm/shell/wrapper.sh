#!/bin/bash

echo "ss, se, op"
ns=$1
ne=$2
op=$3

for n in `seq $ns $ne`
do
  source ssh-do.sh $n $op &
done

sleep 15

./cat.sh $ns $ne
