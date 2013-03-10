
echo "start service(ns, ne), seed(y/n)"
ns=$1
ne=$2
s=$3
for i in `seq $ns $ne`; do ssh hec-$i "/mnt/common/tony/ZHT_Debug/src/test/dm/hec-ss.sh hec-$i $s f a"; done
