
echo "neighbor seeds(ss,se), node pool(ns,ne)"

ss=$1
se=$2
ns=$3
ne=$4

rm -f neighbor2
rm -f node2.cfg

echo "generating neighbor seeds(neighbor2)"
for((i=$ss;i<=$se;i++))
do
echo "hec-$i 50000" 
echo "hec-$i 50000" >> neighbor2
done
sleep 2

echo "generating node pool(node2.cfg)"
for((i=ns;i<=$ne;i++))
do
echo "hec-$i 50000" 
echo "hec-$i 50000" >> node2.cfg
done
sleep 2

