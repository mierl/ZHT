#!/bin/bash
for i in .cxx .h
do mv phashmap$i novoht$i;
done

for i in $(ls .)
do
   if [ "$i" != "rename.sh" ]
   then
      for x in .cxx .o .h
         do sed -i "s/phashmap$x/novoht$x/g" $i
        done;
        sed -i "s/phashmap/NoVoHT/g" $i;
   fi
done;
