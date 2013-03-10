
echo "kill zht server(ns, ne)"
ns=$1
ne=$2
for i in `seq $ns $ne`; do ssh hec-$i "/mnt/common/tony/ZHT_Debug/src/test/dm/hec-kill.sh"; done
