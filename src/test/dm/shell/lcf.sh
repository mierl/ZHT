
echo "zhtserver port(ss, se)"

ss=$1
se=$2


rm -f neighbor
rm -f node.cfg

echo "generating neighbor seeds(neighbor)"
for((i=$ss;i<=$se;i++))
do
echo "localhost 5000$i" 
echo "localhost 5000$i" >> neighbor
done
sleep 2

echo "generating node (node)"
for((i=$ss;i<=$se;i++))
do
echo "localhost 6000$i" 
echo "localhost 6000$i" >> node.cfg
done
sleep 2


