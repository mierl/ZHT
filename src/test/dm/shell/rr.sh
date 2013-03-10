
echo "running benchmark server(ns, ne)"
ns=$1
ne=$2
op=$3

nfs="/mnt/common/tony/ZHT_Debug/src/test/dm"

for i in `seq $ns $ne`; do ssh hec-$i "$nfs/hec-run.sh hec-$i $op"; done
