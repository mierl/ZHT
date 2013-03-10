
echo "man prepare (ns, ne)"

ns=$1
ne=$2
op=$3
#gen neighbor and node
./hec-cf.sh $ns $ne

sleep 2

#kill and remove, 
./kill.sh $ns $ne

sleep 5
#echo "start service(ns, ne), seed(y/n)"
./ss.sh $ns $ne y  1 4

sleep 2
#cat, show log(ns, ne)
./cat.sh $ns $ne

sleep 10


