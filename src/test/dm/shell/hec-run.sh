nfs="/mnt/common/tony/ZHT_Debug/src/test/dm"
local="/home/tony"

v=$1
o=$2
echo "$v running benchmark..."
$nfs/zhtclienttest $o $nfs/neighbor2 $nfs/zht2.cfg TCP -i -l -r > $local/zbt.log &
sleep 15
echo ""
