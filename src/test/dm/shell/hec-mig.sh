
echo "add nodes, node pool(ns,ne)"

nfs="/mnt/common/tony/ZHT_Debug/src/test/dm"
local="/home/tony"

ns=$1
ne=$2

rm -f mig.log
for((i=ns;i<=$ne;i++))
do
echo "adding hec-$i node"
./adminconsole -t hec-$i -p 40000 -a > mig.log
sleep 2
done
