
echo "running benchmark(op,cr)..."

op=$1
#cr=$2


#for i in `seq 1 $cr`; do ./zhtclienttest $op neighbor zht.cfg TCP -i -l -r;  done > $i.log


./zhtclienttest $op neighbor zht.cfg TCP -i -l -r  &
./zhtclienttest $op neighbor zht.cfg TCP -i -l -r  &
./zhtclienttest $op neighbor zht.cfg TCP -i -l -r  &
./zhtclienttest $op neighbor zht.cfg TCP -i -l -r

sleep(15)
