
echo "run henchmark(ns, ne, op)"

ns=$1
ne=$2
op=$3

./rr.sh $ns $ne $op

sleep 10
#cat, show log(ns, ne)
./cat.sh $ns $ne
