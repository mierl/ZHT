
echo ""
echo "start server(v), as neighbor seed(y/n), file server(f), admin server(a), )"


v=$1
s=$2
f=$3
a=$4



nfs="/mnt/common/tony/ZHT_Debug/src/test/dm"
local="/home/tony"

if [[ $s == "y" ]]; then

echo "starting zht server..."
$nfs/zhtnode -t $v -p 50000 -d $nfs/node2.cfg -n $nfs/neighbor2 -z $nfs/zht2.cfg -s > $local/zss.log &

sleep 1
elif [[ $s == "n" ]]; then

echo "starting zht server..."
$nfs/zhtnode -t $v -p 50000 -d $nfs/node2.cfg -n $nfs/neighbor2 -z $nfs/zht2.cfg > $local/zss.log &

sleep 1

else

echo "no zht server started"

fi

if [[ $f == "f" ]]; then
echo "starting file server..."
$nfs/fileserver > $local/fss.log &
sleep 1
fi

if [[ $a == "a" ]]; then
echo "starting admin server..."
$nfs/adminserver -t $v -p 40000 -d $nfs/node2.cfg -n $nfs/neighbor2 -z $nfs/zht2.cfg > $local/mss.log &
sleep 1
fi
