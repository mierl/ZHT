n=$1
o=$2

nfs="/mnt/common/tony/ZHT_Debug/src/test/dm"
local="/home/tony"


echo "running benchmark..."
ssh hec-$n "$nfs/zhtclienttest $o $nfs/neighbor2 $nfs/zht2.cfg TCP -i -l -r > $local/zbt.log"
#ssh hec-$n hostname
