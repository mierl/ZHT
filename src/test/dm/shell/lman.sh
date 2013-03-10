"running totally(ss, se, op, cr)...."

ss=$1
se=$2
op=$3
#cr=$4


echo "kill..."
./hec-kill.sh
sleep 5


echo "configing(neighbor seed..."
./lcf.sh $ss $se
sleep 5

echo "starting service..."
./lss.sh $ss $se y
sleep 5


echo "running benchmark..."
./lrr.sh $op

